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
	
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|VFX")
	UFXSystemAsset* TrailFX;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|VFX", meta = (EditCondition = "TrailFX != nullptr", EditConditionHides))
	FName TrailTargetParam = "ShockBeamEnd";
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Sound")
	USoundCue* FlyBySound;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Sound", meta = (EditCondition = "FlyBySound != nullptr", EditConditionHides))
	UCurveFloat* DegreesCurve;
	
private:
	void Internal_ProcessInstantHit(const FHitResult& Impact, const FVector& Origin, FVector& ShootDirection);
	void Internal_SpawnTrailEffect(const FVector& EndPoint);
	void Internal_SpawnFlybySound(const FHitResult& Impact, FVector& ShootDir);
	void Internal_PlayWeaponMissEffectFX(const FHitResult& Impact);

	UPROPERTY()
	UFXSystemComponent* TrailParticle; 
	
};