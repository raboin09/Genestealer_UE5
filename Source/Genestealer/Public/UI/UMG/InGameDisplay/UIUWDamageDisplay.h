// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/UMG/UIUWImage.h"
#include "Types/EventDeclarations.h"
#include "UIUWDamageDisplay.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class GENESTEALER_API UUIUWDamageDisplay : public UUIUWImage
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void HandleCurrentHealthChanged(const FCurrentWoundEventPayload& CurrentWoundEventPayload);
	
protected:
	void PlayLightDamageDisplay();
	void PlayHeavyDamageDisplay();
	virtual void NativeOnInitialized() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Animations, Transient, meta = (BindWidgetAnimOptional))
	UWidgetAnimation* LightDamageAnim;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Animations, Transient, meta = (BindWidgetAnimOptional))
	UWidgetAnimation* HeavyDamageAnim;

private:
	bool Internal_IsLowOnHealthOnLastWound(const FCurrentWoundEventPayload& CurrentWoundEventPayload) const;
	bool Internal_IsHighOnHealthOnLastWound(const FCurrentWoundEventPayload& CurrentWoundEventPayload) const;
};
