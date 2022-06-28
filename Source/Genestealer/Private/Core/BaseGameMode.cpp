// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/BaseGameMode.h"

#include "Core/BasePlayerController.h"
#include "Utils/CoreUtils.h"


// Sets default values
ABaseGameMode::ABaseGameMode()
{
	PrimaryActorTick.bCanEverTick = false;
}

#if !UE_BUILD_SHIPPING
APlayerController* ABaseGameMode::SpawnPlayerController(ENetRole InRemoteRole, const FString& Options)
{
	ABasePlayerController* PlayerController = Cast<ABasePlayerController>(Super::SpawnPlayerController(InRemoteRole, Options));
	const UDevSandboxManager* DevSandboxManager = UCoreUtils::GetBaseGameInstance(this)->DevSandboxManager;
	DevSandboxManager->SetupSandboxGame();
	return PlayerController;
}
#endif