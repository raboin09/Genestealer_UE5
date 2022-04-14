// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AIPawn.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UAIPawn : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GENESTEALER_API IAIPawn
{
	GENERATED_BODY()

public:
	virtual class UBehaviorTree* GetAIBehavior() const PURE_VIRTUAL(IAIPawn::GetAIBehavior, return nullptr;)
};
