
#include "Weapons/BaseWeapon.h"
#include "Sound/SoundCue.h"
#include "Characters/BaseCharacter.h"
#include "Components/AudioComponent.h"
#include "Core/BasePlayerController.h"
#include "Genestealer/Genestealer.h"
#include "Kismet/GameplayStatics.h"
#include "Utils/CoreUtils.h"
#include "Utils/EffectUtils.h"
#include "Utils/FeedbackUtils.h"
#include "Utils/GameplayTagUtils.h"

ABaseWeapon::ABaseWeapon()
{
	WeaponRootComponent = CreateDefaultSubobject<USphereComponent>(TEXT("WeaponRoot"));
	
	WeaponSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponSkeletalMesh"));
	WeaponSkeletalMesh->bReceivesDecals = false;
	WeaponSkeletalMesh->CastShadow = true;
	WeaponSkeletalMesh->SetCollisionObjectType(ECC_WorldDynamic);
	WeaponSkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponSkeletalMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	WeaponSkeletalMesh->SetCollisionResponseToChannel(TRACE_WEAPON, ECR_Ignore);
	WeaponSkeletalMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	WeaponSkeletalMesh->SetupAttachment(WeaponRootComponent);

	WeaponStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponStaticMesh"));
	WeaponStaticMesh->bReceivesDecals = false;
	WeaponStaticMesh->CastShadow = true;
	WeaponStaticMesh->SetCollisionObjectType(ECC_WorldDynamic);
	WeaponStaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponStaticMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	WeaponStaticMesh->SetCollisionResponseToChannel(TRACE_WEAPON, ECR_Ignore);
	WeaponStaticMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	WeaponStaticMesh->SetupAttachment(WeaponRootComponent);

	SecondaryWeaponSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SecondaryWeaponSkeletalMesh"));
	SecondaryWeaponSkeletalMesh->bReceivesDecals = false;
	SecondaryWeaponSkeletalMesh->CastShadow = true;
	SecondaryWeaponSkeletalMesh->SetCollisionObjectType(ECC_WorldDynamic);
	SecondaryWeaponSkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SecondaryWeaponSkeletalMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	SecondaryWeaponSkeletalMesh->SetCollisionResponseToChannel(TRACE_WEAPON, ECR_Ignore);
	SecondaryWeaponSkeletalMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	SecondaryWeaponSkeletalMesh->SetupAttachment(WeaponRootComponent);

	SecondaryWeaponStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SecondaryWeaponStaticMesh"));
	SecondaryWeaponStaticMesh->bReceivesDecals = false;
	SecondaryWeaponStaticMesh->CastShadow = true;
	SecondaryWeaponStaticMesh->SetCollisionObjectType(ECC_WorldDynamic);
	SecondaryWeaponStaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SecondaryWeaponStaticMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	SecondaryWeaponStaticMesh->SetCollisionResponseToChannel(TRACE_WEAPON, ECR_Ignore);
	SecondaryWeaponStaticMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	SecondaryWeaponStaticMesh->SetupAttachment(WeaponRootComponent);

	DefaultGameplayTags.Add(TAG_ACTOR_WEAPON);
	
	SetRootComponent(WeaponRootComponent);
}

void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();	
}

void ABaseWeapon::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if(GetWeaponMesh())
	{
		GetWeaponMesh()->SetHiddenInGame(true);
	}
}

void ABaseWeapon::OnEquip(const TScriptInterface<IWeapon> LastWeapon)
{
	if(GetWeaponMesh())
	{
		GetWeaponMesh()->SetHiddenInGame(false);
	}
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

	if (OwningPawn && OwningPawn->IsPlayerControlled())
	{
		PlayWeaponSound(EquipSound);
	}
}

void ABaseWeapon::OnEquipFinished()
{	
	bIsEquipped = true;
	bPendingEquip = false;
	DetermineWeaponState();
}

void ABaseWeapon::OnUnEquip()
{
	if(GetWeaponMesh())
	{
		GetWeaponMesh()->SetHiddenInGame(true);
	}
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

void ABaseWeapon::StartWeaponRagdoll()
{
	Internal_StartMeshRagdoll(WeaponSkeletalMesh);
	Internal_StartMeshRagdoll(WeaponStaticMesh);
	Internal_StartMeshRagdoll(SecondaryWeaponStaticMesh);
	Internal_StartMeshRagdoll(SecondaryWeaponSkeletalMesh);
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

void ABaseWeapon::HandleFiring()
{
	if (CheckChildFireCondition() && CanFire())
	{
		SimulateWeaponFire();
		FireWeapon();
		BurstCounter++;
	}
	else if (OwningPawn)
	{
		if (BurstCounter > 0)
		{
			OnBurstFinished();
		}
	}
	
	if (OwningPawn)
	{
		bRefiring = (CurrentState == EWeaponState::Firing && TimeBetweenShots > 0.0f);
		if (bRefiring)
		{
			GetWorldTimerManager().SetTimer(TimerHandle_HandleFiring, this, &ABaseWeapon::HandleFiring, TimeBetweenShots, false);
		}
	}

	LastFireTime = GetWorld()->GetTimeSeconds();
}

void ABaseWeapon::OnBurstStarted()
{
	K2_OnBurstStarted();
	const float GameTime = GetWorld()->GetTimeSeconds();
	if (IsWeaponOnCooldown())
	{
		GetWorldTimerManager().SetTimer(TimerHandle_HandleFiring, this, &ABaseWeapon::HandleFiring, LastFireTime + TimeBetweenShots - GameTime, false);
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
	// InMeshComp->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	InMeshComp->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	InMeshComp->SetCollisionProfileName("BlockAll");
	InMeshComp->SetSimulatePhysics(true);
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
	if(GetWeaponMesh())
	{
		GetWeaponMesh()->SetHiddenInGame(true);
	}
}

void ABaseWeapon::OnLeaveInventory()
{
	SetOwningPawn(nullptr);
	if(GetWeaponMesh())
	{
		GetWeaponMesh()->SetHiddenInGame(true);
	}
}

void ABaseWeapon::PlayCameraShake()
{
	if(!GetOwningPawn() || !UFeedbackUtils::ShouldCameraShake())
		return;
	
	if(ABasePlayerController* CurrCon = Cast<ABasePlayerController>(GetOwningPawn()->GetController()))
	{
		CurrCon->ClientStartCameraShake(FireCameraShake, CameraShakeScale);		
	}
}

bool ABaseWeapon::IsWeaponOnCooldown() const
{
	const float GameTime = GetWorld()->GetTimeSeconds();
	if (LastFireTime > 0 && TimeBetweenShots > 0.0f && LastFireTime + TimeBetweenShots > GameTime)
	{
		return true;
	}
	return false;
}
