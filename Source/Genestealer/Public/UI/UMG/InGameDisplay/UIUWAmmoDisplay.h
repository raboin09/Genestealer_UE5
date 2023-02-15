// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Types/EventDeclarations.h"
#include "UI/UMG/UIUserWidget.h"
#include "UIUWAmmoDisplay.generated.h"

/**
 * 
 */
UCLASS()
class GENESTEALER_API UUIUWAmmoDisplay : public UUIUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void HandleCurrentHealthChanged(const FAmmoAmountChangedPayload& AmmoAmountChangedPayload);

protected:
	void ShowAmmoDisplay();
	void HideAmmoDisplay();
	
	UFUNCTION(BlueprintImplementableEvent)
	void K2_HandleAmmoChange(int32 CurrentAmmoInClip, int32 ClipCapacity, int32 CurrentAmmo, int32 MaxAmmo);
	virtual void NativeConstruct() override;
	
};
