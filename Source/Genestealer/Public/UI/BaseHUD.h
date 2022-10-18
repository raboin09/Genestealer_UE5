// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Core/BasePlayerController.h"
#include "GameFramework/HUD.h"
#include "UMG/InGameDisplay/UIUWAmmoDisplay.h"
#include "UMG/InGameDisplay/UIUWDamageDisplay.h"
#include "UMG/InGameDisplay/UIUWHealthDisplay.h"

#include "BaseHUD.generated.h"

struct FWound;

/**
 * 
 */
UCLASS(Abstract)
class GENESTEALER_API ABaseHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;

	void InitHUDOnNewPawnPossessed();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category="UI|Defaults")
	float TimeUntilUIHidden;
	UPROPERTY(EditDefaultsOnly, Category="UI|Classes")
	TSubclassOf<UUIUWAmmoDisplay> AmmoDisplayClass;
	UPROPERTY(EditDefaultsOnly, Category="UI|Classes")
	TSubclassOf<UUIUWDamageDisplay> DamageDisplayClass;
	UPROPERTY(EditDefaultsOnly, Category="UI|Classes")
	TSubclassOf<UUIUWHealthDisplay> HealthDisplayClass;
	
private:

	UFUNCTION()
	void HandleNewActorTargeted(const FNewActorTargetedPayload& NewActorTargetedPayload);
	UFUNCTION()
	void HandlePlayerAimingChanged(const FPlayerAimingChangedPayload& PlayerAimingChangedPayload);
	
	template<typename T>
	FORCEINLINE T* Internal_CreateWidget(TSubclassOf<T> WidgetClass) const
	{
		if(!WidgetClass)
		{
			return nullptr;
		}

		if(T* TempWidget = CreateWidget<T>(Controller, WidgetClass))
		{
			TempWidget->AddToViewport();
			return TempWidget;
		}
		return nullptr;	
	}

	UPROPERTY()
	ABasePlayerController* Controller;

	UPROPERTY()
	UUIUWAmmoDisplay* AmmoDisplay;
	UPROPERTY()
	UUIUWDamageDisplay* DamageDisplay;
	UPROPERTY()
	UUIUWHealthDisplay* HealthDisplay;

	UPROPERTY()
	bool bShouldDrawCrosshair;
	UPROPERTY()
	FTimerHandle HideDisplaysTimer;
	UPROPERTY()
	float CrosshairDrawSize;
	UPROPERTY()
	UTexture2D* CurrentCrosshair;
	UPROPERTY()
	FLinearColor CurrentCrosshairColor = FLinearColor::White;
	
};
