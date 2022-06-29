// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/BaseOverlapQuestPickup.h"
#include "GameFramework/Character.h"
#include "Utils/WorldUtils.h"

ABaseOverlapQuestPickup::ABaseOverlapQuestPickup()
{
	CollisionComp->SetSphereRadius(128.f);
	bDiesAfterOverlap = true;

	BlockedOverlapTags.Add(TAG_ACTOR_AI);
	
	RequiredOverlapTags.Add(TAG_ACTOR_PLAYER);
}

void ABaseOverlapQuestPickup::BeginPlay()
{
	Super::BeginPlay();
	UWorldUtils::TryAddActorToQuestableArray(this);
}

bool ABaseOverlapQuestPickup::CanPickup(ACharacter* PotentialChar)
{
	return QuestObjectiveEvent.IsBound();
}

void ABaseOverlapQuestPickup::ConsumePickup(ACharacter* ConsumingChar)
{
	QuestObjectiveEvent.Broadcast(FQuestObjectiveEventPayload(this, EQuestObjectiveAction::Overlap));
}