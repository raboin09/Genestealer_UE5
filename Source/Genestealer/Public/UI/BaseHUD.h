// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Core/BasePlayerController.h"
#include "GameFramework/HUD.h"
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
	FORCEINLINE UUIUWHealthDisplay* GetHealthDisplay() const { return HealthDisplay; }

protected:
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

	UPROPERTY(EditDefaultsOnly, Category="UI|Defaults")
	float TimeUntilUIHidden;
	UPROPERTY(EditDefaultsOnly, Category="UI|Classes")
	TSubclassOf<UUIUWDamageDisplay> DamageDisplayClass;
	UPROPERTY(EditDefaultsOnly, Category="UI|Classes")
	TSubclassOf<UUIUWHealthDisplay> HealthDisplayClass;
	
private:	
	
	UPROPERTY()
	UUIUWDamageDisplay* DamageDisplay;
	UPROPERTY()
	UUIUWHealthDisplay* HealthDisplay;
	UPROPERTY()
	FTimerHandle HideDisplaysTimer;
	UPROPERTY()
	ABasePlayerController* Controller;
};
