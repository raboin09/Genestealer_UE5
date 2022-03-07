// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Effects/BaseEffect.h"
#include "Weapons/BaseWeapon.h"
#include "BaseRangedWeapon.generated.h"

class ABaseImpactEffect;
USTRUCT(BlueprintType)
struct FRangedWeaponData : public FWeaponData
{
	GENERATED_USTRUCT_BODY()

	FRangedWeaponData()
	{
		FireFX = nullptr;
		FireSound = nullptr;
		FireLoopSound = nullptr;
		FireFinishSound = nullptr;
		FireAnim = nullptr;
	}

	UPROPERTY(EditDefaultsOnly, Category="Weapon|Ammo")
    bool bInfiniteAmmo = true;
    UPROPERTY(EditDefaultsOnly, Category="Weapon|Ammo")
    bool bInfiniteClip = false;
    UPROPERTY(EditDefaultsOnly, Category="Weapon|Ammo")
    int32 MaxAmmo = 100;
    UPROPERTY(EditDefaultsOnly, Category="Weapon|Ammo")
    int32 AmmoPerClip = 20;
    UPROPERTY(EditDefaultsOnly, Category="Weapon|Ammo")
    int32 InitialClips = 4;

	UPROPERTY(EditDefaultsOnly, Category="Weapon|Effects")
	FName MuzzleAttachPoint = "Muzzle";	
	UPROPERTY(EditDefaultsOnly, Category="Weapon|Effects")
	UFXSystemAsset* FireFX;
	UPROPERTY(EditDefaultsOnly, Category="Weapon|Effects")
	FRotator AltFireRotation;
	UPROPERTY(EditDefaultsOnly, Category="Weapon|Effects")
	bool bLoopedMuzzleFX = true;

	UPROPERTY(EditDefaultsOnly, Category="Weapon|Sound")
	USoundCue* FireSound;
	UPROPERTY(EditDefaultsOnly, Category="Weapon|Sound")
	USoundCue* FireLoopSound;
	UPROPERTY(EditDefaultsOnly, Category="Weapon|Sound")
	bool bLoopedFireSound = true;
	UPROPERTY(EditDefaultsOnly, Category="Weapon|Sound")
	USoundCue* FireFinishSound;

	UPROPERTY(EditDefaultsOnly, Category="Weapon|Animation")
	UAnimMontage* FireAnim;
	UPROPERTY(EditDefaultsOnly, Category="Weapon|Animation")
	bool bLoopedFireAnim = true;
};

UCLASS(Abstract, NotBlueprintable)
class GENESTEALER_API ABaseRangedWeapon : public ABaseWeapon
{
	GENERATED_BODY()

public:
	virtual bool CanReload() override;
    
protected:
	virtual void BeginPlay() override;
	
	virtual void SimulateWeaponFire() override;
	virtual void StopSimulatingWeaponFire() override;
	virtual void ReloadWeapon() override;
	virtual void GiveAmmo(int AddAmount) override;
	virtual void UseAmmo() override;
	virtual void OnUnEquip() override;
	virtual void BroadcastAmmoUsage() override;
	virtual FHitResult AdjustHitResultIfNoValidHitComponent(const FHitResult& Impact);
	
	FVector GetMuzzleLocation();
	FVector GetMuzzleDirection();
	
	virtual FVector GetAdjustedAim();
    FVector GetCameraAim() const;
    FVector GetCameraDamageStartLocation(const FVector& AimDir);
    FHitResult WeaponTrace(const FVector& TraceFrom, const FVector& TraceTo, bool bSphereTrace = true) const;

	UFUNCTION(BlueprintImplementableEvent)
    void DebugFire(FVector Origin, FVector End, FColor ColorToDraw);

	virtual FRangedWeaponData GetRangedWeaponData() PURE_VIRTUAL(ABaseRangedWeapon::GetRangedWeaponData, return FRangedWeaponData();)
	
private:

	UPROPERTY(Transient)
	UFXSystemComponent* FireFX;
	UPROPERTY(Transient)
	UAudioComponent* FireAC;

	UPROPERTY(Transient)
	int32 CurrentAmmo;
	UPROPERTY(Transient)
	int32 CurrentAmmoInClip;

	UPROPERTY(Transient) 
	bool bPlayingFireAnim;

public:
	FORCEINLINE	virtual int32 GetCurrentAmmo() override { return CurrentAmmo; }
	FORCEINLINE virtual int32 GetCurrentAmmoInClip() override { return CurrentAmmoInClip; }
	FORCEINLINE virtual int32 GetMaxAmmo() override { return GetRangedWeaponData().MaxAmmo; }
	FORCEINLINE virtual int32 GetAmmoPerClip() override { return GetRangedWeaponData().AmmoPerClip; }
	FORCEINLINE virtual bool HasInfiniteAmmo() override { return GetRangedWeaponData().bInfiniteAmmo; }
	FORCEINLINE virtual bool HasInfiniteClip() override { return GetRangedWeaponData().bInfiniteClip; }
	FORCEINLINE virtual void OnBurstFinished() override { Super::OnBurstFinished(); }
};