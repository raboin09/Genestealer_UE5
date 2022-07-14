// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/BaseOverlapProjectile.h"

#include "NiagaraFunctionLibrary.h"
#include "Characters/EffectContainerComponent.h"
#include "Components/AudioComponent.h"
#include "Genestealer/Genestealer.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Utils/CoreUtils.h"
#include "Utils/EffectUtils.h"

ABaseOverlapProjectile::ABaseOverlapProjectile() 
{
	MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	MovementComp->UpdatedComponent = SummonedMesh;
	MovementComp->InitialSpeed = 2000.0f;
	MovementComp->MaxSpeed = 2000.0f;
	MovementComp->bRotationFollowsVelocity = true;
	MovementComp->ProjectileGravityScale = 0.f;	

	SummonedMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SummonedMesh"));
	SummonedMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SummonedMesh->SetCollisionProfileName("BlockAllDynamic");
	SummonedMesh->SetCollisionObjectType(GENESTEALER_OBJECT_TYPE_PROJECTILE);
	SummonedMesh->SetCollisionResponseToChannel(GENESTEALER_OBJECT_TYPE_PROJECTILE, ECR_Ignore);
	SummonedMesh->SetupAttachment(RootComponent);
	SummonedMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SummonedMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	SetRootComponent(SummonedMesh);
	
	ParticleComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleComp"));
	ParticleComp->bAutoActivate = true;
	ParticleComp->SetupAttachment(SummonedMesh);
	ParticleComp->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	ParticleComp->LODMethod = PARTICLESYSTEMLODMETHOD_ActivateAutomatic;

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;
	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	bReplicates = true;
	SetReplicatingMovement(true);
	
	FlybyRange = 1000.f;
	bFlybyPlayed = false;
}

void ABaseOverlapProjectile::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if(!bFlybyPlayed)
	{
		PlayFlybySound();
	}
}

void ABaseOverlapProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if(MovementComp)
	{
		MovementComp->OnProjectileStop.AddDynamic(this, &ABaseOverlapProjectile::OnImpact);
	}
	
	if(NiagaraSystem && GetOwner())
	{
		NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(NiagaraSystem, SummonedMesh, "", FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, true);
		NiagaraComponent->bAutoActivate = true;
		NiagaraComponent->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	}
}

void ABaseOverlapProjectile::BeginPlay()
{
	Super::BeginPlay();
	if(const ABaseCharacter* CurrChar = UCoreUtils::GetPlayerCharacter(this))
	{
		if(UKismetMathLibrary::DegAcos(GetDotProductTo(CurrChar)) >= 90.f)
		{
			bFlybyIsInFront = false;
		} else
		{
			bFlybyIsInFront = true;
		}
	}	
}

void ABaseOverlapProjectile::InitVelocity(const FVector& ShootDirection) const
{
	if (MovementComp)
	{
		MovementComp->Velocity = ShootDirection * MovementComp->InitialSpeed;
	}
}

void ABaseOverlapProjectile::K2_HandleImpact_Implementation(const FHitResult& HitResult)
{
	ApplyHitEffects(HitResult);
}

void ABaseOverlapProjectile::PlayFlybySound()
{
	if(bFlybyIsInFront)
	{
		if(const ABaseCharacter* CurrChar = UCoreUtils::GetPlayerCharacter(this))
		{
			if(GetDistanceTo(CurrChar) <= FlybyRange && UKismetMathLibrary::DegAcos(GetDotProductTo(CurrChar)) >= 90.f)
			{
				UAudioManager::SpawnSoundAtLocation(this, FlyBySound, GetRootComponent()->GetComponentLocation());
				bFlybyPlayed = true;
			}
		}
	}
}

void ABaseOverlapProjectile::OnImpact(const FHitResult& HitResult)
{
	if(!HitResult.GetActor())
	{
		HandleActorDeath();
		return;
	}
	const UClass* HitActorClass = HitResult.GetActor()->GetClass();
	if(!HitActorClass)
	{
		return;
	}

	if(HitActorClass->ImplementsInterface(UAttackable::StaticClass()))
	{
		K2_HandleImpact(HitResult);
	} else
	{
		ApplyMissEffects(HitResult);
	}
	HandleActorDeath();
}

void ABaseOverlapProjectile::ApplyHitEffects(const FHitResult& Impact) const
{
	UEffectUtils::ApplyEffectsToHitResult(ProjectileEffectsToApply, Impact, GetInstigator());
}

void ABaseOverlapProjectile::ApplyMissEffects(const FHitResult Impact)
{
	for(const TSubclassOf<AActor> CurrEffectClass : ProjectileEffectsToApply)
	{
		if(const TScriptInterface<IEffect> TempEffect = UEffectContainerComponent::CreateEffectInstanceFromHitResult(this, CurrEffectClass, Impact, GetInstigator(), true))
		{
			TempEffect->PlayEffectFX();
			TempEffect->DestroyEffect();
		}
	}
}

void ABaseOverlapProjectile::HandleActorDeath()
{
	if (UAudioComponent* ProjAudioComp = FindComponentByClass<UAudioComponent>(); ProjAudioComp && ProjAudioComp->IsPlaying())
	{
		ProjAudioComp->FadeOut(0.1f, 0.f);
	}

	if(NiagaraComponent)
	{
		NiagaraComponent->Deactivate();
	}
	
	MovementComp->StopMovementImmediately();
	if(DeathBuffer > 0.f)
	{
		SetLifeSpan(DeathBuffer);	
	} else
	{
		Destroy();
	}
}