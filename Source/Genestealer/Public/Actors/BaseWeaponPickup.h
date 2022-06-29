// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/BaseOverlapPickup.h"
#include "API/Weapon.h"
#include "BaseWeaponPickup.generated.h"


UCLASS(Abstract, Blueprintable)
class GENESTEALER_API ABaseWeaponPickup : public ABaseOverlapPickup
{
	GENERATED_BODY()

public:
	
	ABaseWeaponPickup();

	virtual UMeshComponent* GetMesh_Implementation() const override;
	
protected:
	virtual void ConsumePickup(ACharacter* ConsumingChar) override;
	virtual bool CanPickup(ACharacter* PotentialChar) override;

	UPROPERTY(VisibleDefaultsOnly, Category="Genestealer")
	UStaticMeshComponent* SummonedStaticMesh;
	UPROPERTY(VisibleDefaultsOnly, Category="Genestealer")
	USkeletalMeshComponent* SummonedSkelMesh;
	
	UPROPERTY(EditDefaultsOnly, Category="Genestealer", meta=(MustImplement="Weapon"))
	TSubclassOf<AActor> WeaponPickupClass;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer")
	UParticleSystemComponent* HologramParticleSystem;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer")
	UStaticMeshComponent* LightingMesh;
};
