// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LockOnComponent.h"
#include "Characters/BaseCharacter.h"
#include "BasePlayerCharacter.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable, AutoExpandCategories=("Genestealer"), PrioritizeCategories = "Genestealer")
class GENESTEALER_API ABasePlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	ABasePlayerCharacter(const FObjectInitializer& ObjectInitializer);

	FORCEINLINE FPlayerAimingChanged& OnPlayerAimingChanged() { return PlayerAimingChanged; }
	
protected:
	virtual void GL_HandleFireAction(bool bValue) override;
	virtual void GL_HandleAimAction(bool bValue) override;

	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Sounds")
	USoundCue* AimInSound;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Sounds")
	USoundCue* AimOutSound;
	
	UPROPERTY(EditDefaultsOnly)
	ULockOnComponent* LockOnComponent;

private:
	FPlayerAimingChanged PlayerAimingChanged;
};
