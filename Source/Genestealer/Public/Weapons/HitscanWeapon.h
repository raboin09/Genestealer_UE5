#pragma once

#include "CoreMinimal.h"
#include "BaseRangedWeapon.h"
#include "HitscanWeapon.generated.h"

class ABaseImpactEffect;
/**
 * 
 */
UCLASS(Abstract, Blueprintable, AutoExpandCategories=("Genestealer"), PrioritizeCategories = "Genestealer")
class GENESTEALER_API AHitscanWeapon : public ABaseRangedWeapon
{
	GENERATED_BODY()

protected:
	// BaseRangedWeapon overrides
	virtual void FireWeapon() override;
	virtual void StopSimulatingWeaponFire() override;
	
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Fire", meta = (ClampMin="1", EditCondition = "FiringMechanism == EFiringMechanism::Burst || FiringMechanism == EFiringMechanism::ScatterShot", EditConditionHides))
	int32 NumberOfShotsPerFire = 1;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Fire", meta = (ClampMin=".01", EditCondition = "FiringMechanism == EFiringMechanism::Burst", EditConditionHides))
	float TimeBetweenBurstShots = .15f;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|VFX")
	UFXSystemAsset* TrailFX;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|VFX", meta = (EditCondition = "TrailFX != nullptr", EditConditionHides))
	FName TrailTargetParam = "ShockBeamEnd";
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Sound")
	USoundCue* FlyBySound;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Sound", meta = (EditCondition = "FlyBySound != nullptr", EditConditionHides))
	UCurveFloat* DegreesCurve;
	
private:
	void Internal_FireShot();
	void Internal_ProcessInstantHit(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDirection);
	void Internal_SpawnTrailEffect(const FVector& EndPoint);
	void Internal_SpawnFlybySound(const FHitResult& Impact, FVector& ShootDir) const;
	void Internal_PlayWeaponMissEffectFX(const FHitResult& Impact);

	UPROPERTY()
	TArray<UFXSystemComponent*> TrailParticles;

	///////////////////////
	// Burst Fire
	///////////////////////
	UFUNCTION()
	void Internal_BurstFireTick();
	
	int32 BurstFireCount;
	FTimerHandle TimerHandle_BurstFire;
};