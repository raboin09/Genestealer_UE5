// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "API/AIPawn.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BaseAIController.generated.h"

/**
 * 
 */
UCLASS()
class GENESTEALER_API ABaseAIController : public AAIController
{
	GENERATED_BODY()

public:
	ABaseAIController();
	virtual void OnPossess(APawn* InPawn) override;
	
private:
	UPROPERTY(Transient)
	UBlackboardComponent* BlackboardComponent;
	UPROPERTY(Transient)
	UBehaviorTreeComponent* BehaviorTreeComponent;
	UPROPERTY(Transient)
	TScriptInterface<IAIPawn> AIPawn;
};
