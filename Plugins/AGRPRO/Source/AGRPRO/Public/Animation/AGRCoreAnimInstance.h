// Copyright 2021 Adam Grodzki All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Components/AGRAnimMasterComponent.h"
#include "Data/AGRTypes.h"
#include "AGRCoreAnimInstance.generated.h"

class UCharacterMovementComponent;
/**
 * 
 */
UCLASS()
class AGRPRO_API UAGRCoreAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UAGRCoreAnimInstance();

protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	void RecastOwnerComponents();
	void GetComponentVariables();
	void SetMovementVectorsAndStates();
	void SetupLeaning();
	void SetupAimOffset();
	void SetupMovementStates();
	float NormalizeLean(const float InValue) const;
	bool IsStanding() const;

public:
	UPROPERTY(BlueprintReadOnly, Category = "Genestealer|Components")
	ACharacter* OwningCharacter;

	UPROPERTY(BlueprintReadOnly, Category = "Genestealer|Components")
	UCharacterMovementComponent* OwnerMovementComponent;

	UPROPERTY(BlueprintReadOnly, Category = "Genestealer|Components")
	UAGRAnimMasterComponent* AnimMasterComponent;

	UPROPERTY(BlueprintReadOnly, Category = "Genestealer|Anim States")
	FGameplayTagContainer ModificationTags;

	UPROPERTY(BlueprintReadOnly, Category = "Genestealer|Anim States")
	FGameplayTag BasePose;

	UPROPERTY(BlueprintReadOnly, Category = "Genestealer|Anim States")
	FGameplayTag OverlayPose;

	UPROPERTY(BlueprintReadOnly, Category = "Genestealer|AimOffset")
	FRotator RawAimOffset;

	UPROPERTY(BlueprintReadOnly, Category = "Genestealer|AimOffset")
	EAGR_AimOffsets AimOffsetType;

	UPROPERTY(BlueprintReadOnly, Category = "Genestealer|AimOffset")
	EAGR_AimOffsetClamp AimOffsetBehavior;

	UPROPERTY(BlueprintReadOnly, Category = "Genestealer|AimOffset")
	FVector LookAtLocation;

	UPROPERTY(BlueprintReadOnly, Category = "Genestealer|AimOffset")
	FVector2D FinalAimOffset;

	UPROPERTY(BlueprintReadOnly, Category = "Genestealer|AimOffset")
	float AimClamp;

	UPROPERTY(BlueprintReadOnly, Category = "Genestealer|Rotation")
	EAGR_RotationMethod RotationMethod;

	UPROPERTY(BlueprintReadOnly, Category = "Genestealer|Runtime")
	float DeltaTick;

	UPROPERTY(BlueprintReadOnly, Category = "Genestealer|Runtime")
	FRotator PreviousRotation;

	UPROPERTY(BlueprintReadOnly, Category = "Genestealer|Movement")
	FRotator PreviousFrameAim;

	UPROPERTY(BlueprintReadOnly, Category = "Genestealer|Movement")
	float Velocity;

	UPROPERTY(BlueprintReadOnly, Category = "Genestealer|Movement")
	float ForwardVelocity;

	UPROPERTY(BlueprintReadOnly, Category = "Genestealer|Movement")
	float StrafeVelocity;

	UPROPERTY(BlueprintReadOnly, Category = "Genestealer|Movement")
	float UpVelocity;

	UPROPERTY(BlueprintReadOnly, Category = "Genestealer|Movement")
	float Direction;

	UPROPERTY(BlueprintReadOnly, Category = "Genestealer|Runtime")
	FVector InputAcceleration;

	UPROPERTY(BlueprintReadOnly, Category = "Genestealer|State")
	bool bFirstPerson;

	UPROPERTY(BlueprintReadOnly, Category = "Genestealer|State")
	bool bIdle;

	UPROPERTY(BlueprintReadOnly, Category = "Genestealer|State")
	bool bInAir;

	UPROPERTY(BlueprintReadOnly, Category = "Genestealer|State")
	bool bStanding;

	UPROPERTY(BlueprintReadOnly, Category = "Genestealer|State")
	bool bSwimming;

	UPROPERTY(BlueprintReadOnly, Category = "Genestealer|State")
	bool bCrouching;

	UPROPERTY(BlueprintReadOnly, Category = "Genestealer|State")
	bool bGrounded;

	UPROPERTY(BlueprintReadOnly, Category = "Genestealer|State")
	bool bWalkingState;

	UPROPERTY(BlueprintReadOnly, Category = "Genestealer|State")
	bool bFlying;

	UPROPERTY(BlueprintReadOnly, Category = "Genestealer|State")
	bool bFalling;

	UPROPERTY(BlueprintReadOnly, Category = "Genestealer|State")
	TEnumAsByte<EMovementMode> MovementMode;

	UPROPERTY(BlueprintReadOnly, Category = "Genestealer|Leans")
	float Lean;

	UPROPERTY(BlueprintReadOnly, Category = "Genestealer|Leans")
	FVector2D AimDelta;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Genestealer|Setup")
	float TargetFrameRate;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Genestealer|Setup")
	float LeanSmooth;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Genestealer|Setup")
	float AimSmooth;

private:
	float IdleXY;
	FVector VelocityVector;
};
