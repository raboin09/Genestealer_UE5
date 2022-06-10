// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/UIEventHub.h"

#include "API/AmmoEntity.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Utils/CoreUtils.h"

UUIEventHub::UUIEventHub()
{
	
}

void UUIEventHub::InitEventHub(ABasePlayerController* InController)
{	
	// IMPORTANT: Use the InController as a ContextObject, using this object doesn't work here
	if(InController)
	{
		InController->OnNewActorTargeted().AddDynamic(this, &UUIEventHub::UIEventHandler_NewActorTargeted);
		if(InController->PlayerCharacter)
		{
			InController->PlayerCharacter->OnCharacterInCombatChanged().AddDynamic(this, &UUIEventHub::UIEventHandler_CharacterInCombatChanged);	
		}
	}

	if(UHealthComponent* CurrHealthComp = UCoreUtils::GetPlayerCharacterHealthComponent(InController))
	{
		CurrHealthComp->OnCurrentWoundHealthChanged().AddDynamic(this, &UUIEventHub::UIEventHandler_CurrentHealthChanged);
		CurrHealthComp->OnMaxWoundsChanged().AddDynamic(this, &UUIEventHub::UIEventHandler_MaxWoundsChanged);
	}
	
	if(UInventoryComponent* InventoryComponent = UCoreUtils::GetPlayerInventoryComponent(InController))
	{
		InventoryComponent->OnNewWeaponAdded().AddDynamic(this, &UUIEventHub::UIEventHandler_NewWeaponAdded);
		InventoryComponent->OnWeaponRemoved().AddDynamic(this, &UUIEventHub::UIEventHandler_WeaponRemoved);
		InventoryComponent->OnCurrentWeaponChanged().AddDynamic(this, &UUIEventHub::UIEventHandler_CurrentWeaponChanged);
		if(const TScriptInterface<IWeapon> CurrentWeapon = InventoryComponent->GetEquippedWeapon())
		{
			if(IAmmoEntity* AmmoEntity = Cast<IAmmoEntity>(CurrentWeapon.GetObject()))
			{
				AmmoEntity->OnAmmoAmountChanged().AddDynamic(this, &UUIEventHub::UIEventHandler_AmmoChanged);	
			}
		}
	}
}

void UUIEventHub::UIEventHandler_CharacterInCombatChanged(const FCharacterInCombatChangedPayload& CharacterInCombatChangedPayload)
{
	CharacterInCombatChanged.Broadcast(CharacterInCombatChangedPayload);
}

void UUIEventHub::UIEventHandler_NewActorTargeted(const FNewActorTargetedPayload& NewActorTargetedPayload)
{
	NewActorTargeted.Broadcast(NewActorTargetedPayload);
}

void UUIEventHub::UIEventHandler_AmmoChanged(const FAmmoAmountChangedPayload& AmmoAmountChangedPayload)
{
	AmmoAmountChanged.Broadcast(AmmoAmountChangedPayload);
}

void UUIEventHub::UIEventHandler_CurrentWeaponChanged(const FCurrentWeaponChangedPayload& CurrentWeaponChangedPayload)
{
	CurrentWeaponChanged.Broadcast(CurrentWeaponChangedPayload);
}

void UUIEventHub::UIEventHandler_NewWeaponAdded(const FNewWeaponAddedPayload& NewWeaponAddedPayload)
{
	NewWeaponAdded.Broadcast(NewWeaponAddedPayload);
}

void UUIEventHub::UIEventHandler_WeaponRemoved(const FWeaponRemovedPayload& WeaponRemovedPayload)
{
	WeaponRemoved.Broadcast(WeaponRemovedPayload);
}

void UUIEventHub::UIEventHandler_CurrentHealthChanged(const FCurrentWoundEventPayload& CurrentWoundEventPayload)
{
	CurrentWoundHealthChanged.Broadcast(CurrentWoundEventPayload);
}

void UUIEventHub::UIEventHandler_MaxWoundsChanged(const FMaxWoundsEventPayload& MaxWoundsChangedEventPayload)
{
	MaxWoundsChanged.Broadcast(MaxWoundsChangedEventPayload);
}

void UUIEventHub::UIEventHandler_ActorDeath(const FActorDeathEventPayload& ActorDeathEventPayload)
{
	ActorDeath.Broadcast(ActorDeathEventPayload);
}
