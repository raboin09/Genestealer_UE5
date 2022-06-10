// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MaterialDissolver.h"
#include "Actors/BaseActor.h"
#include "API/CoverPoint.h"
#include "API/Interactable.h"
#include "Characters/BaseCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/TimelineComponent.h"
#include "Utils/GameplayTagUtils.h"
#include "BaseCoverActor.generated.h"

UCLASS(Abstract, Blueprintable)
class GENESTEALER_API ABaseCoverActor : public ABaseActor, public ICoverPoint, public IInteractable
{
	GENERATED_BODY()

public:
	ABaseCoverActor();

	////////////////////////////////
	/// IInteractable override
	////////////////////////////////
	virtual void SwitchOutlineOnMesh(bool bShouldOutline) override;
	virtual void InteractWithActor(AActor* InstigatingActor) override;

	////////////////////////////////
	/// ICoverPoint override
	////////////////////////////////
	virtual void OccupyCover(ABaseCharacter* InActor, const FVector& InTargetCoverLocation, const FVector& InHitNormal) override;
	virtual void VacateCover(ABaseCharacter* InActor) override;
	virtual void StartCoverFire() override;
	virtual void StopCoverFire() override;
	virtual void StartCoverAim() override;
	virtual void StopCoverAim() override;

	virtual bool HasOccupant() { return IsValid(OccupiedActor); }
protected:
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	
	UFUNCTION()
	virtual void ActorBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void ActorEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	UPROPERTY(EditDefaultsOnly, Category="Genestealer")
	UStaticMeshComponent* CoverMesh;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Genestealer")
	float CoverWallOffset;
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
	UBoxComponent* LeftCoverRollbackBox;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Genestealer")
	bool bLeftCoverEnabled;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Genestealer")
	UBoxComponent* RightCoverEdgeBox;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Genestealer")
	UBoxComponent* RightCoverPeekBox;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Genestealer")
	UBoxComponent* RightCoverRollbackBox;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Genestealer")
	bool bRightCoverEnabled;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Genestealer|Audio")
	USoundCue* HitCoverFoley;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Genestealer|Dissolve")
	TSubclassOf<AMaterialDissolver> MaterialDissolverClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Genestealer|Dissolve")
	USkeletalMeshComponent* DissolveMesh;
	
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Camera")
	TSubclassOf<UCameraShakeBase> CoverHitCameraShake;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Genestealer|Camera")
	float MinDistanceToPlayCameraShake = 200.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Genestealer|Camera")
	float DistanceWhenCameraShakePlays = 100.f;
private:
	void InitCoverBox(UBoxComponent* InBox, FColor InBoxColor, FVector Offset, FVector BoxExtent);
	
	void Internal_StartPeekFire();
	void Internal_StopPeekFire() const;
	
	void Internal_HandlePeekCoverOverlap(bool bLeftCoverPoint, AActor* OtherActor);
	void Internal_HandlePeekCoverOverlapEnd(bool bLeftCoverPoint, AActor* OtherActor);
	void Internal_ActivateOverlapBoxes(bool bActivate) const;
	void Internal_ApplyEdgeTagToActor(bool bLeftEdge);

	void Internal_AdjustStance(bool bNewStanceCrouching) const;
	void Internal_ResetCharacterValuesOnCoverExit() const;
	void Internal_SetCoverNormalRotationValues() const;
	void Internal_SetCoverAimingRotationValues(bool bRightShoulder) const;

	void Internal_TryPeekRolloutAndFire(const UShapeComponent* TargetPeekBox, bool bRightCameraShoulder);
	void Internal_StartPeekRollout(const UShapeComponent* TargetPeekBox, bool bRightCameraShoulder);
	void Internal_StartPeekRollback();

	void Internal_StartCoverTransition();
	UFUNCTION()
	void Internal_CoverTransitionUpdate(float Alpha);
	UFUNCTION()
	void Internal_CoverTransitionFinished();

	FORCEINLINE bool ActorInLeftEdge() const { return UGameplayTagUtils::ActorHasGameplayTag(OccupiedActor, TAG_COVER_LEFTEDGE); }
	FORCEINLINE bool ActorInLeftPeek() const { return UGameplayTagUtils::ActorHasGameplayTag(OccupiedActor, TAG_COVER_LEFTPEEK); }
	FORCEINLINE bool ActorInRightEdge() const { return UGameplayTagUtils::ActorHasGameplayTag(OccupiedActor, TAG_COVER_RIGHTEDGE); }
	FORCEINLINE bool ActorInRightPeek() const { return UGameplayTagUtils::ActorHasGameplayTag(OccupiedActor, TAG_COVER_RIGHTPEEK); }
	FORCEINLINE bool ActorAiming() const { return UGameplayTagUtils::ActorHasGameplayTag(OccupiedActor, TAG_STATE_AIMING); }
	FORCEINLINE bool ActorFiring() const { return UGameplayTagUtils::ActorHasGameplayTag(OccupiedActor, TAG_STATE_FIRING); }

	UPROPERTY()
	ABaseCharacter* OccupiedActor;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer")
	UCurveFloat* CoverTransitionCurve;
	UPROPERTY()
	UTimelineComponent* CoverTransitionTimeline;
	UPROPERTY()
	AMaterialDissolver* MaterialDissolver;
	
	float CameraShakeScale;
	bool bShouldPlayCoverDissolve;
	bool bShouldPlayCoverHitFeedback;
	
	FTransform CachedTransform;
	
	FVector TargetCoverLocation;
	FRotator TargetCoverRotation;

	FTimerHandle TimerHandle_AddTags;
	FTimerHandle TimerHandle_StartFiringDelay;
};
