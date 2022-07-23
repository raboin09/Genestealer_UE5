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

protected:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Genestealer")
	void K2_HandleImpact(const FHitResult& HitResult);
	virtual void K2_HandleImpact_Implementation(const FHitResult& HitResult);
	UFUNCTION(BlueprintImplementableEvent)
	void K2_HandleActorDeath();
	void PlayFlybySound();	
	UFUNCTION()
	virtual void OnImpact(const FHitResult& HitResult);
	virtual void HandleActorDeath();
	void ApplyMissEffects(const FHitResult Impact);
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Genestealer")
	UProjectileMovementComponent* MovementComp;
	UPROPERTY(VisibleDefaultsOnly, Category="Genestealer")
	UParticleSystemComponent* ParticleComp;
	UPROPERTY(VisibleDefaultsOnly)
	USphereComponent* CollisionComp;
	UPROPERTY(VisibleDefaultsOnly)
	UStaticMeshComponent* SummonedMesh;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer", meta=(MustImplement="Effect"))
	TArray<TSubclassOf<AActor>> ProjectileEffectsToApply;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer")
	float DeathBuffer = 0.f;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer")
	USoundCue* FlyBySound;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer")
	UNiagaraSystem* NiagaraSystem;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer")
	float FlybyRange;
private:
	UPROPERTY(Transient)
	UNiagaraComponent* NiagaraComponent; 
	UPROPERTY(Transient)
	bool bFlybyPlayed;
	UPROPERTY(Transient)
	bool bFlybyIsInRange;
	UPROPERTY(Transient)
	bool bFlybyIsInFront;

	void ApplyHitEffects(const FHitResult& Impact) const;
	virtual void BeginPlay() override;
};
