// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NavigationSystem.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_GetRandomLocation.generated.h"

/**
 * 
 */
UCLASS()
class GENESTEALER_API UBTTask_GetRandomLocation : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTTask_GetRandomLocation();

	/** Maximum distance the random location picked may be from pawn. */
	UPROPERTY(Category = Navigation, EditAnywhere, meta=(ClampMin = 1))
	float MaxDistance = 1000.0f;

	/** Navigation Query filter used when picking the location, only picks locations reachable using this filter when assigned. */
	UPROPERTY(Category = Navigation, EditAnywhere)
	TSubclassOf<UNavigationQueryFilter> Filter = nullptr;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;
};
