// Fill out your copyright notice in the Description page of Project Settings.


#include "Quest/Transition_QuestSection.h"

#include "API/Questable.h"
#include "Kismet/GameplayStatics.h"
#include "Quest/QuestStateMachine.h"
#include "Utils/WorldUtils.h"

bool UTransition_QuestSection::CanEnterTransition_Implementation() const
{
	return QuestSectionData.IsQuestSectionDone();
}

void UTransition_QuestSection::OnTransitionShutdown_Implementation()
{
	QuestUpdated.RemoveAll(this);
	DeactivateQuest();
}

void UTransition_QuestSection::OnTransitionInitialized_Implementation()
{
	ActivateQuest();
}

void UTransition_QuestSection::ActivateQuest()
{
	for(TSubclassOf<AActor> CurrClass : QuestSectionData.TargetObjectiveClasses)
	{
		ActivateAllObjectivesOfClass(CurrClass);
	}
}

void UTransition_QuestSection::DeactivateQuest()
{
	QuestUpdated.RemoveAll(this);
	for(TSubclassOf<AActor> CurrClass : QuestSectionData.TargetObjectiveClasses)
	{
		DeactivateAllObjectivesOfClass(CurrClass);
	}
}

void UTransition_QuestSection::ActivateAllObjectivesOfClass(UClass* ObjectiveClass)
{
	if(!ObjectiveClass)
	{
		return;
	}
	
	for(AActor* CurrActor : UWorldUtils::QuestRelevantActors)
	{
		if(!CurrActor)
		{
			continue;
		}
		
		if(QuestSectionData.ObjectiveTag.IsNone() || CurrActor->ActorHasTag(QuestSectionData.ObjectiveTag))
		{
			ActivateQuestObjectiveActor(CurrActor);	
		}
	}
}

void UTransition_QuestSection::ActivateQuestObjectiveActor(AActor* InActor)
{
	if(!InActor)
	{
		return;
	}

	if(IQuestable* CastedActor = Cast<IQuestable>(InActor))
	{
		CastedActor->OnQuestObjectiveEvent().AddDynamic(this, &UTransition_QuestSection::HandleQuestEventTrigger);
	}
}

void UTransition_QuestSection::DeactivateAllObjectivesOfClass(UClass* ObjectiveClass) const
{
	if(ObjectiveClass)
	{
		for(AActor* CurrActor : UWorldUtils::QuestRelevantActors)
		{
			DeactivateQuestObjectiveActor(CurrActor);
		}
	}
}

void UTransition_QuestSection::DeactivateQuestObjectiveActor(AActor* InActor) const
{
	if(IQuestable* CastedActor = Cast<IQuestable>(InActor))
	{
		CastedActor->OnQuestObjectiveEvent().RemoveAll(this);
	}
}

void UTransition_QuestSection::HandleQuestEventTrigger(const FQuestObjectiveEventPayload& InEvent)
{
	if(InEvent.EventObjective && QuestSectionData.ValidObjectiveActions.Contains(InEvent.EventAction))
	{
		ApplyQuestIncrementsForEvent(InEvent.EventAction, InEvent.EventObjective->GetClass());
	}
}

void UTransition_QuestSection::ApplyQuestIncrementsForEvent(EQuestObjectiveAction Action, UClass* ValidClass)
{
	if(QuestSectionData.TargetObjectiveClasses.Contains(ValidClass) && QuestSectionData.ValidObjectiveActions.Contains(Action))
	{
		if(!QuestSectionData.IsQuestSectionDone())
		{
			QuestSectionData.NumCurrentActions++;
		}
		
		if(UQuestStateMachine* CurrQuest = Cast<UQuestStateMachine>(GetStateMachineInstance(true)))
		{
			QuestUpdated.Broadcast(CurrQuest);
		}
	}
}