// Copyright Epic Games, Inc. All Rights Reserved.

#include "Characters/BaseCharacter.h"

#include "Characters/EffectContainerComponent.h"
#include "Characters/HealthComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
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

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	PlayerInCombatChanged.Broadcast(false, nullptr);
	if(HealthComponent)
	{
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

void ABaseCharacter::HandleCurrentWeaponChanged(TScriptInterface<IWeapon> NewWeapon, TScriptInterface<IWeapon> PreviousWeapon)
{
	if(!NewWeapon)
		return;
	
	if(NewWeapon->GetWeaponMesh() == nullptr)
	{
		// TODO handle bare handed weapon
	} else if(UStaticMeshComponent* CastedStaticMesh = Cast<UStaticMeshComponent>(NewWeapon->GetWeaponMesh()))
	{
		AttachToHand(CastedStaticMesh->GetStaticMesh(), nullptr, nullptr, false, FVector::ZeroVector);
	} else if(USkeletalMeshComponent* CastedSkeletalMesh = Cast<USkeletalMeshComponent>(NewWeapon->GetWeaponMesh()))
	{
		AttachToHand(nullptr, CastedSkeletalMesh->SkeletalMesh ,nullptr, false, FVector::ZeroVector);
	}
	NewWeapon->GetWeaponMesh()->AttachToComponent(HeldObjectRoot, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	SetOverlayState(NewWeapon->GetWeaponOverlay());
}

void ABaseCharacter::HandleDeathEvent(const FDeathEventPayload& DeathEventPayload)
{
	
}

void ABaseCharacter::Internal_StopAllAnimMontages() const
{
	
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
			GetWorldTimerManager().SetTimer(TimerHandle, this, &ABaseCharacter::RagdollStart, FMath::Max(0.1f, TriggerRagdollTime), false);
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
