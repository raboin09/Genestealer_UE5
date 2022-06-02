// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/ProjectileWeapon.h"
#include "GameFramework/Character.h"
#include "Actors/BaseOverlapProjectile.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Utils/SpawnUtils.h"

void AProjectileWeapon::FireWeapon()
{
	Internal_SpawnProjectile();
}

void AProjectileWeapon::Internal_SpawnProjectile()
{
	FVector ShootDir = GetAdjustedAim();
	FVector Origin = GetRaycastOriginLocation();
	const FVector StartTrace = GetCameraDamageStartLocation(ShootDir);
	const FVector EndTrace = StartTrace + ShootDir * TraceRange;
	if (FHitResult Impact = WeaponTrace(StartTrace, EndTrace, ShouldLineTrace(), RadiusOfAimAdjust); Impact.bBlockingHit)
	{
		const FVector AdjustedDir = (Impact.ImpactPoint - Origin);
		bool bWeaponPenetration = false;

		if (const float DirectionDot = FVector::DotProduct(AdjustedDir, ShootDir); DirectionDot < 0.0f)
		{
			bWeaponPenetration = true;
		}
		else if (DirectionDot < 0.5f)
		{
			FVector MuzzleStartTrace = Origin - GetRaycastOriginRotation() * 25.0f;
			FVector MuzzleEndTrace = Origin;
			if (FHitResult MuzzleImpact = WeaponTrace(MuzzleStartTrace, MuzzleEndTrace, ShouldLineTrace(), RadiusOfAimAdjust); MuzzleImpact.bBlockingHit)
			{
				bWeaponPenetration = true;
			}
		}

		if (bWeaponPenetration)
		{
			Origin = Impact.ImpactPoint - ShootDir * 10.0f;
		}
		else
		{
			ShootDir = AdjustedDir;
		}
	}
	FTransform SpawnTrans = FTransform();
	SpawnTrans.SetLocation(Origin);
	if (ABaseOverlapProjectile* Projectile = USpawnUtils::SpawnActorToWorld_Deferred<ABaseOverlapProjectile>(this, ProjectileClass, this, GetInstigator(), ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn))
	{
		Projectile->InitVelocity(ShootDir);
		Projectile->SetLifeSpan(ProjectileLife);
		Projectile->AddAdditionalEffectsToApply(WeaponEffects);
		USpawnUtils::FinishSpawningActor_Deferred(Projectile, SpawnTrans);
		if(UProjectileMovementComponent* ProjectileMovementComponent = Projectile->FindComponentByClass<UProjectileMovementComponent>())
		{
			if(!IsWeaponPlayerControlled() && bSlowDownProjectileOnAIShooters)
			{
				// Make projectiles slower for easier dodging
				ProjectileMovementComponent->InitialSpeed = AIProjectileSpeedOverride;
				ProjectileMovementComponent->MaxSpeed = AIProjectileSpeedOverride;
			}
		}
	}
}
