// Copyright Epic Games, Inc. All Rights Reserved.

#include "Characters/BaseCharacter.h"

#include "Character/ALSCharacterMovementComponent.h"
#include "Characters/EffectContainerComponent.h"
#include "Characters/HealthComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Utils/CombatUtils.h"
#include "Utils/EffectUtils.h"
#include "Utils/GameplayTagUtils.h"

ABaseCharacter::ABaseCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	MovementModel.DataTable = LoadObject<UDataTable>(nullptr, UTF8_TO_TCHAR("DataTable'/Game/_Genestealer/Characters/GenstealerMovement.GenstealerMovement'"));
	MovementModel.RowName = "Responsive";
	
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	GetCapsuleComponent()->SetCollisionResponseToChannels(ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	
	GetMesh()->SetCollisionObjectType(ECC_Pawn);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionResponseToChannels(ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetMesh()->SetGenerateOverlapEvents(true);

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	EffectContainerComponent = CreateDefaultSubobject<UEffectContainerComponent>(TEXT("EffectContainer"));
	CurrentAffiliation = EAffiliation::Neutral;
}

void ABaseCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

float ABaseCharacter::PlayWeaponAnimation(EWeaponAnimArchetype WeaponArchetype, EWeaponAnimAction WeaponAction)
{
	FAnimMontagePlayData PlayData;
	PlayData.MontageToPlay = Internal_GetWeaponAnimation(WeaponArchetype, WeaponAction);
	return Internal_PlayMontage(PlayData);
}

void ABaseCharacter::StopWeaponAnimation(EWeaponAnimArchetype WeaponArchetype, EWeaponAnimAction WeaponAction)
{
	StopAnimMontage(Internal_GetWeaponAnimation(WeaponArchetype, WeaponAction));
}

UAnimMontage* ABaseCharacter::Internal_GetWeaponAnimation(EWeaponAnimArchetype WeaponArchetype, EWeaponAnimAction WeaponAction) const
{
	switch (WeaponAction)
	{
	case EWeaponAnimAction::Fire:
		return K2_GetWeaponFireAnimation(WeaponArchetype);
	case EWeaponAnimAction::Reload:
		return K2_GetWeaponReloadAnimation(WeaponArchetype);
	case EWeaponAnimAction::Equip:
		return K2_GetWeaponEquipAnimation(WeaponArchetype);
	default:
		return nullptr;
	}
}

void ABaseCharacter::ChangeOverlayState(EALSOverlayState InOverlayState)
{
	SetOverlayState(InOverlayState);
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	PlayerInCombatChanged.Broadcast(false, nullptr);
	if(HealthComponent)
	{
		HealthComponent->OnCurrentWoundHealthChanged().AddUObject(this, &ABaseCharacter::HandleCurrentWoundChangedEvent);
		HealthComponent->OnActorDeath().AddUObject(this, &ABaseCharacter::HandleDeathEvent);
		HealthComponent->InitHealthComponent(StartingHealth);
	}
	UEffectUtils::ApplyEffectsToActor(DefaultEffects, this);
	Internal_AddDefaultTagsToContainer();
}

void ABaseCharacter::PreInitializeComponents()
{
	Super::PreInitializeComponents();
	if(InventoryComponent)
	{
		InventoryComponent->OnCurrentWeaponChanged().AddDynamic(this, &ABaseCharacter::HandleCurrentWeaponChanged);
	}
}

void ABaseCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if(InventoryComponent)
	{
		InventoryComponent->SpawnInventoryActors(StartingPistolClass, StartingRifleClass, StartingMeleeClass);
	}
}

void ABaseCharacter::ForwardMovementAction_Implementation(float Value)
{
	if(UGameplayTagUtils::ActorHasGameplayTag(this, GameplayTag::State::Stunned))
	{
		return;
	}
	Super::ForwardMovementAction_Implementation(Value);
}

void ABaseCharacter::RightMovementAction_Implementation(float Value)
{
	if(UGameplayTagUtils::ActorHasGameplayTag(this, GameplayTag::State::Stunned))
	{
		return;
	}
	Super::RightMovementAction_Implementation(Value);
}

void ABaseCharacter::AimAction_Implementation(bool bValue)
{
	Super::AimAction_Implementation(bValue);
}

void ABaseCharacter::JumpAction_Implementation(bool bValue)
{
	if(UGameplayTagUtils::ActorHasGameplayTag(this, GameplayTag::State::Stunned))
	{
		return;
	}
	Super::JumpAction_Implementation(bValue);
}

void ABaseCharacter::OnOverlayStateChanged(EALSOverlayState PreviousState)
{
	Super::OnOverlayStateChanged(PreviousState);
}

void ABaseCharacter::RagdollEnd()
{
	UpdateHeldObject();
	
	/** Re-enable Replicate Movement and if the host is a dedicated server set mesh visibility based anim
	tick option back to default*/

	if (UKismetSystemLibrary::IsDedicatedServer(GetWorld()))
	{
		GetMesh()->VisibilityBasedAnimTickOption = DefVisBasedTickOp;
	}

	MyCharacterMovementComponent->bIgnoreClientMovementErrorChecksAndCorrection = 0;
	SetReplicateMovement(true);

	UAnimInstance* MainAnimInstance = GetMesh()->GetAnimInstance();
	if (!MainAnimInstance)
	{
		return;
	}
	// Step 1: Save a snapshot of the current Ragdoll Pose for use in AnimGraph to blend out of the ragdoll
	MainAnimInstance->SavePoseSnapshot(TEXT("RagdollPose"));
	// Step 2: If the ragdoll is on the ground, set the movement mode to walking and play a Get Up animation.
	// If not, set the movement mode to falling and update the character movement velocity to match the last ragdoll velocity.
	if (bRagdollOnGround)
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		if(UAnimMontage* FoundMontage = GetGetUpAnimation(bRagdollFaceUp))
		{
			const float FastestBlendTime = UCombatUtils::GetKnockbackRecoveryTime(EHitReactType::Knockback_VeryLight);
			const float ThisBlendTime = FastestBlendTime - UCombatUtils::GetKnockbackRecoveryTime(LastKnownHitReact);
			FoundMontage->BlendIn.SetBlendTime(ThisBlendTime);
			MainAnimInstance->Montage_Play(FoundMontage, UCombatUtils::GetKnockbackRecoveryTime(LastKnownHitReact), EMontagePlayReturnType::MontageLength, 0.0f, true);	
		}
	}
	else
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Falling);
		GetCharacterMovement()->Velocity = LastRagdollVelocity;
	}

	// Step 3: Re-Enable capsule collision, and disable physics simulation on the mesh.
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionObjectType(ECC_Pawn);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetMesh()->SetAllBodiesSimulatePhysics(false);

	if (RagdollStateChangedDelegate.IsBound())
	{
		RagdollStateChangedDelegate.Broadcast(false);
	}
}

void ABaseCharacter::HandleCurrentWoundChangedEvent(const FCurrentWoundEventPayload& EventPayload)
{
	if(!EventPayload.bNaturalChange)
	{
		return;
	}

	LastKnownHitReact = EventPayload.DamageHitReactEvent.HitReactType;
	if(UCombatUtils::ShouldHitKnockback(LastKnownHitReact))
	{
		Internal_TryStartCharacterKnockback(EventPayload.DamageHitReactEvent); 
	} else
	{
		Internal_TryPlayHitReact(EventPayload.DamageHitReactEvent);
	}
}

void ABaseCharacter::HandleCurrentWeaponChanged(TScriptInterface<IWeapon> NewWeapon, TScriptInterface<IWeapon> PreviousWeapon)
{
	if(!NewWeapon)
		return;
	
	if(NewWeapon->GetWeaponMesh() == nullptr)
	{
		// TODO handle bare handed weapon
	} else if(const UStaticMeshComponent* CastedStaticMesh = Cast<UStaticMeshComponent>(NewWeapon->GetWeaponMesh()))
	{
		AttachToHand(CastedStaticMesh->GetStaticMesh(), nullptr, nullptr, false, FVector::ZeroVector);
	} else if(const USkeletalMeshComponent* CastedSkeletalMesh = Cast<USkeletalMeshComponent>(NewWeapon->GetWeaponMesh()))
	{
		AttachToHand(nullptr, CastedSkeletalMesh->SkeletalMesh ,nullptr, false, FVector::ZeroVector);
	}
	NewWeapon->GetWeaponMesh()->AttachToComponent(HeldObjectRoot, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	SetOverlayState(NewWeapon->GetWeaponOverlay());
}

void ABaseCharacter::HandleDeathEvent(const FDeathEventPayload& DeathEventPayload)
{
	if (IsDying())
	{
		return;
	}
	
	GameplayTagContainer.AddTag(GameplayTag::State::Dead);
	GetMesh()->SetRenderCustomDepth(false);
	Internal_TryStartCharacterKnockback(DeathEventPayload.HitReactEvent);
}

void ABaseCharacter::Internal_StopAllAnimMontages() const
{
	GetMesh()->AnimScriptInstance->Montage_Stop(0.0f);
}

float ABaseCharacter::TryPlayAnimMontage(const FAnimMontagePlayData& AnimMontageData)
{
	if (!GetCurrentMontage() && AnimMontageData.MontageToPlay)
	{
		return Internal_PlayMontage(AnimMontageData);
	}
	return -1.f;
}

float ABaseCharacter::ForcePlayAnimMontage(const FAnimMontagePlayData& AnimMontageData)
{
	if(!AnimMontageData.MontageToPlay)
	{
		return -1.f;
	}
	UAnimMontage* CurrentMontage = GetCurrentMontage();
	StopAnimMontage(CurrentMontage);
	return Internal_PlayMontage(AnimMontageData);
}

float ABaseCharacter::Internal_PlayMontage(const FAnimMontagePlayData& AnimMontagePlayData)
{
	if (!AnimMontagePlayData.MontageToPlay)
	{
		return 0.f;
	}
	AnimMontagePlayData.MontageToPlay->bEnableAutoBlendOut = AnimMontagePlayData.bShouldBlendOut;
	return PlayAnimMontage(AnimMontagePlayData.MontageToPlay, AnimMontagePlayData.PlayRate, AnimMontagePlayData.MontageSection);
}

void ABaseCharacter::Internal_ApplyCharacterKnockback(const FVector& Impulse, const float ImpulseScale, const FName BoneName, bool bVelocityChange, float KnockdownDuration)
{
	RagdollStart();
	GetMesh()->AddImpulse(Impulse * ImpulseScale, BoneName, bVelocityChange);
	GetWorldTimerManager().SetTimer(TimerHandle_Ragdoll, this, &ABaseCharacter::Internal_TryCharacterKnockbackRecovery, KnockdownDuration, false);
}

void ABaseCharacter::Internal_TryStartCharacterKnockback(const FDamageHitReactEvent& HitReactEvent)
{	
	const float ImpulseValue = UCombatUtils::GetHitImpulseValue(HitReactEvent.HitReactType);
	const float KnockdownDuration = UCombatUtils::GetKnockbackRecoveryTime(HitReactEvent.HitReactType);
	const FName HitBoneName = HitReactEvent.HitResult.BoneName;
	Internal_ApplyCharacterKnockback(HitReactEvent.HitDirection, ImpulseValue, HitBoneName, false, KnockdownDuration);	
}

void ABaseCharacter::Internal_TryCharacterKnockbackRecovery()
{
	if(IsDying())
	{
		return;
	}
	
	if (LastRagdollVelocity.Size() > 100)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_Ragdoll, this, &ABaseCharacter::Internal_TryCharacterKnockbackRecovery, 1.f, false);
	}
	else
	{
		RagdollEnd();
	}
}

void ABaseCharacter::Internal_TryPlayHitReact(const FDamageHitReactEvent& HitReactEvent)
{
	if(!HealthComponent || !HealthComponent->IsAlive())
	{
		return;
	}
	
	Internal_StopAllAnimMontages();
	FAnimMontagePlayData PlayData;
	PlayData.MontageToPlay = K2_GetHitReactAnimation(Internal_GetHitDirectionTag(HitReactEvent.HitDirection));
	PlayData.PlayRate = 1.f;
	PlayData.bShouldBlendOut = false;
	PlayData.MontageSection = FName();
	ForcePlayAnimMontage(PlayData);
}

FGameplayTag ABaseCharacter::Internal_GetHitDirectionTag(const FVector& OriginatingLocation) const
{
	const FVector& ActorLocation = GetActorLocation();
	const float DistanceToFrontBackPlane = FVector::PointPlaneDist(OriginatingLocation, ActorLocation, GetActorRightVector());
	const float DistanceToRightLeftPlane = FVector::PointPlaneDist(OriginatingLocation, ActorLocation, GetActorForwardVector());
	
	if (FMath::Abs(DistanceToFrontBackPlane) <= FMath::Abs(DistanceToRightLeftPlane))
	{
		if (DistanceToRightLeftPlane >= 0)
		{
			return GameplayTag::HitReact::Front;
		}
		return GameplayTag::HitReact::Back;
	}
	if (DistanceToFrontBackPlane >= 0)
	{
		return GameplayTag::HitReact::Right;
	}
	return GameplayTag::HitReact::Left;
}

void ABaseCharacter::Internal_AddDefaultTagsToContainer()
{
	GameplayTagContainer.AppendTags(FGameplayTagContainer::CreateFromArray(DefaultGameplayTags));
}

void ABaseCharacter::Die(FDeathEventPayload DeathEventPayload)
{
	if (GameplayTagContainer.HasTag(GameplayTag::State::Dead))
	{
		return;
	}
	
	GameplayTagContainer.AddTag(GameplayTag::State::Dead);
	
	if (DeathSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());
	}

	if(InventoryComponent)
	{
		InventoryComponent->DestroyInventory();
	}
	
	ClearHeldObject();
	Internal_StopAllAnimMontages();
	GetMesh()->SetRenderCustomDepth(false);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if(DeathAnimation)
	{
		const float DeathAnimDuration = PlayAnimMontage(DeathAnimation);
		if (DeathAnimDuration > 0.f)
		{
			const float TriggerRagdollTime = DeathAnimDuration - .3f;
			GetMesh()->bBlendPhysics = true;
			GetWorldTimerManager().SetTimer(TimerHandle_DeathRagoll, this, &ABaseCharacter::RagdollStart, FMath::Max(0.1f, TriggerRagdollTime), false);
			SetLifeSpan(10.f);
		}
		else
		{
			RagdollStart();
			SetLifeSpan(10.f);
		}
	} else
	{
		RagdollStart();
		SetLifeSpan(10.f);
	}
}

void ABaseCharacter::ReloadAction_Implementation()
{
}

void ABaseCharacter::TargetingAction_Implementation(bool bTargeting)
{
}

void ABaseCharacter::FireAction_Implementation(bool bFiring)
{
}
