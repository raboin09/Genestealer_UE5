// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Library/ALSCharacterEnumLibrary.h"
#include "UObject/Interface.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	Idle,
    Firing,
    Reloading,
    Equipping,
};

UENUM(BlueprintType)
enum class EWeaponSlot : uint8
{
	Pistol,
	Rifle,
	Melee
};

// Broadcasts CurrentAmmoInClip and CurrentAmmo
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FAmmoAmountChanged, int32, CurrentAmmoInClip, int32, ClipCapacity, int32, CurrentAmmo);

// This class does not need to be modified.
UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UWeapon : public UInterface
{
	GENERATED_BODY()
};

class GENESTEALER_API IWeapon
{
	GENERATED_BODY()

public:
	virtual FAmmoAmountChanged& OnAmmoAmountChanged() = 0;
	virtual void BroadcastAmmoUsage() = 0;

	virtual EALSOverlayState GetWeaponOverlay() PURE_VIRTUAL(IWeapon::GetWeaponOverlay, return EALSOverlayState::Default;);
	virtual EWeaponState GetCurrentState() const PURE_VIRTUAL(IWeapon::GetCurrentWeapon, return EWeaponState::Idle; );
	
	virtual void OnEnterInventory(ACharacter* NewOwner) PURE_VIRTUAL(IWeapon::OnEnterInventory,);
	virtual void OnLeaveInventory() PURE_VIRTUAL(IWeapon::OnLeaveInventory,);
	
	virtual void OnEquip(const TScriptInterface<IWeapon> LastWeapon) PURE_VIRTUAL(IWeapon::OnEquip,);
	virtual void OnEquipFinished() PURE_VIRTUAL(IWeapon::OnEquipFinished,);
	virtual void OnUnEquip() PURE_VIRTUAL(IWeapon::OnUnEquip,);

	virtual void StartReload() PURE_VIRTUAL(IWeapon::StartReload,);
	virtual void StopReload() PURE_VIRTUAL(IWeapon::StopReload,);

	virtual void StartFire() PURE_VIRTUAL(IWeapon::StartFire,);
	virtual void StopFire() PURE_VIRTUAL(IWeapon::StopFire,);

	virtual bool CanFire() const PURE_VIRTUAL(IWeapon::CanFire, return false;);
	virtual bool CanReload() PURE_VIRTUAL(IWeapon::CanReload, return false;);

	virtual void GiveAmmo(int32 AddAmount) PURE_VIRTUAL(IWeapon::GiveAmmo,);
	virtual int32 GetCurrentAmmo() PURE_VIRTUAL(IWeapon::GetCurrentAmmo, return 0;);
	virtual int32 GetCurrentAmmoInClip() PURE_VIRTUAL(IWeapon::GetCurrentAmmoInClip, return 0;)
	virtual int32 GetAmmoPerClip() PURE_VIRTUAL(IWeapon::GetAmmoPerClip, return 0;)
	virtual int32 GetMaxAmmo() PURE_VIRTUAL(IWeapon::GetMaxAmmo, return 0;);
	virtual bool HasInfiniteAmmo() PURE_VIRTUAL(IWeapon::HasInfiniteAmmo, return false;);
	virtual bool HasInfiniteClip() PURE_VIRTUAL(IWeapon::HasInfiniteClip, return false;);

	virtual UMeshComponent* GetWeaponMesh() const PURE_VIRTUAL(IWeapon::GetWeaponMesh, return nullptr;);
	virtual UMeshComponent* GetSecondaryWeaponMesh() const PURE_VIRTUAL(IWeapon::GetSecondaryWeaponMesh, return nullptr;);
	
	virtual ACharacter* GetOwningPawn() const PURE_VIRTUAL(IWeapon::GetOwningPawn, return nullptr;);
	virtual void SetOwningPawn(ACharacter* IncomingCharacter) PURE_VIRTUAL(IWeapon::SetOwningPawn,);

	virtual void StartWeaponRagdoll() PURE_VIRTUAL(IWeapon::StartWeaponRagdoll,);
	virtual void DestroyWeapon() PURE_VIRTUAL(IWeapon::DestroyWeapon,);

	UFUNCTION(BlueprintCallable)
	virtual float GetWeaponRange() PURE_VIRTUAL(IWeapon::GetWeaponRange, return 0.f;)  
};