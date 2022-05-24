// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FeedbackUtils.generated.h"

/**
 * 
 */
UCLASS()
class GENESTEALER_API UFeedbackUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	static bool ShouldCameraShake();	
};
