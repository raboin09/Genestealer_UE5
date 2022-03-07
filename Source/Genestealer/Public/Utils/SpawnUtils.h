// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SpawnUtils.generated.h"

/**
 * 
 */
UCLASS()
class GENESTEALER_API USpawnUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable)
	static void FinishSpawningActor_Deferred(AActor* InActor, const FTransform& ActorTransform);
	UFUNCTION(BlueprintCallable)
	static AActor* K2_SpawnActorWorld(UObject* ContextObject, TSubclassOf<AActor> ClassToSpawn, const FTransform& SpawnTransform);
	
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
};