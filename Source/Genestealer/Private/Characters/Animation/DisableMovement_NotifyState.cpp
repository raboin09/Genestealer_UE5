// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Animation/DisableMovement_NotifyState.h"

#include "Utils/GameplayTagUtils.h"

void UDisableMovement_NotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	if(!MeshComp)
	{
		return;
	}
	UGameplayTagUtils::AddTagToActor(MeshComp->GetOwner(), TAG_STATE_STUNNED);
}

void UDisableMovement_NotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	if(!MeshComp)
	{
		return;
	}
	UGameplayTagUtils::RemoveTagFromActor(MeshComp->GetOwner(), TAG_STATE_STUNNED);
}
