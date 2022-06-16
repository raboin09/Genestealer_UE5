// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SMInstance.h"
#include "Transition_QuestComplete.h"
#include "BaseQuestStateMachineInstance.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class GENESTEALER_API UBaseQuestStateMachineInstance : public USMInstance
{
	GENERATED_BODY()

public:
	UBaseQuestStateMachineInstance();
	virtual void Initialize(UObject* Context) override;

	UPROPERTY()
	TArray<UTransition_QuestComplete*> QuestTransitions;
};
