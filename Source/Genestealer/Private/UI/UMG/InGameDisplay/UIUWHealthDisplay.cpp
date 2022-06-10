// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/UMG/InGameDisplay/UIUWHealthDisplay.h"

#include "Core/UIEventHub.h"
#include "UI/BaseHUD.h"
#include "Utils/CoreUtils.h"

void UUIUWHealthDisplay::HandleCurrentHealthChanged(const FCurrentWoundEventPayload& CurrentWoundEventPayload)
{
	K2_HealthChange(CurrentWoundEventPayload.Percentage, CurrentWoundEventPayload.MaxWounds - CurrentWoundEventPayload.NewWound.WoundArrayIndex, CurrentWoundEventPayload.MaxWounds);
	if(CurrentWoundEventPayload.bNaturalChange)
	{
		ShowHealthBar();
	}
}

void UUIUWHealthDisplay::ShowHealthBar()
{
	if(!IsInViewport()) {
			AddToViewport();
	}
	FadeInDisplay(true);
}

void UUIUWHealthDisplay::HideHealthBar()
{
	FadeOutDisplay();
}

void UUIUWHealthDisplay::NativeConstruct()
{
	Super::NativeConstruct();
	if(UUIEventHub* UIEventHub = UCoreUtils::GetUIEventHub(this))
	{
		UIEventHub->OnCurrentWoundHealthChanged().AddDynamic(this, &UUIUWHealthDisplay::HandleCurrentHealthChanged);
	}
}
