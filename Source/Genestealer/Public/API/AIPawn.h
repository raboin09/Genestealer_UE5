// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Attackable.h"
#include "Types/EventDeclarations.h"
#include "UObject/Interface.h"
#include "AIPawn.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UAIPawn : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GENESTEALER_API IAIPawn
{
	GENERATED_BODY()

public:
	virtual class UBehaviorTree* GetDefaultBehavior() const PURE_VIRTUAL(IAIPawn::GetDefaultBehavior, return nullptr;)
	virtual UBehaviorTree* GetAttackBehavior() const PURE_VIRTUAL(IAIPawn::GetAttackBehavior, return nullptr;)
	virtual bool IsAIFiring() PURE_VIRTUAL(IAIPawn::IsAIFiring, return false;)
	UFUNCTION(BlueprintCallable)
	virtual void FireWeapon(bool bStartFiring) PURE_VIRTUAL(IAIPawn::FireWeapon, )
	virtual void Aim(bool bStartAiming) PURE_VIRTUAL(IAIPawn::Aim, )
	virtual float GetWeaponRange() const PURE_VIRTUAL(IAIPawn::GetWeaponRange, return 0.f; )
	virtual EBallisticSkill GetBallisticSkill() const PURE_VIRTUAL(IAIPawn::GetBallisticSkill, return EBallisticSkill::Default; )
	virtual FVector GetSocketLocation(FName SocketName, bool bWeaponMesh = false) const PURE_VIRTUAL(IAIPawn::GetWeaponMuzzleSocketLocation, return FVector::ZeroVector; )
	virtual EAbsoluteAffiliation GetPawnAffiliation() const PURE_VIRTUAL(IAIPawn::GetPawnAffiliation, return EAbsoluteAffiliation::Neutral; )
	virtual FCharacterInCombatChanged& OnCombatStateChanged() = 0;
};
