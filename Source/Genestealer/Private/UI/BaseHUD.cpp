// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/BaseHUD.h"
#include "Utils/CoreUtils.h"

void ABaseHUD::BeginPlay()
{
	Super::BeginPlay();
	Controller = UCoreUtils::GetBasePlayerController(this);
	
	HealthDisplay = Internal_CreateWidget<UUIUWHealthDisplay>(HealthDisplayClass);
	DamageDisplay = Internal_CreateWidget<UUIUWDamageDisplay>(DamageDisplayClass);
}