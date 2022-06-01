// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"
#include "Components/SphereComponent.h"
#include "Weapons/ProjectileWeapon.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Sound/SoundCue.h"
#include "Actors/BaseActor.h"
#include "BaseOverlapProjectile.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class GENESTEALER_API ABaseOverlapProjectile : public ABaseActor
{
	GENERATED_BODY()

public:
	ABaseOverlapProjectile();
	virtual void Tick(float DeltaSeconds) override;
	virtual void PostInitializeComponents() override;
	
	void InitVelocity(const FVector& ShootDirection) const;

	FORCEINLINE void AddAdditionalEffectsToApply(TArray<TSubclassOf<AActor>> AdditionalEffectsToApply) { ProjectileEffectsToApply.Append(AdditionalEffectsToApply);}
	
private:
	void PlayFlybySound();
	
	UFUNCTION()
	void OnImpact(const FHitResult& HitResult);
	
	UPROPERTY(VisibleDefaultsOnly, Category="Overlap|Projectile")
	UProjectileMovementComponent* MovementComp;
	UPROPERTY(VisibleDefaultsOnly, Category="Overlap|Projectile")
	UParticleSystemComponent* ParticleComp;
	UPROPERTY(VisibleDefaultsOnly, Category="Overlap|Projectile")
	UNiagaraComponent* NiagaraComp;
	UPROPERTY(VisibleDefaultsOnly)
	USphereComponent* CollisionComp;
	UPROPERTY(VisibleDefaultsOnly)
	UStaticMeshComponent* SummonedMesh;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="Genestealer", meta=(MustImplement="Effect"))
	TArray<TSubclassOf<AActor>> ProjectileEffectsToApply;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer" )
	USoundCue* FlyBySound;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer")
	float FlybyRange;
	UPROPERTY(Transient)
	bool bFlybyPlayed;
	UPROPERTY(Transient)
	bool bFlybyIsInRange;
	UPROPERTY(Transient)
	bool bFlybyIsInFront;

	virtual void Impact(const FHitResult& Impact);
	void ApplyHitEffects(const FHitResult& Impact) const;
	void ApplyMissEffects(const FHitResult Impact);
	virtual void HandleActorDeath();
	virtual void BeginPlay() override;
};
