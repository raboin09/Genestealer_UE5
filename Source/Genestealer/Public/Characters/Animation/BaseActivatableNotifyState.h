// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "UObject/Object.h"
#include "BaseActivatableNotifyState.generated.h"

/**
 * 
 */
UCLASS()
class GENESTEALER_API UBaseActivatableNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()

protected:
	UObject* GetActivatableAbilityFromOwner(const USkeletalMeshComponent* MeshComp) const;
};
