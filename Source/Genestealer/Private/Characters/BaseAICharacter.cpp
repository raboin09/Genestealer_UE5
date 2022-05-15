// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BaseAICharacter.h"

#include "AI/BaseAIController.h"

ABaseAICharacter::ABaseAICharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	AIControllerClass = ABaseAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void ABaseAICharacter::FireWeapon(bool bStartFiring)
{
	Input_Fire();
}

void ABaseAICharacter::Aim(bool bStartAiming)
{
	Input_Aim();
}

float ABaseAICharacter::GetWeaponRange() const
{
	if(!InventoryComponent || !InventoryComponent->GetEquippedWeapon())
	{
		return -1.f;
	}
	return InventoryComponent->GetEquippedWeapon()->GetWeaponRange();
}
