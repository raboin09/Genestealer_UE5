// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseActivatableNotifyState.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Weapons/BaseWeapon.h"
#include "GLAN_EnableComboWindow.generated.h"

/**
 * 
 */
UCLASS()
class GENESTEALER_API UGLAN_EnableComboWindow : public UBaseActivatableNotifyState
{
	GENERATED_BODY()

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};