// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_GetRandomEnemy.generated.h"

/**
 * 
 */
UCLASS(Category = Genestealer, meta = (DisplayName = "Find Random Enemy"))
class GENESTEALER_API UBTTask_GetRandomEnemy : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Genestealer")
	FBlackboardKeySelector EnemyKey;
}; 
