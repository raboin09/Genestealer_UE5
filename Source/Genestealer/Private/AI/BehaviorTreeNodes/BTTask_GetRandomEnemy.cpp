// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BehaviorTreeNodes/BTTask_GetRandomEnemy.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "API/AIPawn.h"
#include "Utils/CombatUtils.h"

EBTNodeResult::Type UBTTask_GetRandomEnemy::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if(!IsValid(&OwnerComp) || !OwnerComp.GetAIOwner())
	{
		return EBTNodeResult::Failed;
	}
	APawn* CurrPawn = OwnerComp.GetAIOwner()->GetPawn();	
	if(IAIPawn* AIPawn = Cast<IAIPawn>(CurrPawn))
	{
		AActor* SelectedEnemy;
		switch (AIPawn->GetPawnAffiliation()) {
		case EAbsoluteAffiliation::GenestealerCult:
			SelectedEnemy = UCombatUtils::GetRandomEnemy(EAbsoluteAffiliation::ChaosCult);
			OwnerComp.GetBlackboardComponent()->SetValueAsObject(EnemyKey.SelectedKeyName, SelectedEnemy);
			return EBTNodeResult::Succeeded;
		case EAbsoluteAffiliation::ChaosCult:
			SelectedEnemy = UCombatUtils::GetRandomEnemy(EAbsoluteAffiliation::GenestealerCult);
			OwnerComp.GetBlackboardComponent()->SetValueAsObject(EnemyKey.SelectedKeyName, SelectedEnemy);
			return EBTNodeResult::Succeeded;
		case EAbsoluteAffiliation::Neutral:
		case EAbsoluteAffiliation::Imperium:
		case EAbsoluteAffiliation::Orks:
		case EAbsoluteAffiliation::Destructible:
		default:
			return EBTNodeResult::Failed;
		}
	}
	return EBTNodeResult::Failed;
}
