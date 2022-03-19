// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Animation/BaseAnimInstance.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UBaseAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	CharacterRef = Cast<ABaseCharacter>(TryGetPawnOwner());
	check(CharacterRef)
	CharacterMovementComponentRef = CharacterRef->GetCharacterMovement();
	check(CharacterMovementComponentRef)
	InitialRotationRate = CharacterMovementComponentRef->RotationRate.Yaw;
	InitialMaxAcceleration = CharacterMovementComponentRef->GetMaxAcceleration();
	InitialSprintMaxSpeed = CharacterRef->SprintSpeed;
	InitialRunMaxSpeed = CharacterRef->JogMaxSpeed;
}

void UBaseAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if(!CharacterRef)
	{
		return;
	}
	
	Time = DeltaSeconds;
	if(const UCharacterMovementComponent* CharacterMovementComponent = CharacterRef->GetCharacterMovement())
	{
		bIsInAir = CharacterMovementComponent->IsFalling();
	}
	MovementVelocity = CharacterRef->GetVelocity().Length();
	CharacterLocation = CharacterRef->GetActorLocation();
	// TODO get AimingPose curve and input as Alpha (from TPSKit BP_PlayerCharacterBase::Calculate Aim Offset Angle)
	PlayerAimYawUnclamped = CharacterRef->CalculateAimOffsetYaw(PlayerAimYawUnclamped, 0.f);
	PlayerAimPitch = CharacterRef->CalculateAimOffsetPitch(PlayerAimPitch);
	ControlRotationYaw = CharacterRef->GetControlRotation().Yaw;
	ActorWorldRotationYaw = CharacterRef->GetActorRotation().Yaw;
	DesiredLocalMovingDirection = CharacterRef->CalculateCurrentInputLocalAngle();
	MovementInputYawAngleWorldSpace = UKismetMathLibrary::Conv_VectorToRotator(CharacterRef->GetLastMovementInputVector()).Yaw;

	Internal_AccelerationUpdate();
	Internal_RotationRateUpdate();

	if(bSprinting)
	{
		CharacterMovementComponentRef->bOrientRotationToMovement = true;
	} else
	{
		CharacterMovementComponentRef->bOrientRotationToMovement = false;
	}

	AnalogInputScale = CharacterMovementComponentRef->GetAnalogInputModifier();
	
	// TODO adjust -50% if walking
	CharacterRef->CalculateMovementInputScale(MoveForwardScale, MoveRightScale);
	if(CurrentAcceleration > 0)
	{
		CurrentLocalMovingAngle = CharacterRef->CalculateCurrentMovingLocalAngle(false);
		LastFrameLocalMovingAngle = CharacterRef->CalculateCurrentMovingLocalAngle(true);
	}

	bWalking = (CharacterMovementComponentRef->MovementMode == EMovementMode::MOVE_Walking) && !bCrouching;
	if(bWalking)
	{
		MoveForwardScale *= .5f;
		MoveRightScale *= .5f;
	}

	if(bUsingCover)
	{
		if(CharacterRef->bExitingCover)
		{
			
		} else
		{
			
		}		
	}

	
	// UKismetSystemLibrary::PrintString(this, FString::SanitizeFloat(PlayerAimPitch));
}

void UBaseAnimInstance::Internal_AccelerationUpdate()
{
	// TODO acceleration for non strafing movement in macro AccelerationUpdate in AnimBP
	CurrentAcceleration = CharacterMovementComponentRef->GetCurrentAcceleration().Size();
	CharacterMovementComponentRef->MaxAcceleration = InitialMaxAcceleration;
	CharacterMovementComponentRef->MaxAcceleration = InitialMaxAcceleration;
	CurrentAccelerationScale = UKismetMathLibrary::MapRangeClamped(CurrentAcceleration, 0.f, InitialMaxAcceleration, 0.f, 1.f);
}

void UBaseAnimInstance::Internal_RotationRateUpdate()
{
	// TODO Rotation rate update when override rotation rate is enabled
}
