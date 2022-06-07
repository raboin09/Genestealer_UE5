// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Types/EventDeclarations.h"
#include "UIEventHub.generated.h"

class ABasePlayerController;

UCLASS(Blueprintable, BlueprintType, EditInlineNew, DefaultToInstanced)
class GENESTEALER_API UUIEventHub : public UObject
{
	GENERATED_BODY()

public:
	UUIEventHub();
	void InitEventHub(ABasePlayerController* InController);
	
	// ABaseCharacter
	UFUNCTION()
	virtual void UIEventHandler_CharacterInCombatChanged(const FCharacterInCombatChangedPayload& CharacterInCombatChangedPayload);
	
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

private:
	UPROPERTY()
	ABasePlayerController* OwningController;
};
