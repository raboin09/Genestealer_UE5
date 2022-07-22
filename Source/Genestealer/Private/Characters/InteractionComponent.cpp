﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/InteractionComponent.h"

#include "Actors/BaseOverlapActor.h"
#include "Actors/BaseOverlapPickup.h"
#include "Actors/BaseWeaponPickup.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Utils/CombatUtils.h"

UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInteractionComponent::SwitchOutlineOnAllMeshes(bool bShouldOutline)
{
	for(UMeshComponent* CurrMesh : OwnerMeshes)
	{
		if(CurrMesh)
		{
			const int32 OutlineColorInt = UCombatUtils::GetOutlineIntFromAffiliation(Internal_GetOwnerAffiliation());
			CurrMesh->SetRenderCustomDepth(bShouldOutline);
			CurrMesh->SetCustomDepthStencilValue(OutlineColorInt);
		}
	}
}

void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
	Internal_AddOwnerMeshesToArray();
}

void UInteractionComponent::Internal_AddOwnerMeshesToArray()
{
	if(const AActor* CurrOwner = GetOwner())
	{
		TSet<UActorComponent*> CurrComponents = CurrOwner->GetComponents();
		for(UActorComponent* CurrComp : CurrComponents)
		{
			if(!CurrComp)
			{
				continue;
			}
			
			if(UMeshComponent* CastedComp = Cast<UMeshComponent>(CurrComp))
			{
				OwnerMeshes.Add(CastedComp);
			}
		}
	}
}

EAffiliation UInteractionComponent::Internal_GetOwnerAffiliation() const
{
	if(const IAttackable* OwnerCast = Cast<IAttackable>(GetOwner()))
	{
		return OwnerCast->GetAffiliation();
	}

	if(const IInteractable* InterCast = Cast<IInteractable>(GetOwner()))
	{
		return InterCast->GetInteractableAffiliation();
	}
	
	return EAffiliation::Neutral;
}