// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/CoreWidgets/UIImage.h"
#include "UI/UMG/UIUserWidget.h"
#include "UIUWImage.generated.h"

/**
 * 
 */
UCLASS()
class GENESTEALER_API UUIUWImage : public UUIUserWidget
{
	GENERATED_BODY()

protected:
	// Custom Image widget
	UPROPERTY(BlueprintReadWrite, meta=(BindWidgetOptional))
	UUIImage* MainImage;
};
