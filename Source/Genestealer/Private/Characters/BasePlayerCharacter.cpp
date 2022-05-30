// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BasePlayerCharacter.h"

ABasePlayerCharacter::ABasePlayerCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	LockOnComponent = CreateDefaultSubobject<ULockOnComponent>(TEXT("LockOnComponent"));
}

void ABasePlayerCharacter::GL_HandleFireAction(bool bValue)
{
	if(!InventoryComponent || !LockOnComponent || !bValue)
	{
		Super::GL_HandleFireAction(bValue);	
	}

	if(const TScriptInterface<IWeapon> TempWeapon = InventoryComponent->GetEquippedWeapon())
	{
		const bool bValidWeaponType = TempWeapon->GetWeaponType() == EWeaponType::Melee;
		const bool bWeaponOnCooldown = TempWeapon->IsWeaponOnCooldown();
		if(bValidWeaponType && !bWeaponOnCooldown)
		{
			LockOnComponent->InterpToBestTargetForMeleeAttack();
		}
	}
	Super::GL_HandleFireAction(bValue);	
}
