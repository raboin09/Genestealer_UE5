// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interactable.generated.h"

UENUM()
enum class EOutlineColor : uint8
{
	Green,
	Red,
	Gray,
	Purple
};

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UInteractable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GENESTEALER_API IInteractable
{
	GENERATED_BODY()

public:
	virtual void SwitchOutlineOnMesh(bool bShouldOutline) PURE_VIRTUAL(IInteractableInterface::SwitchOutlineOnMesh, );
	virtual void InteractWithActor(AActor* InstigatingActor) PURE_VIRTUAL(IInteractableInterface::InteractWithActor, );
};
