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
	
	if(AActor* SourceActor = GetOwner(); bUseControllerRotation)
	{
		SourceActor->SetActorRotation(Internal_GetControllerAndActorBlendedRotation(SourceActor));
		const FRotator LookAt = UKismetMathLibrary::FindLookAtRotation(SourceActor->GetActorLocation(), SelectedActor->GetActorLocation());
		const FRotator SourceRot = SourceActor->GetActorRotation();
		TargetActorRotation.Pitch = SourceRot.Pitch;
		TargetActorRotation.Yaw = LookAt.Yaw;
		TargetActorRotation.Roll = SourceRot.Roll;
	}	
	TargetActorLocation = SelectedActor->GetActorLocation();
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
	if(!SourceActor)
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

		FVector StartTrace;
		FVector EndTrace;
		
		if(bUseControllerRotation)
		{
			StartTrace = SourceActor->GetActorLocation() + TraceOffset;
			FRotator FinalRot = Internal_GetControllerAndActorBlendedRotation(SourceActor);
			EndTrace = StartTrace + (FinalRot.Vector().RotateAngleAxis(YawFinal, FVector(0, 0, 1))) * ArcDistance;
		} else
		{
			FVector RotatedVector = SourceActor->GetActorForwardVector().RotateAngleAxis(YawFinal, FVector(0, 0, 1)) * ArcDistance;
			StartTrace = SourceActor->GetActorLocation() + TraceOffset;
			EndTrace = RotatedVector + StartTrace;
			StartTrace += TraceOffset;
		}

		TArray<TEnumAsByte <EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
		UKismetSystemLibrary::SphereTraceMultiForObjects(this, StartTrace, EndTrace, SweepRadius, ObjectTypes, false, { SourceActor }, bDrawDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None, TempHitResults, true);
		for (FHitResult HitResult : TempHitResults) {
			if(UCombatUtils::AreActorsEnemies(SourceActor, HitResult.GetActor()))
			{
				OutHitResults.Add(HitResult);
			}
		}
	}
	return Internal_FindBestTargetFromActors(OutHitResults);
}

AActor* ULockOnComponent::Internal_FindBestTargetFromActors(TArray<FHitResult> PotentialHitResults) const
{
	if(PotentialHitResults.Num() <= 0)
	{
		return nullptr;
	}
	FHitResult ClosestHit = PotentialHitResults[0];
	TArray<AActor*> HitActors;
	for(FHitResult TempHit : PotentialHitResults)
	{
		if(!UGameplayTagUtils::ActorHasGameplayTag(TempHit.GetActor(), TAG_STATE_DEAD))
		{
			HitActors.Add(TempHit.GetActor());
		}
		// if(TempHit.Distance < ClosestHit.Distance)
		// {
		// 	ClosestHit = TempHit;
		// }
	}
	//return ClosestHit.GetActor();
	return HitActors[0];
}

FRotator ULockOnComponent::Internal_GetControllerAndActorBlendedRotation(AActor* SourceActor) const
{
	APlayerController* PlayerController = nullptr;
	if(const APawn* PawnObj = Cast<APawn>(SourceActor))
	{
		PlayerController = Cast<APlayerController>(PawnObj->Controller);
	}

	if(!PlayerController)
	{
		return SourceActor->GetActorRotation();
	}
	
	const FRotator StartRot = SourceActor->GetActorRotation();		
	FVector TempLoc;
	FRotator TempRot;
	PlayerController->GetPlayerViewPoint(TempLoc, TempRot);		
	return FRotator(StartRot.Pitch, TempRot.Yaw, StartRot.Roll);
}
