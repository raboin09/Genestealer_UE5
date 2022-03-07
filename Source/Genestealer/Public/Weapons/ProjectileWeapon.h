// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/BaseRangedWeapon.h"
#include "ProjectileWeapon.generated.h"

USTRUCT()
struct FProjectileWeaponData : public FRangedWeaponData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<ABaseActor> ProjectileClass;
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	float ProjectileLife = -1.f;
};


/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class GENESTEALER_API AProjectileWeapon : public ABaseRangedWeapon
{
	GENERATED_BODY()

public:
	void ApplyWeaponConfig(FProjectileWeaponData& Data) const;
	
protected:
	virtual void FireWeapon() override;

	virtual void SpawnProjectile(bool bShouldSphereTrace = true);

	FORCEINLINE virtual FWeaponData GetWeaponData() override { return FWeaponData(ProjectileConfig); }
	FORCEINLINE virtual FRangedWeaponData GetRangedWeaponData() override { return FRangedWeaponData(ProjectileConfig); }

private:
	UPROPERTY(EditDefaultsOnly, Category="Weapon|Config")
	FProjectileWeaponData ProjectileConfig;	
};