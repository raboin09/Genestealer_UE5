// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/BaseActor.h"

#include "Quest/QuestManagerComponent.h"
#include "Utils/WorldUtils.h"

ABaseActor::ABaseActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ABaseActor::BeginPlay()
{
	Super::BeginPlay();
	Internal_AddDefaultTagsToContainer();
	UWorldUtils::TryAddActorToQuestableArray(this);
	UQuestManagerComponent::TryAddActorToActiveQuests(this);
}

void ABaseActor::Internal_AddDefaultTagsToContainer()
{
	for(const FGameplayTag CurrentTag : DefaultGameplayTags)
	{
		GameplayTagContainer.AddTag(CurrentTag);
	}
}