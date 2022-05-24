// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/BasePlayerController.h"

#include "EnhancedInputComponent.h"
#include "Characters/BasePlayerCharacter.h"

void ABasePlayerController::CoverAction(const FInputActionValue& Value)
{
	if (PlayerCharacter)
	{
		PlayerCharacter->CoverDodgeAction();
	}
}

void ABasePlayerController::OnPossess(APawn* NewPawn)
{
	Super::OnPossess(NewPawn);
	PlayerCharacter = Cast<ABasePlayerCharacter>(NewPawn);
}
