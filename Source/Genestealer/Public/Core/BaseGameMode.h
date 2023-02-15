// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Core/BasePlayerController.h"
#include "Utils/CoreUtils.h"
#include "BaseGameMode.generated.h"

UCLASS()
class GENESTEALER_API ABaseGameMode : public AGameModeBase
{
	GENERATED_BODY()

	ABaseGameMode();
#if WITH_EDITOR
	virtual APlayerController* SpawnPlayerController(ENetRole InRemoteRole, const FString& Options) override
	{
		ABasePlayerController* PlayerController = Cast<ABasePlayerController>(Super::SpawnPlayerController(InRemoteRole, Options));
		const UDevSandboxManager* DevSandboxManager = UCoreUtils::GetBaseGameInstance(this)->DevSandboxManager;
		DevSandboxManager->SetupSandboxGame();
		return PlayerController;
	}
#endif
};
