// Copyright Epic Games, Inc. All Rights Reserved.

#include "Characters/BaseCharacter.h"

#include "Actors/BaseCoverPoint.h"
#include "Camera/CameraComponent.h"
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
	InitCapsuleCollisionDefaults();
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	
	InitMeshCollisionDefaults();
	GetMesh()->SetGenerateOverlapEvents(true);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->TargetArmLength = 200.f;
	SpringArm->SocketOffset = FVector(0.f, 75.f, 85.f);
	SpringArm->SetupAttachment(RootComponent);
	ThirdPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
	ThirdPersonCamera->FieldOfView = 70.f;
	ThirdPersonCamera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	EffectContainerComponent = CreateDefaultSubobject<UEffectContainerComponent>(TEXT("EffectContainer"));
	AnimComponent = CreateDefaultSubobject<UAGRAnimMasterComponent>(TEXT("AnimationComponent"));
	CurrentAffiliation = EAffiliation::Neutral;

	BaseLookupRate = 45.f;
	BaseTurnRate = 45.f;
	InCombatTime = 5.f;
	
	InitAGRDefaults();
}

void ABaseCharacter::InitAGRDefaults()
{
	// AGR Defaults
	bUseControllerRotationYaw = false;
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	NetPriority = 4.0f;
	UCharacterMovementComponent* CharacterMovementComponent = GetCharacterMovement();
	CharacterMovementComponent->PerchRadiusThreshold = 40.0f;
	CharacterMovementComponent->PerchAdditionalHeight = 25.0f;
	CharacterMovementComponent->NavAgentProps.bCanFly = true;
	CharacterMovementComponent->NavAgentProps.bCanCrouch = true;
	CharacterMovementComponent->bCanWalkOffLedgesWhenCrouching = true;
	USkeletalMeshComponent* MeshComponent = GetMesh();
	MeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -88.0f));
	MeshComponent->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	MeshComponent->bEnableUpdateRateOptimizations = true;
	MeshComponent->bPropagateCurvesToSlaves = true;
	MeshComponent->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	AnimComponent->SetupBasePose(TAG_BASEPOSE_MOCAP_UNARMED);
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
		InventoryComponent->SpawnInventoryActors(StartingPistolClass, StartingRifleClass, StartingMeleeClass);
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

	AnimComponent->SetupBasePose(NewWeapon->GetWeaponBasePose());
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
	}
	
	GameplayTagContainer.AddTag(TAG_STATE_DEAD);
	GetMesh()->SetRenderCustomDepth(false);
	DetachFromControllerPendingDestroy();
	Internal_TryStartCharacterKnockback(DeathEventPayload.HitReactEvent, false);
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
	if (!AnimMontagePlayData.MontageToPlay)
	{
		return 0.f;
	}
	AnimMontagePlayData.MontageToPlay->bEnableAutoBlendOut = AnimMontagePlayData.bShouldBlendOut;
	return PlayAnimMontage(AnimMontagePlayData.MontageToPlay, AnimMontagePlayData.PlayRate, AnimMontagePlayData.MontageSection);
}

void ABaseCharacter::Internal_ApplyCharacterKnockback(const FVector& Impulse, const float ImpulseScale, const FName BoneName, bool bVelocityChange)
{
	Internal_StartRagdoll();
	GetMesh()->AddImpulse(Impulse * ImpulseScale, BoneName, bVelocityChange);
}

void ABaseCharacter::Internal_TryStartCharacterKnockback(const FDamageHitReactEvent& HitReactEvent, bool bShouldRecoverFromKnockback)
{	
	const float ImpulseValue = UCombatUtils::GetHitImpulseValue(HitReactEvent.HitReactType);
	const float KnockdownDuration = UCombatUtils::GetKnockbackRecoveryTime(HitReactEvent.HitReactType);
	const FName HitBoneName = HitReactEvent.HitResult.BoneName;
	Internal_ApplyCharacterKnockback(HitReactEvent.HitDirection, ImpulseValue, HitBoneName, false);
	if(bShouldRecoverFromKnockback)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_Ragdoll, this, &ABaseCharacter::Internal_TryCharacterKnockbackRecovery, KnockdownDuration, false);	
	}
}

void ABaseCharacter::Internal_TryCharacterKnockbackRecovery()
{
	if (LastRagdollVelocity.Size() > 100)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_Ragdoll, this, &ABaseCharacter::Internal_TryCharacterKnockbackRecovery, 1.f, false);
	}
	else
	{
		Internal_EndRagdoll();
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

void ABaseCharacter::Internal_AddDefaultTagsToContainer()
{
	GameplayTagContainer.AppendTags(FGameplayTagContainer::CreateFromArray(DefaultGameplayTags));
}

void ABaseCharacter::Internal_CoverAnimState() const
{
	AnimComponent->SetupAimOffset(EAGR_AimOffsets::NONE, EAGR_AimOffsetClamp::Left, 90.f, true);
	AnimComponent->SetupRotation(EAGR_RotationMethod::NONE, 180.f, 90.f, 5.f);
}

void ABaseCharacter::Internal_AimingAnimState() const
{
	AnimComponent->SetupAimOffset(EAGR_AimOffsets::Aim, EAGR_AimOffsetClamp::Nearest, 90.f, false);
	AnimComponent->SetupRotation(EAGR_RotationMethod::DesiredAtAngle, 360.f, 90.f, 5.f);
}

void ABaseCharacter::Internal_NormalAnimState() const
{
	AnimComponent->SetupAimOffset(EAGR_AimOffsets::Look, EAGR_AimOffsetClamp::Left, 90.f, true);
	AnimComponent->SetupRotation(EAGR_RotationMethod::RotateToVelocity, 180.f, 90.f, 5.f);
}

void ABaseCharacter::Internal_RemoveReadyState()
{
	GameplayTagContainer.RemoveTag(TAG_STATE_READY);
}

void ABaseCharacter::Internal_SetOutOfCombat()
{
	GameplayTagContainer.RemoveTag(TAG_STATE_IN_COMBAT);
}

void ABaseCharacter::Internal_StartRagdoll()
{
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	GetCharacterMovement()->SetMovementMode(MOVE_None);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionObjectType(ECC_PhysicsBody);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetAllBodiesBelowSimulatePhysics("pelvis", true, true);
	GetMesh()->bOnlyAllowAutonomousTickPose = true;
	GameplayTagContainer.AddTag(TAG_STATE_RAGDOLL);
}

void ABaseCharacter::Internal_EndRagdoll()
{
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	GetMesh()->bOnlyAllowAutonomousTickPose = false;
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionObjectType(ECC_Pawn);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetMesh()->SetAllBodiesSimulatePhysics(false);
	GetCapsuleComponent()->SetWorldLocation(GetMesh()->GetComponentLocation());
	GameplayTagContainer.RemoveTag(TAG_STATE_RAGDOLL);
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

void ABaseCharacter::Input_ForwardMovement(float Value)
{
	if(Value == 0.f)
	{
		return;
	}	
	if(UGameplayTagUtils::ActorHasAnyGameplayTags(this, {TAG_STATE_IN_COVER}, true))
	{
		return;
	}
	
	if (Controller != nullptr)
	{
		const FRotator DirRotator(0.0f, GetControlRotation().Yaw, 0.0f);
		AddMovementInput(UKismetMathLibrary::GetForwardVector(DirRotator), Value);
	}
}

void ABaseCharacter::Input_RightMovement(float Value)
{
	if(Value == 0.f)
	{
		return;
	}
	
	if(Value > 0.f && UGameplayTagUtils::ActorHasGameplayTag(this, TAG_COVER_RIGHTEDGE))
	{
		return;
	}
	
	if(Value < 0.f && UGameplayTagUtils::ActorHasGameplayTag(this, TAG_COVER_LEFTEDGE))
	{
		return;
	}
	
	if (Controller != nullptr)
	{
		if(Value > 0.f)
		{
			bHasRightInput = true;
		} else if(Value < 0.f)
		{
			bHasRightInput = false;
		}
		const FRotator DirRotator(0.0f, GetControlRotation().Yaw, 0.0f);
		AddMovementInput(UKismetMathLibrary::GetRightVector(DirRotator), Value);
	}
}

void ABaseCharacter::Input_CameraUp(float Value)
{
	AddControllerPitchInput(Value * BaseTurnRate * UGameplayStatics::GetWorldDeltaSeconds(this));
}

void ABaseCharacter::Input_CameraRight(float Value)
{
	AddControllerYawInput(Value * BaseLookupRate * UGameplayStatics::GetWorldDeltaSeconds(this));
}

void ABaseCharacter::Input_Fire()
{
	if(!InventoryComponent)
	{
		return;
	}

	if(!GameplayTagContainer.HasTag(TAG_STATE_FIRING))
	{
		GameplayTagContainer.AddTag(TAG_STATE_FIRING);
		InventoryComponent->StartFiring();
		GetWorldTimerManager().ClearTimer(TimerHandle_InCombat);
		if(!GetTagContainer().HasTag(TAG_STATE_IN_COMBAT))
		{
			SetInCombat(true, nullptr);
		}
	} else
	{
		GameplayTagContainer.RemoveTag(TAG_STATE_FIRING);
		GetWorldTimerManager().SetTimer(TimerHandle_InCombat, this, &ABaseCharacter::Internal_RemoveReadyState, 2.f, false);
		InventoryComponent->StopFiring();
		if(GetTagContainer().HasTag(TAG_STATE_IN_COMBAT))
		{
			GetWorldTimerManager().SetTimer(TimerHandle_InCombat, this, &ABaseCharacter::Internal_SetOutOfCombat, InCombatTime, false);
		}
	}
}

void ABaseCharacter::Input_CoverAction()
{
	if(!CurrentCoverPoint)
	{
		const FVector StartTrace = ThirdPersonCamera->GetComponentLocation();
		const FVector EndTrace = StartTrace + (ThirdPersonCamera->GetForwardVector() * 800.f);
		const TArray<AActor*> IgnoreActors;
		FHitResult HitResult;
		UKismetSystemLibrary::LineTraceSingle(this, StartTrace, EndTrace, UEngineTypes::ConvertToTraceType(TRACE_COVER_WALL), true, IgnoreActors, EDrawDebugTrace::None, HitResult, true);
		if(!HitResult.bBlockingHit)
		{
			return;
		}

		if(ICoverPoint* CoverPoint = Cast<ICoverPoint>(HitResult.GetActor()))
		{
			CoverPoint->OccupyCover(this, HitResult.ImpactPoint, HitResult.ImpactNormal);
			Internal_CoverAnimState();
			TScriptInterface<ICoverPoint> NewCover;
			NewCover.SetObject(HitResult.GetActor());
			NewCover.SetInterface(CoverPoint);
			CurrentCoverPoint = NewCover;
		}
	} else
	{
		CurrentCoverPoint->VacateCover(this);
		CurrentCoverPoint = nullptr;
		Internal_NormalAnimState();
	}
}

void ABaseCharacter::SetAimOffset(EAGR_AimOffsets InOffset)
{
	if(!AnimComponent)
	{
		return;
	}
	AnimComponent->SetupAimOffset(InOffset);
}

void ABaseCharacter::Input_Aim()
{
	if(GameplayTagContainer.HasTag(TAG_STATE_AIMING))
	{
		GameplayTagContainer.RemoveTag(TAG_STATE_AIMING);
		Internal_NormalAnimState();
		K2_StopAiming();
	} else
	{
		GameplayTagContainer.AddTag(TAG_STATE_AIMING);
		Internal_AimingAnimState();
		K2_Aim();
	}
}