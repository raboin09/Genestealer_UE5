// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WorldUtils.generated.h"

/**
 * 
 */
UCLASS()
class GENESTEALER_API UWorldUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	/////////////////////////////////////////////////
	// Anything that has UQuestObjectiveComponent
	// Added in UQuestObjectiveComponent::BeginPlay()
	/////////////////////////////////////////////////
	static TArray<AActor*> QuestRelevantActors;

	
	UFUNCTION(BlueprintCallable, Category="Genestealer|WorldUtils")
	static void K2_FinishSpawningActor_Deferred(AActor* InActor, const FTransform& ActorTransform);
	UFUNCTION(BlueprintCallable, Category="Genestealer|WorldUtils")
	static AActor* K2_SpawnActorWorld_Deferred(UObject* ContextObject, TSubclassOf<AActor> ClassToSpawn, AActor* Owner = nullptr, APawn* Instigator = nullptr, ESpawnActorCollisionHandlingMethod CollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);

	
	static void FinishSpawningActor_Deferred(AActor* InActor, const FTransform& ActorTransform);
	template<typename T>
	FORCEINLINE static T* SpawnActorToWorld_Deferred(UObject* ContextObject, TSubclassOf<AActor> ClassToSpawn, AActor* Owner = nullptr, APawn* Instigator = nullptr, ESpawnActorCollisionHandlingMethod CollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn)
	{
		if(!ContextObject)
		{
			return nullptr;
		}
		return Internal_TemplatedSpawnActorFromClass<T>(ContextObject->GetWorld(), ClassToSpawn, Owner, Instigator, CollisionHandlingOverride);
	}
	
private:
	template<typename T>
	FORCEINLINE static T* Internal_TemplatedSpawnActorFromClass(UWorld* World, TSubclassOf<AActor> ClassToSpawn, AActor* Owner = nullptr, APawn* Instigator = nullptr, ESpawnActorCollisionHandlingMethod CollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn)
	{
		if(!World)
		{
			return nullptr;
		}
		return World->SpawnActorDeferred<T>(ClassToSpawn, FTransform(), Owner, Instigator, CollisionHandlingOverride);
	}

	static AActor* Internal_SpawnActorFromClass(UWorld* World, UClass* Class, const FTransform& SpawnTransform);
	static void Internal_AddActorToRelevantArrays(AActor* InActor);
};