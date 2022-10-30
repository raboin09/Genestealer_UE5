// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CombatTypes.generated.h"

UENUM(BlueprintType)
enum class EAbsoluteAffiliation : uint8
{
	Neutral,
	GenestealerCult,
	ChaosCult,
	Imperium,
	Orks,
	Destructible
};

UENUM(BlueprintType)
enum class EAffectedAffiliation : uint8
{
	Allies,
	Enemies,
	Neutral,
	All
};

UENUM(BlueprintType)
enum class EArmorSave : uint8
{
	Default UMETA(DisplayName="Default"),
	Invulnerable UMETA(DisplayName="Invulnerable"),
	TwoPlus UMETA(DisplayName="2+"),
	ThreePlus UMETA(DisplayName="3+"),
	FourPlus UMETA(DisplayName="4+"),
	FivePlus UMETA(DisplayName="5+"),
	SixPlus UMETA(DisplayName="6+")
};

UENUM(BlueprintType)
enum class EBallisticSkill : uint8
{
	Default UMETA(DisplayName="Default"),
	FullAccuracy UMETA(DisplayName="FullAccuracy"),
	TwoPlus UMETA(DisplayName="2+"),
	ThreePlus UMETA(DisplayName="3+"),
	FourPlus UMETA(DisplayName="4+"),
	FivePlus UMETA(DisplayName="5+"),
	SixPlus UMETA(DisplayName="6+")
};

UENUM()
enum class EOutlineColor : uint8
{
	Green,
	Red,
	Gray,
	Purple
};