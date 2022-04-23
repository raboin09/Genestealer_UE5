// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BehaviorTreeNodes/BTTask_FireSelectedWeapon.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "API/AIPawn.h"

EBTNodeResult::Type UBTTask_FireSelectedWeapon::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if(IAIPawn* AIPawn = Cast<IAIPawn>(OwnerComp.GetAIOwner()->GetPawn()))
	{
		AIPawn->Aim(true);
		AIPawn->FireWeapon(true);
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}
