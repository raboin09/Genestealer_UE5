// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "Characters/BasePlayerCharacter.h"
#include "BasePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class GENESTEALER_API ABasePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void OnPossess(APawn* NewPawn) override;
	virtual void SetupInputComponent() override;
	virtual void BindActions(UInputMappingContext* Context);
	
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
	
	UPROPERTY(BlueprintReadOnly)
	ABaseCharacter* PossessedCharacter = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Genestealer")
	UInputMappingContext* DefaultInputMappingContext = nullptr;

private:
	void Internal_SetupInputs();
	
	UPROPERTY()
	ABasePlayerCharacter* PlayerCharacter;
};
