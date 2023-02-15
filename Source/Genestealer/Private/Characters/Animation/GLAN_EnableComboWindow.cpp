// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Animation/GLAN_EnableComboWindow.h"
#include "API/Activatable.h"
#include "Characters/InventoryComponent.h"

void UGLAN_EnableComboWindow::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	const TScriptInterface<IActivatable> ActivatableAbility = GetActivatableAbilityFromOwner(MeshComp);
	if(!ActivatableAbility)
	{
		return;
	}
	ActivatableAbility->EnableComboWindow();
}

void UGLAN_EnableComboWindow::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	const TScriptInterface<IActivatable> ActivatableAbility = GetActivatableAbilityFromOwner(MeshComp);
	if(!ActivatableAbility)
	{
		return;
	}
	ActivatableAbility->DisableComboWindow();
}