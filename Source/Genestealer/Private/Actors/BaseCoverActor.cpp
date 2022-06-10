// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/BaseCoverActor.h"

#include "Characters/BaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Genestealer/Genestealer.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "NavAreas/NavArea_Obstacle.h"
#include "Sound/SoundCue.h"
#include "Utils/CombatUtils.h"
#include "Utils/CoreUtils.h"
#include "Utils/FeedbackUtils.h"
#include "Utils/GameplayTagUtils.h"
#include "Utils/SpawnUtils.h"

ABaseCoverActor::ABaseCoverActor()
{
	CoverMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CoverMesh"));
	SetRootComponent(CoverMesh);
	
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
	LeftCoverPeekBox->SetupAttachment(MiddleCoverWall);
	InitCoverBox(LeftCoverPeekBox);
	bLeftCoverEnabled = true;
	
	RightCoverPeekBox = CreateDefaultSubobject<UBoxComponent>(TEXT("RightCoverPeekBox"));
	RightCoverPeekBox->SetupAttachment(MiddleCoverWall);
	InitCoverBox(RightCoverPeekBox);	
	bRightCoverEnabled = true;
	
	LeftCoverEdgeBox = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftCoverEdgeBox"));
	LeftCoverEdgeBox->SetupAttachment(MiddleCoverWall);
	InitCoverBox(LeftCoverEdgeBox);
	
	RightCoverEdgeBox = CreateDefaultSubobject<UBoxComponent>(TEXT("RightCoverEdgeBox"));
	RightCoverEdgeBox->SetupAttachment(MiddleCoverWall);
	InitCoverBox(RightCoverEdgeBox);

	LeftCoverRollbackBox = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftCoverRollbackBox"));
	LeftCoverRollbackBox->SetupAttachment(MiddleCoverWall);
	InitCoverBox(LeftCoverRollbackBox);

	RightCoverRollbackBox = CreateDefaultSubobject<UBoxComponent>(TEXT("RightCoverRollbackBox"));
	RightCoverRollbackBox->SetupAttachment(MiddleCoverWall);
	InitCoverBox(RightCoverRollbackBox);
	
	CoverTransitionTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("CoverTransitionTimeline"));
	
	DissolveMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("DissolveMesh"));
	DissolveMesh->SetupAttachment(MiddleCoverWall);
	DissolveMesh->AddWorldOffset(FVector(0.f, 600.f, -60.f));
	DissolveMesh->SetUsingAbsoluteScale(true);
	DissolveMesh->SetWorldScale3D(FVector(1.f, 1.f, 1.f));
	
	DefaultGameplayTags.Add(TAG_ACTOR_COVER);
	CoverWallOffset = 40.f;
}

void ABaseCoverActor::InitCoverBox(UBoxComponent* InBox)
{
	if(!InBox)
	{
		return;
	}
	
	if(InBox == LeftCoverPeekBox)
	{
		InBox->ShapeColor = FColor::Green;
		InBox->AddWorldOffset(FVector( -77.5f, 500.f, 0.f));
		InBox->SetBoxExtent(FVector(25.f, 25.f, 40.f));
	} else if(InBox == RightCoverPeekBox)
	{
		InBox->ShapeColor = FColor::Green;
		InBox->AddWorldOffset(FVector( 77.5f, 500.f, 0.f));
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
	} else if(InBox == LeftCoverRollbackBox)
	{
		InBox->ShapeColor = FColor::Blue;
		InBox->SetBoxExtent(FVector(25.f, 25.f, 40.f));
		InBox->AddWorldOffset(FVector( -40.f, 300.f, 0.f));
	} else if(InBox == RightCoverRollbackBox)
	{
		InBox->ShapeColor = FColor::Blue;
		InBox->SetBoxExtent(FVector(25.f, 25.f, 40.f));
		InBox->AddWorldOffset(FVector( 40.f, 300.f, 0.f));
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

void ABaseCoverActor::SwitchOutlineOnMesh(bool bShouldOutline)
{
	if(CoverMesh)
	{
		const int32 OutlineColorInt = UCombatUtils::GetOutlineIntFromColor(EOutlineColor::Gray);
		CoverMesh->SetRenderCustomDepth(bShouldOutline);
		CoverMesh->SetCustomDepthStencilValue(OutlineColorInt);
	}

	if(!MaterialDissolver)
	{
		return;
	}

	if(!bShouldOutline)
	{
		MaterialDissolver->StartDissolveTimeline();
		bShouldPlayCoverDissolve = true;
	} else if(bShouldPlayCoverDissolve && bShouldOutline)
	{
		bShouldPlayCoverDissolve = false;
		MaterialDissolver->StartAppearTimeline();
	}
}

void ABaseCoverActor::InteractWithActor(AActor* InstigatingActor)
{
	
}

void ABaseCoverActor::BeginPlay()
{
	Super::BeginPlay();
	MaterialDissolver = USpawnUtils::SpawnActorToWorld_Deferred<AMaterialDissolver>(this, MaterialDissolverClass, this);
	USpawnUtils::FinishSpawningActor_Deferred(MaterialDissolver, FTransform());
	if(MaterialDissolver)
	{
		MaterialDissolver->InitDissolveableMesh(DissolveMesh);
	}

	LeftCoverPeekBox->OnComponentBeginOverlap.AddDynamic(this, &ABaseCoverActor::ActorBeginOverlap);
	LeftCoverPeekBox->OnComponentEndOverlap.AddDynamic(this, &ABaseCoverActor::ActorEndOverlap);
	
	RightCoverPeekBox->OnComponentBeginOverlap.AddDynamic(this, &ABaseCoverActor::ActorBeginOverlap);
	RightCoverPeekBox->OnComponentEndOverlap.AddDynamic(this, &ABaseCoverActor::ActorEndOverlap);

	LeftCoverEdgeBox->OnComponentBeginOverlap.AddDynamic(this, &ABaseCoverActor::ActorBeginOverlap);
	LeftCoverEdgeBox->OnComponentEndOverlap.AddDynamic(this, &ABaseCoverActor::ActorEndOverlap);
	
	RightCoverEdgeBox->OnComponentBeginOverlap.AddDynamic(this, &ABaseCoverActor::ActorBeginOverlap);
	RightCoverEdgeBox->OnComponentEndOverlap.AddDynamic(this, &ABaseCoverActor::ActorEndOverlap);
	
	FOnTimelineFloat CoverLerpFunction;
	CoverLerpFunction.BindDynamic(this, &ABaseCoverActor::Internal_CoverTransitionUpdate);
	CoverTransitionTimeline->AddInterpFloat(CoverTransitionCurve, CoverLerpFunction);
	CoverTransitionTimeline->SetLooping(false);

	FOnTimelineEvent CoverLerpFinishedEvent;
	CoverLerpFinishedEvent.BindDynamic(this, &ABaseCoverActor::Internal_CoverTransitionFinished);
	CoverTransitionTimeline->SetTimelineFinishedFunc(CoverLerpFinishedEvent);

	bShouldPlayCoverDissolve = true;
}

void ABaseCoverActor::OccupyCover(ABaseCharacter* InActor, const FVector& InTargetCoverLocation, const FVector& InHitNormal)
{
	if(OccupiedActor || !InActor)
	{
		return;
	}
	
	OccupiedActor = InActor;
	if(bCrouchingCover)
	{
		Internal_AdjustStance(true);
	}
	
	Internal_SetCoverNormalRotationValues();
	OccupiedActor->SetDesiredGait(EALSGait::Walking);
	UGameplayTagUtils::AddTagToActor(OccupiedActor, TAG_STATE_IN_COVER);
	
	TargetCoverLocation = InTargetCoverLocation - (UKismetMathLibrary::GetRightVector(MiddleCoverWall->K2_GetComponentRotation()) * (CoverWallOffset * -1.f));
	TargetCoverRotation = UKismetMathLibrary::MakeRotFromZX(UKismetMathLibrary::Vector_Up(), InHitNormal * -1);
	
	Internal_ActivateOverlapBoxes(true);	
	Internal_StartCoverTransition();

	if(MaterialDissolver)
	{
		MaterialDissolver->ResetDissolveState(true);
	}
}

void ABaseCoverActor::VacateCover(ABaseCharacter* InActor)
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

void ABaseCoverActor::StartCoverFire()
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
				Internal_AdjustStance(false);
			}
			Internal_SetCoverAimingRotationValues(true);
			GetWorldTimerManager().SetTimer(TimerHandle_StartFiringDelay, OccupiedActor->GetInventoryComponent(), &UInventoryComponent::StartFiring, DelayBeforeCrouchToStandShoot, false);
		}
	}
}

void ABaseCoverActor::StopCoverFire()
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
		
		if(bCrouchingCover)
		{
			Internal_AdjustStance(true);
		}
		
		Internal_StopPeekFire();
	} else
	{
		if(!ActorAiming())
		{
			if(bCrouchingCover)
			{
				Internal_AdjustStance(true);
			}
			Internal_SetCoverNormalRotationValues();
		}
		OccupiedActor->GetInventoryComponent()->StopFiring();
	}
}

void ABaseCoverActor::StartCoverAim()
{
	if(!OccupiedActor)
	{
		return;
	}

	if(ActorInLeftEdge() && !UGameplayTagUtils::ActorHasGameplayTag(this, TAG_COVER_ROLLEDOUT))
	{
		Internal_StartPeekRollout(LeftCoverPeekBox, false);
	} else if(ActorInRightEdge() && !UGameplayTagUtils::ActorHasGameplayTag(this, TAG_COVER_ROLLEDOUT))
	{
		Internal_StartPeekRollout(RightCoverPeekBox, true);
	} else if(bCrouchingCover)
	{
		Internal_AdjustStance(false);
	}
	OccupiedActor->SetRotationMode(EALSRotationMode::Aiming);
}

void ABaseCoverActor::StopCoverAim()
{
	if(!OccupiedActor)
	{
		return;
	}
	
	if(ActorInLeftEdge() || ActorInRightEdge())
	{
		Internal_AdjustStance(true);
		Internal_StartPeekRollback();
	} else if(bCrouchingCover)
	{
		if(!ActorFiring())
		{
			Internal_AdjustStance(true);
		} else
		{
			return;
		}
	}
	OccupiedActor->SetRotationMode(EALSRotationMode::VelocityDirection);
}

void ABaseCoverActor::ActorBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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

void ABaseCoverActor::ActorEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
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
		if(!UGameplayTagUtils::ActorHasGameplayTag(OccupiedActor, TAG_COVER_ROLLEDOUT))
		{
			UGameplayTagUtils::RemoveTagFromActor(OtherActor, TAG_COVER_LEFTEDGE);	
		}
	} else if(OverlappedComp == RightCoverEdgeBox)
	{
		if(!UGameplayTagUtils::ActorHasGameplayTag(OccupiedActor, TAG_COVER_ROLLEDOUT))
		{
			UGameplayTagUtils::RemoveTagFromActor(OtherActor, TAG_COVER_RIGHTEDGE);	
		}
	}
}

void ABaseCoverActor::Internal_StartPeekRollout(const UShapeComponent* TargetPeekBox, bool bRightCameraShoulder)
{
	if(!CoverTransitionTimeline)
	{
		return;
	}
	
	Internal_SetCoverAimingRotationValues(bRightCameraShoulder);
	TargetCoverLocation = TargetPeekBox->GetComponentLocation();
	UGameplayTagUtils::AddTagToActor(OccupiedActor, TAG_COVER_ROLLEDOUT);
	Internal_StartCoverTransition();
}

void ABaseCoverActor::Internal_StartPeekRollback()
{
	if(UGameplayTagUtils::ActorHasGameplayTag(OccupiedActor, TAG_COVER_LEFTPEEK) && LeftCoverRollbackBox)
	{
		UKismetSystemLibrary::PrintString(this, "L Rollback");
		TargetCoverLocation = LeftCoverRollbackBox->GetComponentLocation();
	} else if(UGameplayTagUtils::ActorHasGameplayTag(OccupiedActor, TAG_COVER_RIGHTPEEK) && RightCoverRollbackBox)
	{
		UKismetSystemLibrary::PrintString(this, "R Rollback");
		TargetCoverLocation = RightCoverRollbackBox->GetComponentLocation();
	} else
	{
		TargetCoverLocation = CachedTransform.GetLocation();
	}
	TargetCoverRotation = CachedTransform.Rotator();
	UGameplayTagUtils::RemoveTagFromActor(OccupiedActor, TAG_COVER_ROLLEDOUT);
	Internal_SetCoverNormalRotationValues();
	Internal_StartCoverTransition();
}

void ABaseCoverActor::Internal_StartPeekFire()
{
	if(!OccupiedActor || !OccupiedActor->GetInventoryComponent())
	{
		return;
	}
	GetWorldTimerManager().SetTimer(TimerHandle_StartFiringDelay, OccupiedActor->GetInventoryComponent(), &UInventoryComponent::StartFiring, DelayBeforePeekShoot, false);
}

void ABaseCoverActor::Internal_StopPeekFire() const
{
	if(!OccupiedActor || !OccupiedActor->GetInventoryComponent())
	{
		return;
	}
	OccupiedActor->GetInventoryComponent()->StopFiring();
}

void ABaseCoverActor::Internal_HandlePeekCoverOverlap(bool bLeftCoverPoint, AActor* OtherActor)
{	
	if(bLeftCoverPoint)
	{
		UGameplayTagUtils::AddTagToActor(OtherActor, TAG_COVER_LEFTPEEK);
	} else
	{
		UGameplayTagUtils::AddTagToActor(OtherActor, TAG_COVER_RIGHTPEEK);
	}
}

void ABaseCoverActor::Internal_HandlePeekCoverOverlapEnd(bool bLeftCoverPoint, AActor* OtherActor)
{
	if(bLeftCoverPoint)
	{
		UGameplayTagUtils::RemoveTagFromActor(OtherActor, TAG_COVER_LEFTPEEK);
	} else
	{
		UGameplayTagUtils::RemoveTagFromActor(OtherActor, TAG_COVER_RIGHTPEEK);
	}
}

void ABaseCoverActor::Internal_ActivateOverlapBoxes(bool bActivate) const
{
	LeftCoverPeekBox->SetGenerateOverlapEvents(bActivate);
	RightCoverPeekBox->SetGenerateOverlapEvents(bActivate);
	LeftCoverEdgeBox->SetGenerateOverlapEvents(bActivate);
	RightCoverEdgeBox->SetGenerateOverlapEvents(bActivate);
}

void ABaseCoverActor::Internal_ApplyEdgeTagToActor(bool bLeftEdge)
{
	if(!OccupiedActor)
	{
		return;
	}
	
	CachedTransform = OccupiedActor->GetActorTransform();
	UGameplayTagUtils::AddTagToActor(OccupiedActor, bLeftEdge ? TAG_COVER_LEFTEDGE : TAG_COVER_RIGHTEDGE);
}

void ABaseCoverActor::Internal_AdjustStance(bool bNewStanceCrouching) const
{
	if(!OccupiedActor)
	{
		return;
	}
	
	if(bNewStanceCrouching)
	{
		OccupiedActor->SetStance(EALSStance::Crouching, true);
	} else
	{
		OccupiedActor->SetStance(EALSStance::Standing, true);
	}
}

void ABaseCoverActor::Internal_ResetCharacterValuesOnCoverExit() const
{
	if(bCrouchingCover)
	{
		Internal_AdjustStance(false);
	}
	OccupiedActor->SetRotationMode(EALSRotationMode::LookingDirection, true, true);
	OccupiedActor->SetDesiredGait(EALSGait::Running);
}

void ABaseCoverActor::Internal_SetCoverNormalRotationValues() const
{
	if(!OccupiedActor)
	{
		return;
	}

	if(bCrouchingCover)
	{
		Internal_AdjustStance(true);
	}
	
	OccupiedActor->SetRightShoulder(true);
	OccupiedActor->SetRotationMode(EALSRotationMode::VelocityDirection, true, true);
}

void ABaseCoverActor::Internal_SetCoverAimingRotationValues(bool bRightShoulder) const
{
	if(!OccupiedActor)
	{
		return;
	}
	OccupiedActor->SetRightShoulder(bRightShoulder);
	OccupiedActor->SetRotationMode(EALSRotationMode::Aiming, true, true);
}

void ABaseCoverActor::Internal_TryPeekRolloutAndFire(const UShapeComponent* TargetPeekBox, bool bRightCameraShoulder)
{
	if(!ActorAiming())
	{
		Internal_StartPeekRollout(TargetPeekBox, bRightCameraShoulder);	
	}
	Internal_StartPeekFire();
}

void ABaseCoverActor::Internal_StartCoverTransition()
{
	if(!OccupiedActor)
	{
		return;
	}

	const float CharToCoverDistance = UKismetMathLibrary::Vector_Distance(OccupiedActor->GetActorLocation(), TargetCoverLocation);
	const float MaxDistance = UCoreUtils::GetCoverPointValidDistance();
	
	if(CharToCoverDistance <= MinDistanceToPlayCameraShake)
	{
		bShouldPlayCoverHitFeedback = false;
	} else
	{
		bShouldPlayCoverHitFeedback = true;
		CameraShakeScale = UKismetMathLibrary::MapRangeClamped(CharToCoverDistance, MinDistanceToPlayCameraShake, MaxDistance, .1f, 1.f);
	}
	
	CoverTransitionTimeline->PlayFromStart();
}

void ABaseCoverActor::Internal_CoverTransitionUpdate(float Alpha)
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

	if(UKismetMathLibrary::Vector_Distance(OccupiedActorTransform.GetLocation(), TargetCoverLocation) <= DistanceWhenCameraShakePlays && bShouldPlayCoverHitFeedback)
	{
		bShouldPlayCoverHitFeedback = false;
		UFeedbackUtils::TryPlayCameraShake(OccupiedActor, CoverHitCameraShake, CameraShakeScale);
		UAudioManager::SpawnSoundAtLocation(this, HitCoverFoley, TargetCoverLocation);
	}
}

void ABaseCoverActor::Internal_CoverTransitionFinished()
{
	if(!OccupiedActor)
	{
		return;
	}
	
	if(UCharacterMovementComponent* CharMoveComp = OccupiedActor->GetCharacterMovement())
	{
		CharMoveComp->SetPlaneConstraintFromVectors(MiddleCoverWall->GetForwardVector(), MiddleCoverWall->GetUpVector());
		CharMoveComp->SetPlaneConstraintEnabled(true);
	}
}