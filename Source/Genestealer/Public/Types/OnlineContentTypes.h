// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CombatTypes.h"
#include "OnlineContentTypes.generated.h"

USTRUCT(BlueprintType)
struct FUnitStats
{
	GENERATED_BODY()

	void ResolveRawToEnums();

	UPROPERTY(BlueprintReadOnly)
	FString UnitName;
	UPROPERTY(BlueprintReadOnly)
	float HealthPerWound = -1.f;
	UPROPERTY(BlueprintReadOnly)
	uint8 RawArmorSave = 0;
	EArmorSave ArmorSave = EArmorSave::SixPlus;
	UPROPERTY(BlueprintReadOnly)
	uint8 RawBallisticSkill = 1;
	EBallisticSkill BallisticSkill = EBallisticSkill::SixPlus;
	UPROPERTY(BlueprintReadOnly)
	int32 Wounds = -1;	
};
