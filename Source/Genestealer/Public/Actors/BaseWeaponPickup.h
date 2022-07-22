// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/BaseOverlapPickup.h"
#include "BaseWeaponPickup.generated.h"


UCLASS(Abstract, Blueprintable)
class GENESTEALER_API ABaseWeaponPickup : public ABaseOverlapPickup
{
	GENERATED_BODY()

public:
	
	ABaseWeaponPickup();

	virtual UMeshComponent* GetMesh_Implementation() const override;
	
protected:
	FORCEINLINE virtual EAffiliation GetInteractableAffiliation() const override { return OutlineAffiliation; }
	virtual void ConsumePickup(ACharacter* ConsumingChar) override;
	virtual bool CanPickup(ACharacter* PotentialChar) override;

	UFUNCTION(BlueprintCallable)
	void GiveWeaponToPlayer(ACharacter* ConsumingChar);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Genestealer")
	EAffiliation OutlineAffiliation = EAffiliation::Allies;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Genestealer")
	UStaticMeshComponent* SummonedStaticMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Genestealer")
	USkeletalMeshComponent* SummonedSkelMesh;
	
	UPROPERTY(EditAnywhere, Category="Genestealer", meta=(MustImplement="Weapon"))
	TSubclassOf<AActor> WeaponPickupClass;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer")
	UParticleSystemComponent* HologramParticleSystem;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer")
	UStaticMeshComponent* LightingMesh;
};
