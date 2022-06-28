// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MaterialDissolver.h"
#include "Actors/BaseActor.h"
#include "API/Mountable.h"
#include "API/Interactable.h"
#include "Characters/BaseCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/TimelineComponent.h"
#include "Utils/GameplayTagUtils.h"
#include "BaseCoverActor.generated.h"

UCLASS(Abstract, Blueprintable)
class GENESTEALER_API ABaseCoverActor : public ABaseActor, public IMountable, public IInteractable
{
	GENERATED_BODY()

public:
	ABaseCoverActor();

	////////////////////////////////
	/// IInteractable override
	////////////////////////////////
	virtual void SwitchOutlineOnMesh(bool bShouldOutline) override;
	virtual void InteractWithActor(AActor* InstigatingActor) override;
	virtual EAffiliation GetInteractableAffiliation() const override { return EAffiliation::Neutral; }

	////////////////////////////////
	/// IMountable override
	////////////////////////////////
	virtual void OccupyMount(ABaseCharacter* InActor, const FVector& InTargetCoverLocation, const FVector& InHitNormal) override;
	virtual void VacateMount(ABaseCharacter* InActor) override;
	virtual void StartMountedFire() override;
	virtual void StopMountedFire() override;
	virtual void StartMountedAim() override;
	virtual void StopMountedAim() override;

	////////////////////////////////
	/// ABaseCoverActor
	////////////////////////////////
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
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Genestealer")
	bool bMiddleCoverEnabled = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Genestealer")
	UBoxComponent* LeftCoverEdgeBox;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Genestealer")
	UBoxComponent* LeftCoverPeekBox;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Genestealer")
	UBoxComponent* LeftCoverRollbackBox;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Genestealer")
	bool bLeftCoverEnabled;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Genestealer")
	UBoxComponent* RightCoverEdgeBox;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Genestealer")
	UBoxComponent* RightCoverPeekBox;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Genestealer")
	UBoxComponent* RightCoverRollbackBox;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Genestealer")
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

	void Internal_ResetCharacterValuesOnCoverExit() const;
	void Internal_SetCoverNormalRotationValues() const;
	void Internal_SetCoverAimingRotationValues(bool bRightShoulder) const;
	IAnimatable* Internal_GetAnimatableOwner() const;

	void Internal_TryPeekRolloutAndFire(const UShapeComponent* TargetPeekBox, bool bRightCameraShoulder);
	void Internal_StartPeekRollout(const UShapeComponent* TargetPeekBox, bool bRightCameraShoulder);
	void Internal_StartPeekRollback();

	void Internal_SetLookingMode() const;
	void Internal_SetVelocityMode() const;
	void Internal_SetAimingMode() const;
	
	void Internal_SetWalkingGait() const;
	void Internal_SetRunningGait() const;

	void Internal_SetStanding() const;
	void Internal_SetCrouching() const;

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
