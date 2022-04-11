// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Animation/BaseAnimInstance.h"

#include "API/Animatable.h"

UBaseAnimInstance::UBaseAnimInstance()
{
}

void UBaseAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
}

void UBaseAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if(const IAnimatable* AnimOwner = Cast<IAnimatable>(TryGetPawnOwner()))
	{
		bFiring = AnimOwner->IsFiring();
		bIsInCover = AnimOwner->IsInCover();
	}
}
