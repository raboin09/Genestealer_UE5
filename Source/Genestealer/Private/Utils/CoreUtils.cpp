// Fill out your copyright notice in the Description page of Project Settings.


#include "Utils/CoreUtils.h"
#include "EngineUtils.h"
// #include "Characters/BasePlayerCharacter.h"
#include "Characters/InventoryComponent.h"
#include "API/Attackable.h"
#include "Core/BasePlayerController.h"
#include "Kismet/GameplayStatics.h"

UPlayerStatsComponent* UCoreUtils::GetPlayerStatsComponent(const UObject* ContextObject)
{
	if(const ABasePlayerController* CurrCon = GetBasePlayerController(ContextObject))
	{
		return CurrCon->GetPlayerStatsComponent();
	}
	return nullptr;
}


ABasePlayerController* UCoreUtils::GetBasePlayerController(const UObject* ContextObject)
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(ContextObject, 0);
	if(!PlayerController)
	{
		return nullptr;
	}	
	if (ABasePlayerController* CastedCon = Cast<ABasePlayerController>(PlayerController))
	{
		return CastedCon;
	}
	return nullptr;
}

ABasePlayerCharacter* UCoreUtils::GetPlayerCharacter(const UObject* ContextObject)
{
	if (const ABasePlayerController* CurrCon = GetBasePlayerController(ContextObject))
	{
		if (ABasePlayerCharacter* CurrChar = Cast<ABasePlayerCharacter>(CurrCon->GetPawn()))
		{
			return CurrChar;
		}
	}
	return nullptr;
}

UInventoryComponent* UCoreUtils::GetPlayerInventoryComponent(const UObject* ContextObject)
{
	if (const ABaseCharacter* CurrChar = GetPlayerCharacter(ContextObject))
	{
		return CurrChar->GetInventoryComponent();
	}
	return nullptr;
}

UOnlineContentManager* UCoreUtils::GetOnlineContentManager(const UObject* ContextObject)
{
	if(const UBaseGameInstance* BaseGameInstance = GetBaseGameInstance(ContextObject))
	{
		return BaseGameInstance->GetOnlineContentManager();
	}
	return nullptr;
}

bool UCoreUtils::IsObjectPlayerControlled(const UObject* Object)
{
	if(!Object)
	{
		return false;
	}

	if(const APawn* CastedPawn = Cast<APawn>(Object))
	{
		return CastedPawn->IsPlayerControlled();
	}
	return false;
}

ABaseHUD* UCoreUtils::GetBaseHUD(const UObject* ContextObject)
{
	if(const ABasePlayerController* BasePlayerController = GetBasePlayerController(ContextObject))
	{
		if(ABaseHUD * BaseHUD = Cast<ABaseHUD>(BasePlayerController->GetHUD()))
		{
			return BaseHUD;
		}	
	}
	return nullptr;
}

UUIEventHub* UCoreUtils::GetUIEventHub(const UObject* ContextObject)
{
	if(const ABasePlayerController* PlayerCon = GetBasePlayerController(ContextObject))
	{
		return PlayerCon->UIEventHub;
	}
	return nullptr;
}

float UCoreUtils::GetPlayerControllerSphereTraceRadius(const UObject* ContextObject)
{
	return 15.f;
}

float UCoreUtils::GetCoverPointValidDistance()
{
	return 800.f;
}

UQuestManagerComponent* UCoreUtils::GetQuestManager(const UObject* ContextObject)
{
	if (const ABasePlayerController* BaseCon = GetBasePlayerController(ContextObject))
	{
		return BaseCon->GetQuestManager();
	}
	return nullptr;
}

TScriptInterface<IInteractable> UCoreUtils::GetTargetedActorByPlayerController(const UObject* ContextObject)
{
	if(const ABasePlayerController* PlayerController = GetBasePlayerController(ContextObject))
	{
		return PlayerController->GetTargetedActor();
	}
	return nullptr;
}

UInventoryComponent* UCoreUtils::GetInventoryComponentFromActor(const AActor* InActor)
{
	if (InActor)
	{
		if (UInventoryComponent* InventoryComponent = Cast<UInventoryComponent>(
			InActor->GetComponentByClass(UInventoryComponent::StaticClass())))
		{
			return InventoryComponent;
		}
	}
	return nullptr;
}

UHealthComponent* UCoreUtils::GetPlayerCharacterHealthComponent(const UObject* WorldContextObject)
{
	if(!WorldContextObject){
		return nullptr;
	}
	return GetHealthComponentFromActor(GetPlayerCharacter(WorldContextObject));
}

UHealthComponent* UCoreUtils::GetHealthComponentFromActor(UObject* ContextObject)
{
	if (const IAttackable* CurrChar = Cast<IAttackable>(ContextObject))
	{
		return CurrChar->GetHealthComponent();
	}
	return nullptr;
}

UBaseGameInstance* UCoreUtils::GetBaseGameInstance(const UObject* ContextObject)
{
	UGameInstance* GameInst = UGameplayStatics::GetGameInstance(ContextObject);
	if(UBaseGameInstance* CastedGameInst = Cast<UBaseGameInstance>(GameInst))
	{
		return CastedGameInst;
	}
	return nullptr;
}