// Copyright Epic Games, Inc. All Rights Reserved.

#include "Characters/BaseCharacter.h"
#include "Camera/CameraComponent.h"
#include "Characters/EffectContainerComponent.h"
#include "Characters/HealthComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Genestealer/Genestealer.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Utils/CombatUtils.h"
#include "Utils/EffectUtils.h"
#include "Utils/GameplayTagUtils.h"

ABaseCharacter::ABaseCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	GetCapsuleComponent()->SetCollisionResponseToChannels(ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(TRACE_WEAPON, ECR_Ignore);
	
	GetMesh()->SetCollisionObjectType(ECC_Pawn);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionResponseToChannels(ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetMesh()->SetGenerateOverlapEvents(true);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	ThirdPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
	ThirdPersonCamera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	EffectContainerComponent = CreateDefaultSubobject<UEffectContainerComponent>(TEXT("EffectContainer"));
	AnimComponent = CreateDefaultSubobject<UAGRAnimMasterComponent>(TEXT("AnimationComponent"));
	CurrentAffiliation = EAffiliation::Neutral;

	BaseLookupRate = 45.f;
	BaseTurnRate = 45.f;

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
}

void ABaseCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	Internal_TraceCameraAim();
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
	
	// if(NewWeapon->GetWeaponMesh() == nullptr)
	// {
	// 	// TODO handle bare handed weapon
	// } else if(const UStaticMeshComponent* CastedStaticMesh = Cast<UStaticMeshComponent>(NewWeapon->GetWeaponMesh()))
	// {
	// 	AttachToHand(CastedStaticMesh->GetStaticMesh(), nullptr, nullptr, false, FVector::ZeroVector);
	// } else if(const USkeletalMeshComponent* CastedSkeletalMesh = Cast<USkeletalMeshComponent>(NewWeapon->GetWeaponMesh()))
	// {
	// 	AttachToHand(nullptr, CastedSkeletalMesh->SkeletalMesh ,nullptr, false, FVector::ZeroVector);
	// }
	// NewWeapon->GetWeaponMesh()->AttachToComponent(HeldObjectRoot, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	// SetOverlayState(NewWeapon->GetWeaponOverlay());
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
	// RagdollStart();
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
	
	// if (LastRagdollVelocity.Size() > 100)
	// {
	// 	GetWorldTimerManager().SetTimer(TimerHandle_Ragdoll, this, &ABaseCharacter::Internal_TryCharacterKnockbackRecovery, 1.f, false);
	// }
	// else
	// {
	// 	RagdollEnd();
	// }
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

void ABaseCharacter::Internal_TraceCameraAim()
{
	const FVector Start = ThirdPersonCamera->K2_GetComponentLocation();
	const FVector End = (ThirdPersonCamera->GetForwardVector() * 10000.f) + Start;
	FHitResult Hit(ForceInit);
	UKismetSystemLibrary::LineTraceSingle(this, Start, End,  UEngineTypes::ConvertToTraceType(ECC_Visibility), false, {}, EDrawDebugTrace::None, Hit, true, FLinearColor::Red, FLinearColor::Green, 10.f);
	CameraTraceEnd = Hit.bBlockingHit ? Hit.Location : Hit.TraceEnd; 
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
	
	// ClearHeldObject();
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
			// GetWorldTimerManager().SetTimer(TimerHandle_DeathRagoll, this, &ABaseCharacter::RagdollStart, FMath::Max(0.1f, TriggerRagdollTime), false);
			SetLifeSpan(10.f);
		}
		else
		{
			// RagdollStart();
			SetLifeSpan(10.f);
		}
	} else
	{
		// RagdollStart();
		SetLifeSpan(10.f);
	}
}

void ABaseCharacter::Input_ForwardMovement(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		MovingForwardInput = Value;
		const FRotator DirRotator(0.0f, GetControlRotation().Yaw, 0.0f);
		AddMovementInput(UKismetMathLibrary::GetForwardVector(DirRotator), Value);
	}
}

void ABaseCharacter::Input_RightMovement(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		MovingSidewaysInput = Value;
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

float ABaseCharacter::CalculateAimOffsetYaw(const float CurrentAimYaw, const float Alpha) const
{
	const FVector ControlRotation = UKismetMathLibrary::Normal(UKismetMathLibrary::GetForwardVector(GetControlRotation()));
	const FVector ForwardVector = UKismetMathLibrary::Normal(GetActorForwardVector());
	const FVector RightVector = UKismetMathLibrary::Normal(GetActorRightVector());
	const float NextAimYaw = UKismetMathLibrary::DegAcos(UKismetMathLibrary::Dot_VectorVector(ControlRotation, ForwardVector)) * UKismetMathLibrary::SignOfFloat(UKismetMathLibrary::Dot_VectorVector(ControlRotation, RightVector));
	return UKismetMathLibrary::Lerp(NextAimYaw, CurrentAimYaw, Alpha);
}

float ABaseCharacter::CalculateAimOffsetPitch(const float CurrentAimPitch) const
{
	const float NextAimPitch = UKismetMathLibrary::FindLookAtRotation(GetMesh()->GetSocketLocation("head"), CameraTraceEnd).Pitch;
	return UKismetMathLibrary::FInterpEaseInOut(CurrentAimPitch, NextAimPitch, .2f, .4f);
}

void ABaseCharacter::CalculateMovementInputScale(float& MoveForwardScale, float& MoveRightScale) const
{
	const FVector CurrentVelocity = GetVelocity();
	const float Factor = CurrentVelocity.Length() / (GetCharacterMovement()->MaxWalkSpeed * .01f) * .01f;
	MoveForwardScale = UKismetMathLibrary::Dot_VectorVector(UKismetMathLibrary::Normal(CurrentVelocity, .0001f), GetActorForwardVector()) * Factor;
	MoveRightScale = UKismetMathLibrary::Dot_VectorVector(UKismetMathLibrary::Normal(CurrentVelocity, .0001f), GetActorRightVector()) * Factor;
}

float ABaseCharacter::CalculateCurrentInputLocalAngle() const
{
	const FVector LastInputVector = GetLastMovementInputVector();
	return Internal_DotProductWithForwardAndRightVector(LastInputVector);
}

float ABaseCharacter::CalculateCurrentMovingLocalAngle(bool bLastFrame) const
{
	const FVector SelectedVelocity = bLastFrame ? GetCharacterMovement()->GetLastUpdateVelocity() : GetVelocity();
	return Internal_DotProductWithForwardAndRightVector(SelectedVelocity);
}

float ABaseCharacter::Internal_DotProductWithForwardAndRightVector(FVector InputVector) const
{
	const FVector Dot1_A = UKismetMathLibrary::Normal({ InputVector.X, InputVector.Y, 0.f }, .0001f);
	const FVector ActorForward = GetActorForwardVector();
	const FVector Dot1_B = UKismetMathLibrary::Normal({ ActorForward.X, ActorForward.Y, 0.f}, .0001f);
	const float Op_1 = UKismetMathLibrary::DegAcos(UKismetMathLibrary::Dot_VectorVector(Dot1_A, Dot1_B));
	const FVector ActorRight = GetActorRightVector();
	const FVector Dot2_B = UKismetMathLibrary::Normal({ ActorRight.X, ActorRight.Y, 0.f}, .0001f);
	const float Op_2 = UKismetMathLibrary::SignOfFloat(UKismetMathLibrary::Dot_VectorVector(Dot1_A, Dot2_B));
	return Op_1 * Op_2;
}
