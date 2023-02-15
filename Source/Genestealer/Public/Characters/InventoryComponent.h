// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "API/Weapon.h"
#include "Types/EventDeclarations.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

class ABaseCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GENESTEALER_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()


public:
	UInventoryComponent();
	
	void SpawnInventoryActors(TSubclassOf<AActor> PrimaryWeaponClass, TSubclassOf<AActor> AlternateWeaponClass);
	void ReplaceCurrentWeapon(TSubclassOf<AActor> WeaponClass);
	void DestroyInventory(bool bRagdollWeapon, bool bSpawnWeaponPickup);
	bool DoesCurrentWeaponForceAimOnFire() const;
	void HideWeapons(bool bShouldHide);
	bool CanWeaponAim() const;
	TSubclassOf<AActor> GetAlternateWeaponClass() const;
	TSubclassOf<AActor> GetPrimaryWeaponClass() const;
	FORCEINLINE TScriptInterface<IWeapon> GetPrimaryWeapon() const { return PrimaryWeapon; }
	FORCEINLINE TScriptInterface<IWeapon> GetAlternateWeapon() const { return PrimaryWeapon; }
	bool IsPrimaryWeaponEquipped() const { return CurrentWeapon == PrimaryWeapon; }
	
	UFUNCTION(BlueprintCallable)
	void StartReload();
	UFUNCTION(BlueprintCallable)
	void AddWeapon(TScriptInterface<IWeapon> Weapon, EWeaponSlot Slot);
	UFUNCTION(BlueprintCallable)
	void StartFiring();
	UFUNCTION(BlueprintCallable)
	void StopFiring();
	void OnTargetingChange(bool bIsTargeting);

	UFUNCTION(BlueprintCallable)
	void EquipAlternateWeapon();
	UFUNCTION(BlueprintCallable)
	void EquipPrimaryWeapon();
	
	bool HasWeapon(const UClass* WeaponClass) const;
	void GiveWeaponClassAmmo(const UClass* WeaponClass, int32 AmmoRoundsToGive);

	FPlayerAimingChangedPayload GetCrosshairPayload() const;
	FORCEINLINE EWeaponType GetCurrentWeaponType() const { return CurrentWeapon ? CurrentWeapon->GetWeaponType() : EWeaponType::NONE; }
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	FORCEINLINE TScriptInterface<IWeapon> GetEquippedWeapon() const{ return CurrentWeapon; }
	EWeaponState GetCurrentWeaponState() const;
	FTransform GetCurrentWeaponLeftHandSocketTransform() const;
	
	FORCEINLINE FCurrentWeaponChanged& OnCurrentWeaponChanged() { return CurrentWeaponChanged; }
	FORCEINLINE FNewWeaponAdded& OnNewWeaponAdded() { return NewWeaponAdded; }
	FORCEINLINE FWeaponRemoved& OnWeaponRemoved() { return WeaponRemoved; }

protected:

	virtual void BeginPlay() override;

private:
	
	TScriptInterface<IWeapon> Internal_FindWeapon(TSubclassOf<class AActor> WeaponClass);
	void Internal_SpawnWeaponFromClass(TSubclassOf<AActor> WeaponClass, EWeaponSlot Slot);
	void Internal_RemoveWeapon(TScriptInterface<IWeapon> Weapon, EWeaponSlot Slot);	
	void Internal_SetCurrentWeapon(TScriptInterface<IWeapon> NewWeapon, class TScriptInterface<IWeapon> LastWeapon = nullptr);

	UPROPERTY()
	TScriptInterface<IWeapon> PrimaryWeapon;
	UPROPERTY()
	TScriptInterface<IWeapon> AlternateWeapon;
	
	UPROPERTY(Transient)
	TScriptInterface<IWeapon> CurrentWeapon;
	
	UPROPERTY(Transient)
	bool bWantsToFire;
	
	FCurrentWeaponChanged CurrentWeaponChanged;
	FNewWeaponAdded NewWeaponAdded;
	FWeaponRemoved WeaponRemoved;	
};