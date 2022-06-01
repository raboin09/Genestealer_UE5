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

	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Fire")
	TSubclassOf<class ABaseOverlapProjectile> ProjectileClass;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Fire", meta=(EditCondition = "ProjectileClass != nullptr", EditConditionHides))
	bool bSlowDownProjectileOnAIShooters = true;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Fire", meta=(ClampMin = "1", EditCondition = "ProjectileClass != nullptr", EditConditionHides))
	float ProjectileLife = 10.f;

private:
	virtual void Internal_SpawnProjectile();
};