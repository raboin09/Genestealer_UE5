// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/BaseOverlapProjectile.h"
#include "API/Activatable.h"
#include "Weapons/BaseRangedWeapon.h"
#include "SpawnProjectileAbility.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class GENESTEALER_API ASpawnProjectileAbility : public ABaseRangedWeapon, public IActivatable
{
	GENERATED_BODY()

public:
	ASpawnProjectileAbility();
	
	///////////////////////////////////////////
	/// IActivatable code
	///////////////////////////////////////////
	FORCEINLINE virtual void EnableComboWindow() override{}
	FORCEINLINE virtual void DisableComboWindow() override{}
	virtual void Activate() override;
	virtual void Deactivate() override;
	
protected:
	///////////////////////////////////////////
	/// ABaseWeapon code
	///////////////////////////////////////////
	FORCEINLINE virtual void FireWeapon() override {}
	FORCEINLINE virtual void StopSimulatingWeaponFire() override {}
	FORCEINLINE virtual float SimulateWeaponFire() override { return 0.f; }
	FORCEINLINE virtual void StopFire() override {}
	virtual void StartFire() override;

	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Fire")
	TSubclassOf<class ABaseOverlapProjectile> ProjectileClass;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Fire", meta=(EditCondition = "ProjectileClass != nullptr", EditConditionHides))
	bool bSlowDownProjectileOnAIShooters = true;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Fire", meta=(ClampMin = "1", EditCondition = "bSlowDownProjectileOnAIShooters && ProjectileClass != nullptr", EditConditionHides))
	float AIProjectileSpeedOverride = 3000.f;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Fire", meta=(ClampMin = "1", EditCondition = "ProjectileClass != nullptr", EditConditionHides))
	float ProjectileLife = 10.f;

private:
	FORCEINLINE TArray<TSubclassOf<AActor>> Internal_GetAdditionalEffectsToApplyToProjectile() const { return WeaponEffects; };

	virtual FAnimMontagePlayData Internal_GetPlayData() const override;
	void Internal_StartAttack();
	void Internal_StopAttack();
	virtual ABaseOverlapProjectile* Internal_SpawnProjectile(const FVector& SpawnOrigin, const FVector& ProjectileVelocity);
};
