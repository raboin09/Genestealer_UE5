// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapons/HitscanWeapon.h"
#include "NiagaraFunctionLibrary.h"
#include "Characters/BaseCharacter.h"
#include "Characters/EffectContainerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystemComponent.h"
#include "Niagara/Public/NiagaraComponent.h"
#include "Sound/SoundCue.h"
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
		for(int i=0; i<NumberOfShotsPerFire; i++)
		{
			Internal_FireShot();
		}
	} else if(FiringMechanism == EFiringMechanism::Burst)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_BurstFire, this, &AHitscanWeapon::Internal_BurstFireTick, TimeBetweenBurstShots, true);
	} else if(FiringMechanism == EFiringMechanism::Automatic)
	{
		Internal_FireShot();
	} else if(FiringMechanism == EFiringMechanism::Continuous)
	{
		Internal_FireShot();
	}
}


void AHitscanWeapon::Internal_BurstFireTick()
{
	if(BurstFireCount >= NumberOfShotsPerFire)
	{
		BurstFireCount = 0;
		StopSimulatingWeaponFire();
		GetWorldTimerManager().ClearTimer(TimerHandle_BurstFire);
		return;
	}

	BurstFireCount++;
	Internal_FireShot();
}

void AHitscanWeapon::Internal_FireShot()
{
	const FVector& AimDirection = GetAdjustedAim();
	const FVector& StartTrace = GetCameraDamageStartLocation(AimDirection);
	const FVector ShootDirection = GetShootDirection(AimDirection);
	const FVector& EndTrace = StartTrace + ShootDirection * TraceRange;
	const FHitResult& Impact = WeaponTrace(StartTrace, EndTrace, ShouldLineTrace());
	Internal_ProcessInstantHit(Impact, StartTrace, ShootDirection);
}


void AHitscanWeapon::Internal_ProcessInstantHit(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDirection)
{
	const FVector EndTrace = Origin + ShootDirection * TraceRange;
	const FVector EndPoint = Impact.GetActor() ? Impact.ImpactPoint : EndTrace;
	Internal_SpawnTrailEffect(EndPoint);
	if(!Impact.GetActor())
	{
		return;
	}

	if(const UClass* HitActorClass = Impact.GetActor()->GetClass(); !HitActorClass->ImplementsInterface(UAttackable::StaticClass()))
	{
		Internal_PlayWeaponMissEffectFX(Impact);
	} else
	{
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

