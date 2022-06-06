// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/DissolveComponent.h"
#include "Kismet/KismetSystemLibrary.h"


UDissolveComponent::UDissolveComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	DissolveTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("DissolveTimeline"));
	DissolveParameterName = "Dissolve Amount";
	ColorParameterName = "Color";
}

void UDissolveComponent::InitDissolveableMesh(UMeshComponent* InMesh)
{
	MeshComponent = InMesh;
	InitDissolveTimeline();
	Internal_TimelineDissolveUpdate(StartingDissolveValue);
}

void UDissolveComponent::StartAppearTimeline()
{
	if(!DissolveTimeline)
	{
		return;
	}
	DissolveTimeline->Play();
}

void UDissolveComponent::StartDissolveTimeline()
{
	if(!DissolveTimeline)
	{
		return;
	}
	DissolveTimeline->Reverse();
}

void UDissolveComponent::ResetDissolveState(bool bShouldDissolve)
{
	if(bShouldDissolve)
	{
		Internal_TimelineDissolveUpdate(StartingDissolveValue);
	}
}

void UDissolveComponent::Internal_TimelineDissolveEnd()
{
	
}

void UDissolveComponent::Internal_TimelineDissolveUpdate(float Value)
{
	if(DissolveDynamicMaterialInstances.Num() > 0)
	{
		for(UMaterialInstanceDynamic* CurrInst : DissolveDynamicMaterialInstances)
		{
			if(CurrInst)
			{
				CurrInst->SetScalarParameterValue(DissolveParameterName, Value);
			}
		}		
	}
}

void UDissolveComponent::InitDissolveTimeline()
{
	if(!DissolveTimeline)
	{
		return;
	}
	
	if(MeshComponent)
	{
		for(int i = 0; i<MeshComponent->GetMaterials().Num(); i++)
		{
			DissolveDynamicMaterialInstances.Add(MeshComponent->CreateDynamicMaterialInstance(i, HologramMaterialInstance));
		}
	}

	if(DissolveCurveFloat)
	{
		FOnTimelineFloat DissolveProgressFunction;
		DissolveProgressFunction.BindDynamic(this, &UDissolveComponent::Internal_TimelineDissolveUpdate);
		DissolveTimeline->AddInterpFloat(DissolveCurveFloat, DissolveProgressFunction);
		DissolveTimeline->SetLooping(false);
	}
	
	FOnTimelineEvent CoverLerpFinishedEvent;
	CoverLerpFinishedEvent.BindDynamic(this, &UDissolveComponent::Internal_TimelineDissolveEnd);
	DissolveTimeline->SetTimelineFinishedFunc(CoverLerpFinishedEvent);
}
