// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasePlayerController.h"
#include "UObject/Object.h"
#include "DevSandboxManager.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class GENESTEALER_API UDevSandboxManager : public UObject
{
	GENERATED_BODY()

public:
	void SetupSandboxGame() const;
};
