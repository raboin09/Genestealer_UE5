// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Effects/BaseEffect.h"
#include "NiagaraComponent.h"
#include "Weapons/BaseWeapon.h"
#include "BaseRangedWeapon.generated.h"

class ABaseImpactEffect;

UCLASS(Abstract, NotBlueprintable)
class GENESTEALER_API ABaseRangedWeapon : public ABaseWeapon
{
	GENERATED_BODY()

public:
	virtual bool CanReload() override;
    
protected:
	virtual void BeginPlay() override;
	
	virtual float SimulateWeaponFire() override;
	virtual void StopSimulatingWeaponFire() override;
	virtual void ReloadWeapon() override;
	virtual void GiveAmmo(int AddAmount) override;
	virtual void UseAmmo() override;
	virtual void OnUnEquip() override;
	virtual void BroadcastAmmoUsage() override;
	virtual FHitResult AdjustHitResultIfNoValidHitComponent(const FHitResult& Impact);
	
	FVector GetRaycastOriginLocation();
	FVector GetRaycastOriginRotation();
	FRotator GetRaycastSocketRotation() const;
	FVector GetShootDirection(const FVector& AimDirection);
	FVector GetAdjustedAim() const;
	FVector GetCameraDamageStartLocation(const FVector& AimDirection);
	FHitResult AdjustHitResultIfNoValidHitComponent(const FHitResult& Impact) const;
	float GetCurrentSpread() const;
	float GetCurrentFiringSpreadPercentage() const;
	FHitResult WeaponTrace(const FVector& StartTrace, const FVector& EndTrace) const;

	UFUNCTION(BlueprintImplementableEvent)
    void DebugFire(FVector Origin, FVector End, FColor ColorToDraw);

	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Fire")
	float TraceSpread = 5.f;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Fire")
	float TraceRange = 10000.f;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Fire")
	bool bRaycastFromWeapon = true;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Fire")
	bool bInitialRaycastFromController = true;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Fire")
	float TargetingSpreadMod;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Fire")
	float FiringSpreadIncrement = 1.0f;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Fire")
	float FiringSpreadMax = 10.f;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Fire")
	bool bAkimbo = false;

	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Ammo")
	bool bInfiniteAmmo = true;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Ammo")
	bool bInfiniteClip = false;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Ammo")
	int32 MaxAmmo = 100;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Ammo")
	int32 AmmoPerClip = 20;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Ammo")
	int32 InitialClips = 4;

	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|VFX")
	FName RaycastSourceSocketName = "Muzzle";
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|VFX")
	UFXSystemAsset* FireFXClass;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|VFX")
	bool bLoopedMuzzleFX = false;

	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Sound")
	USoundCue* FireSound;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Sound")
	bool bLoopedFireSound = true;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Sound")
	USoundCue* FireFinishSound;

	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Animation")
	UAnimMontage* FireAnim;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Animation")
	UAnimMontage* CoverFireRightAnim;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Animation")
	UAnimMontage* CoverFireLeftAnim;
	
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
	UNiagaraComponent* NiagaraFX;
	UPROPERTY(Transient)
	UAudioComponent* FireAC;
	UPROPERTY(Transient)
	float CurrentFiringSpread;
	UPROPERTY(Transient)
	int32 CurrentAmmo;
	UPROPERTY(Transient)
	int32 CurrentAmmoInClip;

	UPROPERTY()
	UMeshComponent* NextFiringMesh;

public:
	FORCEINLINE	virtual int32 GetCurrentAmmo() override { return CurrentAmmo; }
	FORCEINLINE virtual int32 GetCurrentAmmoInClip() override { return CurrentAmmoInClip; }
	FORCEINLINE virtual int32 GetMaxAmmo() override { return MaxAmmo; }
	FORCEINLINE virtual int32 GetAmmoPerClip() override { return AmmoPerClip; }
	FORCEINLINE virtual bool HasInfiniteAmmo() override { return bInfiniteAmmo; }
	FORCEINLINE virtual bool HasInfiniteClip() override { return bInfiniteClip; }
	FORCEINLINE virtual void OnBurstFinished() override { Super::OnBurstFinished(); }
};