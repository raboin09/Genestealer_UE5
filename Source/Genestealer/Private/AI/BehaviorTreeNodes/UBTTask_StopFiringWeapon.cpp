﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BehaviorTreeNodes/UBTTask_StopFiringWeapon.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "API/AIPawn.h"

EBTNodeResult::Type UUBTTask_StopFiringWeapon::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if(IAIPawn* AIPawn = Cast<IAIPawn>(OwnerComp.GetAIOwner()->GetPawn()))
	{
		AIPawn->FireWeapon(false);
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}