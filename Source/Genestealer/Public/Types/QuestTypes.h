// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SMInstance.h"
#include "QuestTypes.generated.h"

UENUM(BlueprintType)
enum class EQuestObjectiveAction : uint8
{
	Initialize,
	Deactivate,
	Death,
	Overlap,
	Interact,
	None
};

USTRUCT(BlueprintType)
struct FQuestSectionData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<AActor>> TargetObjectiveClasses;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<EQuestObjectiveAction> ValidObjectiveActions;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ObjectiveTag = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 NumActionsRequired = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TimeToComplete = -1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText QuestText;
	
	int32 NumCurrentActions = 0;

	bool IsQuestSectionDone() const
	{
		return NumCurrentActions >= NumActionsRequired;
	}
};

USTRUCT(BlueprintType)
struct FQuestData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<USMInstance> QuestStateMachineClass;

	UPROPERTY()
	class UQuestStateMachine* QuestStateMachineInstance = nullptr;
	
	int32 QuestID = -1;
};