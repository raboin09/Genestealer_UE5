
#include "Weapons/BaseWeapon.h"

#include "Actors/BaseWeaponPickup.h"
#include "Sound/SoundCue.h"
#include "Characters/BaseCharacter.h"
#include "Components/AudioComponent.h"
#include "Components/CapsuleComponent.h"
#include "Core/BasePlayerController.h"
#include "Genestealer/Genestealer.h"
#include "Kismet/GameplayStatics.h"
#include "Utils/CoreUtils.h"
#include "Utils/EffectUtils.h"
#include "Utils/FeedbackUtils.h"
#include "Utils/GameplayTagUtils.h"
#include "Utils/SpawnUtils.h"

ABaseWeapon::ABaseWeapon()
{
	WeaponRootComponent = CreateDefaultSubobject<USphereComponent>(TEXT("WeaponRoot"));
	WeaponRootComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	WeaponRootComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	WeaponSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponSkeletalMesh"));
	InitWeaponMesh(WeaponSkeletalMesh);
	WeaponSkeletalMesh->SetupAttachment(WeaponRootComponent);

	WeaponStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponStaticMesh"));
	InitWeaponMesh(WeaponStaticMesh);
	WeaponStaticMesh->SetupAttachment(WeaponRootComponent);

	SecondaryWeaponSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SecondaryWeaponSkeletalMesh"));
	InitWeaponMesh(SecondaryWeaponSkeletalMesh);
	SecondaryWeaponSkeletalMesh->SetupAttachment(WeaponRootComponent);

	SecondaryWeaponStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SecondaryWeaponStaticMesh"));
	InitWeaponMesh(SecondaryWeaponStaticMesh);
	SecondaryWeaponStaticMesh->SetupAttachment(WeaponRootComponent);

	DefaultGameplayTags.Add(TAG_ACTOR_WEAPON);
	
	SetRootComponent(WeaponRootComponent);
}


void ABaseWeapon::InitWeaponMesh(UMeshComponent* InMeshComp)
{
	InMeshComp->bReceivesDecals = false;
	InMeshComp->CastShadow = true;
	InMeshComp->SetCollisionObjectType(ECC_WorldDynamic);
	InMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	InMeshComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	InMeshComp->SetCollisionResponseToChannel(GENESTEALER_TRACE_WEAPON, ECR_Ignore);
	InMeshComp->SetCollisionResponseToChannel(GENESTEALER_OBJECT_TYPE_PROJECTILE, ECR_Ignore);
	InMeshComp->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
}

void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();	
}

void ABaseWeapon::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	Internal_HideMesh(true);
}

void ABaseWeapon::OnEquip(const TScriptInterface<IWeapon> LastWeapon)
{
	Internal_HideMesh(false);
	bPendingEquip = true;
	DetermineWeaponState();

	if(WeaponType == EWeaponType::Heavy)
	{
		UGameplayTagUtils::AddTagToActor(OwningPawn, TAG_STATE_CANNOT_GET_IN_COVER);
	} else
	{
		UGameplayTagUtils::RemoveTagFromActor(OwningPawn, TAG_STATE_CANNOT_GET_IN_COVER);
	}
	
	if (LastWeapon)
	{
		FAnimMontagePlayData PlayData;
		PlayData.MontageToPlay = EquipAnim;
		const float Duration = PlayWeaponAnimation(PlayData);
		if (Duration <= 0.0f)
		{
			OnEquipFinished();
		}		
		GetWorldTimerManager().SetTimer(TimerHandle_OnEquipFinished, this, &ABaseWeapon::OnEquipFinished, Duration, false);
	}
	else
	{
		OnEquipFinished();
	}

	if (IsWeaponPlayerControlled())
	{
		PlayWeaponSound(EquipSound);
	}
	K2_OnEquip();
}


bool ABaseWeapon::IsWeaponPlayerControlled() const
{
	return OwningPawn && OwningPawn->IsPlayerControlled();
}

void ABaseWeapon::OnEquipFinished()
{	
	bIsEquipped = true;
	bPendingEquip = false;
	DetermineWeaponState();
}

void ABaseWeapon::OnUnEquip()
{
	K2_OnUnEquip();
	Internal_HideMesh(true);
	bIsEquipped = false;
	StopFire();

	if (bPendingEquip)
	{
		StopWeaponAnimation(EquipAnim);
		bPendingEquip = false;

		GetWorldTimerManager().ClearTimer(TimerHandle_OnEquipFinished);
	}
	DetermineWeaponState();
}

void ABaseWeapon::StartFire()
{
	if (!bWantsToFire)
	{
		bWantsToFire = true;
		FireStartAudio = PlayWeaponSound(FireWarmupSound);
		if(FireWarmUpTime > 0.f)
		{
			GetWorldTimerManager().SetTimer(TimerHandle_FireBlendIn, this, &ABaseWeapon::DetermineWeaponState, FireWarmUpTime, false);	
		} else
		{
			DetermineWeaponState();
		}
	}
}

void ABaseWeapon::StopFire()
{
	if (bWantsToFire)
	{
		bWantsToFire = false;
		if(FireStartAudio)
		{
			FireStartAudio->FadeOut(.3f, 0.f);
			FireStartAudio = nullptr;
		}
		DetermineWeaponState();
	}
}

bool ABaseWeapon::CanFire() const
{
	bool bCanFire = true;
	if(ITaggable* OwnerTag = Cast<ITaggable>(OwningPawn))
	{
		FGameplayTagContainer TagContainer = FGameplayTagContainer();
		TagContainer.AddTag(TAG_STATE_RAGDOLL);
		TagContainer.AddTag(TAG_STATE_DEAD);
		bCanFire = !OwnerTag->GetTagContainer().HasAny(TagContainer);
	}
	const bool bStateOKToFire = (CurrentState == EWeaponState::Idle) || (CurrentState == EWeaponState::Firing);	
	return  bCanFire && bStateOKToFire ;
}

void ABaseWeapon::SetOwningPawn(ACharacter* IncomingCharacter)
{
	if (OwningPawn != IncomingCharacter)
	{
		SetInstigator(IncomingCharacter);
		OwningPawn = IncomingCharacter;
		SetOwner(IncomingCharacter);
		OwningInventory = UCoreUtils::GetInventoryComponentFromActor(IncomingCharacter);
	}
}

void ABaseWeapon::StartWeaponRagdoll(bool bSpawnPickup)
{
	Internal_StartMeshRagdoll(GetWeaponMesh());
	// TODO Alternate Mesh
}

UMeshComponent* ABaseWeapon::GetWeaponMesh() const
{
	if(WeaponSkeletalMesh && WeaponSkeletalMesh->SkeletalMesh)
	{
		return WeaponSkeletalMesh;
	}
	
	if(WeaponStaticMesh && WeaponStaticMesh->GetStaticMesh())
	{
		return WeaponStaticMesh;
	}
	return nullptr;
}

UMeshComponent* ABaseWeapon::GetSecondaryWeaponMesh() const
{
	if(SecondaryWeaponSkeletalMesh && SecondaryWeaponSkeletalMesh->SkeletalMesh)
	{
		return SecondaryWeaponSkeletalMesh;
	}
	
	if(SecondaryWeaponStaticMesh && SecondaryWeaponStaticMesh->GetStaticMesh())
	{
		return SecondaryWeaponStaticMesh;
	}
	return nullptr;
}

FTransform ABaseWeapon::GetLeftHandSocketTransform() const
{
	if(!GetWeaponMesh())
	{
		return FTransform();
	}
	return GetWeaponMesh()->GetSocketTransform(ik_hand_l_Socket, RTS_Component);
}

void ABaseWeapon::HandleFiring()
{
	if (CheckChildFireCondition() && CanFire())
	{
		SimulateWeaponFire();
		FireWeapon();
		BurstCounter++;
	}
	else if (BurstCounter > 0)
	{
		OnBurstFinished();
	}
	
	if (CurrentState == EWeaponState::Firing && TimeBetweenShots > 0.0f)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_HandleFiring, this, &ABaseWeapon::HandleFiring, TimeBetweenShots, false);
	}
	
	LastFireTime = GetWorld()->GetTimeSeconds();
}

void ABaseWeapon::OnBurstStarted()
{
	K2_OnBurstStarted();
	if (IsWeaponOnCooldown())
	{
		const float GameTime = GetWorld()->GetTimeSeconds();
		GetWorldTimerManager().SetTimer(TimerHandle_HandleFiring, this, &ABaseWeapon::HandleFiring,  LastFireTime + TimeBetweenShots - GameTime, false);
	}
	else
	{
		HandleFiring();
	}
}

void ABaseWeapon::OnBurstFinished()
{
	BurstCounter = 0;
	StopSimulatingWeaponFire();
	K2_OnBurstFinished();
	GetWorldTimerManager().ClearTimer(TimerHandle_HandleFiring);
	bRefiring = false;
}

void ABaseWeapon::PlayWeaponMissEffectFX(const FHitResult& Impact, const bool bShouldRotateHit)
{
	for(const TSubclassOf<AActor> CurrEffectClass : WeaponEffects)
	{
		if(const TScriptInterface<IEffect> TempEffect = UEffectContainerComponent::CreateEffectInstanceFromHitResult(this, CurrEffectClass, Impact, GetOwningPawn(), bShouldRotateHit))
		{
			TempEffect->PlayEffectFX();
			TempEffect->DestroyEffect();
		}
	}	
}

void ABaseWeapon::Internal_StartMeshRagdoll(UMeshComponent* InMeshComp) const
{
	if(!InMeshComp)
	{
		return;
	}
	InMeshComp->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	InMeshComp->SetCollisionProfileName("BlockAll");
	InMeshComp->SetSimulatePhysics(true);
}

void ABaseWeapon::Internal_HideMesh(bool bShouldHide)
{
	// if(GetWeaponMesh())
	// {
	// 	GetWeaponMesh()->SetHiddenInGame(bShouldHide);
	// }
}

void ABaseWeapon::ApplyWeaponEffectsToActor(const FHitResult& Impact, const bool bShouldRotateHit)
{
	if(!Impact.GetActor())
	{
		return;
	}
	const UClass* HitActorClass = Impact.GetActor()->GetClass();
	if(!HitActorClass->ImplementsInterface(UEffectible::StaticClass()))
	{
		PlayWeaponMissEffectFX(Impact, bShouldRotateHit);
	} else
	{
		UEffectUtils::ApplyEffectsToHitResult(WeaponEffects, Impact, GetOwningPawn(), bShouldRotateHit);	
	}
}

void ABaseWeapon::SetWeaponState(EWeaponState NewState)
{
	const EWeaponState PrevState = CurrentState;

	if (PrevState == EWeaponState::Firing && NewState != EWeaponState::Firing)
	{
		OnBurstFinished();
	}

	CurrentState = NewState;

	if (PrevState != EWeaponState::Firing && NewState == EWeaponState::Firing)
	{
		OnBurstStarted();
	}
}

void ABaseWeapon::DetermineWeaponState()
{
	EWeaponState NewState = EWeaponState::Idle;
	if (bIsEquipped &&  bWantsToFire && ( CanFire()))
	{	
		NewState = EWeaponState::Firing;
	}
	else if (bPendingEquip)
	{
		NewState = EWeaponState::Equipping;
	}
	SetWeaponState(NewState);
}

UAudioComponent* ABaseWeapon::PlayWeaponSound(USoundCue* Sound) const
{
	UAudioComponent* AC = nullptr;
	if (Sound && OwningPawn)
	{
		AC = UAudioManager::SpawnSoundAttached(Sound, OwningPawn->GetRootComponent());
	}
	return AC;
}

float ABaseWeapon::PlayWeaponAnimation(const FAnimMontagePlayData& PlayData) const
{	
	float Duration = 0.0f;
	if (IAnimatable* AnimationOwner = Cast<IAnimatable>(OwningPawn))
	{
		Duration = AnimationOwner->ForcePlayAnimMontage(PlayData);
	}
	return Duration;
}

void ABaseWeapon::StopWeaponAnimation(UAnimMontage* AnimMontage) const
{
	if (IAnimatable* AnimationOwner = Cast<IAnimatable>(OwningPawn))
	{
		AnimationOwner->ForceStopAnimMontage(AnimMontage);
	}
}

void ABaseWeapon::OnEnterInventory(ACharacter* NewOwner)
{
	SetOwningPawn(NewOwner);
	Internal_HideMesh(true);
	K2_OnEnterInventory();
}

void ABaseWeapon::OnLeaveInventory()
{
	StopFire();
	StartWeaponRagdoll();
	SetOwningPawn(nullptr);
}

bool ABaseWeapon::IsWeaponOnCooldown() const
{
	const float GameTime = GetWorld()->GetTimeSeconds();
	if (LastFireTime > 0 && TimeBetweenShots > 0.0f &&  LastFireTime + TimeBetweenShots > GameTime)
	{
		return true;
	}
	return false;
}
