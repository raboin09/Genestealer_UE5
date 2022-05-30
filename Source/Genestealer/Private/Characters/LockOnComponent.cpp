// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/LockOnComponent.h"

#include "Core/BasePlayerController.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Utils/CombatUtils.h"

ULockOnComponent::ULockOnComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	LockOnInterpTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("LockOnInterpTimeline"));
}

void ULockOnComponent::BeginPlay()
{
	Super::BeginPlay();
	if(!LockOnInterpTimeline)
	{
		return;
	}
	FOnTimelineFloat CoverLerpFunction;
	CoverLerpFunction.BindDynamic(this, &ULockOnComponent::Internal_CoverTransitionUpdate);
	LockOnInterpTimeline->AddInterpFloat(LockOnTransitionCurve, CoverLerpFunction);
	LockOnInterpTimeline->SetLooping(false);

	FOnTimelineEvent CoverLerpFinishedEvent;
	CoverLerpFinishedEvent.BindDynamic(this, &ULockOnComponent::Internal_CoverTransitionFinished);
	LockOnInterpTimeline->SetTimelineFinishedFunc(CoverLerpFinishedEvent);
}

void ULockOnComponent::InterpToBestTargetForMeleeAttack()
{
	SelectedActor = Internal_TraceForTarget();
	if(!SelectedActor)
	{
		return;
	}
	TargetActorLocation = SelectedActor->GetActorLocation();
	TargetActorRotation = GetOwner()->GetActorRotation();
	Internal_StartInterpTransition();
}

void ULockOnComponent::Internal_StartInterpTransition()
{
	if(!SelectedActor || !LockOnInterpTimeline)
	{
		return;
	}
	LockOnInterpTimeline->PlayFromStart();
}

void ULockOnComponent::Internal_CoverTransitionUpdate(float Alpha)
{
	if(!SelectedActor)
	{
		return;
	}

	const FTransform& SelectedActorTransform = GetOwner()->GetActorTransform();
	FTransform TargetTransform;
	TargetTransform.SetLocation(FVector(TargetActorLocation.X, TargetActorLocation.Y, SelectedActorTransform.GetLocation().Z));
	TargetTransform.SetRotation(FQuat(TargetActorRotation));
	const FTransform& NewActorTransform = UKismetMathLibrary::TLerp(SelectedActorTransform, TargetTransform, Alpha, ELerpInterpolationMode::QuatInterp);
	GetOwner()->SetActorLocationAndRotation(NewActorTransform.GetTranslation(), NewActorTransform.GetRotation(), true);
}

void ULockOnComponent::Internal_CoverTransitionFinished()
{
	SelectedActor = nullptr;
}

AActor* ULockOnComponent::Internal_TraceForTarget() const
{
	AActor* SourceActor = GetOwner();
	APlayerController* PlayerController = nullptr;
	if(APawn* PawnObj = Cast<APawn>(SourceActor))
	{
		PlayerController = Cast<APlayerController>(PawnObj->Controller);
	}

	if(!PlayerController)
	{
		return nullptr;
	}
	
	TArray<FHitResult> TempHitResults;
	TArray<FHitResult> OutHitResults;
	float DegreeIncrements = ConeTraceArcWidth / UKismetMathLibrary::Max(1.f, NumCones);
	float YawValueFirst = ConeTraceArcWidth * -.5;
	TArray<AActor*> AlreadyHitActors;
	for (int i = 0; i < NumCones; i++) {
		float YawFinal = YawValueFirst + (DegreeIncrements * i);
		FVector RotatedVector = SourceActor->GetActorForwardVector().RotateAngleAxis(YawFinal, FVector(0, 0, 1)) * ArcDistance;
		FVector CharOwnerLocation = SourceActor->GetActorLocation() + TraceOffset;
		CharOwnerLocation += TraceOffset;
		FVector FinalVectorInput = RotatedVector + CharOwnerLocation;
		TArray<TEnumAsByte <EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
		UKismetSystemLibrary::SphereTraceMultiForObjects(this, CharOwnerLocation, FinalVectorInput, SweepRadius, ObjectTypes, false, { SourceActor }, bDrawDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None, TempHitResults, true);
		for (FHitResult HitResult : TempHitResults) {
			if(UCombatUtils::AreActorsEnemies(SourceActor, HitResult.GetActor()))
			{
				OutHitResults.Add(HitResult);
			}
		}
	}
	return Internal_FindBestTargetFromActors(OutHitResults);
}

AActor* ULockOnComponent::Internal_FindBestTargetFromActors(const TArray<FHitResult> PotentialHitResults) const
{
	if(PotentialHitResults.Num() <= 0)
	{
		return nullptr;
	}

	return Cast<AActor>(PotentialHitResults[0].GetActor());
}