// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Effects/BaseEffect.h"
#include "Utils/CombatUtils.h"
#include "BaseStatModifierEffect.generated.h"


UENUM(BlueprintType)
enum class EEffectStatType : uint8 
{
	Health_MaxWounds,
	Health_Damage,
	Health_Heal,
	Movespeed
};

UCLASS(Blueprintable, BlueprintType, EditInlineNew, DefaultToInstanced)
class UBaseStatsModifierData : public UObject
{
	GENERATED_BODY()
	
public:
	UBaseStatsModifierData() {}
	
	UPROPERTY(EditDefaultsOnly, Category="Genestealer")
	EEffectStatType StatToModify = EEffectStatType::Health_Damage;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer")
	float BaseModifierValue;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer", meta=(EditCondition = "StatToModify == EEffectStatType::Health_Damage", EditConditionHides))
	EHitReactType HitImpulse;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer", meta=(EditCondition = "StatToModify == EEffectStatType::Health_Damage && HitImpulse != EHitReactType::None", EditConditionHides))
	bool bOnlyHitReactOnDeath = true;
};

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class GENESTEALER_API ABaseStatModifier : public ABaseEffect
{
	GENERATED_BODY()
	
protected:
	virtual void K2_ActivateEffect_Implementation() override;

	// Optionally override one of these in child classes
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void K2_ApplyStatChange(float ModifiedStatValue);
	virtual void K2_ApplyStatChange_Implementation(float ModifiedStatValue);

	UPROPERTY(EditDefaultsOnly, Instanced, Category = "Weapon")
	UBaseStatsModifierData* StatEffectDataObj;
	
private:
	float CalculateModifierValues();
	void CalculateModifier(const FModifierExpression& ModifierExpression, const FGameplayTag& ModifierTag, float& ModifiedBaseValue) const;
};
