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

void ABaseActor::HandleTagChanged(const FGameplayTag& ChangedTag, bool bAdded)
{
	if(bAdded)
	{
		K2_HandleTagAdded(ChangedTag);
	}
	else
	{
		K2_HandleTagRemoved(ChangedTag);
	}
}

void ABaseActor::K2_HandleTagRemoved_Implementation(const FGameplayTag& AddedTag)
{
}

void ABaseActor::K2_HandleTagAdded_Implementation(const FGameplayTag& AddedTag)
{
}

void ABaseActor::Internal_AddDefaultTagsToContainer()
{
	for(const FGameplayTag CurrentTag : DefaultGameplayTags)
	{
		GameplayTagContainer.AddTag(CurrentTag);
	}
}