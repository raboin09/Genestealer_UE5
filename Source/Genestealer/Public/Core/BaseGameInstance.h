// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AudioManager.h"
#include "DevSandboxManager.h"
#include "OnlineContentManager.h"
#include "Engine/GameInstance.h"
#include "BaseGameInstance.generated.h"

/**
 * 
 */
UCLASS(AutoExpandCategories=("Genestealer"), PrioritizeCategories = "Genestealer")
class GENESTEALER_API UBaseGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	virtual void Init() override;
	FORCEINLINE UOnlineContentManager* GetOnlineContentManager() const { return OnlineContentManager; }
	
#if WITH_EDITOR
	UDevSandboxManager* DevSandboxManager;
#endif

protected:
	UPROPERTY(EditDefaultsOnly, Category="Genestealer")
	TSubclassOf<UOnlineContentManager> ContentManagerClass;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer")
	TSoftObjectPtr<UWorld> PostProcessWorld;
	
private:
	UPROPERTY()
	UAudioManager* AudioManager;
	UPROPERTY()
	UOnlineContentManager* OnlineContentManager;

	void Internal_SetupManagers();
};
