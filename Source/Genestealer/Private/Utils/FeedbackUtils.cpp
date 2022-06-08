// Fill out your copyright notice in the Description page of Project Settings.


#include "Utils/FeedbackUtils.h"

#include "Core/BasePlayerController.h"

void UFeedbackUtils::TryPlayCameraShake(APawn* InPawn, TSubclassOf<UCameraShakeBase> CameraShake, float Scale)
{
	if(!InPawn)
	{
		return;
	}

	if(ABasePlayerController* BasePlayerController = Cast<ABasePlayerController>(InPawn->GetController()))
	{
		Interna_TryPlayCameraShake(BasePlayerController, CameraShake, Scale);
	}
}

void UFeedbackUtils::TryPlayCameraShake(AController* InPlayerController, TSubclassOf<UCameraShakeBase> CameraShake, float Scale)
{
	if(!InPlayerController)
	{
		return;
	}

	if(ABasePlayerController* BasePlayerController = Cast<ABasePlayerController>(InPlayerController))
	{
		Interna_TryPlayCameraShake(BasePlayerController, CameraShake, Scale);
	}
}

bool UFeedbackUtils::ShouldCameraShake()
{
	return true;
}

void UFeedbackUtils::Interna_TryPlayCameraShake(ABasePlayerController* InPlayerController, TSubclassOf<UCameraShakeBase> CameraShake, float Scale)
{
	if(ShouldCameraShake() && InPlayerController)
	{
		InPlayerController->ClientStartCameraShake(CameraShake, Scale);
	}
}
