// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "API/Attackable.h"
#include "API/Interactable.h"
#include "Characters/HealthComponent.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CombatUtils.generated.h"

/**
 * 
 */
UCLASS()
class GENESTEALER_API UCombatUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static float GetHitImpulseValue(EHitReactType InHit);
	static bool ShouldHitKnockback(EHitReactType InHit);
	static float GetKnockbackRecoveryTime(EHitReactType InHit);
	static bool IsBoneNameHead(FName InBone);
	static FRotator GetRotationFromComponentHit(const FHitResult& Impact);
	static bool AreActorsAllies(AActor* FirstActor, AActor* SecondActor);
	UFUNCTION(BlueprintCallable)
	static bool AreActorsEnemies(AActor* FirstActor, AActor* SecondActor);
	static bool IsActorNeutral(AActor* FirstActor);
	static int32 GetOutlineIntFromColor(const EOutlineColor InColor);
	static int32 GetOutlineIntFromAffiliation(const EAffiliation InAffiliation);
};
