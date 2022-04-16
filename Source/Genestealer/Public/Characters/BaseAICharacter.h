// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "API/AIPawn.h"
#include "Characters/BaseCharacter.h"
#include "BaseAICharacter.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class GENESTEALER_API ABaseAICharacter : public ABaseCharacter, public IAIPawn
{
	GENERATED_BODY()

public:
	ABaseAICharacter(const FObjectInitializer& ObjectInitializer);

	////////////////////////////////
	/// IAIPawn override
	////////////////////////////////
	FORCEINLINE virtual UBehaviorTree* GetAIBehavior() const override { return InstancedBehaviorTree ? InstancedBehaviorTree : DefaultBehaviorTree; }
	virtual void FireWeapon(bool bStartFiring) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Defaults")
	UBehaviorTree* DefaultBehaviorTree;
	UPROPERTY(EditInstanceOnly, Category="Genestealer|Defaults")
	UBehaviorTree* InstancedBehaviorTree;
};
