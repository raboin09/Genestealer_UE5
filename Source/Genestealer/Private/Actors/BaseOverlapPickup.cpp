// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/BaseOverlapPickup.h"

#include "Genestealer/Genestealer.h"
#include "Utils/CombatUtils.h"
#include "Utils/CoreUtils.h"
#include "Utils/SpawnUtils.h"

ABaseOverlapPickup::ABaseOverlapPickup()
{
	bDiesAfterOverlap = true;
	bActivateOnStart = true;
	
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(50.0f);
	CollisionComp->bTraceComplexOnMove = true;
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComp->SetCollisionObjectType(GENESTEALER_TRACE_INTERACTION);
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComp->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	CollisionComp->SetCollisionResponseToChannel(GENESTEALER_TRACE_INTERACTION, ECR_Block);
	RootComponent = CollisionComp;

	PickupBase = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupBaseMesh"));
	PickupBase->SetupAttachment(CollisionComp);
	PickupBase->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PickupBase->SetCollisionResponseToAllChannels(ECR_Ignore);

	RotatingMovementComponent = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingComponent"));
	RotatingMovementComponent->bRotationInLocalSpace = true;
	RotatingMovementComponent->RotationRate = FRotator(0.f, 90.f, 0.f);
	
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
	if(PickupBase)
	{
		PickupBase->SetRenderCustomDepth(bShouldOutline);
		PickupBase->SetCustomDepthStencilValue(UCombatUtils::GetOutlineIntFromColor(EOutlineColor::Purple));
	}
}

void ABaseOverlapPickup::InteractWithActor(AActor* InstigatingActor)
{
	
}

void ABaseOverlapPickup::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	MaterialDissolver = USpawnUtils::SpawnActorToWorld_Deferred<AMaterialDissolver>(this, MaterialDissolverClass, this);
	USpawnUtils::FinishSpawningActor_Deferred(MaterialDissolver, FTransform());
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
