// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Effects/BaseEffect.h"
#include "NiagaraComponent.h"
#include "API/AmmoEntity.h"
#include "Weapons/BaseWeapon.h"
#include "BaseRangedWeapon.generated.h"

class ABaseImpactEffect;

UCLASS(Abstract, NotBlueprintable)
class GENESTEALER_API ABaseRangedWeapon : public ABaseWeapon, public IAmmoEntity
{
	GENERATED_BODY()

public:
	virtual bool CanReload() override;
    
protected:
	virtual void BeginPlay() override;
	virtual void StartReload() override;
	virtual void StopReload() override;
	virtual void StopFire() override;
	virtual void OnEquipFinished() override;
	virtual void OnEquip(const TScriptInterface<IWeapon> LastWeapon) override;
	virtual float SimulateWeaponFire() override;
	virtual void StopSimulatingWeaponFire() override;
	virtual void GiveAmmo(int AddAmount) override;
	virtual void OnUnEquip() override;
	virtual bool CanFire() const override;
	virtual void BroadcastAmmoUsage() override;
	virtual void HandleFiring() override;
	virtual void DetermineWeaponState() override;
	
	virtual FHitResult AdjustHitResultIfNoValidHitComponent(const FHitResult& Impact);
	virtual void UseAmmo();
	virtual void ReloadWeapon();
	
	FVector GetRaycastOriginLocation();
	FVector GetRaycastOriginRotation();
	FRotator GetRaycastSocketRotation() const;
	FVector GetShootDirection(const FVector& AimDirection);
	FVector GetAdjustedAim() const;
	FVector GetCameraDamageStartLocation(const FVector& AimDirection);
	FHitResult AdjustHitResultIfNoValidHitComponent(const FHitResult& Impact) const;
	float GetCurrentSpread() const;
	float GetCurrentFiringSpreadPercentage() const;
	bool ShouldLineTrace() const;
	FHitResult WeaponTrace(const FVector& StartTrace, const FVector& EndTrace, bool bLineTrace, float CircleRadius = 5.f) const;

	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Fire")
	float TraceRange = 10000.f;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Fire")
	bool bHasFiringSpread = true;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Fire", meta = (EditCondition = "bHasFiringSpread", EditConditionHides))
	float TraceSpread = 5.f;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Fire", meta = (EditCondition = "bHasFiringSpread", EditConditionHides))
	float FiringSpreadIncrement = 1.0f;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Fire", meta = (EditCondition = "bHasFiringSpread", EditConditionHides))
	float FiringSpreadMax = 10.f;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Fire")
	FName RaycastSourceSocketName = "Muzzle";
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Fire")
	bool bRaycastFromWeaponMeshInsteadOfPawnMesh = true;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Fire")
	bool bAimOriginIsPlayerEyesInsteadOfWeapon = true;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Fire")
	bool bAkimbo = false;

	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Ammo")
	bool bInfiniteAmmo = true;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Ammo")
	bool bInfiniteClip = false;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Ammo", meta = (EditCondition = "!bInfiniteAmmo", EditConditionHides))
	int32 MaxAmmo = 100;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Ammo", meta = (EditCondition = "!bInfiniteClip", EditConditionHides))
	int32 AmmoPerClip = 20;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Ammo", meta = (EditCondition = "!bInfiniteAmmo", EditConditionHides))
	int32 InitialClips = 4;
	
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|VFX")
	UFXSystemAsset* FireFXClass;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|VFX", meta = (EditCondition = "FireFXClass != nullptr", EditConditionHides))
	bool bLoopedMuzzleFX = false;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|VFX", meta = (EditCondition = "FireFXClass != nullptr", EditConditionHides))
	bool bAdjustVFXScaleOnSpawn = false;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|VFX", meta = (EditCondition = "bAdjustVFXScaleOnSpawn", EditConditionHides))
	FString MaxSpawnScaleName = "User.SpawnScaleMax";
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|VFX", meta = (EditCondition = "bAdjustVFXScaleOnSpawn", EditConditionHides))
	FVector MaxVFXScaleAdjust = FVector(.1, .1, 30);
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|VFX", meta = (EditCondition = "bAdjustVFXScaleOnSpawn", EditConditionHides))
	float ParticleMeshZ = 33.f;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|VFX")
	bool bDeactivateVFXImmediately = false;

	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Sound")
	USoundCue* FireSound;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Sound", meta = (EditCondition = "FireSound != nullptr", EditConditionHides))
	bool bLoopedFireSound = true;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Sound", meta = (EditCondition = "FireSound != nullptr", EditConditionHides))
	USoundCue* FireFinishSound;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Sound", meta = (EditCondition = "WeaponType != EWeaponType::Melee", EditConditionHides))
	USoundCue* ReloadSound;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Sound", meta = (EditCondition = "WeaponType != EWeaponType::Melee", EditConditionHides))
	USoundCue* OutOfAmmoSound;
	
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Animation")
	UAnimMontage* FireAnim;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Animation", meta = (EditCondition = "FireAnim != nullptr", EditConditionHides))
	bool bLoopedFireAnim = true;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Animation")
	UAnimMontage* CoverFireRightAnim;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Animation")
	UAnimMontage* CoverFireLeftAnim;

	UPROPERTY(Transient)
	UNiagaraComponent* NiagaraFX;
private:
	UFUNCTION()
	UFXSystemComponent* Internal_PlayParticleFireEffects();
	UFUNCTION()
	UFXSystemComponent* Internal_PlayNiagaraFireEffects();
	UFUNCTION()
	FAnimMontagePlayData Internal_GetPlayData() const;
	UFUNCTION()
	void Internal_AlternateFiringMesh();
	bool Internal_IsInCover() const;
	bool Internal_HasRightInput() const;
	
	UPROPERTY(Transient)
	bool bSecondaryWeaponsTurn;
	UPROPERTY(Transient)
	UAnimMontage* CurrentMontage;
	UPROPERTY(Transient)
	UFXSystemComponent* FireFXSystem;
	UPROPERTY(Transient)
	UParticleSystemComponent* ParticleFX;
	UPROPERTY(Transient)
	UAudioComponent* ReloadAudio;
	UPROPERTY(Transient)
	UAudioComponent* FireAC;
	UPROPERTY(Transient)
	float CurrentFiringSpread;
	UPROPERTY(Transient)
	bool bPendingReload;
	UPROPERTY(Transient)
	int32 CurrentAmmo;
	UPROPERTY(Transient)
	int32 CurrentAmmoInClip;

	UPROPERTY()
	UMeshComponent* NextFiringMesh;
	UPROPERTY()
	FAmmoAmountChanged AmmoAmountChanged;

	FTimerHandle TimerHandle_StopReload;
	FTimerHandle TimerHandle_ReloadWeapon;
public:
	FORCEINLINE virtual bool CheckChildFireCondition() override { return GetCurrentAmmoInClip() > 0 || HasInfiniteClip(); }
	FORCEINLINE	virtual int32 GetCurrentAmmo() override { return HasInfiniteAmmo() ? 1 : CurrentAmmo; }
	FORCEINLINE virtual int32 GetCurrentAmmoInClip() override { return HasInfiniteClip() ? 1 : CurrentAmmoInClip; }
	FORCEINLINE virtual int32 GetMaxAmmo() override { return HasInfiniteAmmo() ? 1 : MaxAmmo; }
	FORCEINLINE virtual int32 GetAmmoPerClip() override { return HasInfiniteClip() ? 1 : AmmoPerClip; }
	FORCEINLINE virtual bool HasInfiniteAmmo() override { return bInfiniteAmmo; }
	FORCEINLINE virtual bool HasInfiniteClip() override { return bInfiniteClip; }
	FORCEINLINE virtual void OnBurstFinished() override { Super::OnBurstFinished(); }
	FORCEINLINE virtual FAmmoAmountChanged& OnAmmoAmountChanged() override { return AmmoAmountChanged; }
};