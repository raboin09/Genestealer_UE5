// Fill out your copyright notice in the Description page of Project Settings.


#include "Quest/QuestStateMachine.h"

UQuestStateMachine::UQuestStateMachine()
{
	
}

void UQuestStateMachine::Initialize(UObject* Context)
{
	Super::Initialize(Context);
	TArray<USMTransitionInstance*> TransArray;
	GetAllTransitionInstances(TransArray);
	for(USMTransitionInstance* TransInst : TransArray)
	{
		if(UTransition_QuestSection* CurrTrans = Cast<UTransition_QuestSection>(TransInst))
		{
			QuestTransitions.Add(CurrTrans);
		}
	}
}

void UQuestStateMachine::TryAddActorToQuest(AActor* InActor)
{
	for(UTransition_QuestSection* Transition : QuestTransitions)
	{
		Transition->ActivateQuestObjectiveActor(InActor);
	}
}

bool UQuestStateMachine::IsQuestComplete() const
{
	for(UTransition_QuestSection* CurrTrans : QuestTransitions)
	{
		if(CurrTrans && !CurrTrans->QuestSectionData.IsQuestSectionDone())
		{
			return false;
		}
	}	
	return true;
}
