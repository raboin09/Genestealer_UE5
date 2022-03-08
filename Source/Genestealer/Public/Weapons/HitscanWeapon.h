#pragma once

#include "CoreMinimal.h"
#include "BaseRangedWeapon.h"
#include "HitscanWeapon.generated.h"

class ABaseImpactEffect;
/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class GENESTEALER_API AHitscanWeapon : public ABaseRangedWeapon
{
	GENERATED_BODY()

protected:
	// BaseRangedWeapon overrides
	virtual void FireWeapon() override;

	UPROPERTY(EditDefaultsOnly, Category="Weapon|VFX")
	UFXSystemAsset* TrailFX;
	UPROPERTY(EditDefaultsOnly, Category="Weapon|VFX")
	FName TrailTargetParam = "ShockBeamEnd";
	UPROPERTY(EditDefaultsOnly, Category="Weapon|Sound")
	UCurveFloat* DegreesCurve;
	UPROPERTY(EditDefaultsOnly, Category="Weapon|Sound")
	USoundCue* FlyBySound;
	
private:
	void Internal_ProcessInstantHit(const FHitResult& Impact, const FVector& Origin, FVector& ShootDirection);
	void Internal_SpawnTrailEffect(const FVector& EndPoint);
	void Internal_SpawnFlybySound(const FHitResult& Impact, FVector& ShootDir);
	void Internal_PlayWeaponMissEffectFX(const FHitResult& Impact);
	
};