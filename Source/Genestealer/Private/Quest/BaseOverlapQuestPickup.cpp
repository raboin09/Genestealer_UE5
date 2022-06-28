// Fill out your copyright notice in the Description page of Project Settings.


#include "Quest/BaseOverlapQuestPickup.h"
#include "GameFramework/Character.h"
#include "Utils/WorldUtils.h"

ABaseOverlapQuestPickup::ABaseOverlapQuestPickup()
{
	CollisionComp->SetSphereRadius(128.f);
	bDiesAfterOverlap = true;
}

void ABaseOverlapQuestPickup::BeginPlay()
{
	Super::BeginPlay();
	UWorldUtils::TryAddActorToQuestableArray(this);
}

bool ABaseOverlapQuestPickup::CanPickup(ACharacter* PotentialChar)
{
	if(PotentialChar && PotentialChar->IsPlayerControlled() && QuestObjectiveEvent.IsBound())
	{
		return true;
	}
	return false;
}

void ABaseOverlapQuestPickup::ConsumePickup(ACharacter* ConsumingChar)
{
	QuestObjectiveEvent.Broadcast(FQuestObjectiveEventPayload(this, EQuestObjectiveAction::Overlap));
}