// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "Taggable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UTaggable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GENESTEALER_API ITaggable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	virtual FGameplayTagContainer& GetTagContainer() = 0;
};
