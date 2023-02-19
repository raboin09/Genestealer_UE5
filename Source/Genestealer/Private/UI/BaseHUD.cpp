// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/BaseHUD.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Core/UIEventHub.h"
#include "Utils/CombatUtils.h"
#include "Utils/CoreUtils.h"

void ABaseHUD::DrawHUD()
{
	Super::DrawHUD();
	if(bShouldDrawCrosshair && CurrentCrosshair)
	{
		const FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(this);
		const float DrawX = (ViewportSize.X / 2) - (CrosshairDrawSize / 2);
		const float DrawY = ViewportSize.Y / 2 - (CrosshairDrawSize / 2);
		DrawTexture(CurrentCrosshair, DrawX, DrawY, CrosshairDrawSize, CrosshairDrawSize, 1.f, 1.f, 1.f, 1.f, CurrentCrosshairColor);
	}
}

void ABaseHUD::InitHUDOnNewPawnPossessed()
{
	Controller = UCoreUtils::GetBasePlayerController(this);
	HealthDisplay = Internal_CreateWidget<UUIUWHealthDisplay>(HealthDisplayClass);
	DamageDisplay = Internal_CreateWidget<UUIUWDamageDisplay>(DamageDisplayClass);
	AmmoDisplay = Internal_CreateWidget<UUIUWAmmoDisplay>(AmmoDisplayClass);
	InteractDisplay = Internal_CreateWidget<UUIUWInteractDisplay>(InteractDisplayClass);
	
	if(UUIEventHub* UIEventHub = UCoreUtils::GetUIEventHub(this))
	{
		UIEventHub->OnNewActorTargeted().AddDynamic(this, &ABaseHUD::HandleNewActorTargeted);
		UIEventHub->OnPlayerAimingChanged().AddDynamic(this, &ABaseHUD::HandlePlayerAimingChanged);
	}
}

void ABaseHUD::BeginPlay()
{
	Super::BeginPlay();
}

void ABaseHUD::HandleNewActorTargeted(const FNewActorTargetedPayload& NewActorTargetedPayload)
{
	if(!Controller)
	{
		return;
	}

	if(!NewActorTargetedPayload.NewlyTargetedActor)
	{
		CurrentCrosshairColor = FLinearColor::White;
	}
	else if(UCombatUtils::AreActorsAllies(NewActorTargetedPayload.NewlyTargetedActor, Controller->PlayerCharacter))
	{
		CurrentCrosshairColor = FLinearColor::Green;
	} else if(UCombatUtils::AreActorsEnemies(NewActorTargetedPayload.NewlyTargetedActor, Controller->PlayerCharacter))
	{
		CurrentCrosshairColor = FLinearColor::Red;
	} else
	{
		CurrentCrosshairColor = FLinearColor::White;	
	}
}

void ABaseHUD::HandlePlayerAimingChanged(const FPlayerAimingChangedPayload& PlayerAimingChangedPayload)
{
	bool bPawnIsNotPaused = true;
	if(Controller)
	{
		bPawnIsNotPaused = !UGameplayTagUtils::ActorHasGameplayTag(Controller->PlayerCharacter, TAG_STATE_MAIN_MENU);
	}
	bShouldDrawCrosshair = PlayerAimingChangedPayload.bIsAiming && bPawnIsNotPaused;
	CurrentCrosshair = PlayerAimingChangedPayload.CrosshairTexture;
	CrosshairDrawSize = PlayerAimingChangedPayload.CrosshairSize;
}