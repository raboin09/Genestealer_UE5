#include "Characters/InventoryComponent.h"

#include "API/RangedEntity.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Utils/CoreUtils.h"
#include "Utils/WorldUtils.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UInventoryComponent::SpawnInventoryActors(TSubclassOf<AActor> PrimaryWeaponClass, TSubclassOf<AActor> AlternateWeaponClass)
{
	if(PrimaryWeaponClass)
	{
		if(PrimaryWeapon)
		{
			Internal_RemoveWeapon(PrimaryWeapon, EWeaponSlot::Primary);
		}
		Internal_SpawnWeaponFromClass(PrimaryWeaponClass, EWeaponSlot::Primary);
	}
	
	if(AlternateWeaponClass)
	{
		if(AlternateWeapon)
		{
			Internal_RemoveWeapon(AlternateWeapon, EWeaponSlot::Alternate);
		}
		Internal_SpawnWeaponFromClass(AlternateWeaponClass, EWeaponSlot::Alternate);
	}

	if(PrimaryWeapon)
	{
		EquipPrimaryWeapon();
	} else if(AlternateWeapon)
	{
		EquipAlternateWeapon();
	}
}

void UInventoryComponent::ReplaceCurrentWeapon(TSubclassOf<AActor> WeaponClass)
{
	const auto Slot = IsPrimaryWeaponEquipped() ? EWeaponSlot::Primary : EWeaponSlot::Alternate;
	if(WeaponClass)
	{
		if(CurrentWeapon)
		{
			Internal_RemoveWeapon(CurrentWeapon, Slot);
		}
		Internal_SpawnWeaponFromClass(WeaponClass, Slot);
	}

	if(Slot == EWeaponSlot::Primary)
	{
		EquipPrimaryWeapon();
	} else if (Slot == EWeaponSlot::Alternate)
	{
		EquipAlternateWeapon();
	}
}

void UInventoryComponent::Internal_SpawnWeaponFromClass(TSubclassOf<AActor> WeaponClass, EWeaponSlot Slot)
{
	if(WeaponClass && WeaponClass->ImplementsInterface(UWeapon::StaticClass()))
	{
		AActor* SpawnedWeapon = UWorldUtils::SpawnActorToWorld_Deferred<AActor>(this, WeaponClass);
		if(const TScriptInterface<IWeapon> NewWeapon = SpawnedWeapon)
		{
			NewWeapon->SetOwningPawn(Cast<ACharacter>(GetOwner()));
		}
		UWorldUtils::FinishSpawningActor_Deferred(SpawnedWeapon, GetOwner()->GetTransform());
		if(const TScriptInterface<IWeapon> NewWeapon = SpawnedWeapon)
		{
			AddWeapon(NewWeapon, Slot);
		}
	}
}

void UInventoryComponent::DestroyInventory(bool bRagdollWeapon, bool bSpawnWeaponPickup)
{
	if(PrimaryWeapon)
	{
		Internal_RemoveWeapon(PrimaryWeapon, EWeaponSlot::Primary);
	}
	
	if(AlternateWeapon)
	{
		Internal_RemoveWeapon(AlternateWeapon, EWeaponSlot::Alternate);
	}	

	CurrentWeapon = nullptr;
}

bool UInventoryComponent::DoesCurrentWeaponForceAimOnFire() const
{
	if(CurrentWeapon)
	{
		return CurrentWeapon->ShouldForceAimOnFire();
	}
	return false;
}

bool UInventoryComponent::CanWeaponAim() const
{
	if(CurrentWeapon)
	{
		return CurrentWeapon->GetWeaponType() == EWeaponType::Rifle;
	}
	return false;
}

TSubclassOf<AActor> UInventoryComponent::GetAlternateWeaponClass() const
{
	if(AlternateWeapon && AlternateWeapon.GetObject() && AlternateWeapon.GetObject()->GetClass()->IsChildOf(AActor::StaticClass()))
	{
		return AlternateWeapon.GetObject()->GetClass();
	}
	return nullptr;
}

TSubclassOf<AActor> UInventoryComponent::GetPrimaryWeaponClass() const
{
	if(PrimaryWeapon && PrimaryWeapon.GetObject() && PrimaryWeapon.GetObject()->GetClass()->IsChildOf(AActor::StaticClass()))
	{
		return PrimaryWeapon.GetObject()->GetClass();
	}
	return nullptr;
}

void UInventoryComponent::EquipAlternateWeapon()
{	
	if(AlternateWeapon && CurrentWeapon != AlternateWeapon)
	{
		Internal_SetCurrentWeapon(AlternateWeapon, CurrentWeapon);
	}
}

void UInventoryComponent::EquipPrimaryWeapon()
{
	if(PrimaryWeapon && CurrentWeapon != PrimaryWeapon)
	{
		Internal_SetCurrentWeapon(PrimaryWeapon, CurrentWeapon);
	}
}

FPlayerAimingChangedPayload UInventoryComponent::GetCrosshairPayload() const
{
	if(const IRangedEntity* RangedEntity = Cast<IRangedEntity>(CurrentWeapon.GetObject()))
	{
		return RangedEntity->GetCrosshairPayload();
	}
	return FPlayerAimingChangedPayload(false, nullptr, 0.f);
}

TScriptInterface<IWeapon> UInventoryComponent::Internal_FindWeapon(TSubclassOf<AActor> WeaponClass)
{
	if(HasWeapon(WeaponClass))
	{
		if(const TScriptInterface<IWeapon> TempWeapon = WeaponClass->GetDefaultObject())
		{
			return TempWeapon;
		}
	}
	return nullptr;
}

void UInventoryComponent::StartFiring()
{
	if (!bWantsToFire)
	{
		bWantsToFire = true;
		if (CurrentWeapon)
		{
			CurrentWeapon->StartFire();
		}
	}
}

void UInventoryComponent::StopFiring()
{
	if (bWantsToFire)
	{
		bWantsToFire = false;
		if (CurrentWeapon)
		{
			CurrentWeapon->StopFire();
		}
	}
}

void UInventoryComponent::OnTargetingChange(bool bIsTargeting)
{
	
}

void UInventoryComponent::StartReload()
{
	if(IRangedEntity* AmmoEntity = Cast<IRangedEntity>(CurrentWeapon.GetObject()))
	{
		AmmoEntity->StartReload();
	}
}

void UInventoryComponent::AddWeapon(TScriptInterface<IWeapon> Weapon, EWeaponSlot Slot)
{
	if(Weapon)
	{
		if(ACharacter* CastedChar = Cast<ACharacter>(GetOwner()))
		{
			Weapon->OnEnterInventory(CastedChar);
			switch (Slot)
			{
			case EWeaponSlot::Alternate:
				AlternateWeapon = Weapon;
				break;
			case EWeaponSlot::Primary:
				PrimaryWeapon = Weapon;
				break;
			default: ;
			}
			NewWeaponAdded.Broadcast(FNewWeaponAddedPayload(Weapon));
		}
	}
}

bool UInventoryComponent::HasWeapon(const UClass* WeaponClass) const
{
	if(!WeaponClass)
	{
		return false;
	}
	
	const UObject* TempWeapon = WeaponClass->GetDefaultObject();
	if(!TempWeapon)
		return false;

	if(const UObject* AltTempObj = AlternateWeapon.GetObject())
	{
		if(AltTempObj->GetClass() == TempWeapon->GetClass())
		{
			return true;
		}
	}

	if(const UObject* PrimaryTempObj = PrimaryWeapon.GetObject())
	{
		if(PrimaryTempObj->GetClass() == TempWeapon->GetClass())
		{
			return true;
		}
	}
	
	return false;
}

void UInventoryComponent::GiveWeaponClassAmmo(const UClass* WeaponClass, int32 AmmoRoundsToGive)
{
	if(WeaponClass == GetPrimaryWeaponClass())
	{
		if (const TScriptInterface<IRangedEntity> PrimaryWeaponAmmo = PrimaryWeapon.GetObject())
		{
			PrimaryWeaponAmmo->GiveAmmo(AmmoRoundsToGive);
		}
	}
	
	if(WeaponClass == GetAlternateWeaponClass())
	{
		if (const TScriptInterface<IRangedEntity> AltWeaponAmmo = AlternateWeapon.GetObject())
		{
			AltWeaponAmmo->GiveAmmo(AmmoRoundsToGive);
		}
	}
}

EWeaponState UInventoryComponent::GetCurrentWeaponState() const
{
	if(CurrentWeapon)
	{
		return CurrentWeapon->GetCurrentState();
	}
	return EWeaponState::Idle;
}

FTransform UInventoryComponent::GetCurrentWeaponLeftHandSocketTransform() const
{
	if(CurrentWeapon)
	{
		return CurrentWeapon->GetLeftHandSocketTransform();
	}
	return FTransform();
}

void UInventoryComponent::Internal_RemoveWeapon(TScriptInterface<IWeapon> Weapon, EWeaponSlot Slot)
{
	if (Weapon)
	{
		Weapon->OnLeaveInventory();
		if(AActor* CastedWeapon = Cast<AActor>(Weapon.GetObject()))
		{
			CastedWeapon->Destroy();
		}
		WeaponRemoved.Broadcast(Weapon);
	}
}

void UInventoryComponent::Internal_SetCurrentWeapon(TScriptInterface<IWeapon> NewWeapon, TScriptInterface<IWeapon> LastWeapon)
{
	if(NewWeapon == LastWeapon)
	{
		return;
	}
	
	TScriptInterface<IWeapon> LocalLastWeapon = nullptr;
	if (LastWeapon != nullptr)
	{
		LocalLastWeapon = LastWeapon;
	}
	else if (NewWeapon != CurrentWeapon)
	{
		LocalLastWeapon = CurrentWeapon;
	}

	if (LocalLastWeapon)
	{
		LocalLastWeapon->OnUnEquip();
	}

	CurrentWeapon = NewWeapon;


	if(NewWeapon)
	{
		if(ACharacter* CastedChar = Cast<ACharacter>(GetOwner()))
		{
			NewWeapon->SetOwningPawn(CastedChar);
			NewWeapon->OnEquip(LastWeapon);		
		}
	}
	CurrentWeaponChanged.Broadcast(FCurrentWeaponChangedPayload(NewWeapon, LastWeapon));
	
	// Needed to catch this for the first BeginPlay loop
	if(IRangedEntity* AmmoEntity = Cast<IRangedEntity>(NewWeapon.GetObject()))
	{
		AmmoEntity->BroadcastAmmoUsage();
	}
}