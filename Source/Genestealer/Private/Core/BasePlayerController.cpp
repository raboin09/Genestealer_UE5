// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/BasePlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Characters/BasePlayerCharacter.h"
#include "Kismet/KismetSystemLibrary.h"

void ABasePlayerController::OnPossess(APawn* NewPawn)
{
	Super::OnPossess(NewPawn);
	PlayerCharacter = Cast<ABasePlayerCharacter>(NewPawn);
	Internal_SetupInputs();
}

void ABasePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();	
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->ClearActionEventBindings();
		EnhancedInputComponent->ClearActionValueBindings();
		EnhancedInputComponent->ClearDebugKeyBindings();

		BindActions(DefaultInputMappingContext);
	}
}

void ABasePlayerController::BindActions(UInputMappingContext* Context)
{
	if (Context)
	{
		const TArray<FEnhancedActionKeyMapping>& Mappings = Context->GetMappings();		
		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
		{
			// There may be more than one keymapping assigned to one action. So, first filter duplicate action entries to prevent multiple delegate bindings
			TSet<const UInputAction*> UniqueActions;
			for (const FEnhancedActionKeyMapping& Keymapping : Mappings)
			{
				UniqueActions.Add(Keymapping.Action);
			}
			for (const UInputAction* UniqueAction : UniqueActions)
			{
				EnhancedInputComponent->BindAction(UniqueAction, ETriggerEvent::Triggered, Cast<UObject>(this), UniqueAction->GetFName());
			}
		}
	}
}

void ABasePlayerController::Internal_SetupInputs()
{
	if (PlayerCharacter)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			FModifyContextOptions Options;
			Options.bForceImmediately = 1;
			Subsystem->AddMappingContext(DefaultInputMappingContext, 1, Options);
		}
	}
}

void ABasePlayerController::CoverAction(const FInputActionValue& Value)
{
	if (PlayerCharacter)
	{
		PlayerCharacter->Input_CoverAction();
	}
}

void ABasePlayerController::ForwardMovementAction(const FInputActionValue& Value)
{
	if (PlayerCharacter)
	{
		PlayerCharacter->Input_ForwardMovement(Value.GetMagnitude());
	}
}

void ABasePlayerController::RightMovementAction(const FInputActionValue& Value)
{
	if (PlayerCharacter)
	{
		PlayerCharacter->Input_RightMovement(Value.GetMagnitude());
	}
}

void ABasePlayerController::CameraUpAction(const FInputActionValue& Value)
{
	if (PlayerCharacter)
	{
		PlayerCharacter->Input_CameraUp(Value.GetMagnitude());
	}
}

void ABasePlayerController::CameraRightAction(const FInputActionValue& Value)
{
	if (PlayerCharacter)
	{
		PlayerCharacter->Input_CameraRight(Value.GetMagnitude());
	}
}