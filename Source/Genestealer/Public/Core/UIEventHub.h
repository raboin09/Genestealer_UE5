// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Types/EventDeclarations.h"
#include "UI/BaseHUD.h"
#include "UIEventHub.generated.h"

class ABasePlayerController;

UCLASS()
class GENESTEALER_API UUIEventHub : public UObject
{
	GENERATED_BODY()

public:
	UUIEventHub();
	void InitEventHub(ABasePlayerController* InController);

	///////////////////////////////////////////////////////////
	// UI Event delegates to broadcast to various UMG Widgets
	///////////////////////////////////////////////////////////
	FORCEINLINE FNewActorTargeted& OnNewActorTargeted() { return NewActorTargeted; }
	FORCEINLINE FCurrentWeaponChanged& OnCurrentWeaponChanged() { return CurrentWeaponChanged; }
	FORCEINLINE FNewWeaponAdded& OnNewWeaponAdded() { return NewWeaponAdded; }
	FORCEINLINE FWeaponRemoved& OnWeaponRemoved() { return WeaponRemoved; }
	FORCEINLINE FAmmoAmountChanged& OnAmmoAmountChanged() { return AmmoAmountChanged; }
	FORCEINLINE FMaxWoundsChanged& OnMaxWoundsChanged() { return MaxWoundsChanged; }
	FORCEINLINE FActorDeath& OnActorDeath() { return ActorDeath; }
	FORCEINLINE FCurrentWoundHealthChanged& OnCurrentWoundHealthChanged() { return CurrentWoundHealthChanged; }
	FORCEINLINE FCharacterInCombatChanged& OnCharacterInCombatChanged() { return CharacterInCombatChanged; }
	FORCEINLINE FPlayerAimingChanged& OnPlayerAimingChanged() { return PlayerAimingChanged; }

	///////////////////////////////////////////////////////////////
	// UI Event listeners coming from various Actors and Components
	///////////////////////////////////////////////////////////////

protected:
	// ABaseCharacter
	UFUNCTION()
	virtual void UIEventHandler_CharacterInCombatChanged(const FCharacterInCombatChangedPayload& CharacterInCombatChangedPayload);
	UFUNCTION()
	virtual void UIEventHandler_PlayerAimingChanged(const FPlayerAimingChangedPayload& PlayerAimingChangedPayload);
	
	// ABasePlayerController
	UFUNCTION()
	virtual void UIEventHandler_NewActorTargeted(const FNewActorTargetedPayload& NewActorTargetedPayload);
	
	// IWeapon
	UFUNCTION()
	virtual void UIEventHandler_AmmoChanged(const FAmmoAmountChangedPayload& AmmoAmountChangedPayload);
	
	// UInventoryComponent
	UFUNCTION()
	virtual void UIEventHandler_CurrentWeaponChanged(const FCurrentWeaponChangedPayload& CurrentWeaponChangedPayload);
	UFUNCTION()
	virtual void UIEventHandler_NewWeaponAdded(const FNewWeaponAddedPayload& NewWeaponAddedPayload);
	UFUNCTION()
	virtual void UIEventHandler_WeaponRemoved(const FWeaponRemovedPayload& WeaponRemovedPayload);
	
	// UHealthComponent
	UFUNCTION()
	virtual void UIEventHandler_CurrentHealthChanged(const FCurrentWoundEventPayload& CurrentWoundEventPayload);
	UFUNCTION()
	virtual void UIEventHandler_MaxWoundsChanged(const FMaxWoundsEventPayload& MaxWoundsChangedEventPayload);
	UFUNCTION()
	virtual void UIEventHandler_ActorDeath(const FActorDeathEventPayload& ActorDeathEventPayload);

private:
	FCurrentWoundHealthChanged CurrentWoundHealthChanged;
	FPlayerAimingChanged PlayerAimingChanged;
	FCharacterInCombatChanged CharacterInCombatChanged;
	FMaxWoundsChanged MaxWoundsChanged;
	FActorDeath ActorDeath;
	FNewActorTargeted NewActorTargeted;
	FWeaponRemoved WeaponRemoved;
	FNewWeaponAdded NewWeaponAdded;
	FCurrentWeaponChanged CurrentWeaponChanged;
	FAmmoAmountChanged AmmoAmountChanged;
};