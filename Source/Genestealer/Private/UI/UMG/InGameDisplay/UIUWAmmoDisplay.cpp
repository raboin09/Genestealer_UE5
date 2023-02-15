// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/UMG/InGameDisplay/UIUWAmmoDisplay.h"

#include "Core/UIEventHub.h"
#include "UI/BaseHUD.h"
#include "Utils/CoreUtils.h"

void UUIUWAmmoDisplay::HandleCurrentHealthChanged(const FAmmoAmountChangedPayload& AmmoAmountChangedPayload)
{
	K2_HandleAmmoChange(AmmoAmountChangedPayload.CurrentAmmoInClip, AmmoAmountChangedPayload.ClipCapacity, AmmoAmountChangedPayload.CurrentAmmo, AmmoAmountChangedPayload.MaxAmmo);
	ShowAmmoDisplay();
}

void UUIUWAmmoDisplay::ShowAmmoDisplay()
{
	if(!IsInViewport()) {
		AddToViewport();
	}
	FadeInDisplay(true);
}

void UUIUWAmmoDisplay::HideAmmoDisplay()
{
	FadeOutDisplay();
}

void UUIUWAmmoDisplay::NativeConstruct()
{
	Super::NativeConstruct();
	if(UUIEventHub* UIEventHub = UCoreUtils::GetUIEventHub(this))
	{
		UIEventHub->OnAmmoAmountChanged().AddDynamic(this, &UUIUWAmmoDisplay::HandleCurrentHealthChanged);
	}
}
