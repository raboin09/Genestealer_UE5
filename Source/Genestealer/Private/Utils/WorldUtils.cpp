// Fill out your copyright notice in the Description page of Project Settings.

#include "Utils/WorldUtils.h"

#include "EngineUtils.h"
#include "API/Questable.h"
#include "Utils/CoreUtils.h"

TArray<AActor*> UWorldUtils::QuestRelevantActors = {};
TArray<AActor*> UWorldUtils::ChaosCultActors = {};
TArray<AActor*> UWorldUtils::GenestealerCultActors = {};

void UWorldUtils::FinishSpawningActor_Deferred(AActor* InActor, const FTransform& ActorTransform)
{
	if(!InActor)
	{
		return;
	}
	InActor->FinishSpawning(ActorTransform);
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

void UWorldUtils::TryAddActorToTeamArray(AActor* InActor, EAbsoluteAffiliation AbsoluteAffiliation)
{
	switch (AbsoluteAffiliation) {
		case EAbsoluteAffiliation::GenestealerCult:
			GenestealerCultActors.AddUnique(InActor);
			break;
		case EAbsoluteAffiliation::ChaosCult:
			ChaosCultActors.AddUnique(InActor);
			break;
	case EAbsoluteAffiliation::Neutral:
		case EAbsoluteAffiliation::Imperium:
		case EAbsoluteAffiliation::Orks:
		case EAbsoluteAffiliation::Destructible:
		default: break;
	}
}

void UWorldUtils::TryAddActorToQuestableArray(AActor* InActor)
{
	if(!InActor || !InActor->GetClass() || !InActor->GetClass()->ImplementsInterface(UQuestable::StaticClass()))
	{
		return;
	}
	QuestRelevantActors.AddUnique(InActor);
}

void UWorldUtils::TryRemoveActorFromQuestableArray(AActor* InActor)
{
	if(!InActor || !InActor->GetClass() || !InActor->GetClass()->ImplementsInterface(UQuestable::StaticClass()))
	{
		return;
	}
	QuestRelevantActors.Remove(InActor);
}

AActor* UWorldUtils::K2_SpawnActorToWorld(UObject* ContextObject, TSubclassOf<AActor> ClassToSpawn, FTransform SpawnTransform, AActor* Owner, APawn* Instigator, ESpawnActorCollisionHandlingMethod CollisionHandlingOverride)
{
	return SpawnActorToWorld<AActor>(ContextObject, ClassToSpawn, SpawnTransform, Owner, Instigator, CollisionHandlingOverride);
}
