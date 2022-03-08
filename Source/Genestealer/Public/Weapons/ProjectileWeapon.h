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
	virtual void SpawnProjectile(bool bShouldSphereTrace = true);

	UPROPERTY(EditDefaultsOnly, Category="Weapon|Projectile")
	TSubclassOf<ABaseActor> ProjectileClass;
	UPROPERTY(EditDefaultsOnly, Category="Weapon|Projectile")
	float ProjectileLife = -1.f;
};