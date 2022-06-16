// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BasePlayerCharacter.h"
#include "Characters/HealthComponent.h"
#include "Characters/InventoryComponent.h"
#include "Core/BaseGameInstance.h"
#include "Core/BasePlayerController.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UI/BaseHUD.h"
#include "CoreUtils.generated.h"

/**
 * 
 */
UCLASS()
class GENESTEALER_API UCoreUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static ABaseHUD* GetBaseHUD(const UObject* ContextObject);
	static UUIEventHub* GetUIEventHub(const UObject* ContextObject);
	static float GetPlayerControllerSphereTraceRadius(const UObject* ContextObject);
	static float GetCoverPointValidDistance();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Genestealer|CoreUtils")
	static UQuestManagerComponent* GetQuestManager(const UObject* ContextObject);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Genestealer|CoreUtils")
	static TScriptInterface<IInteractable> GetTargetedActorByPlayerController(const UObject* ContextObject);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Genestealer|CoreUtils")
	static UInventoryComponent* GetInventoryComponentFromActor(const AActor* InActor);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Genestealer|CoreUtils")
	static UHealthComponent* GetPlayerCharacterHealthComponent(const UObject* WorldContextObject);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Genestealer|CoreUtils")
	static UHealthComponent* GetHealthComponentFromActor(UObject* ContextObject);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Genestealer|CoreUtils")
	static UBaseGameInstance* GetBaseGameInstance(const UObject* ContextObject);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Genestealer|CoreUtils")
	static ABasePlayerController* GetBasePlayerController(const UObject* ContextObject);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Genestealer|CoreUtils")
	static ABasePlayerCharacter* GetPlayerCharacter(const UObject* ContextObject);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Genestealer|CoreUtils")
	static UInventoryComponent* GetPlayerInventoryComponent(const UObject* ContextObject);
};
