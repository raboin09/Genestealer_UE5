// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Types/EventDeclarations.h"
#include "Questable.generated.h"

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UQuestable : public UInterface
{
	GENERATED_BODY()
};

class GENESTEALER_API IQuestable
{
	GENERATED_BODY()
	
public:
    	virtual FQuestObjectiveEvent& OnQuestObjectiveEvent() = 0;
};
