// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/Animation/ActivateWeapon_NotifyState.h"

#include "API/Activatable.h"
#include "Characters/InventoryComponent.h"

void UActivateWeapon_NotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	const TScriptInterface<IActivatable>  ActivatableAbility = Internal_GetActivatableAbilityFromOwner(MeshComp);
	if(!ActivatableAbility)
	{
		return;
	}
	ActivatableAbility->Activate();
}

void UActivateWeapon_NotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	const TScriptInterface<IActivatable>  ActivatableAbility = Internal_GetActivatableAbilityFromOwner(MeshComp);
	if(!ActivatableAbility)
	{
		return;
	}
	ActivatableAbility->Deactivate();
}

UObject* UActivateWeapon_NotifyState::Internal_GetActivatableAbilityFromOwner(const USkeletalMeshComponent* MeshComp) const
{
	if (!IsValid(MeshComp) || !IsValid(MeshComp->GetOwner())) {
		return nullptr;
	}
	
	UInventoryComponent* FoundInventory = MeshComp->GetOwner()->FindComponentByClass<UInventoryComponent>();
	if(!FoundInventory)
	{
		return nullptr;
	}

	return FoundInventory->GetEquippedWeapon().GetObject();
}
