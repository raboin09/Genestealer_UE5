// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "API/AIPawn.h"
#include "BehaviorTree/BehaviorTree.h"
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
	FORCEINLINE virtual EAffiliation GetAffiliation() const override { return CurrentAffiliation; }
	FORCEINLINE virtual UBehaviorTree* GetAIBehavior() const override { return InstancedBehaviorTree ? InstancedBehaviorTree : DefaultBehaviorTree; }
	FORCEINLINE virtual FPlayerInCombatChanged& OnCombatStateChanged() override { return PlayerInCombatChanged; }
	FORCEINLINE virtual bool IsAIFiring() const override { return IsFiring(); }
	virtual FVector GetSocketLocation(FName SocketName, bool bWeaponMesh = false) const override;
	virtual void FireWeapon(bool bStartFiring) override;
	virtual void Aim(bool bStartAiming) override;
	virtual float GetWeaponRange() const override;

protected:
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Defaults")
	UBehaviorTree* DefaultBehaviorTree;
	UPROPERTY(EditInstanceOnly, Category="Genestealer|Defaults")
	UBehaviorTree* InstancedBehaviorTree;
};
