// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/BaseCoverPoint.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Genestealer/Genestealer.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "NavAreas/NavArea_Obstacle.h"
#include "Utils/GameplayTagUtils.h"

ABaseCoverPoint::ABaseCoverPoint()
{
	DefaultGameplayTags.Add(GameplayTag::ActorType::Cover);

	CoverWallOffset = 40.f;
	bCantMoveInThisCoverPoint = false;

	MiddleCoverWall = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MiddleCoverWall"));
	MiddleCoverWall->SetStaticMesh(ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'")).Object);
	MiddleCoverWall->GetStaticMesh()->SetMaterial(0, ConstructorHelpers::FObjectFinder<UMaterial>(TEXT("Material'/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial'")).Object);
	MiddleCoverWall->SetHiddenInGame(true);
	MiddleCoverWall->AddLocalOffset(FVector(0.f, -130.f, 50.f));
	MiddleCoverWall->SetRelativeScale3D(FVector(4.f, .1f, 1.3f));
	MiddleCoverWall->SetCollisionResponseToAllChannels(ECR_Ignore);
	MiddleCoverWall->SetCollisionResponseToChannel(TRACE_COVER_WALL, ECR_Block);
	bMiddleCoverEnabled = true;
	
	LeftCoverCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftCoverCollisionBox"));
	LeftCoverCollisionBox->SetBoxExtent(FVector(25, 25, 80));
	LeftCoverCollisionBox->SetupAttachment(RootComponent);
	LeftCoverCollisionBox->AreaClass = UNavArea_Obstacle::StaticClass();
	LeftCoverCollisionBox->AddLocalOffset(FVector(-44.f, 250.f, 30.f));
	LeftCoverCollisionBox->SetRelativeScale3D(FVector(.25f, 10.f, 1.f));
	LeftCoverCollisionBox->SetupAttachment(MiddleCoverWall);
	LeftCoverCollisionBox->SetCollisionObjectType(ECC_WorldDynamic);
	LeftCoverCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	LeftCoverCollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	LeftCoverCollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	bLeftCoverEnabled = true;
	
	RightCoverCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("RightCoverCollisionBox"));
	RightCoverCollisionBox->SetBoxExtent(FVector(25, 25, 80));
	RightCoverCollisionBox->SetupAttachment(RootComponent);
	RightCoverCollisionBox->AreaClass = UNavArea_Obstacle::StaticClass();
	RightCoverCollisionBox->AddLocalOffset(FVector(44.f, 250.f, 30.f));
	RightCoverCollisionBox->SetRelativeScale3D(FVector(.25f, 10.f, 1.f));
	RightCoverCollisionBox->SetupAttachment(MiddleCoverWall);
	RightCoverCollisionBox->SetCollisionObjectType(ECC_WorldDynamic);
	RightCoverCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	RightCoverCollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	RightCoverCollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	bRightCoverEnabled = true;

	CoverTransitionTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("RotationTimeline"));
}

void ABaseCoverPoint::BeginPlay()
{
	Super::BeginPlay();

	LeftCoverCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ABaseCoverPoint::ActorBeginOverlap);
	LeftCoverCollisionBox->OnComponentEndOverlap.AddDynamic(this, &ABaseCoverPoint::ActorEndOverlap);
	
	RightCoverCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ABaseCoverPoint::ActorBeginOverlap);
	RightCoverCollisionBox->OnComponentEndOverlap.AddDynamic(this, &ABaseCoverPoint::ActorEndOverlap);
	
	FOnTimelineFloat CoverLerpFunction;
	CoverLerpFunction.BindDynamic(this, &ABaseCoverPoint::Internal_CoverTransitionUpdate);
	CoverTransitionTimeline->AddInterpFloat(CoverTransitionCurve, CoverLerpFunction);
	CoverTransitionTimeline->SetLooping(false);

	FOnTimelineEvent CoverLerpFinishedEvent;
	CoverLerpFinishedEvent.BindDynamic(this, &ABaseCoverPoint::Internal_CoverTransitionFinished);
	CoverTransitionTimeline->SetTimelineFinishedFunc(CoverLerpFinishedEvent);
}

void ABaseCoverPoint::OccupyCover(ABaseCharacter* InActor, const FVector& InTargetCoverLocation)
{
	if(OccupiedActor || !InActor)
	{
		return;
	}
	
	OccupiedActor = InActor;
	OccupiedActor->SetStance(EALSStance::Crouching);
	OccupiedActor->SetRotationMode(EALSRotationMode::VelocityDirection);
	OccupiedActor->SetGait(EALSGait::Walking);
	TargetCoverLocation = InTargetCoverLocation - (UKismetMathLibrary::GetRightVector(MiddleCoverWall->K2_GetComponentRotation()) * (CoverWallOffset * -1.f));
	TargetCoverRotation = FRotator(OccupiedActor->GetActorRotation().Pitch, MiddleCoverWall->K2_GetComponentRotation().Yaw - 90, OccupiedActor->GetActorRotation().Roll);
	Internal_StartCoverTransition();
	UGameplayTagUtils::AddTagToActor(OccupiedActor, GameplayTag::State::InCover_Middle);
}

void ABaseCoverPoint::VacateCover(ABaseCharacter* InActor)
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
	if(!OverlappedComp || !OtherActor || OtherActor != OccupiedActor || !OtherActor->IsA(ABaseCharacter::StaticClass()))
	{
		return;
	}

	if(OverlappedComp == LeftCoverCollisionBox)
	{
		Internal_HandleEdgeCoverOverlap(true, OtherActor);
	} else if(OverlappedComp == RightCoverCollisionBox)
	{
		Internal_HandleEdgeCoverOverlap(false, OtherActor);
	}
}

void ABaseCoverPoint::ActorEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(!OverlappedComp || !OtherActor || OtherActor != OccupiedActor || !OtherActor->IsA(ABaseCharacter::StaticClass()))
	{
		return;
	}
	
	if(OverlappedComp == LeftCoverCollisionBox)
	{
		Internal_HandleEdgeCoverOverlapEnd(true, OtherActor);
	} else if(OverlappedComp == RightCoverCollisionBox)
	{
		Internal_HandleEdgeCoverOverlapEnd(false, OtherActor);
	} else
	{
		UKismetSystemLibrary::PrintString(this, "Bad");
	}
}

void ABaseCoverPoint::Internal_HandleEdgeCoverOverlap(bool bLeftCoverPoint, AActor* OtherActor)
{
	if(bCantMoveInThisCoverPoint)
	{
		return;
	}
	
	if(bLeftCoverPoint)
	{
		UGameplayTagUtils::AddTagToActor(OtherActor, GameplayTag::State::InCover_Left);
	} else
	{
		UGameplayTagUtils::AddTagToActor(OtherActor, GameplayTag::State::InCover_Right);
	}
}

void ABaseCoverPoint::Internal_HandleEdgeCoverOverlapEnd(bool bLeftCoverPoint, AActor* OtherActor)
{
	if(bCantMoveInThisCoverPoint)
	{
		return;
	}
	
	if(bLeftCoverPoint)
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
		CharMoveComp->SetPlaneConstraintFromVectors(MiddleCoverWall->GetForwardVector(), MiddleCoverWall->GetUpVector());
		CharMoveComp->SetPlaneConstraintEnabled(true);
	}
}
