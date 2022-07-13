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
	bool bForceInPlace = false;
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
	virtual FTransform GetWeaponLeftHandPlacementLocation() const PURE_VIRTUAL(IAnimatable::GetWeaponLeftHandPlacementLocation, return FTransform();) 

	virtual void SetCameraLookingMode() PURE_VIRTUAL(IAnimatable::SetCameraLookingMode, )
	virtual void SetCameraOverRightShoulder(bool bNewRightShoulder) PURE_VIRTUAL(IAnimatable::SetCameraOverRightShoulder, )
	virtual void SetAimingMode(bool bForce = true, bool bAffectsCamera = true) PURE_VIRTUAL(IAnimatable::SetAimingMode, )
	virtual void SetLookingMode(bool bForce = true, bool bAffectsCamera = true) PURE_VIRTUAL(IAnimatable::SetLookingMode, )
	virtual void SetVelocityMode(bool bForce = true, bool bAffectsCamera = true) PURE_VIRTUAL(IAnimatable::SetVelocityMode, )

	virtual void SetWalkingGait() PURE_VIRTUAL(IAnimatable::SetWalkingGait, )
	virtual void SetRunningGait() PURE_VIRTUAL(IAnimatable::SetRunningGait, )
	virtual void SetSprintingGait() PURE_VIRTUAL(IAnimatable::SetSprintingGait, )

	virtual void SetStanding() PURE_VIRTUAL(IAnimatable::SetStanding, )
	virtual void SetCrouching() PURE_VIRTUAL(IAnimatable::SetCrouching, )
	
	virtual bool IsAiming() PURE_VIRTUAL(IAnimatable::IsAiming, return false;)
	virtual bool IsFiring() PURE_VIRTUAL(IAnimatable::IsFiring, return false;)
	virtual bool IsReady() PURE_VIRTUAL(IAnimatable::IsReady, return false;)
	virtual bool IsInCover() PURE_VIRTUAL(IAnimatable::IsInCover, return false;)
	virtual bool IsRagdoll() PURE_VIRTUAL(IAnimatable::IsRagdoll, return false;)
	virtual bool HasRightInput() PURE_VIRTUAL(IAnimatable::HasRightInput, return false;)
};
