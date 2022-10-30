// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/SpawnProjectileAbility.h"

#include "Actors/BaseOverlapProjectile.h"
#include "Kismet/KismetMathLibrary.h"
#include "Utils/CoreUtils.h"
#include "Utils/WorldUtils.h"

ASpawnProjectileAbility::ASpawnProjectileAbility()
{
	// ABaseWeapon
	bForceAimOnFire = false;
	TimeBetweenShots = .5f;
	WeaponType = EWeaponType::Melee;
	
	// ABaseRangedWeapon
	bHasFiringSpread = false;
	TraceSpread = 0.f;
	FiringSpreadIncrement = 0.f;
	FiringSpreadMax = 0.f;
	RaycastSourceSocketName = "hand_r";
	bRaycastFromWeaponMeshInsteadOfPawnMesh = true;
	bAimOriginIsPlayerEyesInsteadOfWeapon = true;
}

void ASpawnProjectileAbility::Deactivate()
{
	Internal_StopAttack();
}

void ASpawnProjectileAbility::ResetActivatable()
{
	UGameplayTagUtils::RemoveTagFromActor(this, TAG_STATE_ATTACK_COMMITTED);
}

void ASpawnProjectileAbility::Activate(TArray<TSubclassOf<AActor>> ActivationEffects)
{
	FVector Origin, ShootDir;
	K2_OnActivated();
	Internal_AimAndShootProjectile(Origin, ShootDir);
	Internal_SpawnProjectile(Origin, ShootDir);
}

void ASpawnProjectileAbility::StartFire()
{	
	if (IsWeaponOnCooldown())
	{
		return;
	}
	
	if(!GetTagContainer().HasTag(TAG_STATE_ATTACK_COMMITTED))
	{
		Internal_StartAttack();
	}
}

FAnimMontagePlayData ASpawnProjectileAbility::GetPlayData()
{
	FAnimMontagePlayData PlayData;	
	PlayData.MontageToPlay = FireAnim;
	return PlayData;
}

void ASpawnProjectileAbility::Internal_StartAttack()
{
	UGameplayTagUtils::AddTagToActor(this, TAG_STATE_ATTACK_COMMITTED);
	const FAnimMontagePlayData PlayData = GetPlayData();
	PlayWeaponAnimation(PlayData);
	K2_OnMontageStarted();
}

void ASpawnProjectileAbility::Internal_StopAttack()
{
	UGameplayTagUtils::RemoveTagFromActor(this, TAG_STATE_ATTACK_COMMITTED);
}

void ASpawnProjectileAbility::Internal_AimAndShootProjectile(FVector& OutSpawnOrigin, FVector& OutVelocity)
{
	OutVelocity = GetShootDirection(GetAdjustedAim());
	if(!IsWeaponPlayerControlled())
	{
		OutVelocity = UKismetMathLibrary::RotateAngleAxis(OutVelocity, AIAdjustYawRotation, FVector(0, 0, 1));
		OutVelocity = UKismetMathLibrary::RotateAngleAxis(OutVelocity, OutVelocity.X > 0 ? -1 * AIAdjustPitchRotation : AIAdjustPitchRotation, FVector(0, 1, 0));
	}
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

ABaseOverlapProjectile* ASpawnProjectileAbility::Internal_SpawnProjectile(const FVector& SpawnOrigin, const FVector& ProjectileVelocity)
{

	FTransform SpawnTrans = FTransform();
	SpawnTrans.SetLocation(SpawnOrigin);
	if (ABaseOverlapProjectile* Projectile = UWorldUtils::SpawnActorToWorld_Deferred<ABaseOverlapProjectile>(this, ProjectileClass, this, GetInstigator(), ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn))
	{
		if(bCanHeadshot)
		{
			RecordStatsEvent(ShotFired);
		}
		
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