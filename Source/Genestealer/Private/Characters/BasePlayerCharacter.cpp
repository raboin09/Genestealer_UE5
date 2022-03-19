// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BasePlayerCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"

ABasePlayerCharacter::ABasePlayerCharacter()
{
	GetCharacterMovement()->GravityScale = 1.4f;
	GetCharacterMovement()->MaxAcceleration = 600.f;
	GetCharacterMovement()->SetCrouchedHalfHeight(60.f);
	GetCharacterMovement()->GroundFriction = 3.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 100.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 800.f;
	GetCharacterMovement()->bCanWalkOffLedgesWhenCrouching = true;
	GetCharacterMovement()->bIgnoreBaseRotation = true;
	GetCharacterMovement()->AirControl = .3f;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 0.f, 230.f);
	GetCharacterMovement()->StandingDownwardForceScale = 0.f;
	GetCharacterMovement()->PushForceFactor = 375000.f;
	GetCharacterMovement()->PushForcePointZOffsetFactor = 0.f;
	FNavAgentProperties NavAgentProperties;
	NavAgentProperties.AgentRadius = 34.f;
	NavAgentProperties.AgentHeight = 192.f;
	NavAgentProperties.bCanCrouch = true;
	GetCharacterMovement()->NavAgentProps = NavAgentProperties;
}
