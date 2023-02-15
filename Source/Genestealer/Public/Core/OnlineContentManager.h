// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Types/OnlineContentTypes.h"
#include "UObject/NoExportTypes.h"
#include "OnlineContentManager.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class GENESTEALER_API UOnlineContentManager : public UObject
{
	GENERATED_BODY()


public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Genestealer|ContentManager")
	static bool GetUnitStatsFromString(const UObject* ContextObject, FUnitStats& UnitStats, const FString& InUnitName);

	FORCEINLINE FUnitStats* GetStats(const FString& InUnitName) { return LoadedUnitStats.Find(InUnitName); }
	void InitOnlineContentManager();
	
protected:
	UFUNCTION(BlueprintImplementableEvent)
	void K2_LoadGoogleSheets();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<FString, FUnitStats> LoadedUnitStats;
};
