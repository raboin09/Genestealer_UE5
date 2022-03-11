// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/BasePlayerController.h"

#include "EnhancedInputComponent.h"
#include "Characters/BasePlayerCharacter.h"

void ABasePlayerController::OnPossess(APawn* NewPawn)
{
	Super::OnPossess(NewPawn);
	PlayerCharacter = Cast<ABasePlayerCharacter>(NewPawn);
}

void ABasePlayerController::ForwardMovementAction(const FInputActionValue& Value)
{
	if (PlayerCharacter)
	{
		PlayerCharacter->ForwardMovementAction(Value.GetMagnitude());
	}
}

void ABasePlayerController::RightMovementAction(const FInputActionValue& Value)
{
	if (PlayerCharacter)
	{
		PlayerCharacter->RightMovementAction(Value.GetMagnitude());
	}
}

void ABasePlayerController::CameraUpAction(const FInputActionValue& Value)
{
	if (PlayerCharacter)
	{
		PlayerCharacter->CameraUpAction(Value.GetMagnitude());
	}
}

void ABasePlayerController::CameraRightAction(const FInputActionValue& Value)
{
	if (PlayerCharacter)
	{
		PlayerCharacter->CameraRightAction(Value.GetMagnitude());
	}
}

void ABasePlayerController::SprintAction(const FInputActionValue& Value)
{
	if (PlayerCharacter)
	{
		PlayerCharacter->SprintAction(Value.Get<bool>());
	}
}

void ABasePlayerController::AimAction(const FInputActionValue& Value)
{
	if (PlayerCharacter)
	{
		PlayerCharacter->K2_HandleAimAction(Value.Get<bool>());
	}
}

void ABasePlayerController::CoverDodgeAction(const FInputActionValue& Value)
{
	if (PlayerCharacter)
	{
		PlayerCharacter->K2_HandleCoverDodgeAction();
	}
}
