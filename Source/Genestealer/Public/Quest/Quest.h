// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Transition_QuestComplete.h"
#include "GameFramework/Info.h"
#include "Types/QuestTypes.h"
#include "Quest.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class GENESTEALER_API AQuest : public AInfo
{
	GENERATED_BODY()
	
public:
	bool IsQuestComplete() const;	
	TArray<UTransition_QuestComplete*> InitStateMachineInstance();
	
	UPROPERTY(EditDefaultsOnly, Category="Genestealer")
	FQuestData QuestData;
};
