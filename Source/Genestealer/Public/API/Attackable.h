// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Types/CombatTypes.h"
#include "UObject/Interface.h"
#include "Attackable.generated.h"

class UHealthComponent;

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UAttackable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GENESTEALER_API IAttackable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Attackable")
	virtual EAbsoluteAffiliation GetAffiliation() const PURE_VIRTUAL(IAttackable::GetAffiliation, return EAbsoluteAffiliation::Neutral;)
	virtual UHealthComponent* GetHealthComponent() const PURE_VIRTUAL(IAttackable::GetHealthComponent, return nullptr;)
	virtual FVector GetHeadLocation() const PURE_VIRTUAL(IAttackable::GetHeadLocation, return FVector::ZeroVector;)
	UFUNCTION(BlueprintCallable, Category = "Attackable")
	virtual FVector GetChestLocation() const PURE_VIRTUAL(IAttackable::GetChestLocation, return FVector::ZeroVector;)
	virtual FVector GetPelvisLocation() const PURE_VIRTUAL(IAttackable::GetPelvisLocation, return FVector::ZeroVector;)
	virtual class UEffectContainerComponent* GetEffectContainerComponent() const PURE_VIRTUAL(IAttackable::GetEffectContainerComponent, return nullptr;)
};
