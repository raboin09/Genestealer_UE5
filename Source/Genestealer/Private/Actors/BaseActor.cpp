// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/BaseActor.h"

ABaseActor::ABaseActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ABaseActor::BeginPlay()
{
	Super::BeginPlay();
	Internal_AddDefaultTagsToContainer();
}

void ABaseActor::Internal_AddDefaultTagsToContainer()
{
	for(const FGameplayTag CurrentTag : DefaultGameplayTags)
	{
		GameplayTagContainer.AddTag(CurrentTag);
	}
}