// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/BaseRangedWeapon.h"
#include "ConeWeapon.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class GENESTEALER_API AConeWeapon : public ABaseRangedWeapon
{
	GENERATED_BODY()

public:
	AConeWeapon();
	
protected:
	virtual void FireWeapon() override;
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Fire")
	FVector AIScaleOverride;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Fire")
	FVector AIAdjustmentOverride;
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* ConeComponent;
	
private:
	bool TryTraceToOverlappedActor(const FHitResult& Impact, const FVector& StartTrace, AActor* TargetActor);

	FVector OriginalScale;
};
