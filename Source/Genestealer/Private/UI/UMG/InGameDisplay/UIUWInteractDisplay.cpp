// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/UMG/InGameDisplay/UIUWInteractDisplay.h"

#include "Core/UIEventHub.h"
#include "Utils/CoreUtils.h"


void UUIUWInteractDisplay::HandleNewInteractEvent(const FNewActorTargetedPayload& NewActorTargetedPayload)
{
	if(NewActorTargetedPayload.NewlyTargetedActor)
	{
		ShowInteractDisplay();
		K2_HandleNewInteract(NewActorTargetedPayload.NewlyTargetedActor->GetInteractionText());
	} else
	{
		HideInteractDisplay();
	}
}

void UUIUWInteractDisplay::ShowInteractDisplay()
{
	if(!IsInViewport()) {
		AddToViewport();
	}
	FadeInDisplay(true);
}

void UUIUWInteractDisplay::HideInteractDisplay()
{
	FadeOutDisplay();
}

void UUIUWInteractDisplay::NativeConstruct()
{
	Super::NativeConstruct();
	if(UUIEventHub* UIEventHub = UCoreUtils::GetUIEventHub(this))
	{
		UIEventHub->OnNewActorTargeted().AddDynamic(this, &UUIUWInteractDisplay::HandleNewInteractEvent);
	}
	FadeOutDisplay(true);
}