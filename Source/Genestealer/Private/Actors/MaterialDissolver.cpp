
#include "Actors/MaterialDissolver.h"
#include "Kismet/KismetSystemLibrary.h"


AMaterialDissolver::AMaterialDissolver()
{
	SetActorTickEnabled(false);
	DissolveTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("DissolveTimeline"));
	DissolveParameterName = "Dissolve Amount";
	ColorParameterName = "Color";
}

void AMaterialDissolver::InitDissolveableMesh(UMeshComponent* InMesh)
{
	MeshComponent = InMesh;
	InitDissolveTimeline();
	Internal_TimelineDissolveUpdate(StartingDissolveValue);
}

void AMaterialDissolver::StartAppearTimeline(bool bPlayFromStart)
{
	if(!DissolveTimeline)
	{
		return;
	}
	bPlayFromStart ? DissolveTimeline->PlayFromStart() : DissolveTimeline->Play();
}

void AMaterialDissolver::StartDissolveTimeline(bool bReverseFromEnd)
{
	if(!DissolveTimeline)
	{
		return;
	}
	bReverseFromEnd ? DissolveTimeline->ReverseFromEnd() : DissolveTimeline->Reverse();
}

void AMaterialDissolver::ResetDissolveState(bool bShouldDissolve)
{
	if(bShouldDissolve)
	{
		Internal_TimelineDissolveUpdate(StartingDissolveValue);
	}
}

void AMaterialDissolver::Internal_TimelineDissolveEnd()
{
	
}

void AMaterialDissolver::Internal_TimelineDissolveUpdate(float Value)
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

void AMaterialDissolver::InitDissolveTimeline()
{
	if(!DissolveTimeline)
	{
		return;
	}
	
	if(MeshComponent)
	{
		for(int i = 0; i<MeshComponent->GetMaterials().Num(); i++)
		{
			DissolveDynamicMaterialInstances.Add(MeshComponent->CreateDynamicMaterialInstance(i));
		}
	}

	if(DissolveCurveFloat)
	{
		FOnTimelineFloat DissolveProgressFunction;
		DissolveProgressFunction.BindDynamic(this, &AMaterialDissolver::Internal_TimelineDissolveUpdate);
		DissolveTimeline->AddInterpFloat(DissolveCurveFloat, DissolveProgressFunction);
		DissolveTimeline->SetLooping(false);
	}
	
	FOnTimelineEvent CoverLerpFinishedEvent;
	CoverLerpFinishedEvent.BindDynamic(this, &AMaterialDissolver::Internal_TimelineDissolveEnd);
	DissolveTimeline->SetTimelineFinishedFunc(CoverLerpFinishedEvent);
}
