#include "Characters/Animation/GLAN_ResetActivatable.h"

#include "API/Activatable.h"
#include "Characters/InventoryComponent.h"

void UGLAN_ResetActivatable::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	const TScriptInterface<IActivatable>  ActivatableAbility = Internal_GetActivatableAbilityFromOwner(MeshComp);
	if(!ActivatableAbility)
	{
		return;
	}
	ActivatableAbility->ResetActivatable();
}

UObject* UGLAN_ResetActivatable::Internal_GetActivatableAbilityFromOwner(const USkeletalMeshComponent* MeshComp) const
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