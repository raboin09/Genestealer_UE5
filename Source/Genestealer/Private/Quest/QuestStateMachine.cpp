// Fill out your copyright notice in the Description page of Project Settings.


#include "Quest/QuestStateMachine.h"

UQuestStateMachine::UQuestStateMachine()
{
	
}

void UQuestStateMachine::Start()
{
	Super::Start();
	TArray<USMTransitionInstance*> TransArray;
	GetAllTransitionInstances(TransArray);
	for(USMTransitionInstance* TransInst : TransArray)
	{
		if(UTransition_QuestComplete* CurrTrans = Cast<UTransition_QuestComplete>(TransInst))
		{
			QuestTransitions.Add(CurrTrans);
		}
	}
}

bool UQuestStateMachine::IsQuestComplete() const
{
	bool bAllQuestsDone = true;	
	for(UTransition_QuestComplete* CurrTrans : QuestTransitions)
	{
		if(CurrTrans)
		{
			bAllQuestsDone = bAllQuestsDone && CurrTrans->QuestSectionData.IsQuestSectionDone();
		}
	}
	
	return bAllQuestsDone;
}