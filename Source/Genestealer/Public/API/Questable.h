// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Types/EventDeclarations.h"
#include "Questable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UQuestable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GENESTEALER_API IQuestable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
		UFUNCTION(BlueprintCallable)
    	virtual void ActivateQuestObjective(int32 QuestID) PURE_VIRTUAL(IQuestObjective::ActivateQuestObjective,);
    	virtual void DeactivateQuestObjective(int32 QuestID) PURE_VIRTUAL(IQuestObjective::DeactivateQuestObjective,);
    	virtual bool IsAssociatedWithQuest(int32 QuestID) PURE_VIRTUAL(IQuestObjective::IsAssociatedWithQuest, return false;);
    	virtual void DeactivateAllQuestObjectives() PURE_VIRTUAL(IQuestObjective::DeactivateAllQuestObjectives,);
    	virtual void BroadcastQuestEvent(EQuestObjectiveAction InEvent) PURE_VIRTUAL(IQuestObjective::BroadcastQuestEvent,);
    	virtual void ClearQuestEventBindings() PURE_VIRTUAL(IQuestObjective::ClearQuestEventBindings,);
    	virtual bool HasAnyActiveQuests() PURE_VIRTUAL(IQuestObjective::HasAnyActiveQuests, return false;);
    	virtual FQuestObjectiveEvent& OnQuestObjectiveEvent() = 0;
};
