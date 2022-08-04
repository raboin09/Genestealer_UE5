// Copyright Epic Games, Inc. All Rights Reserved.

#include "Characters/BaseCharacter.h"

#include "API/Activatable.h"
#include "Camera/CameraComponent.h"
#include "Characters/EffectContainerComponent.h"
#include "Characters/HealthComponent.h"
#include "Characters/Animation/BaseAnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Genestealer/Genestealer.h"
#include "Kismet/GameplayStatics.h"
#include "Utils/CombatUtils.h"
#include "Utils/CoreUtils.h"
#include "Utils/EffectUtils.h"
#include "Utils/GameplayTagUtils.h"


ABaseCharacter::ABaseCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	MovementModel.DataTable = LoadObject<UDataTable>(this , TEXT("DataTable'/Game/_Genestealer/Data/Genestealer_MovementModel.Genestealer_MovementModel'"));
	MovementModel.RowName = "Responsive";
	
	InitCapsuleCollisionDefaults();
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	
	InitMeshCollisionDefaults();
	GetMesh()->SetGenerateOverlapEvents(true);

	StaticMesh->SetCollisionResponseToChannel(GENESTEALER_TRACE_WEAPON, ECR_Ignore);
	SkeletalMesh->SetCollisionResponseToChannel(GENESTEALER_TRACE_WEAPON, ECR_Ignore);

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
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(GENESTEALER_TRACE_WEAPON, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(GENESTEALER_TRACE_INTERACTION, ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(GENESTEALER_OBJECT_TYPE_PROJECTILE, ECR_Ignore);
}

void ABaseCharacter::InitMeshCollisionDefaults() const
{
	GetMesh()->SetCollisionObjectType(ECC_Pawn);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionResponseToChannels(ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(GENESTEALER_TRACE_WEAPON, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(GENESTEALER_OBJECT_TYPE_PROJECTILE, ECR_Block);
}

void ABaseCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ABaseCharacter::RagdollEnd()
{
	Super::RagdollEnd();
	UGameplayTagUtils::RemoveTagFromActor(this, TAG_STATE_RAGDOLL);
	InitCapsuleCollisionDefaults();
	InitMeshCollisionDefaults();
}

void ABaseCharacter::RagdollStart()
{
	Super::RagdollStart();
	UGameplayTagUtils::AddTagToActor(this, TAG_STATE_RAGDOLL);
}

void ABaseCharacter::K2_HandleTagRemoved_Implementation(const FGameplayTag& AddedTag)
{
}

void ABaseCharacter::K2_HandleTagAdded_Implementation(const FGameplayTag& AddedTag)
{
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	Internal_AddDefaultTagsToContainer();
	CharacterInCombatChanged.Broadcast(FCharacterInCombatChangedPayload(false, nullptr));
	if(HealthComponent)
	{
		HealthComponent->OnCurrentWoundHealthChanged().AddDynamic(this, &ABaseCharacter::HandleCurrentWoundChangedEvent);
		HealthComponent->OnActorDeath().AddDynamic(this, &ABaseCharacter::HandleDeathEvent);
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
		InventoryComponent->OnNewWeaponAdded().AddDynamic(this, &ABaseCharacter::HandleNewWeaponAddedEvent);
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

	if(!IsPlayerControlled())
	{
		UPlayerStatsComponent::RecordStatsEvent(this, DamageGiven, EventPayload.Delta, this);
	} else
	{
		UPlayerStatsComponent::RecordStatsEvent(this, DamageGiven, EventPayload.Delta);
	}

	GetWorldTimerManager().ClearTimer(TimerHandle_InCombat);
	SetInCombat(true, EventPayload.InstigatingActor);
	GetWorldTimerManager().SetTimer(TimerHandle_InCombat, this, &ABaseCharacter::Internal_SetOutOfCombat, InCombatTime, false);
	
	if(IsAlive())
	{
		const auto HitReactType = EventPayload.DamageHitReactEvent.HitReactType; 
		if(IsPlayerControlled() && GetCurrentPlayingMontage())
		{
			if(HitReactType == EHitReactType::Knockback_VeryLight
				|| HitReactType == EHitReactType::Knockback_Tiny
				|| HitReactType == EHitReactType::Knockback_Light
				|| HitReactType == EHitReactType::HitReact_Light)
			{
				LastKnownHitReact = EHitReactType::None;
			}
		} else
		{
			LastKnownHitReact = HitReactType;	
		}
	} else
	{
		if(EventPayload.DamageHitReactEvent.DeathReactType == EHitReactType::None)
		{
			LastKnownHitReact = EventPayload.DamageHitReactEvent.DeathReactType == EHitReactType::None ? EventPayload.DamageHitReactEvent.HitReactType : EventPayload.DamageHitReactEvent.DeathReactType;
		}
	}
	
	if(LastKnownHitReact == EHitReactType::None)
	{
		return;
	}

	if(UGameplayTagUtils::ActorHasGameplayTag(this, TAG_STATE_IMMOVABLE) || !IsAlive())
	{
		return;
	}

	if(InventoryComponent)
	{
		if(IActivatable* Activatable = Cast<IActivatable>(InventoryComponent->GetEquippedWeapon().GetObject()))
		{
			Activatable->ResetActivatable();
		}
	}
	
	if(UCombatUtils::ShouldHitKnockback(LastKnownHitReact))
	{
		Internal_TryStartCharacterKnockback(EventPayload.DamageHitReactEvent);
	} else
	{
		Internal_TryPlayHitReact(EventPayload.DamageHitReactEvent);
	}
}

void ABaseCharacter::HandleNewWeaponAddedEvent(const FNewWeaponAddedPayload& EventPayload)
{
	const TScriptInterface<IWeapon> AddedWeapon = EventPayload.AddedWeapon;
	if(AddedWeapon && AddedWeapon->GetWeaponMesh())
	{
		FName SocketAttach = "WeaponBack_R";
		if(AddedWeapon->GetWeaponType() == EWeaponType::Pistol)
		{
			SocketAttach = "WeaponSide_R";
		} else if(AddedWeapon->GetWeaponType() == EWeaponType::Melee)
		{
			SocketAttach = "WeaponBack_L";
		}
		AddedWeapon->GetWeaponMesh()->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, SocketAttach);
	}
}

void ABaseCharacter::HandleCurrentWeaponChanged(const FCurrentWeaponChangedPayload& CurrentWeaponChangedPayload)
{
	const TScriptInterface<IWeapon> OldWeapon = CurrentWeaponChangedPayload.PreviousWeapon;
	if(OldWeapon && OldWeapon->GetWeaponMesh())
	{
		FName SocketAttach = "WeaponBack_R";
		if(OldWeapon->GetWeaponType() == EWeaponType::Pistol)
		{
			SocketAttach = "WeaponSide_L";
		} else if(OldWeapon->GetWeaponType() == EWeaponType::Melee)
		{
			SocketAttach = "WeaponBack_L";
		}
		OldWeapon->GetWeaponMesh()->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, SocketAttach);
	}
	
	const TScriptInterface<IWeapon> NewWeapon = CurrentWeaponChangedPayload.NewWeapon;
	if(!NewWeapon || !NewWeapon->GetWeaponMesh())
		return;

	auto SocketName = "VB RHS_ik_hand_gun";
	if(NewWeapon->GetWeaponType() == EWeaponType::Melee)
	{
		SocketName = "hand_r";	
	}
	NewWeapon->GetWeaponMesh()->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, SocketName);
	SetOverlayState(NewWeapon->GetWeaponOverlay());
	if(NewWeapon->GetWeaponType() == EWeaponType::Heavy && IsInCover())
	{
		Internal_CoverDodgeTryEnd();
	}
}

void ABaseCharacter::HandleDeathEvent(const FActorDeathEventPayload& DeathEventPayload)
{
	if (!IsAlive())
	{
		return;
	}
	
	ClearHeldObject();
	
	UGameplayTagUtils::AddTagToActor(this, TAG_STATE_DEAD);
	GetMesh()->SetRenderCustomDepth(false);
	DetachFromControllerPendingDestroy();
	if(UGameplayTagUtils::ActorHasGameplayTag(this, TAG_STATE_IMMOVABLE))
	{
		FDamageHitReactEvent NewEvent = DeathEventPayload.HitReactEvent;
		NewEvent.HitReactType = EHitReactType::Knockback_VeryLight;
		Internal_TryStartCharacterKnockback(NewEvent, false);
	} else
	{
		Internal_TryStartCharacterKnockback(DeathEventPayload.HitReactEvent, false);
	}
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
	if(IsMounted())
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
	PlayData.MontageSection = FName();
	PlayData.PlayRate = 1.f;
	
	if(HitReactEvent.HitReactType == EHitReactType::HitReact_Chainsaw || HitReactEvent.DeathReactType == EHitReactType::HitReact_Chainsaw)
	{
		PlayData.MontageToPlay = K2_GetHitReactAnimation(TAG_HITREACT_CHAINSAW);
		PlayData.bShouldBlendOut = false;
	} else
	{
		PlayData.MontageToPlay = K2_GetHitReactAnimation(Internal_GetHitDirectionTag(HitReactEvent.HitDirection));
		PlayData.bShouldBlendOut = true;
	}
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
		UGameplayTagUtils::AddTagToActor(this, TAG_INPUT_RIGHT_MOVEMENT);
		UGameplayTagUtils::RemoveTagFromActor(this, TAG_INPUT_LEFT_MOVEMENT);
	} else if(Value < 0.f)
	{
		UGameplayTagUtils::AddTagToActor(this, TAG_INPUT_LEFT_MOVEMENT);
		UGameplayTagUtils::RemoveTagFromActor(this, TAG_INPUT_RIGHT_MOVEMENT);
	} else
	{
		UGameplayTagUtils::RemoveTagFromActor(this, TAG_INPUT_LEFT_MOVEMENT);
		UGameplayTagUtils::RemoveTagFromActor(this, TAG_INPUT_RIGHT_MOVEMENT);
	}

	const TArray BaseTagsToCheck = {TAG_COVER_ROLLEDOUT, TAG_STATE_STUNNED};
	if(GetTagContainer().HasAny(FGameplayTagContainer::CreateFromArray(BaseTagsToCheck)))
	{
		return false;
	}

	const TArray RightTagsToCheck = {TAG_COVER_RIGHTEDGE};
	if(GetTagContainer().HasTag(TAG_INPUT_RIGHT_MOVEMENT) && GetTagContainer().HasAny(FGameplayTagContainer::CreateFromArray(RightTagsToCheck)))
	{
		return false;
	}

	const TArray LeftTagsToCheck = {TAG_COVER_LEFTEDGE};
	if(GetTagContainer().HasTag(TAG_INPUT_LEFT_MOVEMENT) && GetTagContainer().HasAny(FGameplayTagContainer::CreateFromArray(LeftTagsToCheck)))
	{
		return false;
	}
	
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
		
		if(CurrentMount && IsMounted())
		{
			CurrentMount->StartMountedFire();
		} else
		{
			InventoryComponent->StartFiring();
			if(InventoryComponent->DoesCurrentWeaponForceAimOnFire())
			{
				SetAimingMode(true, false);
			}
		}
		
		UGameplayTagUtils::AddTagToActor(this, TAG_STATE_FIRING);
		GetWorldTimerManager().ClearTimer(TimerHandle_InCombat);
		if(!GetTagContainer().HasTag(TAG_STATE_IN_COMBAT))
		{
			SetInCombat(true, nullptr);
		}
	} else
	{		
		if(CurrentMount && IsMounted())
		{
			CurrentMount->StopMountedFire();
		} else
		{
			InventoryComponent->StopFiring();
			if(!IsAiming())
			{
				SetLookingMode(true, true);
			}
		}
		
		UGameplayTagUtils::RemoveTagFromActor(this, TAG_STATE_FIRING);
		if(GetTagContainer().HasTag(TAG_STATE_IN_COMBAT))
		{
			GetWorldTimerManager().SetTimer(TimerHandle_InCombat, this, &ABaseCharacter::Internal_SetOutOfCombat, InCombatTime, false);
		}
	}
}

void ABaseCharacter::GL_HandleCoverDodgeAction()
{
	if(!CanGetInCover())
	{
		return;
	}
	
	if(!CurrentMount)
	{
		Internal_CoverDodgeTryStart();
	} else
	{
		Internal_CoverDodgeTryEnd();
	}
}

void ABaseCharacter::GL_HandleAimAction(bool bValue)
{
	if(bValue)
	{
	    if(!InventoryComponent || !InventoryComponent->CanWeaponAim()){
	        return;
	    }
	    
		UGameplayTagUtils::AddTagToActor(this, TAG_STATE_AIMING);
		if(IsMounted())
		{
			CurrentMount->StartMountedAim();
		} else
		{
			SetAimingMode(true, true);
		}
	} else
	{
		UGameplayTagUtils::RemoveTagFromActor(this, TAG_STATE_AIMING);
		if(IsFiring() && !IsMounted())
		{
			SetAimingMode(true, true);
			SetCameraLookingMode();
			return;
		}
		
		if(IsMounted() && CurrentMount)
		{
			CurrentMount->StopMountedAim();
		} else
		{
			SetLookingMode();
		}
	}
}

void ABaseCharacter::GL_HandleSprintAction(bool bValue)
{
	if(IsMounted() || (InventoryComponent && InventoryComponent->GetCurrentWeaponType() == EWeaponType::Heavy))
	{
		return;
	}

	if (bValue)
	{
		SetSprintingGait();
	}
	else
	{
		SetRunningGait();
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
	const FVector EndTrace = StartTrace + (CamRot.Vector() * UCoreUtils::GetCoverPointValidDistance());
	const TArray<AActor*> IgnoreActors;
	FHitResult HitResult;
	UKismetSystemLibrary::SphereTraceSingle(this, StartTrace, EndTrace, UCoreUtils::GetPlayerControllerSphereTraceRadius(this), UEngineTypes::ConvertToTraceType(GENESTEALER_TRACE_COVER_WALL), true, IgnoreActors, EDrawDebugTrace::None, HitResult, true);
	if(!HitResult.bBlockingHit)
	{
		return;
	}

	if(!UGameplayTagUtils::ActorHasGameplayTag(HitResult.GetActor(), TAG_ACTOR_COVER))
	{
		return;
	}

	// We want to ignore it if the component hit is a different side than the invisible cover middle wall
	if(UGameplayTagUtils::ComponentHasNameTag(HitResult.GetComponent(), TAG_NAME_COVERMESH))
	{
		return;
	}
	
	if(IMountable* CoverPoint = Cast<IMountable>(HitResult.GetActor()))
	{
		CoverPoint->OccupyMount(this, FVector::ZeroVector, HitResult.ImpactNormal);
		TScriptInterface<IMountable> NewCover;
		NewCover.SetObject(HitResult.GetActor());
		NewCover.SetInterface(CoverPoint);
		CurrentMount = NewCover;
	}
}

void ABaseCharacter::Internal_CoverDodgeTryEnd()
{
	if(CurrentMount && IsInCover())
	{
		CurrentMount->VacateMount(this);	
	}
	CurrentMount = nullptr;
}

void ABaseCharacter::Internal_AddDefaultTagsToContainer()
{
	UGameplayTagUtils::AddTagsToActor(this, DefaultGameplayTags);
}

void ABaseCharacter::Internal_SetOutOfCombat()
{
	SetInCombat(false, nullptr);
}

void ABaseCharacter::HandleTagChanged(const FGameplayTag& ChangedTag, bool bAdded)
{
	if(bAdded)
	{
		K2_HandleTagAdded(ChangedTag);
	}
	else
	{
		K2_HandleTagRemoved(ChangedTag);
	}
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
		UGameplayTagUtils::AddTagToActor(this, TAG_STATE_IN_COMBAT);
	} else
	{
		UGameplayTagUtils::RemoveTagFromActor(this, TAG_STATE_IN_COMBAT);
	}
	
	if(IsAlive())
	{
		CharacterInCombatChanged.Broadcast(FCharacterInCombatChangedPayload(bInNewState, DamageCauser));
	}
}
