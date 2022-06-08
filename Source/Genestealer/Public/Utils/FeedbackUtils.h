// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FeedbackUtils.generated.h"


class UCameraShakeBase;

/**
 * 
 */
UCLASS()
class GENESTEALER_API UFeedbackUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static void TryPlayCameraShake(APawn* InPawn, TSubclassOf<UCameraShakeBase> CameraShake, float Scale = 1.f);
	static void TryPlayCameraShake( AController* InPlayerController, TSubclassOf<UCameraShakeBase> CameraShake, float Scale = 1.f);
	static bool ShouldCameraShake();

private:
	static void Interna_TryPlayCameraShake(class ABasePlayerController* InPlayerController, TSubclassOf<UCameraShakeBase> Shake, float Scale = 1.f);
};
