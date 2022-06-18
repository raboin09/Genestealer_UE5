// Fill out your copyright notice in the Description page of Project Settings.


#include "Quest/Transition_QuestComplete.h"

#include "API/Questable.h"
#include "Quest/QuestObjectiveComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Quest/QuestStateMachine.h"

bool UTransition_QuestComplete::CanEnterTransition_Implementation() const
{
	return QuestSectionData.IsQuestSectionDone();
}

void UTransition_QuestComplete::OnTransitionShutdown_Implementation()
{
	QuestUpdated.RemoveAll(this);
	DeactivateQuest();
}

void UTransition_QuestComplete::OnTransitionInitialized_Implementation()
{
	ActivateQuest();
}

void UTransition_QuestComplete::ActivateQuest()
{
	for(TSubclassOf<AActor> CurrClass : QuestSectionData.TargetObjectiveClasses)
	{
		ActivateAllObjectivesOfClass(CurrClass);
	}
}

void UTransition_QuestComplete::DeactivateQuest()
{
	QuestUpdated.RemoveAll(this);
	for(TSubclassOf<AActor> CurrClass : QuestSectionData.TargetObjectiveClasses)
	{
		DeactivateAllObjectivesOfClass(CurrClass);
	}
}

void UTransition_QuestComplete::ActivateAllObjectivesOfClass(UClass* ObjectiveClass)
{
	if(ObjectiveClass)
	{
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ObjectiveClass, FoundActors);
		for(AActor* CurrActor : FoundActors)
		{
			if(QuestSectionData.ObjectiveTag.IsNone() || CurrActor->ActorHasTag(QuestSectionData.ObjectiveTag))
			{
				ActivateQuestObjectiveActor(CurrActor);	
			}
		}
	}
}

void UTransition_QuestComplete::ActivateQuestObjectiveActor(AActor* InActor)
{
	if(InActor)
	{
		if(IQuestable* CastedActor = Cast<IQuestable>(InActor->GetComponentByClass(UQuestObjectiveComponent::StaticClass())))
		{
			CastedActor->OnQuestObjectiveEvent().AddDynamic(this, &UTransition_QuestComplete::HandleQuestEventTrigger);
			CastedActor->ActivateQuestObjective(QuestID);
		}
	}
}

void UTransition_QuestComplete::DeactivateAllObjectivesOfClass(UClass* ObjectiveClass) const
{
	if(ObjectiveClass)
	{
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ObjectiveClass, FoundActors);
		for(AActor* CurrActor : FoundActors)
		{
			DeactivateQuestObjectiveActor(CurrActor);
		}
	}
}

void UTransition_QuestComplete::DeactivateQuestObjectiveActor(AActor* InActor) const
{
	if(InActor)
	{
		if(IQuestable* CastedActor = Cast<IQuestable>(InActor->GetComponentByClass(UQuestObjectiveComponent::StaticClass())))
		{
			CastedActor->OnQuestObjectiveEvent().RemoveAll(this);
			CastedActor->DeactivateQuestObjective(QuestID);
		}
	}
}

void UTransition_QuestComplete::HandleQuestEventTrigger(const FQuestObjectiveEventPayload& InEvent)
{
	if(InEvent.EventObjective)
	{
		ApplyQuestIncrementsForEvent(InEvent.EventAction, InEvent.EventObjective->GetClass());
	}
}

void UTransition_QuestComplete::ApplyQuestIncrementsForEvent(EQuestObjectiveAction Action, UClass* ValidClass)
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