// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BehaviorTreeNodes/BTDecorator_CanMove.h"

#include "AIController.h"
#include "Types/GenestealerTags.h"
#include "Utils/GameplayTagUtils.h"

bool UBTDecorator_CanMove::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	if(!OwnerComp.GetAIOwner())
	{
		return false;
	}
	return !UGameplayTagUtils::ActorHasGameplayTag(OwnerComp.GetAIOwner()->GetPawn(), TAG_STATE_STUNNED);
}
