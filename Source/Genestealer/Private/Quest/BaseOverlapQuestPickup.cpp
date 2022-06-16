// Fill out your copyright notice in the Description page of Project Settings.


#include "Quest/BaseOverlapQuestPickup.h"
#include "GameFramework/Character.h"
#include "Quest/QuestObjectiveComponent.h"

ABaseOverlapQuestPickup::ABaseOverlapQuestPickup()
{
	QuestObjectiveComponent = CreateDefaultSubobject<UQuestObjectiveComponent>(TEXT("QuestObjectiveComponent"));
	CollisionComp->SetSphereRadius(128.f);
	bDiesAfterOverlap = true;
}

bool ABaseOverlapQuestPickup::CanPickup(ACharacter* PotentialChar)
{
	if(PotentialChar && PotentialChar->IsPlayerControlled() && QuestObjectiveComponent && QuestObjectiveComponent->HasAnyActiveQuests())
	{
		return true;
	}
	return false;
}

void ABaseOverlapQuestPickup::ConsumePickup(ACharacter* ConsumingChar)
{
	if(ConsumingChar && QuestObjectiveComponent)
	{
		QuestObjectiveComponent->BroadcastQuestEvent(EQuestObjectiveAction::Overlap);
		QuestObjectiveComponent->ClearQuestEventBindings();
	}
}