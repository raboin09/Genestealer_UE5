// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/BaseDestructibleActor.h"
#include "Characters/InteractionComponent.h"
#include "Characters/EffectContainerComponent.h"
#include "Characters/HealthComponent.h"
#include "Genestealer/Genestealer.h"
#include "Utils/GameplayTagUtils.h"

ABaseDestructibleActor::ABaseDestructibleActor()
{
	PrimaryActorTick.bCanEverTick = false;

	DestructibleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DestructibleMesh"));
	DestructibleMesh->SetCollisionObjectType(ECC_WorldDynamic);
	DestructibleMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	DestructibleMesh->SetCollisionResponseToChannels(ECR_Block);
	DestructibleMesh->SetCollisionResponseToChannel(GENESTEALER_TRACE_WEAPON, ECR_Block);
	DestructibleMesh->SetCollisionResponseToChannel(GENESTEALER_OBJECT_TYPE_PROJECTILE, ECR_Block);
	SetRootComponent(DestructibleMesh);

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	EffectContainerComponent = CreateDefaultSubobject<UEffectContainerComponent>(TEXT("EffectContainer"));
	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));

	StartingHealth.MaxWounds = 1;
	StartingHealth.MaxHealthPerWound = 50;
	
	DestructibleAffiliation = EAffiliation::Destructible;
}

void ABaseDestructibleActor::BeginPlay()
{
	Super::BeginPlay();
	if(HealthComponent)
	{
		HealthComponent->OnActorDeath().AddDynamic(this, &ABaseDestructibleActor::HandleDeathEvent);
		HealthComponent->InitHealthComponent(StartingHealth);
	}
}

void ABaseDestructibleActor::SwitchOutlineOnMesh(bool bShouldOutline)
{
	if(InteractionComponent)
	{
		InteractionComponent->SwitchOutlineOnAllMeshes(bShouldOutline);
	}
}

void ABaseDestructibleActor::InteractWithActor(AActor* InstigatingActor)
{
	
}

void ABaseDestructibleActor::HandleDeathEvent(const FActorDeathEventPayload& EventPayload)
{
	GameplayTagContainer.AddTag(TAG_STATE_DEAD);
	if(InteractionComponent)
	{
		InteractionComponent->SwitchOutlineOnAllMeshes(false);	
	}
	K2_OnDeath(EventPayload.HitResult);
	SetLifeSpan(DeathBuffer);
}
