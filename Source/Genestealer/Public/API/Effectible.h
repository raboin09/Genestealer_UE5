// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/EffectContainerComponent.h"
#include "UObject/Interface.h"
#include "Effectible.generated.h"

UINTERFACE(MinimalAPI)
class UEffectible : public UInterface
{
	GENERATED_BODY()
};

class GENESTEALER_API IEffectible
{
	GENERATED_BODY()
	
public:
	virtual UEffectContainerComponent* GetEffectContainerComponent() const = 0;
};
