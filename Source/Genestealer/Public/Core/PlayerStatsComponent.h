// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerStatsComponent.generated.h"

UENUM()
enum EStatsEvent
{
	Headshot,
	DamageGiven,
	DamageTaken,
	ShotFired,
	ShotHit,
	MeleeSwung,
	MeleeHit
};

USTRUCT(BlueprintType)
struct FPlayerStats
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	float Headshots;
	UPROPERTY(BlueprintReadOnly)
	float ShotsFired;
	UPROPERTY(BlueprintReadOnly)
	float ShotsHit;
	UPROPERTY(BlueprintReadOnly)
	float MeleeSwung;
	UPROPERTY(BlueprintReadOnly)
	float MeleeHit;
	UPROPERTY(BlueprintReadOnly)
	float PlayerTotalDamageGiven;
	UPROPERTY(BlueprintReadOnly)
	float PlayerTotalDamageTaken;
	UPROPERTY(BlueprintReadOnly)
	TMap<AActor*, float> DamageTakenMap;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GENESTEALER_API UPlayerStatsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPlayerStatsComponent();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FPlayerStats GetPlayerStatistics(const UObject* ContextObject);
	static void RecordStatsEvent(const UObject* ContextObject, EStatsEvent Event, float Modifier = 1.f, AActor* NonPlayerActorInstigator = nullptr);

	UFUNCTION()
	void HandleStatsEvent(EStatsEvent Event, float Modifier = 1.f, AActor* NonPlayerActorInstigator = nullptr);
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE FPlayerStats& GetPlayerStats() { return PlayerStats; }

private:
	UPROPERTY()
	FPlayerStats PlayerStats;
};
