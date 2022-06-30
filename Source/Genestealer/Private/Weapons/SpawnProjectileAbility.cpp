// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/SpawnProjectileAbility.h"

#include "Actors/BaseOverlapProjectile.h"
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

void ASpawnProjectileAbility::Activate()
{
	const FVector Origin = GetRaycastOriginLocation();
	const FVector Direction = GetAdjustedAim();
	Internal_SpawnProjectile(Origin, Direction);
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

FAnimMontagePlayData ASpawnProjectileAbility::GetPlayData() const
{
	FAnimMontagePlayData PlayData;	
	PlayData.MontageToPlay = FireAnim;
	return PlayData;
}

void ASpawnProjectileAbility::Internal_StartAttack()
{
	GetTagContainer().AddTag(TAG_STATE_ATTACK_COMMITTED);
	const FAnimMontagePlayData PlayData = GetPlayData();
	PlayWeaponAnimation(PlayData);
}

void ASpawnProjectileAbility::Internal_StopAttack()
{
	GetTagContainer().RemoveTag(TAG_STATE_ATTACK_COMMITTED);
}

ABaseOverlapProjectile* ASpawnProjectileAbility::Internal_SpawnProjectile(const FVector& SpawnOrigin, const FVector& ProjectileVelocity)
{
	FTransform SpawnTrans = FTransform();
	SpawnTrans.SetLocation(SpawnOrigin);
	if (ABaseOverlapProjectile* Projectile = UWorldUtils::SpawnActorToWorld_Deferred<ABaseOverlapProjectile>(this, ProjectileClass, this, GetInstigator(), ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn))
	{
		Projectile->InitVelocity(ProjectileVelocity);
		Projectile->SetLifeSpan(ProjectileLife);
		Projectile->AddAdditionalEffectsToApply(Internal_GetAdditionalEffectsToApplyToProjectile());
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
