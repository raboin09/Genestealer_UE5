// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "BaseAICharacter.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class GENESTEALER_API ABaseAICharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	ABaseAICharacter(const FObjectInitializer& ObjectInitializer);
};
