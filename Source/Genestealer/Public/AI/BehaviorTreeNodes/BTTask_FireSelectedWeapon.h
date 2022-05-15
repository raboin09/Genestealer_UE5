// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FireSelectedWeapon.generated.h"

/**
 * 
 */
UCLASS(Category = Genestealer, meta = (DisplayName = "Fire Weapon"))
class GENESTEALER_API UBTTask_FireSelectedWeapon : public UBTTaskNode
{
	GENERATED_BODY()

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Genestealer")
	FBlackboardKeySelector TargetActorKey;
};