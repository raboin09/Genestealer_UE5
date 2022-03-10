
#include "Weapons/BaseWeapon.h"
#include "Sound/SoundCue.h"
#include "Characters/BaseCharacter.h"
#include "Core/BasePlayerController.h"
#include "Genestealer/Genestealer.h"
#include "Kismet/GameplayStatics.h"
#include "Utils/CoreUtils.h"
#include "Utils/EffectUtils.h"
#include "Utils/GameplayTagUtils.h"

ABaseWeapon::ABaseWeapon()
{
	WeaponSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponSkeletalMesh"));
	WeaponSkeletalMesh->bReceivesDecals = false;
	WeaponSkeletalMesh->CastShadow = true;
	WeaponSkeletalMesh->SetCollisionObjectType(ECC_WorldDynamic);
	WeaponSkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponSkeletalMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	WeaponSkeletalMesh->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);
	WeaponSkeletalMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	WeaponStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponStaticMesh"));
	WeaponStaticMesh->bReceivesDecals = false;
	WeaponStaticMesh->CastShadow = true;
	WeaponStaticMesh->SetCollisionObjectType(ECC_WorldDynamic);
	WeaponStaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponStaticMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	WeaponStaticMesh->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);
	WeaponStaticMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	DefaultGameplayTags.Add(GameplayTag::ActorType::Weapon);
	
	SetRootComponent(WeaponStaticMesh);
}

void ABaseWeapon::UseAmmo()
{
	BroadcastAmmoUsage();
}

void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();	
	if(GetWeaponMesh())
	{
		GetWeaponMesh()->SetHiddenInGame(true);
	}
}

void ABaseWeapon::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ABaseWeapon::OnEquip(const TScriptInterface<IWeapon> LastWeapon)
{
	if(GetWeaponMesh())
	{
		GetWeaponMesh()->SetHiddenInGame(false);
	}
	bPendingEquip = true;
	DetermineWeaponState();
	if (LastWeapon)
	{
		const float Duration = PlayWeaponAnimation(EWeaponAnimAction::Equip);
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
	BroadcastAmmoUsage();
}

void ABaseWeapon::OnEquipFinished()
{	
	bIsEquipped = true;
	bPendingEquip = false;
	DetermineWeaponState(); 
	BlueprintEquip(true);
	if (OwningPawn)
	{
		if (GetCurrentAmmoInClip() <= 0 && CanReload())
		{
			StartReload();
		}
	}
	BroadcastAmmoUsage();
}

void ABaseWeapon::OnUnEquip()
{
	if(GetWeaponMesh())
	{
		BlueprintEquip(false);
		GetWeaponMesh()->SetHiddenInGame(true);
	}
	bIsEquipped = false;
	StopFire();
	if (bPendingReload)
	{
		StopWeaponAnimation(EWeaponAnimAction::Reload);
		bPendingReload = false;

		GetWorldTimerManager().ClearTimer(TimerHandle_StopReload);
		GetWorldTimerManager().ClearTimer(TimerHandle_ReloadWeapon);
	}

	if (bPendingEquip)
	{
		StopWeaponAnimation(EWeaponAnimAction::Equip);
		bPendingEquip = false;

		GetWorldTimerManager().ClearTimer(TimerHandle_OnEquipFinished);
	}
	DetermineWeaponState();
}

void ABaseWeapon::StartReload()
{
	if (CanReload())
	{
		bPendingReload = true;
		DetermineWeaponState();
		float AnimDuration = PlayWeaponAnimation(EWeaponAnimAction::Reload);	
		if (AnimDuration <= 0.0f)
		{
			AnimDuration = ReloadDurationIfNoAnim;
		}
		GetWorldTimerManager().SetTimer(TimerHandle_StopReload, this, &ABaseWeapon::StopReload, AnimDuration, false);
		GetWorldTimerManager().SetTimer(TimerHandle_ReloadWeapon, this, &ABaseWeapon::ReloadWeapon, FMath::Max(0.1f, AnimDuration - 0.1f), false);
		
		if (OwningPawn)
		{
			PlayWeaponSound(ReloadSound);
		}
	}
}

void ABaseWeapon::StopReload()
{
	if (CurrentState == EWeaponState::Reloading)
	{
		bPendingReload = false;
		DetermineWeaponState();
		StopWeaponAnimation(EWeaponAnimAction::Reload);
		BroadcastAmmoUsage();
	}
}

void ABaseWeapon::StartFire()
{
	if (!bWantsToFire)
	{
		bWantsToFire = true;
		DetermineWeaponState();
	}
}

void ABaseWeapon::StopFire()
{
	if (bWantsToFire)
	{
		bWantsToFire = false;
		DetermineWeaponState();
	}
}

bool ABaseWeapon::CanFire() const
{
	const bool bCanFire = OwningPawn != nullptr;
	const bool bStateOKToFire = ( ( CurrentState ==  EWeaponState::Idle ) || ( CurrentState == EWeaponState::Firing) );	
	return (( bCanFire == true ) && ( bStateOKToFire == true ) && ( bPendingReload == false ));
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

UMeshComponent* ABaseWeapon::GetWeaponMesh() const
{
	if(WeaponSkeletalMesh && WeaponSkeletalMesh->SkeletalMesh != nullptr)
	{
		return WeaponSkeletalMesh;
	}
	return WeaponStaticMesh;
}

void ABaseWeapon::SetWeaponMesh(UMeshComponent* InMesh)
{
	if(UStaticMeshComponent* StaticMesh = Cast<UStaticMeshComponent>(InMesh))
	{
		WeaponStaticMesh = StaticMesh;
	} else if(USkeletalMeshComponent* SkelMesh = Cast<USkeletalMeshComponent>(InMesh))
	{
		WeaponSkeletalMesh = SkelMesh;	
	}
}

void ABaseWeapon::HandleFiring()
{
	if ((GetCurrentAmmoInClip() > 0 || HasInfiniteClip() || HasInfiniteAmmo()) && CanFire())
	{
		SimulateWeaponFire();
		if (OwningPawn)
		{
			FireWeapon();
			UseAmmo();
			BurstCounter++;
		}
	}
	else if (CanReload())
	{
		StartReload();
	}
	else if (OwningPawn)
	{
		if (GetCurrentAmmo() == 0 && !bRefiring)
		{
			PlayWeaponSound(OutOfAmmoSound);
		}

		if (BurstCounter > 0)
		{
			OnBurstFinished();
		}
	}

	if (OwningPawn)
	{
		if (GetCurrentAmmoInClip() <= 0 && CanReload())
		{
			StartReload();
		}
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
	const float GameTime = GetWorld()->GetTimeSeconds();
	if (LastFireTime > 0 && TimeBetweenShots > 0.0f && LastFireTime + TimeBetweenShots > GameTime)
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
	if (bIsEquipped)
	{
		if( bPendingReload  )
		{
			if(CanReload() == false )
			{
				NewState = CurrentState;
			}
			else
			{
				NewState = EWeaponState::Reloading;
			}
		}		
		else if ( (bPendingReload == false ) && ( bWantsToFire == true ) && ( CanFire() == true ))
		{
			NewState = EWeaponState::Firing;
		}
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
		AC = UGameplayStatics::SpawnSoundAttached(Sound, OwningPawn->GetRootComponent());
	}
	return AC;
}

float ABaseWeapon::PlayWeaponAnimation(EWeaponAnimAction WeaponAction) const
{
	float Duration = 0.0f;
	if (IAnimatable* AnimationOwner = Cast<IAnimatable>(OwningPawn))
	{
		Duration = AnimationOwner->PlayWeaponAnimation(WeaponArchetype, WeaponAction);
	}
	return Duration;
}

void ABaseWeapon::StopWeaponAnimation(EWeaponAnimAction WeaponAction) const
{
	if (IAnimatable* AnimationOwner = Cast<IAnimatable>(OwningPawn))
	{
		AnimationOwner->StopWeaponAnimation(WeaponArchetype, WeaponAction);
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
	if(!GetOwningPawn())
		return;
	if(ABasePlayerController* CurrCon = Cast<ABasePlayerController>(GetOwningPawn()->GetController()))
	{
		
		if(CurrCon && FireCameraShake)
		{
			CurrCon->ClientStartCameraShake(FireCameraShake, CameraShakeScale);
		}
		
	}
}