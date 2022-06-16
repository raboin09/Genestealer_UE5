// Fill out your copyright notice in the Description page of Project Settings.


#include "Quest/Quest.h"

#include "SMUtils.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Quest/BaseQuestStateMachineInstance.h"

bool AQuest::IsQuestComplete() const
{
	if(!QuestData.QuestStateMachineInstance)
		return false;

	bool bAllQuestsDone = true;
	
	for(UTransition_QuestComplete* CurrTrans : QuestData.QuestStateMachineInstance->QuestTransitions)
	{
		if(CurrTrans)
		{
			bAllQuestsDone = bAllQuestsDone && CurrTrans->QuestSectionData.IsQuestSectionDone();
		}
	}
	
	return bAllQuestsDone;
}

TArray<UTransition_QuestComplete*> AQuest::InitStateMachineInstance()
{
	USMInstance* QuestMachine = USMBlueprintUtils::CreateStateMachineInstance(QuestData.QuestStateMachineClass, this);
	if(!QuestMachine)
	{
		return {};
	}
	
	QuestMachine->Start();
	if(UBaseQuestStateMachineInstance* QuestSMInst = Cast<UBaseQuestStateMachineInstance>(QuestMachine))
	{
		QuestData.QuestStateMachineInstance = QuestSMInst;
		return QuestSMInst->QuestTransitions;
	}
	
	return {};
}