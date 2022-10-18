// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/ProjectileWeapon.h"
#include "GameFramework/Character.h"
#include "Actors/BaseOverlapProjectile.h"
#include "Core/PlayerStatsComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Utils/CoreUtils.h"
#include "Utils/WorldUtils.h"

AProjectileWeapon::AProjectileWeapon()
{
	bHasFiringSpread = false;
}

void AProjectileWeapon::FireWeapon()
{
	HandleProjectileFire();
	Super::FireWeapon();
}

ABaseOverlapProjectile* AProjectileWeapon::HandleProjectileFire()
{
	FVector Origin, ShootDir;
	Internal_AimAndShootProjectile(Origin, ShootDir);
	return Internal_SpawnProjectile(Origin, ShootDir);
}

void AProjectileWeapon::Internal_AimAndShootProjectile(FVector& OutSpawnOrigin, FVector& OutVelocity)
{
	OutVelocity = GetShootDirection(GetAdjustedAim());
	const FVector StartTrace = GetCameraDamageStartLocation(OutVelocity);
	OutSpawnOrigin = GetRaycastOriginLocation();
	const FVector EndTrace = StartTrace + OutVelocity * TraceRange;
	const float RaycastCircleRadius = UCoreUtils::GetPlayerControllerSphereTraceRadius(this) * 1.5f; 
	if (FHitResult Impact = WeaponTrace(StartTrace, EndTrace, ShouldLineTrace(), RaycastCircleRadius); Impact.bBlockingHit)
	{
		const FVector AdjustedDir = (Impact.ImpactPoint - OutSpawnOrigin);
		bool bWeaponPenetration = false;

		if (const float DirectionDot = FVector::DotProduct(AdjustedDir, OutVelocity); DirectionDot < 0.0f)
		{
			bWeaponPenetration = true;
		}
		else if (DirectionDot < 0.5f)
		{
			FVector MuzzleStartTrace = OutSpawnOrigin - GetRaycastOriginRotation() * 25.0f;
			FVector MuzzleEndTrace = OutSpawnOrigin;
			if (FHitResult MuzzleImpact = WeaponTrace(MuzzleStartTrace, MuzzleEndTrace, ShouldLineTrace(), RaycastCircleRadius); MuzzleImpact.bBlockingHit)
			{
				bWeaponPenetration = true;
			}
		}

		if (bWeaponPenetration)
		{
			OutSpawnOrigin = Impact.ImpactPoint - OutVelocity * 10.0f;
		}
		else
		{
			OutVelocity = AdjustedDir;
		}
	}
}

ABaseOverlapProjectile* AProjectileWeapon::Internal_SpawnProjectile(const FVector& SpawnOrigin, const FVector& ProjectileVelocity)
{

	FTransform SpawnTrans = FTransform();
	SpawnTrans.SetLocation(SpawnOrigin);
	if (ABaseOverlapProjectile* Projectile = UWorldUtils::SpawnActorToWorld_Deferred<ABaseOverlapProjectile>(this, ProjectileClass, this, GetInstigator(), ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn))
	{
		RecordStatsEvent(ShotFired);
		Projectile->InitVelocity(ProjectileVelocity);
		Projectile->SetLifeSpan(ProjectileLife);
		Projectile->AddAdditionalEffectsToApply(Internal_GetAdditionalEffectsToApplyToProjectile());
		Projectile->IgnoreActor(this);
		Projectile->IgnoreActor(GetInstigator());
		for(AActor* TempActor : GetActorsToIgnoreCollision())
		{
			Projectile->IgnoreActor(TempActor);
		}
		UWorldUtils::FinishSpawningActor_Deferred(Projectile, SpawnTrans);
		if(UProjectileMovementComponent* ProjectileMovementComponent = Projectile->FindComponentByClass<UProjectileMovementComponent>())
		{
			if(!IsWeaponPlayerControlled() && bSlowDownProjectileOnAIShooters)
			{
				// Make projectiles slower for easier dodging
				ProjectileMovementComponent->InitialSpeed = AIProjectileSpeedOverride;
				ProjectileMovementComponent->MaxSpeed = AIProjectileSpeedOverride;
			}
		}
		return Projectile;
	}
	return nullptr;
}
