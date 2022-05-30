// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LockOnComponent.h"
#include "Characters/BaseCharacter.h"
#include "BasePlayerCharacter.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class GENESTEALER_API ABasePlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	ABasePlayerCharacter(const FObjectInitializer& ObjectInitializer);
protected:
	virtual void GL_HandleFireAction(bool bValue) override;

	UPROPERTY()
	UTimelineComponent* LockOnInterpTimeline;
	UPROPERTY(EditDefaultsOnly)
	ULockOnComponent* LockOnComponent;
};
