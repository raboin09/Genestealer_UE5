// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseActivatableNotifyState.h"
#include "API/Interactable.h"
#include "UObject/Object.h"
#include "GLAN_ResetActivatable.generated.h"

/**
 * 
 */
UCLASS()
class GENESTEALER_API UGLAN_ResetActivatable : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

private:
	UObject* Internal_GetActivatableAbilityFromOwner(const USkeletalMeshComponent* MeshComp) const;
};