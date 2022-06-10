// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/BaseHUD.h"
#include "Utils/CoreUtils.h"

void ABaseHUD::BeginPlay()
{
	Super::BeginPlay();
	Controller = UCoreUtils::GetBasePlayerController(this);
	if(HealthDisplayClass)
	{
		HealthDisplay = CreateWidget<UUIUWHealthDisplay>(Controller, HealthDisplayClass);
		if(HealthDisplay)
		{
			HealthDisplay->AddToViewport();
		}
	}
	
	if(DamageDisplayClass)
	{
		DamageDisplay = CreateWidget<UUIUWDamageDisplay>(Controller, DamageDisplayClass);
		if(DamageDisplay)
		{
			DamageDisplay->AddToViewport();
		}
	}


}

void ABaseHUD::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}