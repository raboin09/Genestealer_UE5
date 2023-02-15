// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AnimUtils.generated.h"

/**
 * 
 */
UCLASS()
class GENESTEALER_API UAnimUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static void AnimUtils_MeshAdditiveFramePose(UObject* InAnimSequence, UAnimSequence* FramePoseAsset);
	UFUNCTION(BlueprintCallable)
	static void AnimUtils_CopyCurves(UObject* AnimToCopy, UAnimSequence* AnimSource);
};
