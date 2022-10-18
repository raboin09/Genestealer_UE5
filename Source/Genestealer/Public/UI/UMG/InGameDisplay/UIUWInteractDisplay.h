// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Types/EventDeclarations.h"
#include "UI/UMG/UIUWDisplayText.h"
#include "UIUWInteractDisplay.generated.h"

/**
 * 
 */
UCLASS()
class GENESTEALER_API UUIUWInteractDisplay : public UUIUWDisplayText
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void HandleNewInteractEvent(const FNewActorTargetedPayload& NewActorTargetedPayload);

protected:
	void ShowInteractDisplay();
	void HideInteractDisplay();
	
	// UFUNCTION(BlueprintImplementableEvent)
	// void K2_HandleNewInteract(FText InteractText);
	virtual void NativeConstruct() override;
};
