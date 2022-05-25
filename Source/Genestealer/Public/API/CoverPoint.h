// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CoverPoint.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UCoverPoint : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GENESTEALER_API ICoverPoint
{
	GENERATED_BODY()

public:
	virtual void OccupyCover(class ABaseCharacter* InActor, const FVector& InTargetCoverLocation, const FVector& InHitNormal) PURE_VIRTUAL(ICoverPoint::OccupyCover, )
	virtual void VacateCover(ABaseCharacter* InActor) PURE_VIRTUAL(ICoverPoint::VacateCover, )
	virtual void StartCoverAim() PURE_VIRTUAL(ICoverPoint::StartCoverAim, )
	virtual void StopCoverAim() PURE_VIRTUAL(ICoverPoint::StopCoverAim, )
	virtual void StartCoverFire() PURE_VIRTUAL(ICoverPoint::StartCoverFire, )
	virtual void StopCoverFire() PURE_VIRTUAL(ICoverPoint::StopCoverFire, )
};
