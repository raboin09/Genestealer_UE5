// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Attackable.h"
#include "Characters/BaseCharacter.h"
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
	virtual class UBehaviorTree* GetAIBehavior() const PURE_VIRTUAL(IAIPawn::GetAIBehavior, return nullptr;)
	virtual bool IsAIFiring() const PURE_VIRTUAL(IAIPawn::IsAIFiring, return false;)
	UFUNCTION(BlueprintCallable)
	virtual void FireWeapon(bool bStartFiring) PURE_VIRTUAL(IAIPawn::FireWeapon, )
	virtual void Aim(bool bStartAiming) PURE_VIRTUAL(IAIPawn::Aim, )
	virtual float GetWeaponRange() const PURE_VIRTUAL(IAIPawn::GetWeaponRange, return 0.f; )
	virtual FVector GetSocketLocation(FName SocketName, bool bWeaponMesh = false) const PURE_VIRTUAL(IAIPawn::GetWeaponMuzzleSocketLocation, return FVector::ZeroVector; )
	virtual EAffiliation GetAffiliation() const PURE_VIRTUAL(IAIPawn::GetAffiliation, return EAffiliation::Neutral; )
	virtual FCharacterInCombatChanged& OnCombatStateChanged() = 0;
};
