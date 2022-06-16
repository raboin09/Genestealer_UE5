// Fill out your copyright notice in the Description page of Project Settings.


#include "Quest/QuestObjectiveComponent.h"

void UQuestObjectiveComponent::ActivateQuestObjective(int32 QuestID)
{
	AssociatedQuests.AddUnique(QuestID);
	BroadcastQuestEvent(EQuestObjectiveAction::Initialize);	
}

void UQuestObjectiveComponent::DeactivateQuestObjective(int32 QuestID)
{
	AssociatedQuests.Remove(QuestID);
	BroadcastQuestEvent(EQuestObjectiveAction::Deactivate);
}

void UQuestObjectiveComponent::DeactivateAllQuestObjectives()
{
	AssociatedQuests.Empty();
}

void UQuestObjectiveComponent::BroadcastQuestEvent(EQuestObjectiveAction InEvent)
{
	QuestEventTrigger.Broadcast(FQuestObjectiveEventPayload(GetOwner(), InEvent));
}

bool UQuestObjectiveComponent::IsAssociatedWithQuest(int32 QuestID)
{
	return AssociatedQuests.Contains(QuestID);
}

void UQuestObjectiveComponent::ClearQuestEventBindings()
{
	QuestEventTrigger.RemoveAll(this);
}

