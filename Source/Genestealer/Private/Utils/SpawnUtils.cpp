// Fill out your copyright notice in the Description page of Project Settings.

#include "Utils/SpawnUtils.h"

#include "EngineUtils.h"

void USpawnUtils::FinishSpawningActor_Deferred(AActor* InActor, const FTransform& ActorTransform)
{
	if(!InActor)
	{
		return;
	}
	InActor->FinishSpawning(ActorTransform);
}

AActor* USpawnUtils::K2_SpawnActorWorld(UObject* ContextObject, TSubclassOf<AActor> ClassToSpawn, const FTransform& SpawnTransform)
{
	if(!ContextObject)
	{
		return nullptr;
	}
	return Internal_SpawnActorFromClass(ContextObject->GetWorld(), ClassToSpawn, SpawnTransform);
}

AActor* USpawnUtils::Internal_SpawnActorFromClass(UWorld* World, UClass* Class, const FTransform& SpawnTransform)
{
	if(!World)
	{
		return nullptr;
	}
	return World->SpawnActor<AActor>(Class, SpawnTransform);
}
