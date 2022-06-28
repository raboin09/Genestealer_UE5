// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BaseGameMode.generated.h"

UCLASS()
class GENESTEALER_API ABaseGameMode : public AGameModeBase
{
	GENERATED_BODY()

	ABaseGameMode();
#if !UE_BUILD_SHIPPING
	virtual APlayerController* SpawnPlayerController(ENetRole InRemoteRole, const FString& Options) override;
#endif
};
