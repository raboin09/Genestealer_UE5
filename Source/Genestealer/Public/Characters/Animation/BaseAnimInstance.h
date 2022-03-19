// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "BaseAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class GENESTEALER_API UBaseAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:

	void Internal_AccelerationUpdate();
	void Internal_RotationRateUpdate();
	
	float Time;

	////////////////////////
	/// INPUT
	////////////////////////

	float AnalogInputScale;
	
	////////////////////////
	/// CHARACTER
	////////////////////////

	// Initials
	UPROPERTY()
	ABaseCharacter* CharacterRef;
	UPROPERTY()
	UCharacterMovementComponent* CharacterMovementComponentRef;

	// Ongoing
	FVector CharacterLocation;	
	float ControlRotationYaw;
	float ActorWorldRotationYaw;	
	float PlayerAimYawUnclamped;
	float PlayerAimPitch;
	
	////////////////////////
	// LOCOMOTION
	////////////////////////

	// Initials
	float InitialMaxAcceleration;
	float InitialRotationRate;
	float InitialSprintMaxSpeed;
	float InitialRunMaxSpeed;

	// Ongoing
	bool bIsInAir;
	bool bSprinting;
	bool bWalking;
	bool bAiming;
	bool bCrouching;
	
	float MovementVelocity;
	float MaxAcceleration;
	float CurrentAcceleration;
	float CurrentAccelerationScale;
	float CurrentLocalMovingAngle;
	float LastFrameLocalMovingAngle;
	float DesiredLocalMovingDirection;
	float MovementInputYawAngleWorldSpace;
	float MoveForwardScale;
	float MoveRightScale;

	////////////////////////
	/// COVER
	////////////////////////

	bool bUsingCover;
	
};
