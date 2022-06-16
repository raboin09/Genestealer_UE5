// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/BaseOverlapPickup.h"
#include "BaseOverlapQuestPickup.generated.h"

UCLASS(Blueprintable, Abstract)
class GENESTEALER_API ABaseOverlapQuestPickup : public ABaseOverlapPickup
{
	GENERATED_BODY()

public:
	ABaseOverlapQuestPickup();

protected:
	virtual bool CanPickup(ACharacter* PotentialChar) override;
	virtual void ConsumePickup(ACharacter* ConsumingChar) override;
	
private:
	UPROPERTY()
	class UQuestObjectiveComponent* QuestObjectiveComponent;
};
