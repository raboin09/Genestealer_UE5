// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/OnlineContentManager.h"

#include "Utils/CoreUtils.h"

bool UOnlineContentManager::GetUnitStatsFromString(const UObject* ContextObject, FUnitStats& UnitStats, const FString& InUnitName)
{
	if(UOnlineContentManager* OnlineContentManager = UCoreUtils::GetOnlineContentManager(ContextObject))
	{
		if(const FUnitStats* FoundStats = OnlineContentManager->GetStats(InUnitName))
		{
			UnitStats = *FoundStats;
			return true;
		}
		return false;
	}
	return false;
}

void UOnlineContentManager::InitOnlineContentManager()
{
	K2_LoadGoogleSheets();
	for (TPair<FString, FUnitStats>& Pair : LoadedUnitStats)
	{      
		Pair.Value.ResolveRawToEnums();
	}
}
