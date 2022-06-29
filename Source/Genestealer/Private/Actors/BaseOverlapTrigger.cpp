// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/BaseOverlapTrigger.h"
#include "SMUtils.h"
#include "Quest/QuestManagerComponent.h"
#include "Utils/CoreUtils.h"

ABaseOverlapTrigger::ABaseOverlapTrigger()
{
	bDiesAfterOverlap = true;

	CollisionComp->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void ABaseOverlapTrigger::K2_HandleEndOverlapEvent_Implementation(AActor* ExitingActor)
{
	if(!TriggerMachine)
	{
		return;
	}
	
	if(ACharacter* CastedChar = Cast<ACharacter>(ExitingActor); CastedChar && CanPickup(CastedChar))
	{
		TriggerMachine->Stop();
	}

}

bool ABaseOverlapTrigger::CanPickup(ACharacter* PotentialChar)
{
	if(PotentialChar && PotentialChar->IsPlayerControlled())
	{
		return true;
	}
	return false;
}

void ABaseOverlapTrigger::ConsumePickup(ACharacter* ConsumingChar)
{
	K2_ConsumePickup(ConsumingChar);
	if(!TriggerLogicClass)
	{
		return;
	}
	
	if(!TriggerMachine)
	{
		TriggerMachine = USMBlueprintUtils::CreateStateMachineInstance(TriggerLogicClass, this);
	}
	

	if(TriggerMachine)
	{
		TriggerMachine->Start();
	}
}