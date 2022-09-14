// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectileWeapon.h"
#include "GameFramework/Actor.h"
#include "ChargeReleaseProjectileWeapon.generated.h"

UCLASS(Abstract, Blueprintable, AutoExpandCategories=("Genestealer"), PrioritizeCategories = "Genestealer")
class GENESTEALER_API AChargeReleaseProjectileWeapon : public AProjectileWeapon
{
	GENERATED_BODY()

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void K2_PlayCooldownEffects(float CooldownTime);
	UFUNCTION(BlueprintImplementableEvent)
	void K2_ChargeStateChange(int32 NewChargeState);

	virtual void FireWeapon() override;
	virtual float SimulateWeaponFire() override;
	virtual void OnBurstFinished() override;
	
	virtual TArray<TSubclassOf<AActor>> Internal_GetAdditionalEffectsToApplyToProjectile() const override;

	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Fire|Charge")
	int32 MaxChargedState = 2;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Sound")
	int32 WhatChargeStateToPlayChargingSound = 1;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Sound")
	float ChargeSoundDelay = 0.f;

	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Animation")
	TArray<UAnimMontage*> ChargeFireAnims;

	// FireFX is the charging VFX, ChargeBlastVFX is on burst finished
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|VFX|Muzzle")
	UNiagaraSystem* ChargeBlastVFX;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|VFX|ChargeBlast")
	FName ChargeBlastNiagaraSmokeSpawnMaxName = "User.SmokeSpawnMax";
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|VFX|ChargeBlast")
	int32 SmokeSpawnMax = 10;	
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|VFX|ChargeBlast")
	FName ChargeBlastNiagaraSmokeSpawnMinName = "User.SmokeSpawnMin";
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|VFX|ChargeBlast")
	int32 SmokeSpawnMin = 4;	
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|VFX|ChargeBlast")
	FName ChargeBlastNiagaraScaleMaxName = "User.2DScaleMax";
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|VFX|ChargeBlast")
	FVector2D ChargeBlastScaleMax = FVector2D(150.f, 150.f);	
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|VFX|ChargeBlast")
	FName ChargeBlastNiagaraScaleMinName = "User.2DScaleMin";
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|VFX|ChargeBlast")
	FVector2D ChargeBlastScaleMin = FVector2D(100.f, 100.f);	
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|VFX|Charge")
	FName ChargeNiagaraScaleMaxName = "User.2DScaleMax";
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|VFX|Charge")
	FVector2D ChargeScaleMax = FVector2D(150.f, 150.f);	
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|VFX|Charge")
	FName ChargeNiagaraScaleMinName = "User.2DScaleMin";
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|VFX|Charge")
	FVector2D ChargeScaleMin = FVector2D(100.f, 100.f);	
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|VFX|Charge")
	FName ChargeNiagaraMaxChargeTimeName = "User.MaxChargeTime";

private:
	void Internal_TryIncreaseChargeState();
	void Internal_FireAndReset();
	void Internal_PlayChargeBlastVFX();
	UFUNCTION()
	void Internal_PlayChargeAudio();

	int32 CurrentChargeState = -1;

	UPROPERTY()
	FTimerHandle TimerHandle_DelayChargeAudio;
	UPROPERTY()
	UAudioComponent* ChargingAudio;
	UPROPERTY()
	UNiagaraComponent* ChargeBlastNiagara;
	UPROPERTY()
	UNiagaraComponent* ChargingNiagara;
};
