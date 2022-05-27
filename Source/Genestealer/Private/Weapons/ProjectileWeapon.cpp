// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/ProjectileWeapon.h"

#include "Actors/BaseOverlapProjectile.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

void AProjectileWeapon::FireWeapon()
{
	SpawnProjectile();
}

void AProjectileWeapon::SpawnProjectile(bool bShouldSphereTrace)
{
	FVector ShootDir = GetAdjustedAim();
	FVector Origin = GetRaycastOriginLocation();
	constexpr float ProjectileAdjustRange = 10000.0f;
	const FVector StartTrace = GetCameraDamageStartLocation(ShootDir);
	const FVector EndTrace = StartTrace + ShootDir * ProjectileAdjustRange;
	FHitResult Impact = WeaponTrace(StartTrace, EndTrace, ShouldLineTrace());
	if (Impact.bBlockingHit)
	{
		const FVector AdjustedDir = (Impact.ImpactPoint - Origin);//.GetSafeNormal();
		bool bWeaponPenetration = false;
		
		const float DirectionDot = FVector::DotProduct(AdjustedDir, ShootDir);
		if (DirectionDot < 0.0f)
		{
			bWeaponPenetration = true;
		}
		else if (DirectionDot < 0.5f)
		{
			FVector MuzzleStartTrace = Origin - GetRaycastOriginRotation() * 150.0f;
			FVector MuzzleEndTrace = Origin;
			FHitResult MuzzleImpact = WeaponTrace(MuzzleStartTrace, MuzzleEndTrace, ShouldLineTrace());
			if (MuzzleImpact.bBlockingHit)
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
	if (ABaseOverlapProjectile* Projectile = Cast<ABaseOverlapProjectile>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, ProjectileClass, SpawnTrans)))
	{
		Projectile->SetInstigator(GetInstigator());
		Projectile->SetOwner(this);
		Projectile->SetActorOwner(GetOwningPawn());	
		Projectile->InitVelocity(ShootDir);
		UGameplayStatics::FinishSpawningActor(Projectile, SpawnTrans);
	}
}