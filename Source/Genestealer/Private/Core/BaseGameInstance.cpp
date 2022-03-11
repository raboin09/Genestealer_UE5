// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/BaseGameInstance.h"

void UBaseGameInstance::Init()
{
	Super::Init();
	AudioManager = NewObject<UAudioManager>(this);
}
