// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SMInstance.h"
#include "Transition_QuestSection.h"
#include "QuestStateMachine.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class GENESTEALER_API UQuestStateMachine : public USMInstance
{
	GENERATED_BODY()

public:
	UQuestStateMachine();
	virtual void Initialize(UObject* Context) override;

	void TryAddActorToQuest(AActor* InActor);
	bool IsQuestComplete() const;
	
	UPROPERTY(EditDefaultsOnly, Category="Genestealer")
	FQuestData QuestData;

	UPROPERTY()
	TArray<UTransition_QuestSection*> QuestTransitions;
};
