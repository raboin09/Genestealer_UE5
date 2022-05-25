// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapons/HitscanWeapon.h"
#include "NiagaraFunctionLibrary.h"
#include "Characters/BaseCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystemComponent.h"
#include "Niagara/Public/NiagaraComponent.h"
#include "Sound/SoundCue.h"
#include "Utils/CoreUtils.h"
#include "Utils/EffectUtils.h"

void AHitscanWeapon::FireWeapon()
{
	const FVector& AimDirection = GetAdjustedAim();
	const FVector& StartTrace = GetCameraDamageStartLocation(AimDirection);
	FVector ShootDirection = GetShootDirection(AimDirection);
	const FVector& EndTrace = StartTrace + ShootDirection * TraceRange;
	const FHitResult& Impact = WeaponTrace(StartTrace, EndTrace);
	if(TrailParticle)
	{
		TrailParticle->DeactivateImmediate();
	}
	Internal_ProcessInstantHit(Impact, StartTrace, ShootDirection);
}

void AHitscanWeapon::Internal_ProcessInstantHit(const FHitResult& Impact, const FVector& Origin, FVector& ShootDirection)
{
	const FVector EndTrace = Origin + ShootDirection * TraceRange;
	const FVector EndPoint = Impact.GetActor() ? Impact.ImpactPoint : EndTrace;
	Internal_SpawnTrailEffect(EndPoint);
	Internal_SpawnFlybySound(Impact, ShootDirection);
	if(!Impact.GetActor())
	{
		return;
	}
	
	const UClass* HitActorClass = Impact.GetActor()->GetClass();
	if(!HitActorClass->ImplementsInterface(UEffectible::StaticClass()))
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
		FRotator EndRotation = UKismetMathLibrary::FindLookAtRotation(Origin, EndPoint);
		if(TrailFX->IsA(UParticleSystem::StaticClass()))
		{
			TrailParticle = UGameplayStatics::SpawnEmitterAtLocation(this, Cast<UParticleSystem>(TrailFX), Origin, EndRotation);
		} else if(TrailFX->IsA(UNiagaraSystem::StaticClass()))
		{
			TrailParticle = UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, Cast<UNiagaraSystem>(TrailFX), Origin, EndRotation);
		}
		
		if(TrailParticle)
		{
			TrailParticle->SetVectorParameter(TrailTargetParam, EndPoint);	
		}
	}
}

void AHitscanWeapon::Internal_SpawnFlybySound(const FHitResult& Impact, FVector& ShootDir)
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
			TempEffect->DestroyEffect();
		}
	}
}