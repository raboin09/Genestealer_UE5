// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/BaseCoverPoint.h"

#include "Characters/BaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Genestealer/Genestealer.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "NavAreas/NavArea_Obstacle.h"
#include "Utils/CombatUtils.h"
#include "Utils/CoreUtils.h"
#include "Utils/FeedbackUtils.h"
#include "Utils/GameplayTagUtils.h"

ABaseCoverPoint::ABaseCoverPoint()
{
	DefaultGameplayTags.Add(TAG_ACTOR_COVER);
	CoverWallOffset = 40.f;
	bShouldPlayCameraShake = true;
	
	CoverMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CoverMesh"));
	RootComponent = CoverMesh;
	
	MiddleCoverWall = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MiddleCoverWall"));
	MiddleCoverWall->SetHiddenInGame(true);
	MiddleCoverWall->AddWorldOffset(FVector(0.f, 60.f, 0.f));
	MiddleCoverWall->SetUsingAbsoluteScale(true);
	MiddleCoverWall->SetWorldScale3D(FVector(1.f, .1f, 1.f));
	MiddleCoverWall->SetCollisionResponseToAllChannels(ECR_Ignore);
	MiddleCoverWall->SetCollisionResponseToChannel(GENESTEALER_TRACE_INTERACTION, ECR_Block);
	MiddleCoverWall->SetCollisionResponseToChannel(GENESTEALER_TRACE_COVER_WALL, ECR_Block);
	MiddleCoverWall->SetupAttachment(CoverMesh);
	bMiddleCoverEnabled = true;
	
	LeftCoverPeekBox = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftCoverPeekBox"));
	InitCoverBox(LeftCoverPeekBox);
	bLeftCoverEnabled = true;

	RightCoverPeekBox = CreateDefaultSubobject<UBoxComponent>(TEXT("RightCoverPeekBox"));
	InitCoverBox(RightCoverPeekBox);
	bRightCoverEnabled = true;
	
	LeftCoverEdgeBox = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftCoverEdgeBox"));
	InitCoverBox(LeftCoverEdgeBox);

	RightCoverEdgeBox = CreateDefaultSubobject<UBoxComponent>(TEXT("RightCoverEdgeBox"));
	InitCoverBox(RightCoverEdgeBox);

	CoverTransitionTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("RotationTimeline"));
	DissolveComponent = CreateDefaultSubobject<UDissolveComponent>(TEXT("DissolveComponent"));
	
	DissolveMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("DissolveMesh"));
	DissolveMesh->SetupAttachment(MiddleCoverWall);
	DissolveMesh->AddWorldOffset(FVector(0.f, 600.f, -60.f));
	DissolveMesh->SetUsingAbsoluteScale(true);
	DissolveMesh->SetWorldScale3D(FVector(1.f, 1.f, 1.f));
}

void ABaseCoverPoint::InitCoverBox(UBoxComponent* InBox)
{
	if(!InBox)
	{
		return;
	}
	
	if(InBox == LeftCoverPeekBox)
	{
		InBox->ShapeColor = FColor::Green;
		InBox->AddWorldOffset(FVector( -57.5f, 600.f, 0.f));
		InBox->SetBoxExtent(FVector(25.f, 25.f, 40.f));
	} else if(InBox == RightCoverPeekBox)
	{
		InBox->ShapeColor = FColor::Green;
		InBox->AddWorldOffset(FVector( 57.5f, 600.f, 0.f));
		InBox->SetBoxExtent(FVector(25.f, 25.f, 40.f));
	} else if(InBox == LeftCoverEdgeBox)
	{
		InBox->ShapeColor = FColor::Red;
		InBox->SetBoxExtent(FVector(3.f, 15.f, 40.f));
		InBox->AddWorldOffset(FVector( -50.f, 200.f, 0.f));
	} else if(InBox == RightCoverEdgeBox)
	{
		InBox->ShapeColor = FColor::Red;
		InBox->SetBoxExtent(FVector(3.f, 15.5, 40.f));
		InBox->AddWorldOffset(FVector( 50.f, 200.f, 0.f));
	}
	InBox->SetUsingAbsoluteScale(true);
	InBox->SetupAttachment(MiddleCoverWall);
	InBox->AreaClass = UNavArea_Obstacle::StaticClass();
	InBox->SetCollisionObjectType(ECC_WorldDynamic);
	InBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	InBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	InBox->IgnoreActorWhenMoving(this, true);
	InBox->SetWorldScale3D(FVector(1.f, 1.f, 1.f));
}

void ABaseCoverPoint::SwitchOutlineOnMesh(bool bShouldOutline)
{
	if(CoverMesh)
	{
		const int32 OutlineColorInt = UCombatUtils::GetOutlineIntFromColor(EOutlineColor::Gray);
		CoverMesh->SetRenderCustomDepth(bShouldOutline);
		CoverMesh->SetCustomDepthStencilValue(OutlineColorInt);
	}

	if(!DissolveComponent)
	{
		return;
	}

	if(!bShouldOutline)
	{
		DissolveComponent->StartDissolveTimeline();
		bShouldPlayCoverDissolve = true;
	} else if(bShouldPlayCoverDissolve && bShouldOutline)
	{
		bShouldPlayCoverDissolve = false;
		DissolveComponent->StartAppearTimeline();
	}
}

void ABaseCoverPoint::InteractWithActor(AActor* InstigatingActor)
{
	
}

void ABaseCoverPoint::PostInitializeComponents()
{
	Super::PostInitializeComponents();
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
	
	if(DissolveComponent)
	{
		DissolveComponent->InitDissolveableMesh(DissolveMesh);
	}

	bShouldPlayCoverDissolve = true;
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

	if(DissolveComponent)
	{
		DissolveComponent->ResetDissolveState(true);
	}
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
	bShouldPlayCameraShake = true;
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
			if(bCrouchingCover)
			{
				OccupiedActor->SetStance(EALSStance::Crouching);
			}
			Internal_SetCoverNormalRotationValues();
		}
		OccupiedActor->GetInventoryComponent()->StopFiring();
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

	if(ActorInLeftEdge() || ActorInRightEdge())
	{
		OccupiedActor->SetStance(EALSStance::Crouching, true);
		Internal_StartPeekRollback();
	} else if(bCrouchingCover)
	{
		if(!ActorFiring())
		{
			OccupiedActor->SetStance(EALSStance::Crouching, true);
		} else
		{
			return;
		}
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
		Internal_ApplyEdgeTagToActor(true);
	} else if(OverlappedComp == RightCoverEdgeBox)
	{
		Internal_ApplyEdgeTagToActor(false);
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
	UGameplayTagUtils::AddTagToActor(OccupiedActor, TAG_COVER_ROLLEDOUT);
	Internal_StartCoverTransition();
}

void ABaseCoverPoint::Internal_StartPeekRollback()
{
	TargetCoverLocation = CachedTransform.GetLocation();
	TargetCoverRotation = CachedTransform.Rotator();
	UGameplayTagUtils::RemoveTagFromActor(OccupiedActor, TAG_COVER_ROLLEDOUT);
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

void ABaseCoverPoint::Internal_ApplyEdgeTagToActor(bool bLeftEdge)
{
	if(!OccupiedActor)
	{
		return;
	}
	
	CachedTransform = OccupiedActor->GetActorTransform();
	UGameplayTagUtils::AddTagToActor(OccupiedActor, bLeftEdge ? TAG_COVER_LEFTEDGE : TAG_COVER_RIGHTEDGE);
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

	if(UKismetMathLibrary::Vector_Distance(OccupiedActorTransform.GetLocation(), TargetCoverLocation) <= DistanceWhenCameraShakePlays && bShouldPlayCameraShake)
	{
		bShouldPlayCameraShake = false;
		UFeedbackUtils::TryPlayCameraShake(OccupiedActor, CoverHitCameraShake);
	}
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