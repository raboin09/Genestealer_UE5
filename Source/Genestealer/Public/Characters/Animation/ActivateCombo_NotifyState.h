// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "API/Activatable.h"
#include "Weapons/BaseWeapon.h"
#include "ActivateCombo_NotifyState.generated.h"

/**
 * 
 */
UCLASS()
class GENESTEALER_API UActivateCombo_NotifyState : public UAnimNotifyState
{
	GENERATED_BODY()

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	UObject* Internal_GetActivatableAbilityFromOwner(const USkeletalMeshComponent* MeshComp) const;
};
