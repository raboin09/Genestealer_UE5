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
	static void ApplyEffectToActor(AActor* ReceivingActor, TSubclassOf<AActor> EffectToApply);
	static void ApplyEffectsToActor(TArray<TSubclassOf<AActor>> EffectsToApply, AActor* ReceivingActor);
	static void ApplyEffectsToHitResult(TArray<TSubclassOf<AActor>> EffectsToApply, const FHitResult& Impact, AActor* InstigatingActor, bool bShouldRotateHitResult = true);
	static void ApplyEffectToHitResult(TSubclassOf<AActor> BaseEffectClass, const FHitResult& Impact, AActor* InstigatingActor, bool bShouldRotateHitResult = true);

	static void TryAddMaxWoundsToActor(AActor* ReceivingActor, float MaxWoundsToAdd);
	static void TryApplyHealToActor(AActor* ReceivingActor, AActor* InstigatingActor, float Heal);
	static void TryApplyDamageToActor(AActor* ReceivingActor, AActor* InstigatingActor, float Damage, const FDamageHitReactEvent& HitReactEvent = FDamageHitReactEvent());

	static UFXSystemAsset* GetVFXAssetFromKey(const FDataTableRowHandle& RowHandle, UPhysicalMaterial* SurfaceMaterial, bool bIsValidHeadshot);
	static USoundCue* GetSFXAssetFromKey(const FDataTableRowHandle& RowHandle, UPhysicalMaterial* SurfaceMaterial, bool bIsValidHeadshot);

};
