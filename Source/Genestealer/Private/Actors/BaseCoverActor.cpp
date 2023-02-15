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
#include "Utils/WorldUtils.h"

ABaseCoverActor::ABaseCoverActor()
{
	CoverMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CoverMesh"));
	CoverMesh->SetCollisionResponseToChannel(GENESTEALER_TRACE_COVER_WALL, ECR_Block);
	CoverMesh->ComponentTags.Add(TAG_NAME_COVERMESH);
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
	InitCoverBox(LeftCoverPeekBox, FColor::Green, FVector( -77.5f, 500.f, 0.f), FVector(25.f, 25.f, 40.f));
	bLeftCoverEnabled = true;
	
	RightCoverPeekBox = CreateDefaultSubobject<UBoxComponent>(TEXT("RightCoverPeekBox"));
	RightCoverPeekBox->SetupAttachment(MiddleCoverWall);
	InitCoverBox(RightCoverPeekBox, FColor::Green, FVector( 77.5f, 500.f, 0.f), FVector(25.f, 25.f, 40.f));	
	bRightCoverEnabled = true;
	
	LeftCoverEdgeBox = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftCoverEdgeBox"));
	LeftCoverEdgeBox->SetupAttachment(MiddleCoverWall);
	InitCoverBox(LeftCoverEdgeBox, FColor::Red, FVector( -50.f, 200.f, 0.f), FVector(3.f, 15.f, 40.f));
	
	RightCoverEdgeBox = CreateDefaultSubobject<UBoxComponent>(TEXT("RightCoverEdgeBox"));
	RightCoverEdgeBox->SetupAttachment(MiddleCoverWall);
	InitCoverBox(RightCoverEdgeBox, FColor::Red, FVector( 50.f, 200.f, 0.f), FVector(3.f, 15.f, 40.f));

	LeftCoverRollbackBox = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftCoverRollbackBox"));
	LeftCoverRollbackBox->SetupAttachment(MiddleCoverWall);
	InitCoverBox(LeftCoverRollbackBox, FColor::Blue, FVector( -40.f, 300.f, 0.f), FVector(25.f, 25.f, 40.f));

	RightCoverRollbackBox = CreateDefaultSubobject<UBoxComponent>(TEXT("RightCoverRollbackBox"));
	RightCoverRollbackBox->SetupAttachment(MiddleCoverWall);
	InitCoverBox(RightCoverRollbackBox, FColor::Blue, FVector( 40.f, 300.f, 0.f), FVector(25.f, 25.f, 40.f));
	
	CoverTransitionTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("CoverTransitionTimeline"));
	
	DissolveMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("DissolveMesh"));
	InitDissolveMesh(DissolveMesh, FVector(0.f, 600.f, -60.f), FVector(1.f, 1.f, 1.f));
	LeftDissolveMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LeftDissolveMesh"));
	InitDissolveMesh(LeftDissolveMesh, FVector(-70.f, 600.f, -60.f), FVector(1.f, 1.f, 1.f));
	RightDissolveMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RightDissolveMesh"));
	InitDissolveMesh(RightDissolveMesh, FVector(70.f, 600.f, -60.f), FVector(1.f, 1.f, 1.f));
	
	DefaultGameplayTags.Add(TAG_ACTOR_COVER);
	CoverWallOffset = 40.f;
}

void ABaseCoverActor::InitCoverBox(UBoxComponent* InBox, FColor InBoxColor, FVector Offset, FVector BoxExtent)
{
	if(!InBox)
	{
		return;
	}
	
	InBox->ShapeColor = InBoxColor;
	InBox->AddWorldOffset(Offset);
	InBox->SetBoxExtent(BoxExtent);
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

void ABaseCoverActor::InitDissolveMesh(UMeshComponent* InDissolveMesh, FVector InOffset, FVector InScale)
{
	if(!InDissolveMesh)
	{
		return;
	}
	
	InDissolveMesh->SetupAttachment(MiddleCoverWall);
	InDissolveMesh->AddWorldOffset(InOffset);
	InDissolveMesh->SetUsingAbsoluteScale(true);
	InDissolveMesh->SetWorldScale3D(InScale);
}

void ABaseCoverActor::SwitchOutlineOnMesh(bool bShouldOutline)
{
	if(CoverMesh)
	{
		const int32 OutlineColorInt = UCombatUtils::GetOutlineInt(this);
		CoverMesh->SetRenderCustomDepth(bShouldOutline);
		CoverMesh->SetCustomDepthStencilValue(OutlineColorInt);
	}

	for(UStaticMeshComponent* AdditionalMesh : K2_GetAdditionalMeshesToOutline())
	{
		if(AdditionalMesh)
		{
			const int32 OutlineColorInt = UCombatUtils::GetOutlineInt(this);
			AdditionalMesh->SetRenderCustomDepth(bShouldOutline);
			AdditionalMesh->SetCustomDepthStencilValue(OutlineColorInt);
		}
	}

	if(!bShouldOutline)
	{
		if(MaterialDissolver)
		{
			MaterialDissolver->StartDissolveTimeline();
		}
		bShouldPlayCoverDissolve = true;
	} else
	{
		if(!bShouldPlayCoverDissolve)
		{
			return;
		}

		if(MaterialDissolver)
		{
			MaterialDissolver->StartAppearTimeline();
		}
	}
}

void ABaseCoverActor::InteractWithActor(AActor* InstigatingActor)
{
	
}

void ABaseCoverActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	MaterialDissolver = UWorldUtils::SpawnActorToWorld_Deferred<AMaterialDissolver>(this, MaterialDissolverClass, this);
	UWorldUtils::FinishSpawningActor_Deferred(MaterialDissolver, FTransform());
	if(MaterialDissolver)
	{
		MaterialDissolver->InitDissolveableMesh(DissolveMesh);
	}

	if(bLeftCoverEnabled && LeftMaterialDissolverClass)
	{
		LeftMaterialDissolver = UWorldUtils::SpawnActorToWorld_Deferred<AMaterialDissolver>(this, LeftMaterialDissolverClass, this);
		UWorldUtils::FinishSpawningActor_Deferred(LeftMaterialDissolver, FTransform());
		if(LeftMaterialDissolver)
		{
			LeftMaterialDissolver->InitDissolveableMesh(LeftDissolveMesh);
		}
	} else
	{
		if(LeftDissolveMesh)
		{
			LeftDissolveMesh->SetVisibility(false);
		}
	}

	if(bRightCoverEnabled && RightMaterialDissolverClass)
	{
		RightMaterialDissolver = UWorldUtils::SpawnActorToWorld_Deferred<AMaterialDissolver>(this, RightMaterialDissolverClass, this);
		UWorldUtils::FinishSpawningActor_Deferred(RightMaterialDissolver, FTransform());
		if(RightMaterialDissolver)
		{
			RightMaterialDissolver->InitDissolveableMesh(RightDissolveMesh);
		}
	} else
	{
		if(RightDissolveMesh)
		{
			RightDissolveMesh->SetVisibility(false);
		}
	}
}

void ABaseCoverActor::BeginPlay()
{
	Super::BeginPlay();

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

void ABaseCoverActor::OccupyMount(ABaseCharacter* InActor, const FVector& InTargetCoverLocation, const FVector& InHitNormal)
{
	if(OccupiedActor || !InActor)
	{
		return;
	}

	UGameplayStatics::PrimeSound(HitCoverFoley);
	
	OccupiedActor = InActor;
	if(bCrouchingCover)
	{
		Internal_SetCrouching();
	}
	
	Internal_SetCoverNormalRotationValues();
	Internal_SetWalkingGait();
	UGameplayTagUtils::AddTagToActor(OccupiedActor, TAG_STATE_IN_COVER);
	
	// TargetCoverLocation = InTargetCoverLocation - (UKismetMathLibrary::GetRightVector(MiddleCoverWall->K2_GetComponentRotation()) * (CoverWallOffset * -1.f));
	TargetCoverLocation = DissolveMesh->GetComponentLocation();
	TargetCoverRotation = UKismetMathLibrary::MakeRotFromZX(UKismetMathLibrary::Vector_Up(), InHitNormal * -1);
	
	Internal_ActivateOverlapBoxes(true);	
	Internal_StartCoverTransition();

	if(MaterialDissolver)
	{
		MaterialDissolver->ResetDissolveState(true);
	}

	if(LeftMaterialDissolver && bLeftCoverEnabled)
	{
		LeftMaterialDissolver->StartAppearTimeline();
	}

	if(RightMaterialDissolver&& bRightCoverEnabled)
	{
		RightMaterialDissolver->StartAppearTimeline();
	}
}

void ABaseCoverActor::VacateMount(ABaseCharacter* InActor)
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
		TAG_COVER_LEFTPEEK, TAG_COVER_RIGHTPEEK, TAG_COVER_ROLLEDOUT});
	if(IAnimatable* AnimatableOwner = Internal_GetAnimatableOwner())
	{
		AnimatableOwner->SetCameraOverRightShoulder(true);
	}
	Internal_ResetCharacterValuesOnCoverExit();
	GetWorldTimerManager().ClearTimer(TimerHandle_StartFiringDelay);
	OccupiedActor = nullptr;
	Internal_ActivateOverlapBoxes(false);

	if(LeftMaterialDissolver)
	{
		LeftMaterialDissolver->StartDissolveTimeline();
	}

	if(RightMaterialDissolver)
	{
		RightMaterialDissolver->StartDissolveTimeline();
	}
}

void ABaseCoverActor::StartMountedFire()
{
	if(!OccupiedActor || !OccupiedActor->GetInventoryComponent())
	{
		return;
	}

	if(OccupiedActor->GetInventoryComponent()->GetCurrentWeaponType() == EWeaponType::Melee)
	{
		return;
	}

	if(LeftMaterialDissolver)
	{
		LeftMaterialDissolver->StartDissolveTimeline();
	}

	if(RightMaterialDissolver)
	{
		RightMaterialDissolver->StartDissolveTimeline();
	}

	if(ActorInLeftEdge() && bLeftCoverEnabled)
	{
		if(ActorInLeftPeek())
		{
			OccupiedActor->GetInventoryComponent()->StartFiring();
		} else
		{
			Internal_TryPeekRolloutAndFire(LeftCoverPeekBox, false);	
		}
	} else if(ActorInRightEdge() && bRightCoverEnabled)
	{
		if(ActorInRightPeek())
		{
			OccupiedActor->GetInventoryComponent()->StartFiring();
		} else
		{
			Internal_TryPeekRolloutAndFire(RightCoverPeekBox, true);	
		}
	} else if (bMiddleCoverEnabled && bCrouchingCover)
	{
		if(ActorAiming())
		{
			OccupiedActor->GetInventoryComponent()->StartFiring();
		} else
		{
			if(bCrouchingCover)
			{
				Internal_SetStanding();
			}
			Internal_SetCoverAimingRotationValues(true);
			GetWorldTimerManager().SetTimer(TimerHandle_StartFiringDelay, OccupiedActor->GetInventoryComponent(), &UInventoryComponent::StartFiring, DelayBeforeCrouchToStandShoot, false);
		}
	}
}

void ABaseCoverActor::StopMountedFire()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_StartFiringDelay);
	if(!OccupiedActor || !OccupiedActor->GetInventoryComponent())
	{
		return;
	}

	if(!ActorInLeftEdge())
	{
		if(LeftMaterialDissolver)
		{
			LeftMaterialDissolver->StartAppearTimeline();
		}
	}

	if(!ActorInRightEdge())
	{
		if(RightMaterialDissolver)
		{
			RightMaterialDissolver->StartAppearTimeline();
		}
	}
	
	
	if((ActorInLeftEdge() && bLeftCoverEnabled) || (ActorInRightEdge() && bRightCoverEnabled))
	{
		if(!ActorAiming())
		{
			if(bCrouchingCover)
			{
				Internal_SetCrouching();
			}
			Internal_StartPeekRollback();
		}		
		Internal_StopPeekFire();
	} else
	{
		if(!ActorAiming())
		{
			if(bCrouchingCover)
			{
				Internal_SetCrouching();
			}
			Internal_SetCoverNormalRotationValues();
		}
		OccupiedActor->GetInventoryComponent()->StopFiring();
	}
}

void ABaseCoverActor::StartMountedAim()
{
	if(!OccupiedActor)
	{
		return;
	}

	if(OccupiedActor->GetInventoryComponent()->GetCurrentWeaponType() == EWeaponType::Melee)
	{
		return;
	}

	if(LeftMaterialDissolver)
	{
		LeftMaterialDissolver->StartDissolveTimeline();
	}

	if(RightMaterialDissolver)
	{
		RightMaterialDissolver->StartDissolveTimeline();
	}

	if(ActorInLeftEdge() && !UGameplayTagUtils::ActorHasGameplayTag(this, TAG_COVER_ROLLEDOUT) && bLeftCoverEnabled)
	{
		Internal_StartPeekRollout(LeftCoverPeekBox, false);
		Internal_SetAimingMode();
	} else if(ActorInRightEdge() && !UGameplayTagUtils::ActorHasGameplayTag(this, TAG_COVER_ROLLEDOUT) && bRightCoverEnabled)
	{
		Internal_StartPeekRollout(RightCoverPeekBox, true);
		Internal_SetAimingMode();
	} else if(bCrouchingCover && bMiddleCoverEnabled)
	{
		Internal_SetStanding();
		Internal_SetAimingMode();
	}
}

void ABaseCoverActor::StopMountedAim()
{
	if(!OccupiedActor)
	{
		return;
	}

	if(!ActorInLeftEdge())
	{
		if(LeftMaterialDissolver)
		{
			LeftMaterialDissolver->StartAppearTimeline();
		}
	}

	if(!ActorInRightEdge())
	{
		if(RightMaterialDissolver)
		{
			RightMaterialDissolver->StartAppearTimeline();
		}
	}
	
	if(ActorInLeftEdge() || ActorInRightEdge())
	{
		Internal_SetCrouching();
		Internal_StartPeekRollback();
	} else if(bCrouchingCover)
	{
		if(!ActorFiring())
		{
			Internal_SetCrouching();
		} else
		{
			return;
		}
	}
	Internal_SetVelocityMode();
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
		if(LeftMaterialDissolver)
		{
			LeftMaterialDissolver->StartDissolveTimeline();
		}
		Internal_ApplyEdgeTagToActor(true);
	} else if(OverlappedComp == RightCoverEdgeBox)
	{
		if(RightMaterialDissolver)
		{
			RightMaterialDissolver->StartDissolveTimeline();
		}
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
			if(LeftMaterialDissolver)
			{
				LeftMaterialDissolver->StartAppearTimeline();
			}
			UGameplayTagUtils::RemoveTagFromActor(OtherActor, TAG_COVER_LEFTEDGE);	
		}
	} else if(OverlappedComp == RightCoverEdgeBox)
	{
		if(!UGameplayTagUtils::ActorHasGameplayTag(OccupiedActor, TAG_COVER_ROLLEDOUT))
		{
			if(RightMaterialDissolver)
			{
				RightMaterialDissolver->StartAppearTimeline();
			}
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
		TargetCoverLocation = LeftCoverRollbackBox->GetComponentLocation();
	} else if(UGameplayTagUtils::ActorHasGameplayTag(OccupiedActor, TAG_COVER_RIGHTPEEK) && RightCoverRollbackBox)
	{
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

void ABaseCoverActor::Internal_SetLookingMode() const
{
	IAnimatable* AnimatableOwner = Internal_GetAnimatableOwner();
	if(!AnimatableOwner)
	{
		return;
	}
	AnimatableOwner->SetLookingMode();
}

void ABaseCoverActor::Internal_SetVelocityMode() const
{
	IAnimatable* AnimatableOwner = Internal_GetAnimatableOwner();
	if(!AnimatableOwner)
	{
		return;
	}
	AnimatableOwner->SetVelocityMode();
}

void ABaseCoverActor::Internal_SetAimingMode() const
{
	IAnimatable* AnimatableOwner = Internal_GetAnimatableOwner();
	if(!AnimatableOwner)
	{
		return;
	}
	AnimatableOwner->SetAimingMode();
}

void ABaseCoverActor::Internal_SetWalkingGait() const
{
	IAnimatable* AnimatableOwner = Internal_GetAnimatableOwner();
	if(!AnimatableOwner)
	{
		return;
	}
	AnimatableOwner->SetWalkingGait();
}

void ABaseCoverActor::Internal_SetRunningGait() const
{
	IAnimatable* AnimatableOwner = Internal_GetAnimatableOwner();
	if(!AnimatableOwner)
	{
		return;
	}
	AnimatableOwner->SetRunningGait();
}

void ABaseCoverActor::Internal_SetStanding() const
{
	IAnimatable* AnimatableOwner = Internal_GetAnimatableOwner();
	if(!AnimatableOwner)
	{
		return;
	}
	AnimatableOwner->SetStanding();
}

void ABaseCoverActor::Internal_SetCrouching() const
{
	IAnimatable* AnimatableOwner = Internal_GetAnimatableOwner();
	if(!AnimatableOwner)
	{
		return;
	}
	AnimatableOwner->SetCrouching();
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

void ABaseCoverActor::Internal_ResetCharacterValuesOnCoverExit() const
{
	if(bCrouchingCover)
	{
		Internal_SetStanding();
	}
	Internal_SetLookingMode();
	Internal_SetRunningGait();
}

void ABaseCoverActor::Internal_SetCoverNormalRotationValues() const
{
	if(bCrouchingCover)
	{
		Internal_SetCrouching();
	} else
	{
		Internal_SetStanding();
	}

	IAnimatable* AnimatableOwner = Internal_GetAnimatableOwner();
	if(!AnimatableOwner)
	{
		return;
	}
	AnimatableOwner->SetCameraOverRightShoulder(true);
	Internal_SetVelocityMode();
}

void ABaseCoverActor::Internal_SetCoverAimingRotationValues(bool bRightShoulder) const
{
	IAnimatable* AnimatableOwner = Internal_GetAnimatableOwner();
	if(!AnimatableOwner)
	{
		return;
	}
	AnimatableOwner->SetCameraOverRightShoulder(bRightShoulder);
	Internal_SetAimingMode();
}

IAnimatable* ABaseCoverActor::Internal_GetAnimatableOwner() const
{
	if(!OccupiedActor)
	{
		return nullptr;
	}

	return Cast<IAnimatable>(OccupiedActor);
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