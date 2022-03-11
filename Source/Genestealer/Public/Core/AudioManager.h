// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AudioManager.generated.h"

/**
 * 
 */
UCLASS()
class GENESTEALER_API UAudioManager : public UObject
{
	GENERATED_BODY()
public:
	static UAudioComponent* SpawnSoundAttached(USoundBase* Sound, USceneComponent* ParentComponent);
	static void PlaySoundAtLocation(const UObject* ContextObj, USoundBase* Sound, const FVector& Location);
};