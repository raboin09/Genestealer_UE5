// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/BaseOverlapProjectile.h"

#include "API/Effectible.h"
#include "Characters/EffectContainerComponent.h"
#include "Components/AudioComponent.h"
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
	MovementComp->OnProjectileStop.AddDynamic(this, &ABaseOverlapProjectile::OnImpact);
	ABaseCharacter* OwnerChar = Cast<ABaseCharacter>(GetInstigator());
	if(OwnerChar)
	{
		CollisionComp->MoveIgnoreActors.Add(OwnerChar);
	}
	AProjectileWeapon* OwnerWeapon = Cast<AProjectileWeapon>(GetOwner());
	if (OwnerWeapon)
	{
		OwnerWeapon->ApplyWeaponConfig(WeaponConfig);
	}

	if(WeaponConfig.ProjectileLife > 0)
	{
		SetLifeSpan( WeaponConfig.ProjectileLife );
	}	
	MyController = GetInstigatorController();
}

void ABaseOverlapProjectile::BeginPlay()
{
	Super::BeginPlay();
	if(ABaseCharacter* CurrChar = UCoreUtils::GetPlayerCharacter(this))
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
		if(ABaseCharacter* CurrChar = UCoreUtils::GetPlayerCharacter(this))
		{
			if(GetDistanceTo(CurrChar) <= FlybyRange && UKismetMathLibrary::DegAcos(GetDotProductTo(CurrChar)) >= 90.f)
			{
				UGameplayStatics::PlaySoundAtLocation(this, FlyBySound, GetRootComponent()->GetComponentLocation(), FRotator::ZeroRotator);
				bFlybyPlayed = true;
			}
		}
	}
}

void ABaseOverlapProjectile::HandleOverlapEvent(AActor* OtherActor, const FHitResult& HitResult)
{
	if(!HitResult.GetActor())
	{
		HandleActorDeath();
		return;
	}
	
	const UClass* HitActorClass = HitResult.GetActor()->GetClass();
	if(!HitActorClass->ImplementsInterface(UEffectible::StaticClass()))
	{
		ApplyMissEffects(HitResult);
	} else
	{
		Impact(HitResult);
	}
	HandleActorDeath();
}

void ABaseOverlapProjectile::OnImpact(const FHitResult& HitResult)
{
	HandleOverlapEvent(nullptr, HitResult);
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
	UEffectUtils::ApplyEffectsToHitResult(WeaponEffectsToApply, Impact, ActorOwner);
	UEffectUtils::ApplyEffectsToHitResult(ProjectileEffectsToApply, Impact, ActorOwner);
}

void ABaseOverlapProjectile::ApplyMissEffects(const FHitResult Impact)
{
	for(const TSubclassOf<AActor> CurrEffectClass : ProjectileEffectsToApply)
	{
		TScriptInterface<IEffect> TempEffect = UEffectContainerComponent::CreateEffectInstanceFromHitResult(this, CurrEffectClass, Impact, ActorOwner, true);
		if(TempEffect)
		{
			TempEffect->PlayEffectFX();
			TempEffect->DestroyEffect();
		}
	}
}

void ABaseOverlapProjectile::HandleActorDeath()
{
	UAudioComponent* ProjAudioComp = FindComponentByClass<UAudioComponent>();
	if (ProjAudioComp && ProjAudioComp->IsPlaying())
	{
		ProjAudioComp->FadeOut(0.1f, 0.f);
	}

	MovementComp->StopMovementImmediately();
	SetLifeSpan(.1f);
}