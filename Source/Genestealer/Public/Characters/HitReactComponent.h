// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HitReactComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GENESTEALER_API UHitReactComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UHitReactComponent();

protected:
	virtual void BeginPlay() override;
		
};
