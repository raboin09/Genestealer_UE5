// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/BaseCoverPoint.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "NavAreas/NavArea_Obstacle.h"
#include "Utils/GameplayTagUtils.h"

ABaseCoverPoint::ABaseCoverPoint()
{
	DefaultGameplayTags.Add(GameplayTag::ActorType::Cover);

	CoverWallOffset = 40.f;

	MiddleCover = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MiddleCover"));
	MiddleCover->SetHiddenInGame(true);
	RootComponent = MiddleCover;
	bMiddleCoverEnabled = true;
	
	RightCoverCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("MiddleCover"));
	RightCoverCollision->SetBoxExtent(FVector(25, 25, 80));
	RightCoverCollision->SetupAttachment(RootComponent);
	RightCoverCollision->AreaClass = UNavArea_Obstacle::StaticClass();
	RightCoverCollision->AddLocalOffset(FVector(-180.f, -100.f, 80.f));
	bRightCoverEnabled = true;
	
	LeftCoverCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftCover"));
	LeftCoverCollision->SetBoxExtent(FVector(25, 25, 80));
	LeftCoverCollision->SetupAttachment(RootComponent);
	LeftCoverCollision->AreaClass = UNavArea_Obstacle::StaticClass();
	RightCoverCollision->AddLocalOffset(FVector(180.f, -100.f, 80.f));
	bLeftCoverEnabled = true;

	CoverTransitionTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("RotationTimeline"));
}

void ABaseCoverPoint::BeginPlay()
{
	Super::BeginPlay();
	
	FOnTimelineFloat CoverLerpFunction;
	CoverLerpFunction.BindDynamic(this, &ABaseCoverPoint::Internal_CoverTransitionUpdate);
	CoverTransitionTimeline->AddInterpFloat(CoverTransitionCurve, CoverLerpFunction);
	CoverTransitionTimeline->SetLooping(false);

	FOnTimelineEvent CoverLerpFinishedEvent;
	CoverLerpFinishedEvent.BindDynamic(this, &ABaseCoverPoint::Internal_CoverTransitionFinished);
	CoverTransitionTimeline->SetTimelineFinishedFunc(CoverLerpFinishedEvent);
}

void ABaseCoverPoint::OccupyCover(AActor* InActor, const FVector& InTargetCoverLocation)
{
	if(OccupiedActor || !InActor)
	{
		return;
	}
	
	TargetCoverLocation = InTargetCoverLocation - (UKismetMathLibrary::GetRightVector(MiddleCover->K2_GetComponentRotation()) * (CoverWallOffset * -1.f));
	TargetCoverRotation = FRotator(OccupiedActor->GetActorRotation().Pitch, MiddleCover->K2_GetComponentRotation().Yaw - 90, OccupiedActor->GetActorRotation().Roll);
	OccupiedActor = InActor;
	Internal_StartCoverTransition();
	UGameplayTagUtils::AddTagToActor(OccupiedActor, GameplayTag::State::InCover_Middle);
}

void ABaseCoverPoint::VacateCover(AActor* InActor)
{
	if(!OccupiedActor || OccupiedActor != InActor)
	{
		return;
	}

	if(UCharacterMovementComponent* CharMoveComp = InActor->FindComponentByClass<UCharacterMovementComponent>())
	{
		CharMoveComp->SetPlaneConstraintEnabled(false);
	}
	
	UGameplayTagUtils::RemoveTagFromActor(OccupiedActor, GameplayTag::State::InCover_Middle);
	OccupiedActor = nullptr;
}

void ABaseCoverPoint::ActorBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!OverlappedComp || !OtherActor || OtherActor != OccupiedActor)
	{
		return;
	}
	
	if(OverlappedComp == LeftCoverCollision)
	{
		Internal_HandleEdgeCoverOverlap(true, OtherActor);
	} else if(OverlappedComp == RightCoverCollision)
	{
		Internal_HandleEdgeCoverOverlap(false, OtherActor);
	}
}

void ABaseCoverPoint::ActorEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(!OverlappedComp || !OtherActor || OtherActor != OccupiedActor)
	{
		return;
	}
	
	if(OverlappedComp == LeftCoverCollision)
	{
		Internal_HandleEdgeCoverOverlapEnd(true, OtherActor);
	} else if(OverlappedComp == RightCoverCollision)
	{
		Internal_HandleEdgeCoverOverlapEnd(false, OtherActor);
	}
}

void ABaseCoverPoint::Internal_HandleEdgeCoverOverlap(bool bLeftCoverPoint, AActor* OtherActor)
{
	if(bLeftCoverPoint && bCantMoveInThisCoverPoint)
	{
		UGameplayTagUtils::AddTagToActor(OtherActor, GameplayTag::State::InCover_Left);
	} else
	{
		UGameplayTagUtils::AddTagToActor(OtherActor, GameplayTag::State::InCover_Right);
	}
}

void ABaseCoverPoint::Internal_HandleEdgeCoverOverlapEnd(bool bLeftCoverPoint, AActor* OtherActor)
{
	if(bLeftCoverPoint && bCantMoveInThisCoverPoint)
	{
		UGameplayTagUtils::RemoveTagFromActor(OtherActor, GameplayTag::State::InCover_Left);
	} else
	{
		UGameplayTagUtils::RemoveTagFromActor(OtherActor, GameplayTag::State::InCover_Right);
	}
}

void ABaseCoverPoint::Internal_StartCoverTransition()
{
	CoverTransitionTimeline->PlayFromStart();
}

void ABaseCoverPoint::Internal_CoverTransitionUpdate(float Alpha)
{
	if(!OccupiedActor)
	{
		return;
	}

	const FTransform& OccupiedActorTransform = OccupiedActor->GetActorTransform();
	FTransform TargetTransform;
	TargetTransform.SetLocation(FVector(TargetCoverLocation.X, TargetCoverLocation.Y, OccupiedActorTransform.GetLocation().Z));
	TargetTransform.SetRotation(FQuat(TargetCoverRotation));
	const FTransform& NewActorTransform = UKismetMathLibrary::TLerp(OccupiedActorTransform, TargetTransform, Alpha, ELerpInterpolationMode::QuatInterp);
	OccupiedActor->SetActorLocationAndRotation(NewActorTransform.GetTranslation(), NewActorTransform.GetRotation());
}

void ABaseCoverPoint::Internal_CoverTransitionFinished()
{
	if(UCharacterMovementComponent* CharMoveComp = OccupiedActor->FindComponentByClass<UCharacterMovementComponent>())
	{
		CharMoveComp->SetPlaneConstraintFromVectors(MiddleCover->GetForwardVector(), MiddleCover->GetUpVector());
		CharMoveComp->SetPlaneConstraintEnabled(true);
	}
}
