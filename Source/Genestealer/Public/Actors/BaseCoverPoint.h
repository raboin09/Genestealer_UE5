// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/BaseActor.h"
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
	void OccupyCover(AActor* InActor, const FVector& InTargetCoverLocation);
	UFUNCTION(BlueprintCallable)
	void VacateCover(AActor* InActor);

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
	UStaticMeshComponent* MiddleCover;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Genestealer")
	bool bMiddleCoverEnabled;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Genestealer")
	UBoxComponent* LeftCoverCollision;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Genestealer")
	bool bLeftCoverEnabled;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Genestealer")
	UBoxComponent* RightCoverCollision;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Genestealer")
	bool bRightCoverEnabled;

private:
	void Internal_HandleEdgeCoverOverlap(bool bLeftCoverPoint, AActor* OtherActor);
	void Internal_HandleEdgeCoverOverlapEnd(bool bLeftCoverPoint, AActor* OtherActor);

	void Internal_StartCoverTransition();
	UFUNCTION()
	void Internal_CoverTransitionUpdate(float Alpha);
	UFUNCTION()
	void Internal_CoverTransitionFinished();
	
private:
	UPROPERTY()
	AActor* OccupiedActor;

	UPROPERTY(EditDefaultsOnly, Category="Genestealer")
	UCurveFloat* CoverTransitionCurve;
	UPROPERTY()
	UTimelineComponent* CoverTransitionTimeline;

	FVector TargetCoverLocation;
	FRotator TargetCoverRotation;
};
