// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayTagUtils.generated.h"

namespace GameplayTag
{
	namespace HitReact
	{
		const FGameplayTag Left = FGameplayTag::RequestGameplayTag("Animation.HitReact.Left");
		const FGameplayTag Front = FGameplayTag::RequestGameplayTag("Animation.HitReact.Front");
		const FGameplayTag Right = FGameplayTag::RequestGameplayTag("Animation.HitReact.Right");
		const FGameplayTag Back = FGameplayTag::RequestGameplayTag("Animation.HitReact.Back");
	}
	
	namespace ActorType
	{
		const FGameplayTag Player = FGameplayTag::RequestGameplayTag("ActorType.Player");
		const FGameplayTag AI = FGameplayTag::RequestGameplayTag("ActorType.AI");
		const FGameplayTag Destructible =  FGameplayTag::RequestGameplayTag("ActorType.Destructible");
		const FGameplayTag Weapon =  FGameplayTag::RequestGameplayTag("ActorType.Weapon");
	}

	namespace State
	{
		const FGameplayTag Stunned =  FGameplayTag::RequestGameplayTag("State.Stunned");	
		const FGameplayTag Loading =  FGameplayTag::RequestGameplayTag("State.Loading");
		const FGameplayTag Dead =  FGameplayTag::RequestGameplayTag("State.Dead");
		const FGameplayTag Aiming =  FGameplayTag::RequestGameplayTag("State.Aiming");
	}

	namespace Attributes
	{
		
	}
}

UCLASS()
class GENESTEALER_API UGameplayTagUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Genestealer|GameplayTagUtils")
	static bool ActorHasGameplayTag(AActor* InActor, const FGameplayTag& InTag);
	UFUNCTION(BlueprintCallable, Category="Genestealer|GameplayTagUtils")
	static void AddTagToActor(AActor* InActor, const FGameplayTag& InTag);
	UFUNCTION(BlueprintCallable, Category="Genestealer|GameplayTagUtils")
	static void RemoveTagFromActor(AActor* InActor, const FGameplayTag& InTag);
};
