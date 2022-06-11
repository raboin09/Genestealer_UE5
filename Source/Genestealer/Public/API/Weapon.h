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
enum class EWeaponType : uint8
{
	NONE,
	Melee,
	Heavy,
	Pistol,
	Rifle
};

UENUM(BlueprintType)
enum class EWeaponSlot : uint8
{
	Primary,
	Alternate
};

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
	virtual EWeaponType GetWeaponType() const PURE_VIRTUAL(IWeapon::GetWeaponType, return EWeaponType::NONE;);
	virtual EALSOverlayState GetWeaponOverlay() PURE_VIRTUAL(IWeapon::GetWeaponOverlay, return EALSOverlayState::Default;);
	virtual EWeaponState GetCurrentState() const PURE_VIRTUAL(IWeapon::GetCurrentState, return EWeaponState::Idle; );
	virtual FTransform GetLeftHandSocketTransform() const PURE_VIRTUAL(IWeapon::GetLeftHandSocketTransform, return FTransform(); );
	
	virtual void OnEnterInventory(ACharacter* NewOwner) PURE_VIRTUAL(IWeapon::OnEnterInventory,);
	virtual void OnLeaveInventory() PURE_VIRTUAL(IWeapon::OnLeaveInventory,);

	virtual bool IsWeaponOnCooldown() const PURE_VIRTUAL(IWeapon::IsWeaponOnCooldown, return false;);
	virtual void OnEquip(const TScriptInterface<IWeapon> LastWeapon) PURE_VIRTUAL(IWeapon::OnEquip,);
	virtual void OnEquipFinished() PURE_VIRTUAL(IWeapon::OnEquipFinished,);
	virtual void OnUnEquip() PURE_VIRTUAL(IWeapon::OnUnEquip,);

	virtual void StartFire() PURE_VIRTUAL(IWeapon::StartFire,);
	virtual void StopFire() PURE_VIRTUAL(IWeapon::StopFire,);

	virtual bool ShouldForceAimOnFire() const PURE_VIRTUAL(IWeapon::ShouldForceAimOnFire, return false;);
	virtual bool CanFire() const PURE_VIRTUAL(IWeapon::CanFire, return false;);
	virtual bool CanReload() PURE_VIRTUAL(IWeapon::CanReload, return false;);

	virtual UMeshComponent* GetWeaponMesh() const PURE_VIRTUAL(IWeapon::GetWeaponMesh, return nullptr;);
	virtual UMeshComponent* GetSecondaryWeaponMesh() const PURE_VIRTUAL(IWeapon::GetSecondaryWeaponMesh, return nullptr;);
	
	virtual ACharacter* GetOwningPawn() const PURE_VIRTUAL(IWeapon::GetOwningPawn, return nullptr;);
	virtual void SetOwningPawn(ACharacter* IncomingCharacter) PURE_VIRTUAL(IWeapon::SetOwningPawn,);

	virtual void StartWeaponRagdoll(bool bSpawnPickup = true) PURE_VIRTUAL(IWeapon::StartWeaponRagdoll,);
	virtual void DestroyWeapon() PURE_VIRTUAL(IWeapon::DestroyWeapon,);

	UFUNCTION(BlueprintCallable)
	virtual float GetWeaponRange() PURE_VIRTUAL(IWeapon::GetWeaponRange, return 0.f;)  
};