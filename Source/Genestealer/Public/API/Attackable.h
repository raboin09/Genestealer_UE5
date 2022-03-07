// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Attackable.generated.h"

class UHealthComponent;

UENUM(BlueprintType)
enum class EAffiliation : uint8
{
	Allies,
	Enemies,
	Neutral,
	All
};

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
	virtual EAffiliation GetAffiliation() PURE_VIRTUAL(IPlayableCharacter::GetAffiliation, return EAffiliation::Neutral;)
	virtual UHealthComponent* GetHealthComponent() = 0;
};
