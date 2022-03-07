// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "HealthComponent.generated.h"

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
	HitReact_Light
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

USTRUCT(BlueprintType)
struct FDeathEventPayload
{
	GENERATED_BODY()

	FDeathEventPayload() : DyingActor(nullptr), KillingActor(nullptr)
	{
		
	}

	FDeathEventPayload(AActor* InDyingActor, AActor* InKillingActor)
	{
		DyingActor = InDyingActor;
		KillingActor = InKillingActor;
	}
	
	UPROPERTY()
	AActor* DyingActor;
	UPROPERTY()
	AActor* KillingActor;
};

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

// Natural change means that it occurs naturally (like gun damage, healing, etc). Not natural is forced resetting of values, initializing values, etc
DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FCurrentWoundHealthChanged, FWound, NewWound, FWound, OldWound, int32, MaxWounds, float, Delta, float, Percentage, bool, bNaturalChange);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FMaxWoundsChanged, float, NewMaxWounds, float, OldMaxWounds, float, Delta);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FActorDeath, FDeathEventPayload, DeathEventPayload);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GENESTEALER_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UHealthComponent();

	UFUNCTION(BlueprintCallable)
	void Execute(AActor* ExecutedActor, AActor* ExecutingActor);
	void InitHealthComponent(const FHealthDefaults& HealthModel);
	UFUNCTION(BlueprintCallable)
	void AddMaxWounds(float MaxWoundsToAdd);
	UFUNCTION(BlueprintCallable)
	float TakeDamage(const float RawDamage, AActor* ReceivingActor, AActor* InstigatingActor, const FDamageHitReactEvent& HitReactEvent);
	UFUNCTION(BlueprintCallable)
	void ApplyHeal(const float RawHeal, AActor* ReceivingActor = nullptr, AActor* InstigatingActor = nullptr);
	FORCEINLINE float GetHealthPercent(){ return WoundContainer.GetCurrentWound().GetWoundHealthPercent(); };
	void OverrideWoundContainer(const FHealthDefaults& InRot);

	DECLARE_EVENT_OneParam(UHealthComponent, FMaxWoundsChanged, const FMaxWoundsEventPayload&)
	FORCEINLINE FMaxWoundsChanged& OnMaxWoundsChanged() { return MaxWoundsChanged; }
	DECLARE_EVENT_OneParam(UHealthComponent, FActorDeath, const FDeathEventPayload&)
	FORCEINLINE FActorDeath& OnActorDeath() { return ActorDeath; }
	DECLARE_EVENT_OneParam(UHealthComponent, FCurrentWoundHealthChanged, const FCurrentWoundEventPayload&)
	FORCEINLINE FCurrentWoundHealthChanged& OnCurrentWoundHealthChanged() { return CurrentHealthChanged; }
	
protected:
	virtual void BeginPlay() override;
	
private:
	float CalculateDamage(const float RawDamage) const;
	
	UPROPERTY()
	FWoundContainer WoundContainer;
	
	FCurrentWoundHealthChanged CurrentHealthChanged;
	FMaxWoundsChanged MaxWoundsChanged;
	FActorDeath ActorDeath;

	float ReservePool;
	
public:
	UFUNCTION(BlueprintCallable, Category = "AICharacter|BaseStats")
	FORCEINLINE float GetCurrentWoundHealth() { return WoundContainer.GetCurrentWound().CurrentHealth; }	
	UFUNCTION(BlueprintCallable, Category = "AICharacter|BaseStats")
	FORCEINLINE float GetHealthPerWound() { return WoundContainer.MaxHealthPerWound; }
	UFUNCTION(BlueprintCallable, Category = "AICharacter|BaseStats")
	FORCEINLINE int32 GetNumWounds() { return WoundContainer.Wounds.Num(); }
	
	UFUNCTION(BlueprintCallable, Category = "AICharacter|BaseStats")
	bool IsAlive();
	
	FORCEINLINE void ZeroizeWoundContainer() { WoundContainer.ZeroizeWoundContainer(); }
	FORCEINLINE void MaximizeWoundContainer() { WoundContainer.MaximizeWoundContainer(); }

		
};
