// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Animation/BaseActivatableNotifyState.h"

#include "Characters/InventoryComponent.h"

UObject* UBaseActivatableNotifyState::GetActivatableAbilityFromOwner(const USkeletalMeshComponent* MeshComp) const
{
	if (!IsValid(MeshComp) || !IsValid(MeshComp->GetOwner())) {
		return nullptr;
	}
	
	const UInventoryComponent* FoundInventory = MeshComp->GetOwner()->FindComponentByClass<UInventoryComponent>();
	if(!FoundInventory)
	{
		return nullptr;
	}

	return FoundInventory->GetEquippedWeapon().GetObject();
}
