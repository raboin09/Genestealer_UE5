// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "API/Activatable.h"
#include "Weapons/BaseWeapon.h"
#include "MeleeWeapon.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class GENESTEALER_API AMeleeWeapon : public ABaseWeapon, public IActivatable
{
	GENERATED_BODY()
	
public:
	virtual void Activate() override;
	virtual void Deactivate() override;
	virtual void EnableComboWindow() override;
	virtual void DisableComboWindow() override;
};
