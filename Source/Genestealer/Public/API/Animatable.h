// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
	virtual float TryPlayAnimMontage(const FAnimMontagePlayData& AnimMontageData) PURE_VIRTUAL(IAnimatable::TryPlayAnimMontage, return -1.f;)
	virtual float ForcePlayAnimMontage(const FAnimMontagePlayData& AnimMontageData) PURE_VIRTUAL(IAnimatable::ForcePlayAnimMontage, return -1.f;)
	virtual void ForceStopAnimMontage(UAnimMontage* AnimMontage) PURE_VIRTUAL(IAnimatable::ForceStopAnimMontage, )
	virtual UAnimMontage* GetCurrentPlayingMontage() const PURE_VIRTUAL(IAnimatable::GetCurrentPlayingMontage, return nullptr; )
	virtual bool IsAiming() const PURE_VIRTUAL(IAnimatable::IsAiming, return false;)
	virtual bool IsFiring() const PURE_VIRTUAL(IAnimatable::IsFiring, return false;)
	virtual bool IsReady() const PURE_VIRTUAL(IAnimatable::IsReady, return false;)
	virtual bool IsInCover() const PURE_VIRTUAL(IAnimatable::IsInCover, return false;)
	virtual bool IsRagdoll() const PURE_VIRTUAL(IAnimatable::IsRagdoll, return false;)
	virtual bool HasRightInput() const PURE_VIRTUAL(IAnimatable::HasRightInput, return false;)
};
