// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/TimelineComponent.h"
#include "DissolveComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GENESTEALER_API UDissolveComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDissolveComponent();
	void InitDissolveableMesh(UMeshComponent* InMesh);
	void StartAppearTimeline();
	void StartDissolveTimeline();
	void ResetDissolveState(bool bShouldDissolve = true);
	
protected:
	UPROPERTY(EditAnywhere, Category="Genestealer|Dissolve")
	float StartingDissolveValue = 0.f;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Dissolve")
	FName DissolveParameterName;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Dissolve")
	FName ColorParameterName;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Dissolve")
	UMaterialInstance* HologramMaterialInstance;
	UPROPERTY(EditAnywhere, Category="Genestealer|Dissolve")
	UCurveFloat* DissolveCurveFloat;
	UPROPERTY()
	UTimelineComponent* DissolveTimeline;
	
private:
	UFUNCTION()
	void Internal_TimelineDissolveEnd();
	UFUNCTION()
	void Internal_TimelineDissolveUpdate(float Value);
	UFUNCTION()
	void InitDissolveTimeline();
	
	UPROPERTY()
	UMeshComponent* MeshComponent;
	UPROPERTY()
	TArray<UMaterialInstanceDynamic*> DissolveDynamicMaterialInstances;
};
