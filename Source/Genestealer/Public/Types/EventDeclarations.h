﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HealthTypes.h"
#include "QuestTypes.h"
#include "API/Interactable.h"
#include "API/Weapon.h"
#include "EventDeclarations.generated.h"


UENUM()
enum class EUIEvent : uint8
{
	PlayerInCombatChanged,
	NewActorTargeted,
	AmmoChanged,
	CurrentWeaponChanged,
	NewWeaponAdded,
	WeaponRemoved,
	CurrentHealthChanged
};

///////////////////////////
// AMMO AMOUNT CHANGED
///////////////////////////
USTRUCT(BlueprintType)
struct FAmmoAmountChangedPayload
{
	GENERATED_BODY()

	FAmmoAmountChangedPayload(int32 InCurrAmmoInClip, int32 InClipCapacity, int32 InCurrentAmmo, int32 InMaxClips)
		: CurrentAmmoInClip(InCurrAmmoInClip), ClipCapacity(InClipCapacity),  CurrentAmmo(InCurrentAmmo), MaxAmmo(InMaxClips) {}

	FAmmoAmountChangedPayload(): CurrentAmmoInClip(0), ClipCapacity(0), CurrentAmmo(0), MaxAmmo(0)
	{
	}

	UPROPERTY()
	int32 CurrentAmmoInClip;
	UPROPERTY()
	int32 ClipCapacity;
	UPROPERTY()
	int32 CurrentAmmo;
	UPROPERTY()
	int32 MaxAmmo;
	
};
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAmmoAmountChanged, const FAmmoAmountChangedPayload&, AmmoAmountChangedPayload);

///////////////////////////
// CURRENT WEAPON CHANGED
///////////////////////////
USTRUCT(BlueprintType)
struct FCurrentWeaponChangedPayload
{
	GENERATED_BODY()
	FCurrentWeaponChangedPayload(){}
	FCurrentWeaponChangedPayload(const TScriptInterface<IWeapon> InNewWeapon, const TScriptInterface<IWeapon> InPreviousWeapon)
	{
		NewWeapon = InNewWeapon;
		PreviousWeapon = InPreviousWeapon;
	}

	
	UPROPERTY()
	TScriptInterface<IWeapon> NewWeapon;
	UPROPERTY()
	TScriptInterface<IWeapon> PreviousWeapon;
};
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCurrentWeaponChanged, const FCurrentWeaponChangedPayload&, CurrentWeaponChangedPayload);

///////////////////////////
// WEAPON ADDED
///////////////////////////
USTRUCT(BlueprintType)
struct FNewWeaponAddedPayload
{
	GENERATED_BODY()
	FNewWeaponAddedPayload(){}
	FNewWeaponAddedPayload(const TScriptInterface<IWeapon> InAddedWeapon) : AddedWeapon(InAddedWeapon)	{ }

	UPROPERTY()
	TScriptInterface<IWeapon> AddedWeapon;
};
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNewWeaponAdded, const FNewWeaponAddedPayload&, NewWeaponAddedPayload);

///////////////////////////
// WEAPON REMOVED
///////////////////////////
USTRUCT(BlueprintType)
struct FWeaponRemovedPayload
{
	GENERATED_BODY()
	FWeaponRemovedPayload(){}
	FWeaponRemovedPayload(const TScriptInterface<IWeapon> InWeapon) : RemovedWeapon(InWeapon) { }

	UPROPERTY()
	TScriptInterface<IWeapon> RemovedWeapon;
};
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWeaponRemoved, const FWeaponRemovedPayload&, WeaponRemovedPayload);

///////////////////////////
// NEW ACTOR TARGETED
///////////////////////////
USTRUCT(BlueprintType)
struct FNewActorTargetedPayload
{
	GENERATED_BODY()

	FNewActorTargetedPayload() {}
	FNewActorTargetedPayload(TScriptInterface<IInteractable> InNewlyTargetedActor) : NewlyTargetedActor(InNewlyTargetedActor) { }

	UPROPERTY()
	TScriptInterface<IInteractable> NewlyTargetedActor;
};
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNewActorTargeted, const FNewActorTargetedPayload&, NewActorTargetedPayload);

///////////////////////////
// DEATH EVENT
///////////////////////////
USTRUCT(BlueprintType)
struct FActorDeathEventPayload
{
	GENERATED_BODY()

	UPROPERTY()
	float DyingDamage = 0.f;
	UPROPERTY()
	AActor* DyingActor = nullptr;
	UPROPERTY()
	AActor* KillingActor = nullptr;
	UPROPERTY()
	FHitResult HitResult;
	UPROPERTY()
	FDamageHitReactEvent HitReactEvent;
};
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FActorDeath, const FActorDeathEventPayload&, DeathEventPayload);

///////////////////////////
// MAX WOUNDS EVENT
///////////////////////////
USTRUCT(BlueprintType)
struct FMaxWoundsEventPayload
{
	GENERATED_BODY()

	UPROPERTY()
	float NewMaxWounds;
	UPROPERTY()
	float OldMaxWounds;
	UPROPERTY()
	float Delta;
};
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMaxWoundsChanged, const FMaxWoundsEventPayload&, MaxWoundsEventPayload);

///////////////////////////
// CURRENT WOUND EVENT
///////////////////////////
USTRUCT(BlueprintType)
struct FCurrentWoundEventPayload
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FWound NewWound;
	UPROPERTY(BlueprintReadOnly)
	FWound OldWound;
	UPROPERTY()
	int32 MaxWounds;
	UPROPERTY()
	float Delta;
	UPROPERTY()
	float Percentage;
	UPROPERTY()
	bool bWasDamage;
	UPROPERTY()
	bool bNaturalChange;
	UPROPERTY()
	FDamageHitReactEvent DamageHitReactEvent;
	UPROPERTY()
	AActor* ReceivingActor;
	UPROPERTY()
	AActor* InstigatingActor;
};
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCurrentWoundHealthChanged, const FCurrentWoundEventPayload&, CurrentWoundEventPayload);

///////////////////////////////////////
// PLAYER IN COMBAT CHANGED EVENT
///////////////////////////////////////
USTRUCT(BlueprintType)
struct FCharacterInCombatChangedPayload
{
	GENERATED_BODY()
	FCharacterInCombatChangedPayload()
	{
		bIsInCombat = false;
		DamageCauser = nullptr;
	}
	FCharacterInCombatChangedPayload(bool bInIsInCombat, AActor* InDamageCauser) : bIsInCombat(bInIsInCombat), DamageCauser(InDamageCauser) { }

	UPROPERTY()
	bool bIsInCombat;
	UPROPERTY()
	AActor* DamageCauser;
};
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCharacterInCombatChanged, const FCharacterInCombatChangedPayload&, PlayerInCombatChangedPayload);

///////////////////////////////////////
// PLAYER AIMING EVENT
///////////////////////////////////////
USTRUCT(BlueprintType)
struct FPlayerAimingChangedPayload
{
	GENERATED_BODY()
	
	FPlayerAimingChangedPayload()
	{
		bIsAiming = false;
		CrosshairSize = 0.f;
		CrosshairTexture = nullptr;
	}

	FPlayerAimingChangedPayload(bool bInIsAiming, UTexture2D* InCrosshairTexture, float InCrosshairSize)
	{
		bIsAiming = bInIsAiming;
		CrosshairTexture = InCrosshairTexture;
		CrosshairSize = InCrosshairSize;
	};

	UPROPERTY()
	bool bIsAiming;
	UPROPERTY()
	float CrosshairSize;
	UPROPERTY()
	UTexture2D* CrosshairTexture;
};
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerAimingChanged, const FPlayerAimingChangedPayload&, PlayerAimingChangedPayload);

///////////////////////////////////////
// QUEST EVENT
///////////////////////////////////////
USTRUCT(BlueprintType)
struct FQuestObjectiveEventPayload
{
	GENERATED_BODY()
	
	// The overlapped actor, quest objective, or killed AI character
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* EventObjective;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EQuestObjectiveAction EventAction;

	FQuestObjectiveEventPayload(AActor* InObjective, EQuestObjectiveAction InAction)
	{
		EventObjective = InObjective;
		EventAction = InAction;
	}

	FQuestObjectiveEventPayload()
	{
		EventObjective = nullptr;
		EventAction = EQuestObjectiveAction::None;
	}
};
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FQuestObjectiveEvent, const FQuestObjectiveEventPayload&, QuestObjectiveEventPayload);

///////////////////////////////////////
// QUEST UPDATE
///////////////////////////////////////

USTRUCT(BlueprintType)
struct FQuestUpdateEventPayload
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UQuestStateMachine* UpdatedQuest;

	FQuestUpdateEventPayload(UQuestStateMachine* InUpdatedQuest)
	{
		UpdatedQuest = InUpdatedQuest;
	}

	FQuestUpdateEventPayload()
	{
		UpdatedQuest = nullptr;
	}
};
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FQuestUpdateEvent, const FQuestUpdateEventPayload&, QuestUpdateEventPayload);