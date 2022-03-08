// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"
#include "Components/SphereComponent.h"
#include "Weapons/ProjectileWeapon.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Sound/SoundCue.h"
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
	
	void SetActorOwner(AActor* InActor) { ActorOwner = InActor; }
	void InitVelocity(const FVector& ShootDirection) const;
	FORCEINLINE void SetWeaponEffects(const TArray<TSubclassOf<AActor>> InWeaponEffectsToApply) { WeaponEffectsToApply = InWeaponEffectsToApply; }; 
private:
	void PlayFlybySound();

	UFUNCTION()
	void OnImpact(const FHitResult& HitResult);
	
	virtual void HandleOverlapEvent(AActor* OtherActor, const FHitResult& HitResult);
	
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
	UPROPERTY(EditDefaultsOnly, Category="Overlap|Projectile|Effects", meta=(MustImplement="Effect"))
	TArray<TSubclassOf<AActor>> ProjectileEffectsToApply;
	
	TWeakObjectPtr<AController> MyController;

	UPROPERTY(EditDefaultsOnly, Category="Overlap|Projectile|Effects" )
	USoundCue* FlyBySound;
	UPROPERTY(EditDefaultsOnly, Category="Overlap|Projectile|Effects")
	float FlybyRange;
	UPROPERTY(Transient)
	bool bFlybyPlayed;
	UPROPERTY(Transient)
	bool bFlybyIsInRange;
	UPROPERTY(Transient)
	bool bFlybyIsInFront;

	UPROPERTY()
	TScriptInterface<IWeapon> OwningWeapon;

	virtual void Impact(const FHitResult& Impact);
	void ApplyHitEffects(const FHitResult& Impact) const;
	void ApplyMissEffects(const FHitResult Impact);
	virtual void HandleActorDeath();
	virtual void BeginPlay() override;

private:
	TArray<TSubclassOf<AActor>> WeaponEffectsToApply;
	UPROPERTY()
	AActor* ActorOwner;
};
