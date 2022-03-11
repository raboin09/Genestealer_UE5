// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/ALSPlayerController.h"
#include "Characters/BasePlayerCharacter.h"
#include "BasePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class GENESTEALER_API ABasePlayerController : public AALSPlayerController
{
	GENERATED_BODY()

public:
	virtual void OnPossess(APawn* NewPawn) override;
	
	AActor* GetTargetedActor() const { return nullptr; }

protected:

	UFUNCTION()
	void ForwardMovementAction(const FInputActionValue& Value);
	UFUNCTION()
	void RightMovementAction(const FInputActionValue& Value);
	UFUNCTION()
	void CameraUpAction(const FInputActionValue& Value);
	UFUNCTION()
	void CameraRightAction(const FInputActionValue& Value);
	UFUNCTION()
	void SprintAction(const FInputActionValue& Value);
	UFUNCTION()
	void AimAction(const FInputActionValue& Value);
	UFUNCTION()
	void CoverDodgeAction(const FInputActionValue& Value);

private:
	UPROPERTY()
	ABasePlayerCharacter* PlayerCharacter;
};
