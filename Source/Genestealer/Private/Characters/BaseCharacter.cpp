// Copyright Epic Games, Inc. All Rights Reserved.

#include "Characters/BaseCharacter.h"

#include "Camera/CameraComponent.h"
#include "Character/Animation/ALSPlayerCameraBehavior.h"
#include "Characters/EffectContainerComponent.h"
#include "Characters/HealthComponent.h"
#include "Characters/Animation/BaseAnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Genestealer/Genestealer.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Utils/CombatUtils.h"
#include "Utils/EffectUtils.h"
#include "Utils/GameplayTagUtils.h"


ABaseCharacter::ABaseCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	MovementModel.RowName = "Responsive";
	
	InitCapsuleCollisionDefaults();
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	
	InitMeshCollisionDefaults();
	GetMesh()->SetGenerateOverlapEvents(true);

	StaticMesh->SetCollisionResponseToChannel(TRACE_WEAPON, ECR_Ignore);
	SkeletalMesh->SetCollisionResponseToChannel(TRACE_WEAPON, ECR_Ignore);

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	EffectContainerComponent = CreateDefaultSubobject<UEffectContainerComponent>(TEXT("EffectContainer"));
	CurrentAffiliation = EAffiliation::Neutral;

	InCombatTime = 5.f;
}

void ABaseCharacter::InitCapsuleCollisionDefaults() const
{
	GetCapsuleComponent()->SetCollisionObjectType(ECC_Pawn);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(TRACE_WEAPON, ECR_Ignore);
}

void ABaseCharacter::InitMeshCollisionDefaults() const
{
	GetMesh()->SetCollisionObjectType(ECC_Pawn);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionResponseToChannels(ECR_Block); 
}

void ABaseCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ABaseCharacter::SprintAction_Implementation(bool bValue)
{
	if(IsInCover() || (InventoryComponent && InventoryComponent->GetCurrentWeaponType() == EWeaponType::Heavy))
	{
		return;
	}
	Super::SprintAction_Implementation(bValue);
}

void ABaseCharacter::RagdollEnd()
{
	Super::RagdollEnd();
	GameplayTagContainer.RemoveTag(TAG_STATE_RAGDOLL);
}

void ABaseCharacter::RagdollStart()
{
	Super::RagdollStart();
	GameplayTagContainer.AddTag(TAG_STATE_RAGDOLL);
}

void ABaseCharacter::OnOverlayStateChanged(EALSOverlayState PreviousState)
{
	Super::OnOverlayStateChanged(PreviousState);
}

bool ABaseCharacter::CanSprint() const
{
	if(IsInCover())
	{
		return false;
	}
	return Super::CanSprint();
}

EALSGait ABaseCharacter::GetAllowedGait() const
{
	if(IsInCover())
	{
		return EALSGait::Walking;
	}
	return Super::GetAllowedGait();
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	Internal_AddDefaultTagsToContainer();
	PlayerInCombatChanged.Broadcast(false, nullptr);
	if(HealthComponent)
	{
		HealthComponent->OnCurrentWoundHealthChanged().AddUObject(this, &ABaseCharacter::HandleCurrentWoundChangedEvent);
		HealthComponent->OnActorDeath().AddUObject(this, &ABaseCharacter::HandleDeathEvent);
		HealthComponent->InitHealthComponent(StartingHealth);
	}
	UEffectUtils::ApplyEffectsToActor(DefaultEffects, this);
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
		InventoryComponent->SpawnInventoryActors(StartingPrimaryWeaponClass, StartingAlternateWeaponClass);
	}
}

void ABaseCharacter::HandleCurrentWoundChangedEvent(const FCurrentWoundEventPayload& EventPayload)
{
	if(!EventPayload.bNaturalChange)
	{
		return;
	}

	if(EventPayload.DamageHitReactEvent.bOnlyHitReactOnDeath)
	{
		return;
	}

	GetWorldTimerManager().ClearTimer(TimerHandle_InCombat);
	SetInCombat(true, EventPayload.InstigatingActor);
	GetWorldTimerManager().SetTimer(TimerHandle_InCombat, this, &ABaseCharacter::Internal_SetOutOfCombat, InCombatTime, false);
	
	LastKnownHitReact = EventPayload.DamageHitReactEvent.HitReactType;
	if(UCombatUtils::ShouldHitKnockback(LastKnownHitReact))
	{
		if(!GetTagContainer().HasTag(TAG_STATE_IMMOVABLE) || !IsAlive())
		{
			Internal_TryStartCharacterKnockback(EventPayload.DamageHitReactEvent);
		}
	} else
	{
		Internal_TryPlayHitReact(EventPayload.DamageHitReactEvent);
	}
}

void ABaseCharacter::HandleCurrentWeaponChanged(TScriptInterface<IWeapon> NewWeapon, TScriptInterface<IWeapon> PreviousWeapon)
{
	if(!NewWeapon)
		return;
	
	// if(NewWeapon->GetWeaponMesh() == nullptr)
	// {
	// 	// TODO handle bare handed weapon
	// } else if(const UStaticMeshComponent* CastedStaticMesh = Cast<UStaticMeshComponent>(NewWeapon->GetWeaponMesh()))
	// {
	// 	AttachToHand(CastedStaticMesh->GetStaticMesh(), nullptr, nullptr, false, FVector::ZeroVector);
	// 	StaticMesh->SetVisibility(false);		
	// } else if(const USkeletalMeshComponent* CastedSkeletalMesh = Cast<USkeletalMeshComponent>(NewWeapon->GetWeaponMesh()))
	// {
	// 	AttachToHand(nullptr, CastedSkeletalMesh->SkeletalMesh ,nullptr, false, FVector::ZeroVector);
	// 	SkeletalMesh->SetVisibility(false);
	// }
	NewWeapon->GetWeaponMesh()->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("VB RHS_ik_hand_gun"));
	SetOverlayState(NewWeapon->GetWeaponOverlay());
	if(NewWeapon->GetWeaponType() == EWeaponType::Heavy && IsInCover())
	{
		Internal_CoverDodgeTryEnd();
	}
}

void ABaseCharacter::HandleDeathEvent(const FDeathEventPayload& DeathEventPayload)
{
	if (IsDying())
	{
		return;
	}

	if(InventoryComponent)
	{
		InventoryComponent->StopFiring();
		if(const TScriptInterface<IWeapon> CurrWeapon = InventoryComponent->GetEquippedWeapon().GetObject())
		{
			CurrWeapon->StartWeaponRagdoll();
		}
	}
	
	ClearHeldObject();
	
	GameplayTagContainer.AddTag(TAG_STATE_DEAD);
	GetMesh()->SetRenderCustomDepth(false);
	DetachFromControllerPendingDestroy();
	Internal_TryStartCharacterKnockback(DeathEventPayload.HitReactEvent, false);
	K2_OnDeath();
	SetLifeSpan(5.f);
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

void ABaseCharacter::ForceStopAnimMontage(UAnimMontage* AnimMontage)
{
	StopAnimMontage(AnimMontage);
}

float ABaseCharacter::Internal_PlayMontage(const FAnimMontagePlayData& AnimMontagePlayData)
{
	if (!AnimMontagePlayData.MontageToPlay || !GetMesh())
	{
		return 0.f;
	}
	
	AnimMontagePlayData.MontageToPlay->bEnableAutoBlendOut = AnimMontagePlayData.bShouldBlendOut;
	if(AnimMontagePlayData.bForceInPlace)
	{
		if(UBaseAnimInstance* BaseAnimInstance = Cast<UBaseAnimInstance>(GetMesh()->GetAnimInstance()))
		{
			const int32 SectionIndex = AnimMontagePlayData.MontageToPlay->GetSectionIndex(AnimMontagePlayData.MontageSection);
			BaseAnimInstance->DisableRootMotionModeForDuration(AnimMontagePlayData.MontageToPlay->GetSectionLength(SectionIndex));
		}
	}	
	return PlayAnimMontage(AnimMontagePlayData.MontageToPlay, AnimMontagePlayData.PlayRate, AnimMontagePlayData.MontageSection);
}

void ABaseCharacter::Internal_ApplyCharacterKnockback(const FVector& Impulse, const float ImpulseScale, const FName BoneName, bool bVelocityChange)
{
	if(IsInCover())
	{
		Internal_CoverDodgeTryEnd();
	}
	RagdollStart();
	GetMesh()->AddImpulse(Impulse * ImpulseScale, BoneName, bVelocityChange);
}

void ABaseCharacter::Internal_TryStartCharacterKnockback(const FDamageHitReactEvent& HitReactEvent, bool bShouldRecoverFromKnockback)
{	
	float ImpulseValue = UCombatUtils::GetHitImpulseValue(HitReactEvent.HitReactType);
	if(ImpulseValue == 0.f)
	{
		ImpulseValue = UCombatUtils::GetHitImpulseValue(EHitReactType::Knockback_VeryLight);
	}
	const float KnockdownDuration = UCombatUtils::GetKnockbackRecoveryTime(HitReactEvent.HitReactType);
	const FName HitBoneName = UCombatUtils::GetNearestValidBoneForImpact(HitReactEvent.HitResult.BoneName);
	Internal_ApplyCharacterKnockback(HitReactEvent.HitDirection, ImpulseValue, HitBoneName, false);
	if(bShouldRecoverFromKnockback)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_Ragdoll, this, &ABaseCharacter::Internal_TryCharacterKnockbackRecovery, KnockdownDuration, false);	
	}
}

void ABaseCharacter::Internal_TryCharacterKnockbackRecovery()
{
	if(!IsAlive())
	{
		return;
	}
	
	if (LastRagdollVelocity.Size() > 100)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_Ragdoll, this, &ABaseCharacter::Internal_TryCharacterKnockbackRecovery, .1f, false);
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
			return TAG_HITREACT_FRONT;
		}
		return TAG_HITREACT_BACK;
	}
	if (DistanceToFrontBackPlane >= 0)
	{
		return TAG_HITREACT_RIGHT;
	}
	return TAG_HITREACT_LEFT;
}

bool ABaseCharacter::GL_IsForwardMovementAllowed(float Value)
{
	if(UGameplayTagUtils::ActorHasAnyGameplayTags(this, {TAG_STATE_IN_COVER, TAG_STATE_STUNNED}, true))
	{
		return false;
	}
	return true;
}

bool ABaseCharacter::GL_IsRightMovementAllowed(float Value)
{
	if(Value > 0.f)
	{
		bHasRightInput = true;
	} else if(Value < 0.f)
	{
		bHasRightInput = false;
	}
	
	if(GetTagContainer().HasTag(TAG_STATE_STUNNED))
	{
		return false;
	}

	const TArray RightTagsToCheck = {TAG_COVER_RIGHTEDGE, TAG_COVER_RIGHTPEEK, TAG_COVER_LEFTPEEK};
	if(bHasRightInput && GetTagContainer().HasAny(FGameplayTagContainer::CreateFromArray(RightTagsToCheck)))
	{
		return false;
	}

	const TArray LeftTagsToCheck = {TAG_COVER_LEFTEDGE, TAG_COVER_RIGHTPEEK, TAG_COVER_LEFTPEEK};
	if(!bHasRightInput && GetTagContainer().HasAny(FGameplayTagContainer::CreateFromArray(LeftTagsToCheck)))
	{
		return false;
	}
	
	return true;
}

bool ABaseCharacter::GL_IsJumpAllowed(bool bValue)
{
	return true;
}

void ABaseCharacter::GL_HandleFireAction(bool bValue)
{
	if(!InventoryComponent)
	{
		return;
	}

	if(bValue)
	{
		if(IsRagdoll())
		{
			return;
		}
		
		if(CurrentCoverPoint && IsInCover())
		{
			CurrentCoverPoint->StartCoverFire();
		} else
		{
			InventoryComponent->StartFiring();
			if(InventoryComponent->DoesCurrentWeaponForceAimOnFire())
			{
				SetRotationMode(EALSRotationMode::Aiming, true, false);
			}
		}
		
		GameplayTagContainer.AddTag(TAG_STATE_FIRING);
		GetWorldTimerManager().ClearTimer(TimerHandle_InCombat);
		if(!GetTagContainer().HasTag(TAG_STATE_IN_COMBAT))
		{
			SetInCombat(true, nullptr);
		}
	} else
	{		
		if(CurrentCoverPoint && IsInCover())
		{
			CurrentCoverPoint->StopCoverFire();
		} else
		{
			InventoryComponent->StopFiring();
			if(!IsAiming())
			{
				SetRotationMode(EALSRotationMode::LookingDirection, true, true);
			}
		}
		
		GameplayTagContainer.RemoveTag(TAG_STATE_FIRING);
		if(GetTagContainer().HasTag(TAG_STATE_IN_COMBAT))
		{
			GetWorldTimerManager().SetTimer(TimerHandle_InCombat, this, &ABaseCharacter::Internal_SetOutOfCombat, InCombatTime, false);
		}
	}
}

void ABaseCharacter::AimAction_Implementation(bool bValue)
{
	if(bValue)
	{
		GameplayTagContainer.AddTag(TAG_STATE_AIMING);
		if(IsInCover() && CurrentCoverPoint)
		{
			CurrentCoverPoint->StartCoverAim();
		} else
		{
			SetRotationMode(EALSRotationMode::Aiming, true, true);
		}
	} else
	{
		GameplayTagContainer.RemoveTag(TAG_STATE_AIMING);
		if(IsFiring() && !IsInCover())
		{
			SetRotationMode(EALSRotationMode::Aiming, true, true);
			if(CameraBehavior)
			{
				CameraBehavior->SetRotationMode(EALSRotationMode::LookingDirection);
			}
			return;
		}
		
		if(IsInCover() && CurrentCoverPoint)
		{
			CurrentCoverPoint->StopCoverAim();
		} else
		{
			SetRotationMode(EALSRotationMode::LookingDirection);
		}
	}
}

void ABaseCharacter::GL_HandleCoverDodgeAction()
{
	if(!Internal_CanGetInCover())
	{
		return;
	}
	
	if(!CurrentCoverPoint)
	{
		Internal_CoverDodgeTryStart();
	} else
	{
		Internal_CoverDodgeTryEnd();
	}
}

void ABaseCharacter::Internal_CoverDodgeTryStart()
{
	
	if(!Controller)
	{
		return;
	}
	FVector CamLoc;
	FRotator CamRot;
	Controller->GetPlayerViewPoint(CamLoc, CamRot);
	const FVector StartTrace = CamLoc;
	const FVector EndTrace = StartTrace + (CamRot.Vector() * 800.f);
	const TArray<AActor*> IgnoreActors;
	FHitResult HitResult;
	UKismetSystemLibrary::LineTraceSingle(this, StartTrace, EndTrace, UEngineTypes::ConvertToTraceType(TRACE_COVER_WALL), true, IgnoreActors, EDrawDebugTrace::ForDuration, HitResult, true);
	if(!HitResult.bBlockingHit)
	{
		return;
	}
	
	if(ICoverPoint* CoverPoint = Cast<ICoverPoint>(HitResult.GetActor()))
	{
		CoverPoint->OccupyCover(this, HitResult.ImpactPoint, HitResult.ImpactNormal);
		TScriptInterface<ICoverPoint> NewCover;
		NewCover.SetObject(HitResult.GetActor());
		NewCover.SetInterface(CoverPoint);
		CurrentCoverPoint = NewCover;
	}	
}

void ABaseCharacter::Internal_CoverDodgeTryEnd()
{
	if(CurrentCoverPoint)
	{
		CurrentCoverPoint->VacateCover(this);	
	}
	CurrentCoverPoint = nullptr;
}

void ABaseCharacter::PlayerForwardMovementInput(float Value)
{
	if(UGameplayTagUtils::ActorHasAnyGameplayTags(this, {TAG_STATE_IN_COVER}, true))
	{
		return;
	}
	ForwardMovementAction(Value);
}

void ABaseCharacter::PlayerRightMovementInput(float Value)
{
	if(UGameplayTagUtils::ActorHasAnyGameplayTags(this, {TAG_STATE_IN_COVER}, true))
	{
		return;
	}
	RightMovementAction(Value);
}

void ABaseCharacter::Internal_AddDefaultTagsToContainer()
{
	GameplayTagContainer.AppendTags(FGameplayTagContainer::CreateFromArray(DefaultGameplayTags));
}

void ABaseCharacter::Internal_SetOutOfCombat()
{
	GameplayTagContainer.RemoveTag(TAG_STATE_IN_COMBAT);
}

bool ABaseCharacter::IsInCombat()
{
	bool bWeaponIsActive = false;
	if(InventoryComponent)
	{
		const EWeaponState CurrWeaponState = InventoryComponent->GetCurrentWeaponState();
		bWeaponIsActive = CurrWeaponState == EWeaponState::Firing || CurrWeaponState == EWeaponState::Reloading;
	}
	return bWeaponIsActive || GetTagContainer().HasTag(TAG_STATE_IN_COMBAT);
}

void ABaseCharacter::SetInCombat(bool bInNewState, AActor* DamageCauser)
{
	if(bInNewState)
	{
		GetTagContainer().AddTag(TAG_STATE_IN_COMBAT);
	} else
	{
		GetTagContainer().RemoveTag(TAG_STATE_IN_COMBAT);
	}
	
	if(IsAlive())
	{
		PlayerInCombatChanged.Broadcast(bInNewState, DamageCauser);
	}
}