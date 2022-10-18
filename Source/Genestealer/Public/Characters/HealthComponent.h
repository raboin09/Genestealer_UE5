// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Types/HealthTypes.h"
#include "Types/EventDeclarations.h"
#include "HealthComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), AutoExpandCategories=("Genestealer"), PrioritizeCategories = "Genestealer" )
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
	
	FORCEINLINE FMaxWoundsChanged& OnMaxWoundsChanged() { return MaxWoundsChanged; }
	FORCEINLINE FActorDeath& OnActorDeath() { return ActorDeath; }
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
	EArmorSave ArmorSave;
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
