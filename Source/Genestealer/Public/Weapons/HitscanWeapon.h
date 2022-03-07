#pragma once

#include "CoreMinimal.h"
#include "BaseRangedWeapon.h"
#include "HitscanWeapon.generated.h"

class ABaseImpactEffect;

USTRUCT()
struct FInstantHitInfo
{
	GENERATED_USTRUCT_BODY()

    UPROPERTY()
	FVector Origin;

	UPROPERTY()
	float ReticleSpread;

	UPROPERTY()
	int32 RandomSeed;
};

USTRUCT()
struct FInstantWeaponData: public FRangedWeaponData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category="Weapon|WeaponStat")
	float WeaponRange;
	
    UPROPERTY(EditDefaultsOnly, Category="Weapon|Accuracy")
	float WeaponSpread;
	UPROPERTY(EditDefaultsOnly, Category="Weapon|Accuracy")
	float TargetingSpreadMod;
	UPROPERTY(EditDefaultsOnly, Category="Weapon|Accuracy")
	float FiringSpreadIncrement;
	UPROPERTY(EditDefaultsOnly, Category="Weapon|Accuracy")
	float FiringSpreadMax;

	UPROPERTY(EditDefaultsOnly, Category="Weapon|HitVerification")
	float AllowedViewDotHitDir;

	UPROPERTY(EditDefaultsOnly, Category="Weapon|Effects")
	UParticleSystem* TrailFX;
	UPROPERTY(EditDefaultsOnly, Category="Weapon|Effects")
	UCurveFloat* DegreesCurve;
	UPROPERTY(EditDefaultsOnly, Category="Weapon|Effects")
	USoundCue* FlyBySound;
	UPROPERTY(EditDefaultsOnly, Category="Weapon|Effects")
	FName TrailTargetParam;
	
	/** defaults */
	FInstantWeaponData()
	{
		WeaponRange = 0;
		DegreesCurve = nullptr;
		FlyBySound = nullptr;
		WeaponSpread = 5.0f;
		TargetingSpreadMod = 0.25f;
		TrailFX = nullptr;
		FiringSpreadIncrement = 1.0f;
		FiringSpreadMax = 10.0f;
		AllowedViewDotHitDir = 0.8f;
	}
};

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
	virtual void OnBurstFinished() override;

	FORCEINLINE virtual FWeaponData GetWeaponData() override { return FWeaponData(RangedWeaponInstant); }
	FORCEINLINE virtual FRangedWeaponData GetRangedWeaponData() override { return FRangedWeaponData(RangedWeaponInstant); }

private:
	void OnHit(const FHitResult& Impact, FVector_NetQuantizeNormal ShootDir, int32 RandomSeed, float ReticleSpread);
	void OnMiss(FVector_NetQuantizeNormal ShootDir, int32 RandomSeed, float ReticleSpread);

	void ProcessInstantHit(const FHitResult& Impact, const FVector& Origin, FVector& ShootDir, int32 RandomSeed, float ReticleSpread);
	void ProcessInstantHit_Confirmed(const FHitResult& Impact, const FVector& Origin, FVector& ShootDir, int32 RandomSeed, float ReticleSpread);

	void SpawnTrailEffect(const FVector& EndPoint);
	void SpawnFlybySound(const FHitResult& Impact, FVector& ShootDir) const;

	float GetCurrentSpread() const;
	float GetCurrentFiringSpreadPercentage() const;
	
	UPROPERTY(EditDefaultsOnly, Category="Weapon|Config")
	FInstantWeaponData RangedWeaponInstant;

	UPROPERTY(Transient)
	FInstantHitInfo HitNotify;
	UPROPERTY(Transient)
	float CurrentFiringSpread;
	
};