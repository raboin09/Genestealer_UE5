// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/HealthComponent.h"
#include "Engine/DataTable.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Effects/BaseEffect.h"
#include "Sound/SoundCue.h"
#include "EffectUtils.generated.h"

/**
 * 
 */
UCLASS()
class GENESTEALER_API UEffectUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Genestealer|EffectUtils")
	static void ApplyEffectsToHitResultsInRadius(AActor* InstigatingActor, TArray<TSubclassOf<AActor>> EffectsToApply, FVector TraceLocation, float TraceRadius, ETraceTypeQuery ValidationTraceType, bool bValidateHit = true, FVector ValidationTraceStartLocation = FVector::ZeroVector, FName HitValidationBone = "spine_02");
	UFUNCTION(BlueprintCallable, Category="Genestealer|EffectUtils")
	static void ApplyEffectAtLocation(AActor* InstigatingActor, TSubclassOf<AActor> EffectToApply, FVector Location, bool bActivateImmediately = true);
	UFUNCTION(BlueprintCallable, Category="Genestealer|EffectUtils")
	static void ApplyEffectToActor(AActor* ReceivingActor, TSubclassOf<AActor> EffectToApply);
	static void ApplyEffectsToActor(TArray<TSubclassOf<AActor>> EffectsToApply, AActor* ReceivingActor);
	static void ApplyEffectsToHitResult(TArray<TSubclassOf<AActor>> EffectsToApply, const FHitResult& Impact, AActor* InstigatingActor, bool bShouldRotateHitResult = true);
	UFUNCTION(BlueprintCallable, Category="Genestealer|EffectUtils")
	static void ApplyEffectToHitResult(TSubclassOf<AActor> BaseEffectClass, const FHitResult& Impact, AActor* InstigatingActor, bool bShouldRotateHitResult = true);

	static void TryAddMaxWoundsToActor(AActor* ReceivingActor, float MaxWoundsToAdd);
	static void TryApplyHealToActor(AActor* ReceivingActor, AActor* InstigatingActor, float Heal);
	static void TryApplyDamageToActor(AActor* ReceivingActor, AActor* InstigatingActor, float Damage, const FDamageHitReactEvent& HitReactEvent = FDamageHitReactEvent());

	static UFXSystemAsset* GetVFXAssetFromKey(const FDataTableRowHandle& RowHandle, const UPhysicalMaterial* SurfaceMaterial, bool bIsValidHeadshot);
	static USoundCue* GetSFXAssetFromKey(const FDataTableRowHandle& RowHandle, const UPhysicalMaterial* SurfaceMaterial, bool bIsValidHeadshot);

};
