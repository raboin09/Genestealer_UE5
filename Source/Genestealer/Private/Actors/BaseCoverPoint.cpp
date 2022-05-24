// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/BaseCoverPoint.h"

#include "Characters/BaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Genestealer/Genestealer.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "NavAreas/NavArea_Obstacle.h"
#include "Utils/GameplayTagUtils.h"

ABaseCoverPoint::ABaseCoverPoint()
{
	DefaultGameplayTags.Add(TAG_ACTOR_COVER);

	CoverWallOffset = 40.f;
	bCantMoveInThisCoverPoint = false;

	MiddleCoverWall = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MiddleCoverWall"));
	// MiddleCoverWall->SetStaticMesh(ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'")).Object);
	// MiddleCoverWall->GetStaticMesh()->SetMaterial(0, ConstructorHelpers::FObjectFinder<UMaterial>(TEXT("Material'/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial'")).Object);
	MiddleCoverWall->SetHiddenInGame(true);
	MiddleCoverWall->AddLocalOffset(FVector(0.f, -130.f, 50.f));
	MiddleCoverWall->SetWorldScale3D(FVector(4.f, .1f, 1.3f));
	MiddleCoverWall->SetCollisionResponseToAllChannels(ECR_Ignore);
	MiddleCoverWall->SetCollisionResponseToChannel(TRACE_COVER_WALL, ECR_Block);
	bMiddleCoverEnabled = true;
	
	LeftCoverPeekBox = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftCoverPeekBox"));
	LeftCoverPeekBox->ShapeColor = FColor::Green;
	LeftCoverPeekBox->SetBoxExtent(FVector(25, 25, 80));
	LeftCoverPeekBox->SetupAttachment(RootComponent);
	LeftCoverPeekBox->AreaClass = UNavArea_Obstacle::StaticClass();
	LeftCoverPeekBox->AddLocalOffset(FVector(-44.f, 250.f, 30.f));
	LeftCoverPeekBox->SetWorldScale3D(FVector(.25f, 10.f, 1.f));
	LeftCoverPeekBox->SetupAttachment(MiddleCoverWall);
	LeftCoverPeekBox->SetCollisionObjectType(ECC_WorldDynamic);
	LeftCoverPeekBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	LeftCoverPeekBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	LeftCoverPeekBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	LeftCoverPeekBox->IgnoreActorWhenMoving(this, true);
	bLeftCoverEnabled = true;

	LeftCoverEdgeBox = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftCoverEdgeBox"));
	LeftCoverEdgeBox->ShapeColor = FColor::Red;
	LeftCoverEdgeBox->SetBoxExtent(FVector(25, 25, 80));
	LeftCoverEdgeBox->SetupAttachment(RootComponent);
	LeftCoverEdgeBox->AreaClass = UNavArea_Obstacle::StaticClass();
	LeftCoverEdgeBox->AddLocalOffset(FVector(-50.f, 250.f, 30.f));
	LeftCoverEdgeBox->SetWorldScale3D(FVector(.05f, 10.f, 1.f));
	LeftCoverEdgeBox->SetupAttachment(MiddleCoverWall);
	LeftCoverEdgeBox->SetCollisionObjectType(ECC_WorldDynamic);
	LeftCoverEdgeBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	LeftCoverEdgeBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	LeftCoverEdgeBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	LeftCoverEdgeBox->IgnoreActorWhenMoving(this, true);
	
	RightCoverPeekBox = CreateDefaultSubobject<UBoxComponent>(TEXT("RightCoverPeekBox"));
	RightCoverPeekBox->ShapeColor = FColor::Green;
	RightCoverPeekBox->SetBoxExtent(FVector(25, 25, 80));
	RightCoverPeekBox->SetupAttachment(RootComponent);
	RightCoverPeekBox->AreaClass = UNavArea_Obstacle::StaticClass();
	RightCoverPeekBox->AddLocalOffset(FVector(44.f, 250.f, 30.f));
	RightCoverPeekBox->SetWorldScale3D(FVector(.25f, 10.f, 1.f));
	RightCoverPeekBox->SetupAttachment(MiddleCoverWall);
	RightCoverPeekBox->SetCollisionObjectType(ECC_WorldDynamic);
	RightCoverPeekBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	RightCoverPeekBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	RightCoverPeekBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	RightCoverPeekBox->IgnoreActorWhenMoving(this, true);
	bRightCoverEnabled = true;

	RightCoverEdgeBox = CreateDefaultSubobject<UBoxComponent>(TEXT("RightCoverEdgeBox"));
	RightCoverEdgeBox->ShapeColor = FColor::Red;
	RightCoverEdgeBox->SetBoxExtent(FVector(25, 25, 80));
	RightCoverEdgeBox->SetupAttachment(RootComponent);
	RightCoverEdgeBox->AreaClass = UNavArea_Obstacle::StaticClass();
	RightCoverEdgeBox->AddLocalOffset(FVector(50.f, 250.f, 30.f));
	RightCoverEdgeBox->SetWorldScale3D(FVector(.05f, 10.f, 1.f));
	RightCoverEdgeBox->SetupAttachment(MiddleCoverWall);
	RightCoverEdgeBox->SetCollisionObjectType(ECC_WorldDynamic);
	RightCoverEdgeBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	RightCoverEdgeBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	RightCoverEdgeBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	RightCoverEdgeBox->IgnoreActorWhenMoving(this, true);

	CoverTransitionTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("RotationTimeline"));
}

void ABaseCoverPoint::BeginPlay()
{
	Super::BeginPlay();

	LeftCoverPeekBox->OnComponentBeginOverlap.AddDynamic(this, &ABaseCoverPoint::ActorBeginOverlap);
	LeftCoverPeekBox->OnComponentEndOverlap.AddDynamic(this, &ABaseCoverPoint::ActorEndOverlap);
	
	RightCoverPeekBox->OnComponentBeginOverlap.AddDynamic(this, &ABaseCoverPoint::ActorBeginOverlap);
	RightCoverPeekBox->OnComponentEndOverlap.AddDynamic(this, &ABaseCoverPoint::ActorEndOverlap);

	LeftCoverEdgeBox->OnComponentBeginOverlap.AddDynamic(this, &ABaseCoverPoint::ActorBeginOverlap);
	LeftCoverEdgeBox->OnComponentEndOverlap.AddDynamic(this, &ABaseCoverPoint::ActorEndOverlap);
	
	RightCoverEdgeBox->OnComponentBeginOverlap.AddDynamic(this, &ABaseCoverPoint::ActorBeginOverlap);
	RightCoverEdgeBox->OnComponentEndOverlap.AddDynamic(this, &ABaseCoverPoint::ActorEndOverlap);
	
	FOnTimelineFloat CoverLerpFunction;
	CoverLerpFunction.BindDynamic(this, &ABaseCoverPoint::Internal_CoverTransitionUpdate);
	CoverTransitionTimeline->AddInterpFloat(CoverTransitionCurve, CoverLerpFunction);
	CoverTransitionTimeline->SetLooping(false);

	FOnTimelineEvent CoverLerpFinishedEvent;
	CoverLerpFinishedEvent.BindDynamic(this, &ABaseCoverPoint::Internal_CoverTransitionFinished);
	CoverTransitionTimeline->SetTimelineFinishedFunc(CoverLerpFinishedEvent);
}

void ABaseCoverPoint::OccupyCover(ABaseCharacter* InActor, const FVector& InTargetCoverLocation, const FVector& InHitNormal)
{
	if(OccupiedActor || !InActor)
	{
		return;
	}
	
	OccupiedActor = InActor;
	OccupiedActor->SetStance(EALSStance::Crouching);
	OccupiedActor->SetRotationMode(EALSRotationMode::VelocityDirection, true, true);
	UGameplayTagUtils::AddTagToActor(OccupiedActor, TAG_STATE_IN_COVER);
	TargetCoverLocation = InTargetCoverLocation - (UKismetMathLibrary::GetRightVector(MiddleCoverWall->K2_GetComponentRotation()) * (CoverWallOffset * -1.f));
	TargetCoverRotation = UKismetMathLibrary::MakeRotFromZX(UKismetMathLibrary::Vector_Up(), InHitNormal * -1); 
	
	Internal_ActivateOverlapBoxes(true);	
	Internal_StartCoverTransition();
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
	
	
	UGameplayTagUtils::RemoveTagsFromActor(OccupiedActor, {TAG_STATE_IN_COVER,
		TAG_COVER_LEFTEDGE, TAG_COVER_RIGHTEDGE,
		TAG_COVER_LEFTPEEK, TAG_COVER_RIGHTPEEK});
	OccupiedActor->SetStance(EALSStance::Standing);
	OccupiedActor->SetRotationMode(EALSRotationMode::LookingDirection, true, true);
	OccupiedActor->SetRightShoulder(true);
	OccupiedActor = nullptr;
	Internal_ActivateOverlapBoxes(false);	
}

void ABaseCoverPoint::ActorBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!OverlappedComp || !OtherActor || OtherActor != OccupiedActor || !OtherActor->IsA(ABaseCharacter::StaticClass()))
	{
		return;
	}

	if(OverlappedComp == LeftCoverPeekBox)
	{
		Internal_HandlePeekCoverOverlap(true, OtherActor);
	} else if(OverlappedComp == RightCoverPeekBox)
	{
		Internal_HandlePeekCoverOverlap(false, OtherActor);
	} else if(OverlappedComp == LeftCoverEdgeBox)
	{
		UGameplayTagUtils::AddTagToActor(OtherActor, TAG_COVER_LEFTEDGE);
	} else if(OverlappedComp == RightCoverEdgeBox)
	{
		UGameplayTagUtils::AddTagToActor(OtherActor, TAG_COVER_RIGHTEDGE);
	}
}

void ABaseCoverPoint::ActorEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(!OverlappedComp || !OtherActor || OtherActor != OccupiedActor || !OtherActor->IsA(ABaseCharacter::StaticClass()))
	{
		return;
	}
	
	if(OverlappedComp == LeftCoverPeekBox)
	{
		Internal_HandlePeekCoverOverlapEnd(true, OtherActor);
	} else if(OverlappedComp == RightCoverPeekBox)
	{
		Internal_HandlePeekCoverOverlapEnd(false, OtherActor);
	} else if(OverlappedComp == LeftCoverEdgeBox)
	{
		UGameplayTagUtils::RemoveTagFromActor(OtherActor, TAG_COVER_LEFTEDGE);
	} else if(OverlappedComp == RightCoverEdgeBox)
	{
		UGameplayTagUtils::RemoveTagFromActor(OtherActor, TAG_COVER_RIGHTEDGE);
	}
}

void ABaseCoverPoint::Internal_HandlePeekCoverOverlap(bool bLeftCoverPoint, AActor* OtherActor)
{
	if(bCantMoveInThisCoverPoint)
	{
		return;
	}
	
	if(bLeftCoverPoint)
	{
		if(OccupiedActor)
		{
			OccupiedActor->SetRightShoulder(false);
		}
		UGameplayTagUtils::AddTagToActor(OtherActor, TAG_COVER_LEFTPEEK);
	} else
	{
		if(OccupiedActor)
		{
			OccupiedActor->SetRightShoulder(true);
		}
		UGameplayTagUtils::AddTagToActor(OtherActor, TAG_COVER_RIGHTPEEK);
	}
}

void ABaseCoverPoint::Internal_HandlePeekCoverOverlapEnd(bool bLeftCoverPoint, AActor* OtherActor)
{
	if(bCantMoveInThisCoverPoint)
	{
		return;
	}
	
	if(bLeftCoverPoint)
	{
		UGameplayTagUtils::RemoveTagFromActor(OtherActor, TAG_COVER_LEFTPEEK);
	} else
	{
		UGameplayTagUtils::RemoveTagFromActor(OtherActor, TAG_COVER_RIGHTPEEK);
	}
}

void ABaseCoverPoint::Internal_ActivateOverlapBoxes(bool bActivate)
{
	LeftCoverPeekBox->SetGenerateOverlapEvents(bActivate);
	RightCoverPeekBox->SetGenerateOverlapEvents(bActivate);
	LeftCoverEdgeBox->SetGenerateOverlapEvents(bActivate);
	RightCoverEdgeBox->SetGenerateOverlapEvents(bActivate);
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