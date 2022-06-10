// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Types/EventDeclarations.h"
#include "UI/UMG/UIUserWidget.h"
#include "UIUWHealthDisplay.generated.h"

struct FWound;

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class GENESTEALER_API UUIUWHealthDisplay : public UUIUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void HandleCurrentHealthChanged(const FCurrentWoundEventPayload& CurrentWoundEventPayload);

protected:
	void ShowHealthBar();
	void HideHealthBar();
	
	UFUNCTION(BlueprintImplementableEvent)
	void K2_HealthChange(float NewPercentage, int32 WoundsLeft, int32 MaxWounds);
	virtual void NativeConstruct() override;
};
