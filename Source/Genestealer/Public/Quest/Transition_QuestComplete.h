// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SMTransitionInstance.h"
#include "Types/EventDeclarations.h"
#include "Types/QuestTypes.h"
#include "Transition_QuestComplete.generated.h"

/**
 * 
 */
UCLASS()
class GENESTEALER_API UTransition_QuestComplete : public USMTransitionInstance
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Genestealer")
	FQuestSectionData QuestSectionData;
	int32 QuestID;

	FORCEINLINE FQuestUpdateEvent& OnQuestUpdated() { return QuestUpdated; }
	
protected:
	virtual bool CanEnterTransition_Implementation() const override;
	virtual void OnTransitionShutdown_Implementation() override;
	virtual void OnTransitionInitialized_Implementation() override;
	
	void ActivateQuest();
	void DeactivateQuest();

private:
	void ActivateAllObjectivesOfClass(UClass* ObjectiveClass);
	void ActivateQuestObjectiveActor(AActor* InActor);
	void DeactivateAllObjectivesOfClass(UClass* ObjectiveClass) const;
	void DeactivateQuestObjectiveActor(AActor* InActor) const;
	void ApplyQuestIncrementsForEvent(EQuestObjectiveAction Action, UClass* ValidClass);
	
	UFUNCTION()
	void HandleQuestEventTrigger(const FQuestObjectiveEventPayload& InEvent);

	FQuestUpdateEvent QuestUpdated;
};
