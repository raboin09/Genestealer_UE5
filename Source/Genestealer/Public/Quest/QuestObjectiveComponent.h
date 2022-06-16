// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "API/Questable.h"
#include "Components/ActorComponent.h"
#include "QuestObjectiveComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GENESTEALER_API UQuestObjectiveComponent : public UActorComponent, public IQuestable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	virtual void ActivateQuestObjective(int32 QuestID) override;
	virtual void DeactivateQuestObjective(int32 QuestID) override;
	virtual void DeactivateAllQuestObjectives() override;
	virtual void BroadcastQuestEvent(EQuestObjectiveAction InEvent) override;
	virtual FQuestObjectiveEvent& OnQuestObjectiveEvent() override { return QuestEventTrigger; };
	virtual bool IsAssociatedWithQuest(int32 QuestID) override;
	virtual void ClearQuestEventBindings() override;
	FORCEINLINE virtual bool HasAnyActiveQuests() override { return AssociatedQuests.Num() > 0; };
	
private:
	TArray<int32> AssociatedQuests;	
	FQuestObjectiveEvent QuestEventTrigger;
};
