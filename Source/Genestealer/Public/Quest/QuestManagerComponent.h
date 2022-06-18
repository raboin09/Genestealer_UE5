// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Quest/QuestStateMachine.h"

#include "QuestManagerComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GENESTEALER_API UQuestManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UQuestManagerComponent();

	UFUNCTION(BlueprintCallable, Category="Genestealer|QuestManager")
	static void GiveQuestClassToPlayer(const UObject* WorldContextObject, TSubclassOf<UQuestStateMachine> QuestClass);

	FORCEINLINE int32 GenerateNextQuestID() { return QuestIndexTicker++; }
	FORCEINLINE TMap<int32, UQuestStateMachine*> GetQuestMap() const { return ActiveQuests; }
	FORCEINLINE FQuestUpdateEvent& OnQuestUpdate() { return QuestUpdate; }

	UFUNCTION(BlueprintCallable, Category="Genestealer|QuestManager")
	void ActivateQuestInstance(TSubclassOf<UQuestStateMachine> QuestClass);
	UFUNCTION(BlueprintCallable, Category="Genestealer|QuestManager")
	void DeactivateAllQuests();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Genestealer|QuestManager")
	UQuestStateMachine* GetActiveQuest(int32 QuestID);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Genestealer|QuestManager")
	int32 GetNumberOfQuests() const { return ActiveQuests.Num(); }
	UFUNCTION(BlueprintCallable, Category="Genestealer|QuestManager")
	void DeactivateQuest(int32 QuestID);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Genestealer|QuestManager")
	bool IsQuestComplete(int32 QuestID);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Genestealer|QuestManager")
	bool HasQuestClassAlready(TSubclassOf<UQuestStateMachine> QuestToCheck);
	UFUNCTION()
	void HandleQuestUpdate(const FQuestUpdateEventPayload& QuestObjectiveEventPayload);
	
private:

	UPROPERTY()
	TMap<int32, UQuestStateMachine*> ActiveQuests;
	int32 QuestIndexTicker;
	FQuestUpdateEvent QuestUpdate;
};
