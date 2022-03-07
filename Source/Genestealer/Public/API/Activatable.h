// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Activatable.generated.h"

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UActivatable : public UInterface
{
	GENERATED_BODY()
};

class GENESTEALER_API IActivatable
{
	GENERATED_BODY()
	
public:
	virtual void Activate() = 0;
	virtual void Deactivate() = 0;
	virtual void EnableComboWindow() = 0;
	virtual void DisableComboWindow() = 0;
};
