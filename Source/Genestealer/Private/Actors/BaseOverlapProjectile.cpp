// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/BaseOverlapProjectile.h"

#include "API/Effectible.h"
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
	MovementComp->UpdatedComponent = CollisionComp;
	MovementComp->InitialSpeed = 2000.0f;
	MovementComp->MaxSpeed = 2000.0f;
	MovementComp->bRotationFollowsVelocity = true;
	MovementComp->ProjectileGravityScale = 0.f;

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComp->SetCollisionProfileName("BlockAllDynamic");
	CollisionComp->SetCollisionObjectType(GENESTEALER_OBJECT_TYPE_PROJECTILE);
	CollisionComp->SetCollisionResponseToChannel(GENESTEALER_OBJECT_TYPE_PROJECTILE, ECR_Ignore);
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->bTraceComplexOnMove = true;
	
	RootComponent = CollisionComp;

	SummonedMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SummonedMesh"));
	SummonedMesh->SetupAttachment(RootComponent);
	SummonedMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SummonedMesh->SetCollisionResponseToAllChannels(ECR_Ignore);

	ParticleComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleComp"));
	ParticleComp->bAutoActivate = true;
	ParticleComp->SetupAttachment(SummonedMesh);
	ParticleComp->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	ParticleComp->LODMethod = PARTICLESYSTEMLODMETHOD_ActivateAutomatic;

	NiagaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComp"));
	NiagaraComp->bAutoActivate = true;
	NiagaraComp->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	NiagaraComp->SetupAttachment(SummonedMesh);

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
	if(CollisionComp)
	{
		CollisionComp->IgnoreActorWhenMoving(GetInstigator(), true);
		CollisionComp->IgnoreActorWhenMoving(GetOwner(), true);
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
	if(const UClass* HitActorClass = HitResult.GetActor()->GetClass(); !HitActorClass->ImplementsInterface(UEffectible::StaticClass()))
	{
		ApplyMissEffects(HitResult);
	} else  
	{
		Impact(HitResult);
	}
	HandleActorDeath();
}

void ABaseOverlapProjectile::Impact(const FHitResult& Impact)
{
	FTransform OutTrans;
	OutTrans.SetLocation(Impact.ImpactPoint);
	OutTrans.SetRotation(Impact.ImpactPoint.Rotation().Quaternion());
	ApplyHitEffects(Impact);
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

	MovementComp->StopMovementImmediately();
	Destroy();
}