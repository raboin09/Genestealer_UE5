// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AGRCoreAnimInstance.h"
#include "BaseAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class GENESTEALER_API UBaseAnimInstance : public UAGRCoreAnimInstance
{
	GENERATED_BODY()

public:
	UBaseAnimInstance();
	virtual void NativeInitializeAnimation() override;
	
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
};
