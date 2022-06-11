// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AGRCoreAnimInstance.h"
#include "Character/Animation/ALSCharacterAnimInstance.h"
#include "BaseAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class GENESTEALER_API UBaseAnimInstance : public UALSCharacterAnimInstance
{
	GENERATED_BODY()

public:
	UBaseAnimInstance();
	virtual void NativeInitializeAnimation() override;
	void DisableRootMotionModeForDuration(float Duration);
	
protected:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Genestealer")
	bool bAiming = false;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Genestealer")
	bool bFiring = false;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Genestealer")
	bool bIsInCover = false;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Genestealer")
	bool bReady = false;

private:

	UFUNCTION()
	void EnableRootMotionMode();
	
	ERootMotionMode::Type RootMotionCache;
	FTimerHandle TimerHandle_RootMotionDisable;
};
