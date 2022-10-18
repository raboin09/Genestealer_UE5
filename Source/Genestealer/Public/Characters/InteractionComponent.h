// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "API/Attackable.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GENESTEALER_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInteractionComponent();
	void SwitchOutlineOnAllMeshes(bool bShouldOutline);
	
protected:
	virtual void BeginPlay() override;

private:

	void Internal_AddOwnerMeshesToArray();
	EAffectedAffiliation Internal_GetOwnerAffiliation() const;
	
	UPROPERTY()
	TArray<UMeshComponent*> OwnerMeshes;
};
