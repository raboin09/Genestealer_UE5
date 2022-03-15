// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/BaseActor.h"
#include "Characters/BaseCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/TimelineComponent.h"
#include "BaseCoverPoint.generated.h"

/**
 * 
 */
UCLASS()
class GENESTEALER_API ABaseCoverPoint : public ABaseActor
{
	GENERATED_BODY()

public:
	ABaseCoverPoint();

	UFUNCTION(BlueprintCallable)
	void OccupyCover(ABaseCharacter* InActor, const FVector& InTargetCoverLocation, const FVector& InHitNormal);
	UFUNCTION(BlueprintCallable)
	void VacateCover(ABaseCharacter* InActor);

protected:
	virtual void BeginPlay() override;
	UFUNCTION()
	virtual void ActorBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void ActorEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Genestealer")
	float CoverWallOffset;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Genestealer")
	bool bCantMoveInThisCoverPoint;
	
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
	void Internal_HandlePeekCoverOverlap(bool bLeftCoverPoint, AActor* OtherActor);
	void Internal_HandlePeekCoverOverlapEnd(bool bLeftCoverPoint, AActor* OtherActor);
	void Internal_ActivateOverlapBoxes(bool bActivate);

	void Internal_StartCoverTransition();
	UFUNCTION()
	void Internal_CoverTransitionUpdate(float Alpha);
	UFUNCTION()
	void Internal_CoverTransitionFinished();
	
	UPROPERTY()
	ABaseCharacter* OccupiedActor;

	UPROPERTY(EditDefaultsOnly, Category="Genestealer")
	UCurveFloat* CoverTransitionCurve;
	UPROPERTY()
	UTimelineComponent* CoverTransitionTimeline;

	FVector TargetCoverLocation;
	FRotator TargetCoverRotation;
};
