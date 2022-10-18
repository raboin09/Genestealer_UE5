// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/ChargeReleaseProjectileWeapon.h"
#include "NiagaraFunctionLibrary.h"
#include "Actors/BaseOverlapProjectile.h"
#include "Components/AudioComponent.h"
#include "Core/AudioManager.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

void AChargeReleaseProjectileWeapon::FireWeapon()
{
	if(Internal_IsWeaponOverheated())
	{
		return;
	}
	
	Internal_TryIncreaseChargeState();
	K2_OnFireWeapon();
}

float AChargeReleaseProjectileWeapon::SimulateWeaponFire()
{
	if(Internal_IsWeaponOverheated())
	{
		return 0.f;
	}
	
	if(!ChargingNiagara && FireFXClass)
	{
		ChargingNiagara = UNiagaraFunctionLibrary::SpawnSystemAttached(Cast<UNiagaraSystem>(FireFXClass), GetWeaponMesh(), RaycastSourceSocketName, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTargetIncludingScale, true);
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
	if(CurrentChargeState > -1.f && !Internal_IsWeaponOverheated())
	{
		Internal_FireAndReset();
	}
}

TArray<TSubclassOf<AActor>> AChargeReleaseProjectileWeapon::Internal_GetAdditionalEffectsToApplyToProjectile() const
{
	if(CurrentChargeState < WeaponEffects.Num() && CurrentChargeState > 0)
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
	} else
	{
		K2_ChargeStateChange(CurrentChargeState);
	}
}

void AChargeReleaseProjectileWeapon::Internal_FireAndReset()
{
	Internal_PlayChargeBlastVFX();
	if (FireFinishSound && OwningPawn)
	{
		constexpr float PitchStep = .15f;
		constexpr float BasePitch = .75f;
		const float PitchMod = BasePitch + (MaxChargedState * PitchStep);
		FireFinishSound->PitchMultiplier = PitchMod - (CurrentChargeState * PitchStep);
		UAudioManager::SpawnSoundAttached(FireFinishSound, OwningPawn->GetRootComponent());
	}
	
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

	if(!CurrentMontage || !bLoopedFireAnim)
	{
		FAnimMontagePlayData PlayData;
		if(ChargeFireAnims.Num() - 1 < CurrentChargeState)
		{
			PlayData.MontageToPlay = ChargeFireAnims[ChargeFireAnims.Num() - 1];
		} else
		{
			PlayData.MontageToPlay = ChargeFireAnims[CurrentChargeState];
		}
		PlayWeaponAnimation(PlayData);
	}
	
	if(ABaseOverlapProjectile* Projectile = HandleProjectileFire())
	{
		const float StateScale = (CurrentChargeState * 1.25) + 1;
		Projectile->SetActorScale3D(FVector(StateScale, StateScale, StateScale));
	}

	float OverheatDuration = TimeBetweenShots;
	if(CurrentChargeState > 0)
	{
		OverheatDuration *= (CurrentChargeState * TimeBetweenShots);
		K2_PlayCooldownEffects(OverheatDuration);
	} else
	{
		OverheatDuration = 0;
		LastFireTime = GetWorld()->GetTimeSeconds() - (TimeBetweenShots / 10);
	}
	OverheatUntilTime = GetWorld()->GetTimeSeconds() + OverheatDuration;
	
	CurrentChargeState = -1;
	K2_ChargeStateChange(CurrentChargeState);
	
	if(GetCurrentAmmoInClip() <= 0)
	{
		StartReload();
	}
}

void AChargeReleaseProjectileWeapon::Internal_PlayChargeBlastVFX()
{
	if(ChargeBlastVFX)
	{
		ChargeBlastNiagara = UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ChargeBlastVFX, GetWeaponMesh() ? GetWeaponMesh()->GetSocketLocation(RaycastSourceSocketName) : FVector::ZeroVector, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true);
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

bool AChargeReleaseProjectileWeapon::Internal_IsWeaponOverheated() const
{
	const float GameTime = GetWorld()->GetTimeSeconds();
	if (GameTime < OverheatUntilTime)
	{
		return true;
	}
	return false;
}
