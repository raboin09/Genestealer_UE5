// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/BaseOverlapPickup.h"
#include "Genestealer/Genestealer.h"
#include "Characters/InteractionComponent.h"
#include "Utils/CoreUtils.h"
#include "Utils/WorldUtils.h"

ABaseOverlapPickup::ABaseOverlapPickup()
{
	bDiesAfterOverlap = true;
	bActivateOnStart = true;
	
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	CollisionComp->InitSphereRadius(15.f);
	CollisionComp->bTraceComplexOnMove = true;
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComp->SetCollisionObjectType(GENESTEALER_TRACE_INTERACTION);
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	CollisionComp->SetCollisionResponseToChannel(GENESTEALER_TRACE_INTERACTION, ECR_Block);
	CollisionComp->CanCharacterStepUpOn = ECB_No;
	SetRootComponent(CollisionComp);

	PickupBase = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupBaseMesh"));
	PickupBase->SetupAttachment(RootComponent);
	PickupBase->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PickupBase->SetCollisionResponseToAllChannels(ECR_Ignore);

	RotatingMovementComponent = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingComponent"));
	RotatingMovementComponent->bRotationInLocalSpace = true;
	RotatingMovementComponent->RotationRate = FRotator(0.f, 90.f, 0.f);

	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
	
	DeathBuffer = 2.f;
}

void ABaseOverlapPickup::HandleOverlapEvent(AActor* OtherActor, const FHitResult& HitResult)
{
	if(ACharacter* CastedChar = Cast<ACharacter>(OtherActor))
	{
		if(CanPickup(CastedChar))
		{
			ConsumePickup(CastedChar);
			if(bDiesAfterOverlap)
			{
				Internal_PlayPickupEffects();
				PickupBase->SetVisibility(false);
				if(MaterialDissolver)
				{
					MaterialDissolver->StartDissolveTimeline(true);
				}
			}
			Super::HandleOverlapEvent(OtherActor, HitResult);	
		}
	}
}

void ABaseOverlapPickup::SwitchOutlineOnMesh(bool bShouldOutline)
{
	if(InteractionComponent)
	{
		InteractionComponent->SwitchOutlineOnAllMeshes(bShouldOutline);
	}
}

void ABaseOverlapPickup::InteractWithActor(AActor* InstigatingActor)
{
	
}

void ABaseOverlapPickup::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	MaterialDissolver = UWorldUtils::SpawnActorToWorld_Deferred<AMaterialDissolver>(this, MaterialDissolverClass, this);
	UWorldUtils::FinishSpawningActor_Deferred(MaterialDissolver, FTransform());
	if(MaterialDissolver)
	{
		MaterialDissolver->InitDissolveableMesh(GetMesh());
	}
	
	if(RotatingMovementComponent)
	{
		RotatingMovementComponent->SetUpdatedComponent(GetMesh());	
	}
}

void ABaseOverlapPickup::Internal_PlayPickupEffects()
{
	if (PickupSound) {
		UAudioManager::SpawnSoundAtLocation(this, PickupSound, GetActorLocation());
	}
}
