// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UIUserWidget.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class GENESTEALER_API UUIUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	virtual void FadeOutDisplay(bool bInstant = false, float PlayRate = 1.f);
	UFUNCTION(BlueprintCallable)
	virtual void FadeInDisplay(bool bInstant = false, float PlayRate = 1.f);

	UFUNCTION(BlueprintCallable)
	virtual void StartBlinking();
	UFUNCTION(BlueprintCallable)
	virtual void StopBlinking();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Animations, Transient, meta = (BindWidgetAnimOptional))
	UWidgetAnimation* FadeAnim;

private:
	UPROPERTY()
	UUMGSequencePlayer* AnimSequencePlayer;	
};
