// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Library/ALSCharacterEnumLibrary.h"
#include "UObject/Interface.h"
#include "Animatable.generated.h"

USTRUCT(BlueprintType)
struct FAnimMontagePlayData
{
	GENERATED_BODY()

	UPROPERTY()
	UAnimMontage* MontageToPlay = nullptr;
	FName MontageSection = NAME_None;
	bool bShouldBlendOut = true;
	bool bForcePlay = false;
	float PlayRate = 1.f;
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UAnimatable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GENESTEALER_API IAnimatable
{
	GENERATED_BODY()

public:
	virtual float PlayWeaponFireAnimation() PURE_VIRTUAL(IAnimatable::PlayWeaponFireAnimation, return -1.f;)
	virtual void StopWeaponFireAnimation() PURE_VIRTUAL(IAnimatable::PlayWeaponFireAnimation,)
	virtual float TryPlayAnimMontage(const FAnimMontagePlayData& AnimMontageData) PURE_VIRTUAL(IAnimatable::TryPlayAnimMontage, return -1.f;)
	virtual float ForcePlayAnimMontage(const FAnimMontagePlayData& AnimMontageData) PURE_VIRTUAL(IAnimatable::ForcePlayAnimMontage, return -1.f;)
	virtual void ChangeOverlayState(EALSOverlayState InOverlayState) PURE_VIRTUAL(IAnimatable::ChangeOverlayState,)
};
