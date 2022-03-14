// Fill out your copyright notice in the Description page of Project Settings.


#include "Utils/GameplayTagUtils.h"

#include "BlueprintGameplayTagLibrary.h"
#include "API/Taggable.h"

bool UGameplayTagUtils::ActorHasGameplayTag(AActor* InActor, const FGameplayTag& InTag, bool bExact)
{
	if(ITaggable* CastedActor = Cast<ITaggable>(InActor))
	{
		return UBlueprintGameplayTagLibrary::HasTag(CastedActor->GetTagContainer(), InTag, bExact);
	}
	return false;
}

void UGameplayTagUtils::AddTagToActor(AActor* InActor, const FGameplayTag& InTag)
{
	if(ITaggable* CastedActor = Cast<ITaggable>(InActor))
	{
		CastedActor->GetTagContainer().AddTag(InTag);
	}
}

void UGameplayTagUtils::RemoveTagFromActor(AActor* InActor, const FGameplayTag& InTag)
{
	if(ITaggable* CastedActor = Cast<ITaggable>(InActor))
	{
		CastedActor->GetTagContainer().RemoveTag(InTag);
	}
}
