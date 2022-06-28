// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseOverlapPickup.h"
#include "BaseOverlapTrigger.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class GENESTEALER_API ABaseOverlapTrigger : public ABaseOverlapPickup
{
	GENERATED_BODY()

public:
	ABaseOverlapTrigger();
	
	FORCEINLINE virtual UMeshComponent* GetMesh() const override { return nullptr; };
	
protected:
	UFUNCTION(BlueprintImplementableEvent)
	void K2_ConsumePickup(ACharacter* ConsumingChar);
	virtual void HandleEndOverlapEvent(AActor* ExitingActor) override;
	virtual bool CanPickup(ACharacter* PotentialChar) override;
	virtual void ConsumePickup(ACharacter* ConsumingChar) override;

	UPROPERTY()
	class USMInstance* TriggerMachine;
	UPROPERTY(EditInstanceOnly, Category="Genestealer")
	TSubclassOf<USMInstance> TriggerLogicClass;
};
