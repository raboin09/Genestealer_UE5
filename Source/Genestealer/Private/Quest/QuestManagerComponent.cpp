// Fill out your copyright notice in the Description page of Project Settings.


#include "Quest/QuestManagerComponent.h"
#include "SMSystem/Public/SMUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Quest/QuestStateMachine.h"
#include "Quest/Transition_QuestComplete.h"
#include "Utils/CoreUtils.h"

UQuestManagerComponent::UQuestManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	QuestIndexTicker = 0;
}

void UQuestManagerComponent::GiveQuestClassToPlayer(const UObject* WorldContextObject, TSubclassOf<UQuestStateMachine> QuestClass)
{
	if(UQuestManagerComponent* QuestManagerComponent = UCoreUtils::GetQuestManager(WorldContextObject))
	{
		QuestManagerComponent->ActivateQuestInstance(QuestClass);
	}
}

void UQuestManagerComponent::ActivateQuestInstance(TSubclassOf<UQuestStateMachine> QuestClass)
{
	if(!QuestClass || !GetWorld() || HasQuestClassAlready(QuestClass))
	{
		return;
	}
	
	UQuestStateMachine* TempQuest = Cast<UQuestStateMachine>(USMBlueprintUtils::CreateStateMachineInstance(QuestClass, this));
	if(!TempQuest)
	{
		return;
	}

	TempQuest->Start();
	for(UTransition_QuestComplete* CurrTrans : TempQuest->QuestTransitions)
	{
		if(CurrTrans)
		{
			CurrTrans->OnQuestUpdated().AddDynamic(this, &UQuestManagerComponent::HandleQuestUpdate);
		}		
	}
	const int32 TempQuestID = QuestIndexTicker++;
	TempQuest->QuestData.QuestID = TempQuestID;
	ActiveQuests.Add(TempQuestID, TempQuest);
	QuestUpdate.Broadcast(FQuestUpdateEventPayload(TempQuest));
}

void UQuestManagerComponent::DeactivateAllQuests()
{
	TArray<int32> Keys;
	ActiveQuests.GetKeys(Keys);
	for(const int32 CurrKey : Keys)
	{
		DeactivateQuest(CurrKey);
	}
}

void UQuestManagerComponent::HandleQuestUpdate(const FQuestUpdateEventPayload& QuestObjectiveEventPayload)
{
	if(!QuestObjectiveEventPayload.UpdatedQuest)
		return;

	if(QuestObjectiveEventPayload.UpdatedQuest->IsQuestComplete())
	{
		ActiveQuests.Remove(QuestObjectiveEventPayload.UpdatedQuest->QuestData.QuestID);
	}
	
	QuestUpdate.Broadcast(QuestObjectiveEventPayload.UpdatedQuest);
}

UQuestStateMachine* UQuestManagerComponent::GetActiveQuest(int32 QuestID)
{
	return *ActiveQuests.Find(QuestID);
}

void UQuestManagerComponent::DeactivateQuest(int32 QuestID)
{
	if(const UQuestStateMachine* QuestInst = GetActiveQuest(QuestID))	{
		if(QuestInst->QuestData.QuestStateMachineInstance)
		{
			QuestInst->QuestData.QuestStateMachineInstance->Stop();
		}
		ActiveQuests.Remove(QuestID);
	}
}

bool UQuestManagerComponent::IsQuestComplete(int32 QuestID)
{
	if(const UQuestStateMachine* QuestInst = GetActiveQuest(QuestID))
	{
		if(QuestInst->QuestData.QuestStateMachineInstance)
		{
			return  QuestInst->QuestData.QuestStateMachineInstance->IsInEndState();
		}
	}
	return false;
}

bool UQuestManagerComponent::HasQuestClassAlready(TSubclassOf<UQuestStateMachine> QuestToCheck)
{
	TArray<int32> Keys;
	ActiveQuests.GetKeys(Keys);
	for(const int32 CurrKey : Keys)
	{
		if(const UQuestStateMachine* CurrQuest = *ActiveQuests.Find(CurrKey))
		{
			if(CurrQuest->GetClass() == QuestToCheck)
			{
				return true;
			}
		}
	}
	return false;
}