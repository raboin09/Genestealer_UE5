// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseOverlapActor.h"
#include "MaterialDissolver.h"
#include "API/Interactable.h"
#include "Components/SphereComponent.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "Sound/SoundCue.h"
#include "BaseOverlapPickup.generated.h"

UCLASS(Abstract, NotBlueprintable)
class GENESTEALER_API ABaseOverlapPickup : public ABaseOverlapActor, public IInteractable
{
	GENERATED_BODY()

public:
	ABaseOverlapPickup();
	
	//////////////////////////////////////
	// Interactable Overrides
	//////////////////////////////////////
	virtual void SwitchOutlineOnMesh(bool bShouldOutline) override;
	virtual void InteractWithActor(AActor* InstigatingActor) override;
	FORCEINLINE virtual EAbsoluteAffiliation GetInteractableAffiliation() const override { return EAbsoluteAffiliation::Neutral; }
	virtual FString GetInteractionText() const override { return InteractionText; }
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Genestealer")
	FString InteractionText;
	
	//////////////////////////////////////
	// ABaseOverlapActor Overrides
	//////////////////////////////////////
	FORCEINLINE virtual UShapeComponent* GetCollisionComponent_Implementation() const override { return CollisionComp; }
	
protected:
	virtual void PostInitializeComponents() override;
	
	virtual void K2_HandleOverlapEvent_Implementation(AActor* OtherActor, const FHitResult& HitResult) override;
	FORCEINLINE virtual UMeshComponent* GetMesh_Implementation() const override { return PickupBase; }
	virtual bool CanPickup(ACharacter* PotentialChar) PURE_VIRTUAL(ABaseOverlapPickup::CanPickup, return false;)
	virtual void ConsumePickup(ACharacter* ConsumingChar);

	UFUNCTION(BlueprintImplementableEvent)
	void K2_HandleConsumePickup(ACharacter* ConsumingChar);
	UFUNCTION(BlueprintImplementableEvent)
	void K2_HandleMeshOutlining(bool bIsOutlining);
	UFUNCTION(BlueprintImplementableEvent)
	void K2_HandleInteraction(AActor* InstigatingActor);
	
	UPROPERTY(EditAnywhere)
	USphereComponent* CollisionComp;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer")
	USoundCue* PickupSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Genestealer")
	UStaticMeshComponent* PickupBase;
	UPROPERTY(EditAnywhere, Category="Genestealer")
	URotatingMovementComponent* RotatingMovementComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Genestealer")
	TSubclassOf<AMaterialDissolver> MaterialDissolverClass;

private:
	UPROPERTY()
	AMaterialDissolver* MaterialDissolver;
	UPROPERTY()
	class UInteractionComponent* InteractionComponent;
	
	UFUNCTION()
	virtual void Internal_PlayPickupEffects();
};
