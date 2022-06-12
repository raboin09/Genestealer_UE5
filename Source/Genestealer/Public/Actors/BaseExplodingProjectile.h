﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseOverlapProjectile.h"
#include "BaseExplodingProjectile.generated.h"

UCLASS(Abstract, Blueprintable)
class GENESTEALER_API ABaseExplodingProjectile : public ABaseOverlapProjectile
{
	GENERATED_BODY()

public:
	ABaseExplodingProjectile();
	
protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void HandleActorDeath() override;
	virtual void K2_HandleImpact_Implementation(const FHitResult& HitResult) override;
	virtual void OnImpact(const FHitResult& HitResult) override;

	UPROPERTY(EditDefaultsOnly, Category="Genestealer")
	USoundCue* ExplosionSound;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer")
	UNiagaraSystem* ExplosionNiagara;
	UPROPERTY(VisibleDefaultsOnly)
	USphereComponent* ExplosionRadius;

private:
	void Internal_ExplodeAllActorsInRadius();
	bool Internal_TryTraceToOverlappedActor(const FHitResult& Impact, const FVector& StartTrace, AActor* TargetActor);
	FHitResult Internal_TraceToLocation(const FVector& StartTrace, const FVector& EndTrace) const;

	UPROPERTY(Transient)
	bool bExplodedAlready = false;
};