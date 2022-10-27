// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseActor.h"
#include "API/Interactable.h"
#include "Types/EventDeclarations.h"
#include "BaseDestructibleActor.generated.h"

UCLASS(Abstract, Blueprintable)
class GENESTEALER_API ABaseDestructibleActor : public ABaseActor, public IInteractable, public IAttackable
{
	GENERATED_BODY()

public:
	ABaseDestructibleActor();
	
protected:
	virtual void BeginPlay() override;
	
	////////////////////////////////
	/// IAttackable override
	////////////////////////////////
	FORCEINLINE virtual EAbsoluteAffiliation GetAffiliation() const override { return EAbsoluteAffiliation::ChaosCult; }
	FORCEINLINE virtual UHealthComponent* GetHealthComponent() const override { return HealthComponent; }
	FORCEINLINE virtual FVector GetHeadLocation() const override { return GetActorLocation(); }
	FORCEINLINE virtual FVector GetChestLocation() const override { return GetActorLocation(); }
	FORCEINLINE virtual FVector GetPelvisLocation() const override { return GetActorLocation(); }
	FORCEINLINE virtual UEffectContainerComponent* GetEffectContainerComponent() const override { return EffectContainerComponent; }

	////////////////////////////////
	// Interactable overrides
	////////////////////////////////
	virtual EAbsoluteAffiliation GetInteractableAffiliation() const override { return EAbsoluteAffiliation::ChaosCult; }
	virtual void SwitchOutlineOnMesh(bool bShouldOutline) override;
	virtual void InteractWithActor(AActor* InstigatingActor) override;
	virtual FString GetInteractionText() const override { return InteractionText; }
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Genestealer")
	FString InteractionText;
	
	////////////////////////////////
	// ABaseDestructibleActor
	////////////////////////////////
	UFUNCTION(BlueprintImplementableEvent)
	void K2_OnDeath(const FHitResult& HitResult);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genestealer|Defaults")
	float DeathBuffer = 5.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genestealer|Defaults")
	FHealthDefaults StartingHealth;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	UHealthComponent* HealthComponent;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	UStaticMeshComponent* DestructibleMesh;
private:
	UFUNCTION()
	void HandleDeathEvent(const FActorDeathEventPayload& EventPayload);
	
	UPROPERTY()
	UEffectContainerComponent* EffectContainerComponent;
	UPROPERTY()
	class UInteractionComponent* InteractionComponent;
}; 