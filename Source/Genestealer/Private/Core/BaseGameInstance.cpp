// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/BaseGameInstance.h"

void UBaseGameInstance::Init()
{
	Super::Init();
	Internal_SetupManagers();
}

void UBaseGameInstance::Internal_SetupManagers()
{
	AudioManager = NewObject<UAudioManager>(this);

#if WITH_EDITOR
	const TSubclassOf<UDevSandboxManager> DevSandboxClass = LoadClass<UDevSandboxManager>(nullptr, TEXT("Blueprint'/Game/_Genestealer/_TESTING/GL_DEV_SandboxManager.GL_DEV_SandboxManager_C'"));
	DevSandboxManager = NewObject<UDevSandboxManager>(this, DevSandboxClass);
#endif
}
