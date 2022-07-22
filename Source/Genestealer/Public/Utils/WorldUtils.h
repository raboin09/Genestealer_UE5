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
	//////////////////////////
	// Static Variables
	//////////////////////////
	static TArray<AActor*> QuestRelevantActors;

	static void TryAddActorToQuestableArray(AActor* InActor);
	static void TryRemoveActorFromQuestableArray(AActor* InActor);

	UFUNCTION(BlueprintCallable, Category="Genestealer|WorldUtils")
	static AActor* K2_SpawnActorToWorld(UObject* ContextObject, TSubclassOf<AActor> ClassToSpawn, FTransform SpawnTransform = FTransform(), AActor* Owner = nullptr, APawn* Instigator = nullptr, ESpawnActorCollisionHandlingMethod CollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
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
		return Internal_TemplatedSpawnActorFromClassDeferred<T>(ContextObject->GetWorld(), ClassToSpawn, Owner, Instigator, CollisionHandlingOverride);
	}

	template<typename T>
	FORCEINLINE static T* SpawnActorToWorld(UObject* ContextObject, TSubclassOf<AActor> ClassToSpawn, FTransform SpawnTransform = FTransform(), AActor* Owner = nullptr, APawn* Instigator = nullptr, ESpawnActorCollisionHandlingMethod CollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn)
	{
		if(!ContextObject)
		{
			return nullptr;
		}
		return Internal_TemplatedSpawnActorFromClass<T>(ContextObject->GetWorld(), ClassToSpawn, SpawnTransform, Owner, Instigator, CollisionHandlingOverride);
	}
	
private:
	template<typename T>
	FORCEINLINE static T* Internal_TemplatedSpawnActorFromClassDeferred(UWorld* World, TSubclassOf<AActor> ClassToSpawn, AActor* Owner = nullptr, APawn* Instigator = nullptr, ESpawnActorCollisionHandlingMethod CollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn)
	{
		if(!World)
		{
			return nullptr;
		}
		return World->SpawnActorDeferred<T>(ClassToSpawn, FTransform(), Owner, Instigator, CollisionHandlingOverride);
	}

	template<typename T>
	FORCEINLINE static T* Internal_TemplatedSpawnActorFromClass(UWorld* World, TSubclassOf<AActor> ClassToSpawn, FTransform SpawnTransform = FTransform(), AActor* Owner = nullptr, APawn* Instigator = nullptr, ESpawnActorCollisionHandlingMethod CollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn)
	{
		if(!World)
		{
			return nullptr;
		}
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.Instigator = Instigator;
		SpawnParameters.Owner = Owner;
		SpawnParameters.SpawnCollisionHandlingOverride = CollisionHandlingOverride;
		return World->SpawnActor<T>(ClassToSpawn, SpawnTransform, SpawnParameters);
	}

	static AActor* Internal_SpawnActorFromClass(UWorld* World, UClass* Class, const FTransform& SpawnTransform);
};