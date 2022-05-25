// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/BaseActor.h"
#include "API/CoverPoint.h"
#include "Components/BoxComponent.h"
#include "Components/TimelineComponent.h"
#include "Utils/GameplayTagUtils.h"
#include "BaseCoverPoint.generated.h"

/**
 * 
 */
UCLASS()
class GENESTEALER_API ABaseCoverPoint : public ABaseActor, public ICoverPoint
{
	GENERATED_BODY()

public:
	ABaseCoverPoint();
	
	virtual void OccupyCover(ABaseCharacter* InActor, const FVector& InTargetCoverLocation, const FVector& InHitNormal) override;
	virtual void VacateCover(ABaseCharacter* InActor) override;

	virtual void StartCoverFire() override;
	virtual void StopCoverFire() override;
	virtual void StartCoverAim() override;
	virtual void StopCoverAim() override;
protected:
	virtual void BeginPlay() override;
	UFUNCTION()
	virtual void ActorBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void ActorEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Genestealer")
	float CoverWallOffset;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Genestealer")
	float DelayBeforeTagsApply = .1f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Genestealer")
	float DelayBeforePeekShoot = .2f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Genestealer")
	float DelayBeforeCrouchToStandShoot = .4f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Genestealer")
	bool bCrouchingCover;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Genestealer")
	UStaticMeshComponent* MiddleCoverWall;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Genestealer")
	bool bMiddleCoverEnabled;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Genestealer")
	UBoxComponent* LeftCoverEdgeBox;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Genestealer")
	UBoxComponent* LeftCoverPeekBox;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Genestealer")
	bool bLeftCoverEnabled;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Genestealer")
	UBoxComponent* RightCoverEdgeBox;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Genestealer")
	UBoxComponent* RightCoverPeekBox;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Genestealer")
	bool bRightCoverEnabled;

private:
	void Internal_StartPeekFire();
	void Internal_StopPeekFire() const;
	
	static void Internal_HandlePeekCoverOverlap(bool bLeftCoverPoint, AActor* OtherActor);
	static void Internal_HandlePeekCoverOverlapEnd(bool bLeftCoverPoint, AActor* OtherActor);
	void Internal_ActivateOverlapBoxes(bool bActivate) const;
	void Internal_ApplyLeftEdgeTagToActor();
	void Internal_ApplyRightEdgeTagToActor();

	void Internal_ResetCharacterValuesOnCoverExit() const;
	void Internal_SetCoverNormalRotationValues() const;
	void Internal_SetCoverAimingRotationValues(bool bRightShoulder) const;

	void Internal_TryPeekRolloutAndFire(const UShapeComponent* TargetPeekBox, bool bRightCameraShoulder);
	void Internal_StartPeekRollout(const UShapeComponent* TargetPeekBox, bool bRightCameraShoulder);
	void Internal_StartPeekRollback();

	void Internal_StartCoverTransition() const;
	UFUNCTION()
	void Internal_CoverTransitionUpdate(float Alpha);
	UFUNCTION()
	void Internal_CoverTransitionFinished();

	bool ActorInLeftEdge() const;
	bool ActorInLeftPeek() const;
	bool ActorInRightEdge() const;
	bool ActorInRightPeek() const;
	bool ActorAiming() const;
	
	UPROPERTY()
	ABaseCharacter* OccupiedActor;

	UPROPERTY(EditDefaultsOnly, Category="Genestealer")
	UCurveFloat* CoverTransitionCurve;
	UPROPERTY()
	UTimelineComponent* CoverTransitionTimeline;
	
	FTransform CachedTransform;
	
	FVector TargetCoverLocation;
	FRotator TargetCoverRotation;

	FTimerHandle TimerHandle_AddTags;
	FTimerHandle TimerHandle_StartFiringDelay;
};