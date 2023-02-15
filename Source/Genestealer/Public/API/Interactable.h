// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Attackable.h"
#include "UObject/Interface.h"
#include "Interactable.generated.h"

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
	virtual EAbsoluteAffiliation GetInteractableAffiliation() const PURE_VIRTUAL(IInteractable::GetInteractableAffiliation, return EAbsoluteAffiliation::Neutral;)
	virtual void SwitchOutlineOnMesh(bool bShouldOutline) PURE_VIRTUAL(IInteractable::SwitchOutlineOnMesh, );
	virtual void InteractWithActor(AActor* InstigatingActor) PURE_VIRTUAL(IInteractable::InteractWithActor, );
	virtual FString GetInteractionText() const PURE_VIRTUAL(IInteractable::GetInteractionText, return "";);
};
