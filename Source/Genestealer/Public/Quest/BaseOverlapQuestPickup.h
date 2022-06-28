// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/BaseOverlapPickup.h"
#include "API/Questable.h"
#include "BaseOverlapQuestPickup.generated.h"

UCLASS(Blueprintable, Abstract, AutoExpandCategories=("Genestealer"), PrioritizeCategories = "Genestealer")
class GENESTEALER_API ABaseOverlapQuestPickup : public ABaseOverlapPickup, public IQuestable
{
	GENERATED_BODY()

public:
	ABaseOverlapQuestPickup();

protected:
	virtual void BeginPlay() override;

	////////////////////////////////
	// IQuestable overrides
	////////////////////////////////
	FORCEINLINE virtual FQuestObjectiveEvent& OnQuestObjectiveEvent() override { return QuestObjectiveEvent; }
	
	virtual bool CanPickup(ACharacter* PotentialChar) override;
	virtual void ConsumePickup(ACharacter* ConsumingChar) override;
	
private:
	FQuestObjectiveEvent QuestObjectiveEvent;
};
