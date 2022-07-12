// Fill out your copyright notice in the Description page of Project Settings.


#include "Effects/BaseStatModifierEffect.h"
#include "Characters/HealthComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Utils/EffectUtils.h"
#include "Utils/GameplayTagUtils.h"

ABaseStatModifier::ABaseStatModifier()
{
	// if(!StatEffectDataObj)
	// {
	// 	StatEffectDataObj = Cast<UBaseStatsModifierData>(UBaseStatsModifierData::StaticClass()->GetDefaultObject());
	// }
}

void ABaseStatModifier::K2_ActivateEffect_Implementation()
{
	K2_ApplyStatChange(CalculateModifierValues());
}

void ABaseStatModifier::K2_ApplyStatChange_Implementation(float ModifiedStatValue)
{	
	switch (StatEffectDataObj->StatToModify) {
	case EEffectStatType::Health_MaxWounds:
		UEffectUtils::TryAddMaxWoundsToActor(EffectContext.ReceivingActor, ModifiedStatValue);
		break;
	case EEffectStatType::Health_Heal:
		UEffectUtils::TryApplyHealToActor(EffectContext.ReceivingActor, EffectContext.InstigatingActor, ModifiedStatValue);
		break;
	case EEffectStatType::Health_Damage:
		{
			FDamageHitReactEvent HitReactEvent;
			HitReactEvent.DamageTaken = CalculateHeadshotDamage(ModifiedStatValue);
			if(EffectContext.InstigatingActor && EffectContext.ReceivingActor)
			{
				const FRotator LookAt = UKismetMathLibrary::FindLookAtRotation(EffectContext.InstigatingActor->GetActorLocation(), EffectContext.ReceivingActor->GetActorLocation());
				HitReactEvent.HitDirection = LookAt.Vector().GetSafeNormal();
			} else
			{
				HitReactEvent.HitDirection = EffectContext.InstigatingActor ?  EffectContext.InstigatingActor->GetActorForwardVector().GetSafeNormal() : FVector::ZeroVector;
			}
			HitReactEvent.HitResult = EffectContext.SurfaceHit;
			HitReactEvent.DeathReactType = StatEffectDataObj->DeathImpulse;
			HitReactEvent.HitReactType = StatEffectDataObj->HitImpulse;
			UEffectUtils::TryApplyDamageToActor(EffectContext.ReceivingActor, EffectContext.InstigatingActor, HitReactEvent.DamageTaken, HitReactEvent);
		}
		break;
	default:;
	}
}

float ABaseStatModifier::CalculateHeadshotDamage(float ModifiedStatValue) const
{
	if(StatEffectDataObj->bAddDamageForHeadshots && UCombatUtils::IsBoneNameHead(EffectContext.SurfaceHit.BoneName))
	{
		return ModifiedStatValue * StatEffectDataObj->HeadshotModifier;
	}
	return ModifiedStatValue;
}

float ABaseStatModifier::CalculateModifierValues()
{
	TArray<FGameplayTag> ModifierKeys;
	EffectDataObj->EffectModifiers.GetKeys(ModifierKeys);
	float ModifiedValue = StatEffectDataObj->BaseModifierValue;
	for(const FGameplayTag& CurrentTag : ModifierKeys)
	{
		CalculateModifier(*EffectDataObj->EffectModifiers.Find(CurrentTag), CurrentTag, ModifiedValue);
	}
	return ModifiedValue;
}

void ABaseStatModifier::CalculateModifier(const FModifierExpression& ModifierExpression, const FGameplayTag& ModifierTag, float& ModifiedBaseValue) const
{
	AActor* ModifierActor = nullptr;
	switch (ModifierExpression.ModifierActor)
	{
	case EModifierActor::Instigator:
		ModifierActor = EffectContext.InstigatingActor;
		break;
	case EModifierActor::Receiver:
		ModifierActor = EffectContext.ReceivingActor;
		break;
	default:
		break;
	}

	if(!UGameplayTagUtils::ActorHasGameplayTag(ModifierActor, ModifierTag))
	{
		return;
	}
	
	switch (ModifierExpression.Operation)
	{
	case EModifierOperator::Add:
		ModifiedBaseValue += ModifierExpression.ModifierOperand;
		break;
	case EModifierOperator::Subtract:
		ModifiedBaseValue -= ModifierExpression.ModifierOperand;
		break;
	case EModifierOperator::Multiply:
		ModifiedBaseValue *= ModifierExpression.ModifierOperand;
		break;
	default: ;
	}
}