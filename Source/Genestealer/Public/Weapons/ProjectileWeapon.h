// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/BaseRangedWeapon.h"
#include "ProjectileWeapon.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class GENESTEALER_API AProjectileWeapon : public ABaseRangedWeapon
{
	GENERATED_BODY()
	
protected:
	virtual void FireWeapon() override;
	virtual class ABaseOverlapProjectile* HandleProjectileFire();
	
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Fire")
	TSubclassOf<class ABaseOverlapProjectile> ProjectileClass;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Fire", meta=(EditCondition = "ProjectileClass != nullptr", EditConditionHides))
	bool bSlowDownProjectileOnAIShooters = true;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Fire", meta=(ClampMin = "1", EditCondition = "bSlowDownProjectileOnAIShooters && ProjectileClass != nullptr", EditConditionHides))
	float AIProjectileSpeedOverride = 3000.f;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Fire", meta=(ClampMin = "1", EditCondition = "ProjectileClass != nullptr", EditConditionHides))
	float ProjectileLife = 10.f;

private:
	FORCEINLINE virtual TArray<TSubclassOf<AActor>> Internal_GetAdditionalEffectsToApplyToProjectile() const { return WeaponEffects; };
	virtual void Internal_AimAndShootProjectile(FVector& OutSpawnOrigin, FVector& OutVelocity);
	virtual ABaseOverlapProjectile* Internal_SpawnProjectile(const FVector& SpawnOrigin, const FVector& ProjectileVelocity);
};