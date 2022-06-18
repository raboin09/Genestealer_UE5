// Fill out your copyright notice in the Description page of Project Settings.

#include "Utils/WorldUtils.h"

#include "EngineUtils.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Quest/QuestObjectiveComponent.h"

TArray<AActor*> UWorldUtils::QuestRelevantActors = {};

void UWorldUtils::FinishSpawningActor_Deferred(AActor* InActor, const FTransform& ActorTransform)
{
	if(!InActor)
	{
		return;
	}
	InActor->FinishSpawning(ActorTransform);
	Internal_AddActorToRelevantArrays(InActor);
}

void UWorldUtils::K2_FinishSpawningActor_Deferred(AActor* InActor, const FTransform& ActorTransform)
{
	FinishSpawningActor_Deferred(InActor, ActorTransform);
}

AActor* UWorldUtils::K2_SpawnActorWorld_Deferred(UObject* ContextObject, TSubclassOf<AActor> ClassToSpawn, AActor* Owner, APawn* Instigator, ESpawnActorCollisionHandlingMethod CollisionHandlingOverride)
{
	return SpawnActorToWorld_Deferred<AActor>(ContextObject, ClassToSpawn, Owner, Instigator, CollisionHandlingOverride);
}

AActor* UWorldUtils::Internal_SpawnActorFromClass(UWorld* World, UClass* Class, const FTransform& SpawnTransform)
{
	if(!World)
	{
		return nullptr;
	}
	return World->SpawnActor<AActor>(Class, SpawnTransform);
}

void UWorldUtils::Internal_AddActorToRelevantArrays(AActor* InActor)
{
	if(!InActor)
	{
		return;
	}

	if(UQuestObjectiveComponent* QuestObjectiveComponent = Cast<UQuestObjectiveComponent>(InActor->GetComponentByClass(UQuestObjectiveComponent::StaticClass())))
	{
		QuestRelevantActors.Add(InActor);
	}
}
