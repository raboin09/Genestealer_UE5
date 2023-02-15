// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BehaviorTreeNodes/BTDecorator_IsInRange.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BlackboardData.h"
#include "Characters/BaseAICharacter.h"
#include "Kismet/KismetSystemLibrary.h"

bool UBTDecorator_IsInRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const ABaseAICharacter* AIPawn = Cast<ABaseAICharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (!AIPawn)
	{
		return false;
	}
	
	AActor* SelectedActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(EnemyKey.SelectedKeyName));
	if(!SelectedActor)
	{
		return false;
	}
	return SelectedActor->GetDistanceTo(AIPawn) <= AIPawn->GetWeaponRange();
}
