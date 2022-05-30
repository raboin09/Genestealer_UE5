// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Animation/BaseAnimInstance.h"

#include "API/Animatable.h"
#include "Kismet/KismetSystemLibrary.h"

UBaseAnimInstance::UBaseAnimInstance()
{
}

void UBaseAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
}

void UBaseAnimInstance::DisableRootMotionModeForDuration(float Duration)
{
	RootMotionCache = RootMotionMode;
	RootMotionMode = ERootMotionMode::IgnoreRootMotion;
	if(const UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(TimerHandle_RootMotionDisable, this, &UBaseAnimInstance::EnableRootMotionMode, Duration, false);	
	}
}

void UBaseAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if(const IAnimatable* AnimOwner = Cast<IAnimatable>(TryGetPawnOwner()))
	{
		bFiring = AnimOwner->IsFiring();
		bIsInCover = AnimOwner->IsInCover();
		bAiming = AnimOwner->IsAiming();
		bReady = AnimOwner->IsReady();
	}
}

void UBaseAnimInstance::EnableRootMotionMode()
{
	RootMotionMode = RootMotionCache;
}
