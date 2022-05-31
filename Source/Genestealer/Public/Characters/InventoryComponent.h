// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "API/Weapon.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

class ABaseCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCurrentWeaponChanged, TScriptInterface<IWeapon>, NewWeapon, TScriptInterface<IWeapon>, PreviousWeapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNewWeaponAdded, TScriptInterface<IWeapon>, AddedWeapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWeaponRemoved, TScriptInterface<IWeapon>, RemovedWeapon);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GENESTEALER_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()


public:
	UInventoryComponent();
	
	void SpawnInventoryActors(TSubclassOf<AActor> PrimaryWeaponClass, TSubclassOf<AActor> AlternateWeaponClass);
	void DestroyInventory();
	bool DoesCurrentWeaponForceAimOnFire() const;
	bool CanWeaponAim() const;
	TSubclassOf<AActor> GetAlternateWeaponClass() const;
	TSubclassOf<AActor> GetPrimaryWeaponClass() const;
	
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

	FORCEINLINE EWeaponType GetCurrentWeaponType() const { return CurrentWeapon ? CurrentWeapon->GetWeaponType() : EWeaponType::NONE; }
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	FORCEINLINE TScriptInterface<IWeapon> GetEquippedWeapon() const{ return CurrentWeapon; }
	EWeaponState GetCurrentWeaponState() const;
	
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