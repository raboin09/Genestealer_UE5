// Fill out your copyright notice in the Description page of Project Settings.

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
	float ExplosionRadius = 512.f;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer")
	USoundCue* ExplosionSound;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer")
	UNiagaraSystem* ExplosionNiagara;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer")
	bool bFriendlyFire = false;

private:
	void Internal_ExplodeAllActorsInRadius();

	UPROPERTY(Transient)
	bool bExplodedAlready = false;
};