﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/ChargeReleaseProjectileWeapon.h"
#include "NiagaraFunctionLibrary.h"
#include "Actors/BaseOverlapProjectile.h"
#include "Components/AudioComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

void AChargeReleaseProjectileWeapon::BeginPlay()
{
	Super::BeginPlay();
	NextFiringMesh = GetWeaponMesh();
}

void AChargeReleaseProjectileWeapon::FireWeapon()
{
	Internal_TryIncreaseChargeState();
}

float AChargeReleaseProjectileWeapon::SimulateWeaponFire()
{
	if(!ChargingNiagara && FireFXClass)
	{
		ChargingNiagara = UNiagaraFunctionLibrary::SpawnSystemAttached(Cast<UNiagaraSystem>(FireFXClass), NextFiringMesh, RaycastSourceSocketName, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTargetIncludingScale, true);
		if(ChargingNiagara)
		{
			ChargingNiagara->SetVariableInt(ChargeBlastNiagaraSmokeSpawnMaxName, SmokeSpawnMax * CurrentChargeState);
			ChargingNiagara->SetVariableInt(ChargeBlastNiagaraSmokeSpawnMinName, SmokeSpawnMin * CurrentChargeState);
			ChargingNiagara->SetVariableFloat(ChargeNiagaraMaxChargeTimeName, 1 + (MaxChargedState));
			ChargingNiagara->SetVariableVec2(ChargeNiagaraScaleMaxName, ChargeBlastScaleMax * CurrentChargeState);
			ChargingNiagara->SetVariableVec2(ChargeNiagaraScaleMinName, ChargeBlastScaleMin * CurrentChargeState);
		}
	}

	if(!ChargingAudio && CurrentChargeState >= WhatChargeStateToPlayChargingSound)
	{
		if(ChargeSoundDelay <= 0.f)
		{
			Internal_PlayChargeAudio();
		} else
		{
			GetWorldTimerManager().SetTimer(TimerHandle_DelayChargeAudio,  this, &AChargeReleaseProjectileWeapon::Internal_PlayChargeAudio, ChargeSoundDelay, false);
		}
	}
	return 0.f;
}

void AChargeReleaseProjectileWeapon::OnBurstFinished()
{
	Super::OnBurstFinished();
	GetWorldTimerManager().ClearTimer(TimerHandle_DelayChargeAudio);
	if(CurrentChargeState > -1.f)
	{
		Internal_FireAndReset();
	}
}

TArray<TSubclassOf<AActor>> AChargeReleaseProjectileWeapon::GetAdditionalEffectsToApplyToProjectile() const
{
	if(CurrentChargeState < WeaponEffects.Num())
	{
		
		 return { WeaponEffects[CurrentChargeState] };
	}
	return {};
}

void AChargeReleaseProjectileWeapon::Internal_TryIncreaseChargeState()
{
	CurrentChargeState++;
	if(CurrentChargeState >= MaxChargedState)
	{
		Internal_FireAndReset();
	}
}

void AChargeReleaseProjectileWeapon::Internal_FireAndReset()
{
	Internal_PlayChargeBlastVFX();
	PlayWeaponSound(FireFinishSound);
	if(ChargingNiagara)
	{
		ChargingNiagara->Deactivate();
		ChargingNiagara = nullptr;
	}
	if(ChargingAudio)
	{
		ChargingAudio->FadeOut(0.3f, 0.0f);
		ChargingAudio = nullptr;
	}
	
	if(ABaseOverlapProjectile* Projectile = HandleProjectileFire())
	{
		const float StateScale = CurrentChargeState + 1;
		Projectile->SetActorScale3D(FVector(StateScale, StateScale, StateScale));
	}
	// K2_PlayCooldownEffects(CurrentChargeState + TimeBetweenShots);
	if(CurrentChargeState > 0)
	{
		K2_PlayCooldownEffects(TimeBetweenShots);
	}
	CurrentChargeState = -1;
}

void AChargeReleaseProjectileWeapon::Internal_PlayChargeBlastVFX()
{
	if(ChargeBlastVFX)
	{
		ChargeBlastNiagara = UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ChargeBlastVFX, NextFiringMesh ? NextFiringMesh->GetSocketLocation(RaycastSourceSocketName) : FVector::ZeroVector, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true);
		if(ChargeBlastNiagara)
		{
			ChargeBlastNiagara->SetVariableInt(ChargeBlastNiagaraSmokeSpawnMaxName, SmokeSpawnMax * CurrentChargeState);
			ChargeBlastNiagara->SetVariableInt(ChargeBlastNiagaraSmokeSpawnMinName, SmokeSpawnMin * CurrentChargeState);
			ChargeBlastNiagara->SetVariableVec2(ChargeBlastNiagaraScaleMaxName, ChargeBlastScaleMax * CurrentChargeState);
			ChargeBlastNiagara->SetVariableVec2(ChargeBlastNiagaraScaleMinName, ChargeBlastScaleMin * CurrentChargeState);
		}
	}
}

void AChargeReleaseProjectileWeapon::Internal_PlayChargeAudio()
{
	ChargingAudio = PlayWeaponSound(FireSound);	
}
