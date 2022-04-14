// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BaseAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetSystemLibrary.h"

ABaseAIController::ABaseAIController()
{
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackBoardComp"));
	BrainComponent = BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorComp"));
}

void ABaseAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if(!InPawn || !UKismetSystemLibrary::DoesImplementInterface(InPawn, UAIPawn::StaticClass()))
	{
		return;
	}

	AIPawn.SetObject(InPawn);
	AIPawn.SetInterface(Cast<IAIPawn>(InPawn));
	BlackboardComponent->InitializeBlackboard(*AIPawn->GetAIBehavior()->BlackboardAsset);
	BehaviorTreeComponent->StartTree(*(AIPawn->GetAIBehavior()));
}