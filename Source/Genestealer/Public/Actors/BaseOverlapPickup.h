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
	FORCEINLINE virtual UShapeComponent* GetCollisionComponent() const override { return CollisionComp; }
	
protected:
	virtual void PostInitializeComponents() override;
	
	virtual void HandleOverlapEvent(AActor* OtherActor, const FHitResult& HitResult) override;
	FORCEINLINE virtual UMeshComponent* GetMesh() const override { return PickupBase; }
	virtual bool CanPickup(ACharacter* PotentialChar) PURE_VIRTUAL(ABaseOverlapPickup::CanPickup, return false;)
	virtual void ConsumePickup(ACharacter* ConsumingChar) PURE_VIRTUAL(ABaseOverlapPickup::ConsumePickup,)
	
	UPROPERTY(EditAnywhere)
	USphereComponent* CollisionComp;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer")
	USoundCue* PickupSound;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer")
	UStaticMeshComponent* PickupBase;
	UPROPERTY(EditAnywhere, Category="Genestealer")
	URotatingMovementComponent* RotatingMovementComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Genestealer")
	TSubclassOf<AMaterialDissolver> MaterialDissolverClass;

private:
	UPROPERTY()
	AMaterialDissolver* MaterialDissolver;

	
	UFUNCTION()
	virtual void Internal_PlayPickupEffects();
};
