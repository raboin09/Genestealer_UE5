// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Attackable.h"
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
	virtual EAffiliation GetInteractableAffiliation() const PURE_VIRTUAL(IInteractable::GetInteractableAffiliation, return EAffiliation::Neutral;)
	virtual void SwitchOutlineOnMesh(bool bShouldOutline) PURE_VIRTUAL(IInteractable::SwitchOutlineOnMesh, );
	virtual void InteractWithActor(AActor* InstigatingActor) PURE_VIRTUAL(IInteractable::InteractWithActor, );
};
