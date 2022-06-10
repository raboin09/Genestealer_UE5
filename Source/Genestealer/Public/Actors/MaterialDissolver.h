// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/BaseActor.h"
#include "Components/TimelineComponent.h"
#include "MaterialDissolver.generated.h"

UCLASS(Abstract, Blueprintable)
class GENESTEALER_API AMaterialDissolver : public ABaseActor
{
	GENERATED_BODY()

public:
	AMaterialDissolver();
	
	void InitDissolveableMesh(UMeshComponent* InMesh);
	void StartAppearTimeline();
	void StartDissolveTimeline();
	void ResetDissolveState(bool bShouldDissolve = true);
	
protected:
	UPROPERTY(EditAnywhere, Category="Genestealer")
	float StartingDissolveValue = 0.f;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer")
	FName DissolveParameterName;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer")
	FName ColorParameterName;
	UPROPERTY(EditAnywhere, Category="Genestealer")
	UCurveFloat* DissolveCurveFloat;

private:
	UFUNCTION()
	void Internal_TimelineDissolveEnd();
	UFUNCTION()
	void Internal_TimelineDissolveUpdate(float Value);
	UFUNCTION()
	void InitDissolveTimeline();
	
	UPROPERTY()
	UTimelineComponent* DissolveTimeline;
	UPROPERTY()
	UMeshComponent* MeshComponent;
	UPROPERTY()
	TArray<UMaterialInstanceDynamic*> DissolveDynamicMaterialInstances;
};
