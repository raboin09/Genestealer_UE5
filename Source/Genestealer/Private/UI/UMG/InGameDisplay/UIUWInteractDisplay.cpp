// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/UMG/InGameDisplay/UIUWInteractDisplay.h"

#include "Core/UIEventHub.h"
#include "Utils/CoreUtils.h"


void UUIUWInteractDisplay::HandleNewInteractEvent(const FNewActorTargetedPayload& NewActorTargetedPayload)
{
	if(NewActorTargetedPayload.NewlyTargetedActor)
	{
		if(UGameplayTagUtils::ActorHasGameplayTag(GetOwningPlayerPawn(), TAG_STATE_MAIN_MENU))
		{
			return;
		}
		if(NewActorTargetedPayload.NewlyTargetedActor->GetInteractionText() != "")
		{
			ShowInteractDisplay();
			K2_HandleNewInteract(NewActorTargetedPayload.NewlyTargetedActor->GetInteractionText());	
		}
	} else
	{
		HideInteractDisplay();
		K2_HandleNewInteract("");	
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
