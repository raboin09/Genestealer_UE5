// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HealthTypes.generated.h"

UENUM(BlueprintType)
enum class EHitReactType : uint8
{
	None,
	Knockback_VeryLight,
	Knockback_Light,
	Knockback_Medium,
	Knockback_Heavy,
	Knockback_VeryHeavy,
	Knockback_Huge,
	HitReact_Light,
	HitReact_Chainsaw
};

 USTRUCT(BlueprintType)
 struct FHealthDefaults
 {
    GENERATED_USTRUCT_BODY()
 	
 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
 	float MaxHealthPerWound = 100;
 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	int32 MaxWounds = 1;
 };

USTRUCT(BlueprintType)
struct FDamageHitReactEvent
{
	GENERATED_BODY()
	
	FVector HitDirection;
	float DamageTaken;
	FHitResult HitResult;
	EHitReactType HitReactType = EHitReactType::None;
	EHitReactType DeathReactType = EHitReactType::None;
};

USTRUCT(BlueprintType)
struct FWound
{
	GENERATED_USTRUCT_BODY()

	FWound(const FHealthDefaults& InData, const int32 WoundIndex)
	{
		CurrentHealth = InData.MaxHealthPerWound;
		MaxHealth = InData.MaxHealthPerWound;
		WoundArrayIndex = WoundIndex;
	}

	FWound()
	{
		CurrentHealth = 0.f;
		MaxHealth = 0.f;
		WoundArrayIndex = 0;
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float CurrentHealth = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float MaxHealth = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	int32 WoundArrayIndex = 0;
	
	float GetWoundHealthPercent() const
	{
		return CurrentHealth / MaxHealth;
	}
	float IsFullHealth() const { return CurrentHealth == MaxHealth; }

	bool operator==(const FWound& Other) const
	{
		return (Other.WoundArrayIndex == WoundArrayIndex);
	}
};

USTRUCT(BlueprintType)
struct FWoundContainer
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	TArray<FWound> Wounds;
	UPROPERTY()
	float MaxHealthPerWound;
	UPROPERTY()
	int32 MaxWounds;
	
	void InitWoundContainer(const FHealthDefaults& InData);
	void AddToMaxWounds(int32 MaxWoundsToAdd);
	
	void ZeroizeWoundContainer();
	void MaximizeWoundContainer();	
	FWound& GetCurrentWound();

	void TakeDamage(float Damage);
	void ApplyHeal(float HealAmount);
	bool IsAlive();
	float GetAllWoundsHealthSum();
	
	float GetHealthPerWound() const { return MaxHealthPerWound; }

	FWoundContainer()
	{
		Wounds = { FWound() };
		MaxHealthPerWound = 100.f;
		MaxWounds = 1;
	}
};