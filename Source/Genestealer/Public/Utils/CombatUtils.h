// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "API/Attackable.h"
#include "API/Interactable.h"
#include "Characters/HealthComponent.h"
#include "Core/PlayerStatsComponent.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CombatUtils.generated.h"

#define OUTLINE_COLOR_RED 254
#define OUTLINE_COLOR_GREEN 252
#define OUTLINE_COLOR_GRAY 255
#define OUTLINE_COLOR_PURPLE 253

/**
 * 
 */
UCLASS()
class GENESTEALER_API UCombatUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static AActor* GetRandomEnemy(EAbsoluteAffiliation EnemyAffiliation);
	static float GetArmorSaveReduction(EArmorSave InArmorSave);
	static float GetBallisticSkillValueFromEnum(EBallisticSkill InSkill);
	static float GetHitImpulseValue(EHitReactType InHit);
	static bool ShouldHitKnockback(EHitReactType InHit);
	static float GetKnockbackRecoveryTime(EHitReactType InHit);
	static bool IsBoneNameHead(const FName& InBone);
	static FName GetNearestValidBoneForImpact(FName InBone);
	static FRotator GetRotationFromComponentHit(const FHitResult& Impact);
	static EAffectedAffiliation GetAffiliationRelatedToPlayerCharacter(AActor* ContextActor);
	static bool AreActorsAllies(TScriptInterface<IInteractable> FirstActor, AActor* SecondActor);
	static bool AreActorsAllies(AActor* FirstActor, AActor* SecondActor);
	UFUNCTION(BlueprintCallable)
	static bool AreActorsEnemies(AActor* FirstActor, AActor* SecondActor);
	static bool AreActorsEnemies(TScriptInterface<IInteractable> FirstActor, AActor* SecondActor);
	static bool IsActorNeutral(AActor* FirstActor);
	static bool IsActorNeutral(TScriptInterface<IInteractable> FirstActor);
	static bool IsActorDestructible(AActor* FirstActor);
	static bool IsActorDestructible(TScriptInterface<IInteractable> FirstActor);
	static int32 GetOutlineInt(AActor* InActor);
};
