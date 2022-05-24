// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BaseAICharacter.h"

#include "AI/BaseAIController.h"

ABaseAICharacter::ABaseAICharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	AIControllerClass = ABaseAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

FVector ABaseAICharacter::GetSocketLocation(FName SocketName, bool bWeaponMesh) const
{
	if(bWeaponMesh)
	{
		if(!InventoryComponent || !InventoryComponent->GetEquippedWeapon()
			|| !InventoryComponent->GetEquippedWeapon()->GetWeaponMesh()
			|| !InventoryComponent->GetEquippedWeapon()->GetWeaponMesh()->DoesSocketExist(SocketName))
		{
			return GetMesh()->GetSocketLocation("head");
		}
		return InventoryComponent->GetEquippedWeapon()->GetWeaponMesh()->GetSocketLocation(SocketName);
	}
	return GetMesh()->GetSocketLocation(SocketName);
}

void ABaseAICharacter::FireWeapon(bool bStartFiring)
{
	if(bStartFiring)
	{
		GL_HandleFireAction(false);	
	}
	GL_HandleFireAction(bStartFiring);
}

void ABaseAICharacter::Aim(bool bStartAiming)
{
	AimAction(bStartAiming);
}

float ABaseAICharacter::GetWeaponRange() const
{
	if(!InventoryComponent || !InventoryComponent->GetEquippedWeapon())
	{
		return -1.f;
	}
	return InventoryComponent->GetEquippedWeapon()->GetWeaponRange();
}
