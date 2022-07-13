// Fill out your copyright notice in the Description page of Project Settings.


#include "Utils/GameplayTagUtils.h"

#include "BlueprintGameplayTagLibrary.h"
#include "API/Taggable.h"
#include "Kismet/KismetSystemLibrary.h"

bool UGameplayTagUtils::ComponentHasNameTag(UActorComponent* InComp, FName InTag)
{
	if(InComp)
	{
		return InComp->ComponentHasTag(InTag);
	}
	return false;
}

bool UGameplayTagUtils::ActorHasAnyGameplayTags(AActor* InActor, TArray<FGameplayTag> InTags, bool bExact)
{
	if(ITaggable* CastedActor = Cast<ITaggable>(InActor))
	{
		return UBlueprintGameplayTagLibrary::HasAnyTags(CastedActor->GetTagContainer(), FGameplayTagContainer::CreateFromArray(InTags), bExact);
	}
	return false;
}

bool UGameplayTagUtils::ActorHasGameplayTag(AActor* InActor, FGameplayTag InTag, bool bExact)
{
	if(ITaggable* CastedActor = Cast<ITaggable>(InActor))
	{
		return UBlueprintGameplayTagLibrary::HasTag(CastedActor->GetTagContainer(), InTag, bExact);
	}
	return false;
}

void UGameplayTagUtils::AddTagToActor(AActor* InActor, FGameplayTag InTag)
{
	if(ITaggable* CastedActor = Cast<ITaggable>(InActor))
	{
		CastedActor->GetTagContainer().AddTag(InTag);
		CastedActor->HandleTagChanged(InTag, true);
	}
}

void UGameplayTagUtils::AddTagsToActor(AActor* InActor, TArray<FGameplayTag> InTags)
{
	if(ITaggable* CastedActor = Cast<ITaggable>(InActor))
	{
		CastedActor->GetTagContainer().AppendTags(FGameplayTagContainer::CreateFromArray(InTags));
		for(FGameplayTag Tag : InTags)
		{
			UKismetSystemLibrary::PrintString(InActor, Tag.ToString());
			CastedActor->HandleTagChanged(Tag, true);
		}
	}
}

void UGameplayTagUtils::RemoveTagsFromActor(AActor* InActor, TArray<FGameplayTag> InTags)
{
	if(ITaggable* CastedActor = Cast<ITaggable>(InActor))
	{
		CastedActor->GetTagContainer().RemoveTags(FGameplayTagContainer::CreateFromArray(InTags));
		for(FGameplayTag Tag : InTags)
		{
			CastedActor->HandleTagChanged(Tag, false);
		}
	}
}

void UGameplayTagUtils::RemoveTagFromActor(AActor* InActor, FGameplayTag InTag)
{
	if(ITaggable* CastedActor = Cast<ITaggable>(InActor))
	{
		CastedActor->GetTagContainer().RemoveTag(InTag);
		CastedActor->HandleTagChanged(InTag, false);
	}
}
