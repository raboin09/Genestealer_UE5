// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Animation/DisableMovement_NotifyState.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Types/GenestealerTags.h"
#include "Utils/GameplayTagUtils.h"

void UDisableMovement_NotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	if(!MeshComp || !MeshComp->GetOwner())
	{
		return;
	}
	AActor* CompOwner = MeshComp->GetOwner();
	if(UCharacterMovementComponent* MoveComp = CompOwner->FindComponentByClass<UCharacterMovementComponent>())
	{
		MoveComp->DisableMovement();	
	}
}

void UDisableMovement_NotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	if(!MeshComp)
	{
		return;
	}
	AActor* CompOwner = MeshComp->GetOwner();
	if(UCharacterMovementComponent* MoveComp = CompOwner->FindComponentByClass<UCharacterMovementComponent>())
	{
		MoveComp->SetMovementMode(MOVE_Walking);	
	}
}
