// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/UMG/InGameDisplay/UIUWDamageDisplay.h"

#include "Core/UIEventHub.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Utils/CoreUtils.h"

void UUIUWDamageDisplay::HandleCurrentHealthChanged(const FCurrentWoundEventPayload& CurrentWoundEventPayload)
{
	if(!CurrentWoundEventPayload.bNaturalChange)
	{
		return;
	}
	
	if(CurrentWoundEventPayload.Delta > 0)
	{
		if(Internal_IsLowOnHealthOnLastWound(CurrentWoundEventPayload))
		{
			StartBlinking();
		} else
		{
			if(CurrentWoundEventPayload.Delta <= 50.f)
			{
				PlayLightDamageDisplay();
			} else
			{
				PlayHeavyDamageDisplay();
			}
		}
	}
	else if (Internal_IsHighOnHealthOnLastWound(CurrentWoundEventPayload)) 
	{
		StopBlinking();
	}
}

void UUIUWDamageDisplay::PlayLightDamageDisplay()
{
	if(LightDamageAnim)
	{
		PlayAnimation(LightDamageAnim);
	}
}

void UUIUWDamageDisplay::PlayHeavyDamageDisplay()
{
	if(HeavyDamageAnim)
	{
		PlayAnimation(HeavyDamageAnim);
	}
}

void UUIUWDamageDisplay::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if(MainImage)
	{
		MainImage->SetOpacity(0.f);
	}
	if(UUIEventHub* UIEventHub = UCoreUtils::GetUIEventHub(this))
	{
		UIEventHub->OnCurrentWoundHealthChanged().AddDynamic(this, &UUIUWDamageDisplay::HandleCurrentHealthChanged);
	}
}

bool UUIUWDamageDisplay::Internal_IsLowOnHealthOnLastWound(const FCurrentWoundEventPayload& CurrentWoundEventPayload) const
{
	return CurrentWoundEventPayload.MaxWounds - CurrentWoundEventPayload.NewWound.WoundArrayIndex <= 1 && (CurrentWoundEventPayload.NewWound.CurrentHealth / UKismetMathLibrary::Max(1, CurrentWoundEventPayload.NewWound.MaxHealth)) <= .25f;
}

bool UUIUWDamageDisplay::Internal_IsHighOnHealthOnLastWound(const FCurrentWoundEventPayload& CurrentWoundEventPayload) const
{
	return CurrentWoundEventPayload.MaxWounds - CurrentWoundEventPayload.NewWound.WoundArrayIndex <= 1 && (CurrentWoundEventPayload.NewWound.CurrentHealth / UKismetMathLibrary::Max(1, CurrentWoundEventPayload.NewWound.MaxHealth)) > .25f;
}
