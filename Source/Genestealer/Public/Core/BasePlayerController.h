// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "Character/ALSPlayerController.h"
#include "Characters/BasePlayerCharacter.h"
#include "BasePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class GENESTEALER_API ABasePlayerController : public AALSPlayerController
{
	GENERATED_BODY()

public:
	AActor* GetTargetedActor() const { return nullptr; }
	
	UPROPERTY()
	ABasePlayerCharacter* PlayerCharacter;
};
