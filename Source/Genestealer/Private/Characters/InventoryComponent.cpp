#include "Characters/InventoryComponent.h"
#include "GameFramework/Character.h"
#include "Utils/CoreUtils.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UInventoryComponent::SpawnInventoryActors(TSubclassOf<AActor> PistolClass, TSubclassOf<AActor> RifleClass, TSubclassOf<AActor> MeleeClass)
{
	if(PistolClass)
	{
		Internal_SpawnWeaponFromClass(PistolClass, EWeaponSlot::Pistol);
	}
	
	if(RifleClass)
	{
		Internal_SpawnWeaponFromClass(RifleClass, EWeaponSlot::Rifle);
	}

	if(MeleeClass)
	{
		Internal_SpawnWeaponFromClass(MeleeClass, EWeaponSlot::Melee);
	}	

	if(RifleWeapon)
	{
		EquipRifleWeapon();
	} else if(PistolWeapon)
	{
		EquipPistolWeapon();
	} else if(MeleeWeapon)
	{
		EquipMeleeWeapon();
	}
}

void UInventoryComponent::Internal_SpawnWeaponFromClass(TSubclassOf<AActor> WeaponClass, EWeaponSlot Slot)
{
	if(WeaponClass && WeaponClass->ImplementsInterface(UWeapon::StaticClass()))
	{
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		AActor* SpawnedWeapon = GetWorld()->SpawnActor<AActor>(WeaponClass, SpawnInfo);
		if(APawn* CastedOwner = Cast<APawn>(GetOwner()))
		{
			SpawnedWeapon->SetInstigator(CastedOwner);			
		}
		SpawnedWeapon->SetOwner(GetOwner());
		if(const TScriptInterface<IWeapon> NewWeapon = SpawnedWeapon)
		{
			AddWeapon(NewWeapon, Slot);
		}
	}
}

void UInventoryComponent::DestroyInventory()
{
	if(PistolWeapon)
	{
		PistolWeapon->StopFire();
		Internal_RemoveWeapon(PistolWeapon, EWeaponSlot::Pistol);
	}

	if(RifleWeapon)
	{
		RifleWeapon->StopFire();
		Internal_RemoveWeapon(RifleWeapon, EWeaponSlot::Rifle);
	}

	if(MeleeWeapon)
	{
		MeleeWeapon->StopFire();
		Internal_RemoveWeapon(MeleeWeapon, EWeaponSlot::Melee);
	}
	
	CurrentWeapon = nullptr;
}

EWeaponState UInventoryComponent::GetCurrentWeaponState() const
{
	if(CurrentWeapon)
	{
		return CurrentWeapon->GetWeaponState();
	}
	return EWeaponState::Idle;
}

TSubclassOf<AActor> UInventoryComponent::GetPistolClass() const
{
	if(PistolWeapon && PistolWeapon.GetObject() && PistolWeapon.GetObject()->GetClass()->IsChildOf(AActor::StaticClass()))
	{
		return PistolWeapon.GetObject()->GetClass();
	}
	return nullptr;
}

TSubclassOf<AActor> UInventoryComponent::GetRifleClass() const
{
	if(RifleWeapon && RifleWeapon.GetObject() && RifleWeapon.GetObject()->GetClass()->IsChildOf(AActor::StaticClass()))
	{
		return RifleWeapon.GetObject()->GetClass();
	}
	return nullptr;
}

void UInventoryComponent::EquipPistolWeapon()
{
	if(PistolWeapon)
	{
		Internal_SetCurrentWeapon(PistolWeapon, CurrentWeapon);
	}
}

void UInventoryComponent::EquipRifleWeapon()
{
	if(RifleWeapon)
	{
		Internal_SetCurrentWeapon(RifleWeapon, CurrentWeapon);
	}
}

void UInventoryComponent::EquipMeleeWeapon()
{
	if(MeleeWeapon)
	{
		Internal_SetCurrentWeapon(MeleeWeapon, CurrentWeapon);
	}
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

void UInventoryComponent::StartReload()
{
	if(CurrentWeapon)
	{
		CurrentWeapon->StartReload();
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
			case EWeaponSlot::Pistol:
				PistolWeapon = Weapon;
				break;
			case EWeaponSlot::Rifle:
				RifleWeapon = Weapon;
				break;
			case EWeaponSlot::Melee:
				MeleeWeapon = Weapon;
				break;
			default: ;
			}
			NewWeaponAdded.Broadcast(Weapon);
		}
	}
}

bool UInventoryComponent::HasWeapon(const UClass* WeaponClass) const
{
	const UObject* TempWeapon = WeaponClass->GetDefaultObject();
	if(!TempWeapon)
		return false;

	if(const UObject* PistolTempObj = PistolWeapon.GetObject())
	{
		if(PistolTempObj->GetClass() == TempWeapon->GetClass())
		{
			return true;
		}
	}

	if(const UObject* RifleTempObj = RifleWeapon.GetObject())
	{
		if(RifleTempObj->GetClass() == TempWeapon->GetClass())
		{
			return true;
		}
	}

	if(const UObject* MeleeTempObj = MeleeWeapon.GetObject())
	{
		if(MeleeTempObj->GetClass() == TempWeapon->GetClass())
		{
			return true;
		}
	}
	
	return false;
}

void UInventoryComponent::GiveWeaponClassAmmo(const UClass* WeaponClass, int32 AmmoRoundsToGive)
{
	if(HasWeapon(WeaponClass))
	{
		if(const TScriptInterface<IWeapon> TempWeapon = WeaponClass->GetDefaultObject())
		{
			TempWeapon->GiveAmmo(AmmoRoundsToGive);
		}
	}
}

void UInventoryComponent::Internal_RemoveWeapon(TScriptInterface<IWeapon> Weapon, EWeaponSlot Slot)
{
	if (Weapon)
	{
		Weapon->OnLeaveInventory();
		WeaponRemoved.Broadcast(Weapon);
	}
}

void UInventoryComponent::Internal_SetCurrentWeapon(TScriptInterface<IWeapon> NewWeapon, TScriptInterface<IWeapon> LastWeapon)
{
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

	if (NewWeapon)
	{
		if(ACharacter* CastedChar = Cast<ACharacter>(GetOwner()))
		{
			NewWeapon->SetOwningPawn(CastedChar);
			NewWeapon->OnEquip(LastWeapon);
		}
	}
	CurrentWeaponChanged.Broadcast(NewWeapon, LastWeapon);
	
	// Needed to catch this for the first BeginPlay loop
	if(NewWeapon)
	{
		NewWeapon->BroadcastAmmoUsage();
	}
}