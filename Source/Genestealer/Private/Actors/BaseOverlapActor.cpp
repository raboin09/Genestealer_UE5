// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/BaseOverlapActor.h"
#include "Components/ShapeComponent.h"
#include "Kismet/KismetSystemLibrary.h"

ABaseOverlapActor::ABaseOverlapActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ABaseOverlapActor::BeginPlay()
{
	Super::BeginPlay();
	if(UShapeComponent* ShapeComponent = GetCollisionComponent())
	{
		ShapeComponent->OnComponentBeginOverlap.AddDynamic(this, &ABaseOverlapActor::ActorBeginOverlap);
		ShapeComponent->OnComponentEndOverlap.AddDynamic(this, &ABaseOverlapActor::ActorEndOverlap);
		ShapeComponent->IgnoreActorWhenMoving(this, true);
		ShapeComponent->IgnoreComponentWhenMoving(GetMesh(), true);
	}

	if(!bActivateOnStart)
	{
		Deactivate();	
	} else
	{
		Activate();
	}
}

void ABaseOverlapActor::K2_HandleOverlapEvent_Implementation(AActor* OtherActor, const FHitResult& HitResult)
{
	if(bDiesAfterOverlap) {
		HandleActorDeath();
	}
}

void ABaseOverlapActor::K2_HandleEndOverlapEvent_Implementation(AActor* ExitingActor)
{
	
}

void ABaseOverlapActor::HandleActorDeath()
{
	Deactivate();
	SetLifeSpan(DeathBuffer);
}

void ABaseOverlapActor::Activate()
{
	if (!IsActive()) {
		HitActors.Empty();
		UGameplayTagUtils::AddTagToActor(this, TAG_STATE_ACTIVE);
		if(UShapeComponent* ShapeComponent = GetCollisionComponent())
		{
			ShapeComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			ShapeComponent->SetGenerateOverlapEvents(true);
		}
	}
}

void ABaseOverlapActor::Deactivate()
{
	if (IsActive()) {
		UGameplayTagUtils::RemoveTagFromActor(this, TAG_STATE_ACTIVE);
		if(UShapeComponent* ShapeComponent = GetCollisionComponent())
		{
			ShapeComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			ShapeComponent->SetGenerateOverlapEvents(false);
		}
		HitActors.Empty();
	}
}

void ABaseOverlapActor::ActorBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!OtherActor || OtherActor->IsA(StaticClass()))
	{
		return;
	}

	const bool bTagsBlocked = BlockedOverlapTags.Num() > 0 ? UGameplayTagUtils::ActorHasAnyGameplayTags(OtherActor, BlockedOverlapTags) : false;
	if(bTagsBlocked)
	{
		return;
	}

	const bool bValidRequiredTags = RequiredOverlapTags.Num() > 0 ? UGameplayTagUtils::ActorHasAnyGameplayTags(OtherActor, RequiredOverlapTags) : true;
	if (IsActive() && !HitActors.Contains(OtherActor) && bValidRequiredTags) {
		HitActors.Add(OtherActor);
		K2_HandleOverlapEvent(OtherActor, SweepResult);
	}
}

void ABaseOverlapActor::ActorEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(OtherActor && !OtherActor->IsA(StaticClass()) && HitActors.Contains(OtherActor))
	{
		HitActors.Remove(OtherActor);
		K2_HandleEndOverlapEvent(OtherActor);
	}
}