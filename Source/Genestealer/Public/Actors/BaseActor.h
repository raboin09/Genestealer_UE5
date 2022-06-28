// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "API/Taggable.h"
#include "GameFramework/Actor.h"
#include "BaseActor.generated.h"

UCLASS(Abstract, Blueprintable, AutoExpandCategories=("Genestealer"), PrioritizeCategories = "Genestealer")
class GENESTEALER_API ABaseActor : public AActor, public ITaggable
{
	GENERATED_BODY()
	
public:	
	ABaseActor();

	////////////////////////////////
	/// ITaggable override
	////////////////////////////////
	UFUNCTION()
	virtual FGameplayTagContainer& GetTagContainer() override { return GameplayTagContainer; }

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly, Category="Genestealer")
	TArray<FGameplayTag> DefaultGameplayTags;

private:
	void Internal_AddDefaultTagsToContainer();
};
