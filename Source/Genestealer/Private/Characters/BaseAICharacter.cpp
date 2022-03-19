// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BaseAICharacter.h"

#include "GameFramework/CharacterMovementComponent.h"

ABaseAICharacter::ABaseAICharacter()
{
	GetCharacterMovement()->GravityScale = 2.f;
	GetCharacterMovement()->MaxAcceleration = 1400.f;
	GetCharacterMovement()->GroundFriction = 3.f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->JumpZVelocity = 900.f;
	GetCharacterMovement()->AirControl = .8f;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 0.f, 540.f);
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->bUseRVOAvoidance = true;
	GetCharacterMovement()->AvoidanceConsiderationRadius = 169.f;
	
	FNavAgentProperties NavAgentProperties;
	NavAgentProperties.AgentRadius = 42.f;
	NavAgentProperties.AgentHeight = 192.f;
	NavAgentProperties.bCanCrouch = true;
	GetCharacterMovement()->NavAgentProps = NavAgentProperties;
	GetCharacterMovement()->bConstrainToPlane = true;
}
