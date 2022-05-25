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
	Internal_SetCoverNormalRotationValues();
	OccupiedActor->SetDesiredGait(EALSGait::Walking);
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
	Internal_ResetCharacterValuesOnCoverExit();
	GetWorldTimerManager().ClearTimer(TimerHandle_StartFiringDelay);
	OccupiedActor = nullptr;
	Internal_ActivateOverlapBoxes(false);	
}

void ABaseCoverPoint::StartCoverFire()
{
	if(!OccupiedActor || !OccupiedActor->GetInventoryComponent())
	{
		return;
	}

	if(ActorInLeftEdge())
	{
		if(ActorInLeftPeek())
		{
			OccupiedActor->GetInventoryComponent()->StartFiring();
		} else
		{
			Internal_TryPeekRolloutAndFire(LeftCoverPeekBox, false);	
		}
	} else if(ActorInRightEdge())
	{
		if(ActorInRightPeek())
		{
			OccupiedActor->GetInventoryComponent()->StartFiring();
		} else
		{
			Internal_TryPeekRolloutAndFire(RightCoverPeekBox, true);	
		}
	} else
	{
		if(ActorAiming())
		{
			OccupiedActor->GetInventoryComponent()->StartFiring();
		} else
		{
			if(bCrouchingCover)
			{
				OccupiedActor->SetStance(EALSStance::Standing);
			}
			Internal_SetCoverAimingRotationValues(true);
			GetWorldTimerManager().SetTimer(TimerHandle_StartFiringDelay, OccupiedActor->GetInventoryComponent(), &UInventoryComponent::StartFiring, DelayBeforeCrouchToStandShoot, false);
		}
	}
}

void ABaseCoverPoint::StopCoverFire()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_StartFiringDelay);
	if(!OccupiedActor || !OccupiedActor->GetInventoryComponent())
	{
		return;
	}
	
	if(ActorInLeftEdge() || ActorInRightEdge())
	{
		if(!ActorAiming())
		{
			Internal_StartPeekRollback();	
		}
		Internal_StopPeekFire();
	} else
	{
		if(!ActorAiming())
		{
			Internal_SetCoverNormalRotationValues();
		}
		OccupiedActor->GetInventoryComponent()->StopFiring();
	}
	
	if(bCrouchingCover)
	{
		OccupiedActor->SetStance(EALSStance::Crouching);
	}
}

void ABaseCoverPoint::StartCoverAim()
{
	if(!OccupiedActor)
	{
		return;
	}

	if(ActorInLeftEdge())
	{
		Internal_StartPeekRollout(LeftCoverPeekBox, false);
	} else if(ActorInRightEdge())
	{
		Internal_StartPeekRollout(RightCoverPeekBox, true);
	} else if(bCrouchingCover)
	{
		OccupiedActor->SetStance(EALSStance::Standing, true);
	}
	OccupiedActor->SetRotationMode(EALSRotationMode::Aiming);
}

void ABaseCoverPoint::StopCoverAim()
{
	if(!OccupiedActor)
	{
		return;
	}

	if(ActorInLeftEdge())
	{
		Internal_StartPeekRollback();
	} else if(ActorInRightEdge())
	{
		Internal_StartPeekRollback();
	} else if(bCrouchingCover)
	{
		OccupiedActor->SetStance(EALSStance::Crouching, true);
	}
	OccupiedActor->SetRotationMode(EALSRotationMode::VelocityDirection);
}

void ABaseCoverPoint::ActorBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!OverlappedComp || !OtherActor || OtherActor != OccupiedActor || !OtherActor->IsA(ABaseCharacter::StaticClass()) || !OtherComp || OtherComp->IsA(UMeshComponent::StaticClass()))
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
		GetWorldTimerManager().SetTimer(TimerHandle_AddTags, this, &ABaseCoverPoint::Internal_ApplyLeftEdgeTagToActor, DelayBeforeTagsApply, false);
	} else if(OverlappedComp == RightCoverEdgeBox)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_AddTags, this, &ABaseCoverPoint::Internal_ApplyRightEdgeTagToActor, DelayBeforeTagsApply, false);
	}
}

void ABaseCoverPoint::ActorEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(!OverlappedComp || !OtherActor || OtherActor != OccupiedActor || !OtherActor->IsA(ABaseCharacter::StaticClass()) || !OtherComp || OtherComp->IsA(UMeshComponent::StaticClass()))
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

void ABaseCoverPoint::Internal_StartPeekRollout(const UShapeComponent* TargetPeekBox, bool bRightCameraShoulder)
{
	Internal_SetCoverAimingRotationValues(bRightCameraShoulder);
	TargetCoverLocation = TargetPeekBox->GetComponentLocation();
	Internal_StartCoverTransition();
}

void ABaseCoverPoint::Internal_StartPeekRollback()
{
	TargetCoverLocation = CachedTransform.GetLocation();
	TargetCoverRotation = CachedTransform.Rotator();
	Internal_SetCoverNormalRotationValues();
	Internal_StartCoverTransition();
}

void ABaseCoverPoint::Internal_StartPeekFire()
{
	if(!OccupiedActor || !OccupiedActor->GetInventoryComponent())
	{
		return;
	}
	GetWorldTimerManager().SetTimer(TimerHandle_StartFiringDelay, OccupiedActor->GetInventoryComponent(), &UInventoryComponent::StartFiring, DelayBeforePeekShoot, false);
}

void ABaseCoverPoint::Internal_StopPeekFire() const
{
	if(!OccupiedActor || !OccupiedActor->GetInventoryComponent())
	{
		return;
	}
	OccupiedActor->GetInventoryComponent()->StopFiring();
}

void ABaseCoverPoint::Internal_HandlePeekCoverOverlap(bool bLeftCoverPoint, AActor* OtherActor)
{	
	if(bLeftCoverPoint)
	{
		UGameplayTagUtils::AddTagToActor(OtherActor, TAG_COVER_LEFTPEEK);
	} else
	{
		UGameplayTagUtils::AddTagToActor(OtherActor, TAG_COVER_RIGHTPEEK);
	}
}

void ABaseCoverPoint::Internal_HandlePeekCoverOverlapEnd(bool bLeftCoverPoint, AActor* OtherActor)
{
	if(bLeftCoverPoint)
	{
		UGameplayTagUtils::RemoveTagFromActor(OtherActor, TAG_COVER_LEFTPEEK);
	} else
	{
		UGameplayTagUtils::RemoveTagFromActor(OtherActor, TAG_COVER_RIGHTPEEK);
	}
}

void ABaseCoverPoint::Internal_ActivateOverlapBoxes(bool bActivate) const
{
	LeftCoverPeekBox->SetGenerateOverlapEvents(bActivate);
	RightCoverPeekBox->SetGenerateOverlapEvents(bActivate);
	LeftCoverEdgeBox->SetGenerateOverlapEvents(bActivate);
	RightCoverEdgeBox->SetGenerateOverlapEvents(bActivate);
}

void ABaseCoverPoint::Internal_ApplyLeftEdgeTagToActor()
{
	if(!OccupiedActor)
	{
		return;
	}
	
	CachedTransform = OccupiedActor->GetActorTransform();
	UGameplayTagUtils::AddTagToActor(OccupiedActor, TAG_COVER_LEFTEDGE);
}

void ABaseCoverPoint::Internal_ApplyRightEdgeTagToActor()
{
	if(!OccupiedActor)
	{
		return;
	}
	
	CachedTransform = OccupiedActor->GetActorTransform();
	UGameplayTagUtils::AddTagToActor(OccupiedActor, TAG_COVER_RIGHTEDGE);
}

void ABaseCoverPoint::Internal_ResetCharacterValuesOnCoverExit() const
{
	if(bCrouchingCover)
	{
		OccupiedActor->SetStance(EALSStance::Standing);
	}
	OccupiedActor->SetRotationMode(EALSRotationMode::LookingDirection, true, true);
	OccupiedActor->SetDesiredGait(EALSGait::Running);
}

void ABaseCoverPoint::Internal_SetCoverNormalRotationValues() const
{
	if(!OccupiedActor)
	{
		return;
	}
	OccupiedActor->SetRightShoulder(true);
	OccupiedActor->SetRotationMode(EALSRotationMode::VelocityDirection, true, true);
}

void ABaseCoverPoint::Internal_SetCoverAimingRotationValues(bool bRightShoulder) const
{
	if(!OccupiedActor)
	{
		return;
	}
	OccupiedActor->SetRightShoulder(bRightShoulder);
	OccupiedActor->SetRotationMode(EALSRotationMode::Aiming, true, true);
}

void ABaseCoverPoint::Internal_TryPeekRolloutAndFire(const UShapeComponent* TargetPeekBox, bool bRightCameraShoulder)
{
	if(!ActorAiming())
	{
		Internal_StartPeekRollout(TargetPeekBox, bRightCameraShoulder);	
	}
	Internal_StartPeekFire();
}

void ABaseCoverPoint::Internal_StartCoverTransition() const
{
	if(!OccupiedActor)
	{
		return;
	}	
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
	OccupiedActor->SetActorLocationAndRotation(NewActorTransform.GetTranslation(), NewActorTransform.GetRotation(), true);
}

void ABaseCoverPoint::Internal_CoverTransitionFinished()
{
	if(!OccupiedActor)
	{
		return;
	}
	
	if(UCharacterMovementComponent* CharMoveComp = OccupiedActor->FindComponentByClass<UCharacterMovementComponent>())
	{
		CharMoveComp->SetPlaneConstraintFromVectors(MiddleCoverWall->GetForwardVector(), MiddleCoverWall->GetUpVector());
		CharMoveComp->SetPlaneConstraintEnabled(true);
	}
}

bool ABaseCoverPoint::ActorInLeftEdge() const
{
	return UGameplayTagUtils::ActorHasGameplayTag(OccupiedActor, TAG_COVER_LEFTEDGE);
}

bool ABaseCoverPoint::ActorInLeftPeek() const
{
	return UGameplayTagUtils::ActorHasGameplayTag(OccupiedActor, TAG_COVER_LEFTPEEK);
}

bool ABaseCoverPoint::ActorInRightEdge() const
{
	return UGameplayTagUtils::ActorHasGameplayTag(OccupiedActor, TAG_COVER_RIGHTEDGE);
}

bool ABaseCoverPoint::ActorInRightPeek() const
{
	return UGameplayTagUtils::ActorHasGameplayTag(OccupiedActor, TAG_COVER_RIGHTPEEK);
}

bool ABaseCoverPoint::ActorAiming() const
{
	return UGameplayTagUtils::ActorHasGameplayTag(OccupiedActor, TAG_STATE_AIMING);
}
