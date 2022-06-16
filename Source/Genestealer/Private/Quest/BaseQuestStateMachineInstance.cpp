// Fill out your copyright notice in the Description page of Project Settings.


#include "Quest/BaseQuestStateMachineInstance.h"

UBaseQuestStateMachineInstance::UBaseQuestStateMachineInstance()
{
	
}

void UBaseQuestStateMachineInstance::Initialize(UObject* Context)
{
	Super::Initialize(Context);
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
