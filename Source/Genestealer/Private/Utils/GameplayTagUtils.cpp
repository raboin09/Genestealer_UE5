// Fill out your copyright notice in the Description page of Project Settings.


#include "Utils/GameplayTagUtils.h"

#include "BlueprintGameplayTagLibrary.h"
#include "API/Taggable.h"
#include "Kismet/KismetSystemLibrary.h"

bool UGameplayTagUtils::ActorHasAnyGameplayTags(AActor* InActor, TArray<FGameplayTag> InTags, bool bExact)
{
	if(ITaggable* CastedActor = Cast<ITaggable>(InActor))
	{
		return UBlueprintGameplayTagLibrary::HasAnyTags(CastedActor->GetTagContainer(), FGameplayTagContainer::CreateFromArray(InTags), bExact);
	}
	return false;
}

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

void UGameplayTagUtils::RemoveTagsFromActor(AActor* InActor, TArray<FGameplayTag> InTags)
{
	if(ITaggable* CastedActor = Cast<ITaggable>(InActor))
	{
		return CastedActor->GetTagContainer().RemoveTags(FGameplayTagContainer::CreateFromArray(InTags));
	}
}

void UGameplayTagUtils::RemoveTagFromActor(AActor* InActor, const FGameplayTag& InTag)
{
	if(ITaggable* CastedActor = Cast<ITaggable>(InActor))
	{
		CastedActor->GetTagContainer().RemoveTag(InTag);
	}
}
