// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapons/HitscanWeapon.h"
#include "NiagaraFunctionLibrary.h"
#include "Characters/BaseCharacter.h"
#include "Characters/EffectContainerComponent.h"
#include "Core/PlayerStatsComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystemComponent.h"
#include "Niagara/Public/NiagaraComponent.h"
#include "Sound/SoundCue.h"
#include "Utils/CombatUtils.h"
#include "Utils/CoreUtils.h"
#include "Utils/EffectUtils.h"

void AHitscanWeapon::FireWeapon()
{
	for(UFXSystemComponent* TrailParticle : TrailParticles)
	{
		if(TrailParticle)
		{
			TrailParticle->DeactivateImmediate();
		}
	}

	if(FiringMechanism == EFiringMechanism::ScatterShot)
	{
		// Fire one shot in the middle for guaranteed hits
		const bool bCacheFiringSpread = bHasFiringSpread;
		bHasFiringSpread = false;
		Internal_FireShot();
		bHasFiringSpread = bCacheFiringSpread;
		for(int i=1; i<NumberOfShotsPerFire; i++)
		{
			Internal_FireShot();
		}
	} else if(FiringMechanism == EFiringMechanism::Burst)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_BurstFire, this, &AHitscanWeapon::Internal_BurstFireTick, TimeBetweenBurstShots, true, 0.f);
	} else if(FiringMechanism == EFiringMechanism::Automatic)
	{
		Internal_FireShot();
	} else if(FiringMechanism == EFiringMechanism::Continuous)
	{
		Internal_FireShot();
	}

	if(FiringMechanism != EFiringMechanism::Continuous)
	{
		RecordStatsEvent(ShotFired, NumberOfShotsPerFire);	
	}
	Super::FireWeapon();
}

void AHitscanWeapon::StopFire()
{
	Super::StopFire();
}

void AHitscanWeapon::StopSimulatingWeaponFire()
{
	if(FiringMechanism == EFiringMechanism::Burst)
	{
		if(BurstFireCount >= NumberOfShotsPerFire)
		{
			Super::StopSimulatingWeaponFire();	
		}
	} else
	{
		Super::StopSimulatingWeaponFire();
	}
}

void AHitscanWeapon::Internal_BurstFireTick()
{
	if(BurstFireCount >= NumberOfShotsPerFire)
	{
		StopSimulatingWeaponFire();
		BurstFireCount = 0;
		GetWorldTimerManager().ClearTimer(TimerHandle_BurstFire);
		return;
	}

	BurstFireCount++;
	if(bSpawnMuzzleFX && !bLoopedMuzzleFX)
	{
		PlayMuzzleFX();
	}
	Internal_FireShot();
}

void AHitscanWeapon::Internal_FireShot()
{
	const FVector& AimDirection = GetAdjustedAim();
	const FVector& StartTrace = GetCameraDamageStartLocation(AimDirection);
	const FVector ShootDirection = GetShootDirection(AimDirection);
	const FVector& EndTrace = StartTrace + ShootDirection * TraceRange;
	const float RaycastCircleRadius = UCoreUtils::GetPlayerControllerSphereTraceRadius(this) * 1.5f; 
	const FHitResult& Impact = WeaponTrace(StartTrace, EndTrace, ShouldLineTrace(), RaycastCircleRadius);
	Internal_ProcessInstantHit(Impact, StartTrace, ShootDirection);
}


void AHitscanWeapon::Internal_ProcessInstantHit(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDirection)
{
	const FVector EndTrace = Origin + ShootDirection * TraceRange;
	const FVector EndPoint = Impact.GetActor() ? Impact.ImpactPoint : EndTrace;
	Internal_SpawnTrailEffect(EndPoint);
	if(!Impact.GetActor() || UCombatUtils::AreActorsAllies(Impact.GetActor(), GetOwningPawn()))
	{
		return;
	}

	if(const UClass* HitActorClass = Impact.GetActor()->GetClass(); !HitActorClass->ImplementsInterface(UAttackable::StaticClass()))
	{
		Internal_PlayWeaponMissEffectFX(Impact);
	} else
	{
		if(FiringMechanism != EFiringMechanism::Continuous)
		{
			RecordStatsEvent(ShotHit, 1.f, Impact.GetActor());
			if(UCombatUtils::IsBoneNameHead(Impact.BoneName))
			{
				RecordStatsEvent(Headshot);
			}
		}
		UEffectUtils::ApplyEffectsToHitResult(WeaponEffects, AdjustHitResultIfNoValidHitComponent(Impact), GetInstigator());
	}
}

void AHitscanWeapon::Internal_SpawnTrailEffect(const FVector& EndPoint)
{
	if (TrailFX)
	{
		const FVector Origin = GetRaycastOriginLocation();
		const FRotator EndRotation = UKismetMathLibrary::FindLookAtRotation(Origin, EndPoint);
		UFXSystemComponent* TempTrailParticle = nullptr;
		if(TrailFX->IsA(UParticleSystem::StaticClass()))
		{
			TempTrailParticle = UGameplayStatics::SpawnEmitterAtLocation(this, Cast<UParticleSystem>(TrailFX), Origin, EndRotation);
		} else if(TrailFX->IsA(UNiagaraSystem::StaticClass()))
		{
			TempTrailParticle = UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, Cast<UNiagaraSystem>(TrailFX), Origin, EndRotation);
		}
		
		if(TempTrailParticle)
		{
			TempTrailParticle->SetVectorParameter(TrailTargetParam, EndPoint);
			TrailParticles.Add(TempTrailParticle);
		}
	}
}

void AHitscanWeapon::Internal_SpawnFlybySound(const FHitResult& Impact, FVector& ShootDir) const
{
	// Translated to C++ from here https://www.youtube.com/watch?v=w7zGmwuRX_Q
	ABasePlayerCharacter* CurrChar = UCoreUtils::GetPlayerCharacter(this);
	if(!CurrChar || CurrChar == GetInstigator() || ! DegreesCurve || ! FlyBySound)
		return;
	
	FVector DiffVector = CurrChar->GetActorLocation() - GetInstigator()->GetActorLocation();
	DiffVector.Normalize();
	ShootDir.Normalize();
	const float LengthPlayer = CurrChar->GetDistanceTo(GetInstigator());
	const float ACosD = UKismetMathLibrary::DegAcos(FVector::DotProduct(DiffVector, ShootDir));
	if(ACosD <= DegreesCurve->GetFloatValue(LengthPlayer))
	{
		const FVector WorldLoc = GetInstigator()->GetRootComponent()->GetComponentLocation();
		FVector DirectionVector = (UKismetMathLibrary::GetDirectionUnitVector(WorldLoc, Impact.ImpactPoint) * LengthPlayer) + WorldLoc;
		FTimerHandle PlayFlybyHandle;
		GetWorld()->GetTimerManager().SetTimer(PlayFlybyHandle, [this, DirectionVector]() {
			UGameplayStatics::PlaySoundAtLocation(GetInstigator(), FlyBySound, DirectionVector, FRotator::ZeroRotator);
		}, LengthPlayer / 31200, false);
	}
}

void AHitscanWeapon::Internal_PlayWeaponMissEffectFX(const FHitResult& Impact)
{
	for(const TSubclassOf<AActor> CurrEffectClass : WeaponEffects)
	{
		if(const TScriptInterface<IEffect> TempEffect = UEffectContainerComponent::CreateEffectInstanceFromHitResult(this, CurrEffectClass, Impact, GetInstigator()))
		{
			TempEffect->PlayEffectFX();
			if(AActor* EffectActor = Cast<AActor>(TempEffect.GetObject()))
			{
				EffectActor->SetLifeSpan(.5f);
			}
		}
	}
}

