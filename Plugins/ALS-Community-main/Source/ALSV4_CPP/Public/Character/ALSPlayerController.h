// Copyright:       Copyright (C) 2022 Doğa Can Yanıkoğlu
// Source Code:     https://github.com/dyanikoglu/ALS-Community

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/PlayerController.h"
#include "ALSPlayerController.generated.h"

class AALSBaseCharacter;
class UInputMappingContext;

/**
 * Player controller class
 */
UCLASS(Blueprintable, BlueprintType)
class ALSV4_CPP_API AALSPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void OnPossess(APawn* NewPawn) override;

	virtual void OnRep_Pawn() override;

	virtual void SetupInputComponent() override;

	virtual void BindActions(UInputMappingContext* Context);

protected:
	void SetupInputs();
	
	void SetupCamera();
	
public:
	/** Main character reference */
	UPROPERTY(BlueprintReadOnly)
	AALSBaseCharacter* PossessedCharacter = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS|Input")
	UInputMappingContext* DefaultInputMappingContext = nullptr;
};
