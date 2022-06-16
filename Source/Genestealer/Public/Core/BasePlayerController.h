// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "API/Interactable.h"
#include "Blueprint/UserWidget.h"
#include "Character/ALSPlayerController.h"
#include "Characters/BasePlayerCharacter.h"
#include "BasePlayerController.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class GENESTEALER_API ABasePlayerController : public AALSPlayerController
{
	GENERATED_BODY()

public:
	ABasePlayerController();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	
	FORCEINLINE class UQuestManagerComponent* GetQuestManager() const { return QuestManager; }
	FORCEINLINE FNewActorTargeted& OnNewActorTargeted() { return NewActorTargeted; }
	TScriptInterface<IInteractable> GetTargetedActor() const;


	UFUNCTION()
	void CoverAction(const FInputActionValue& Value);
	virtual void OnPossess(APawn* NewPawn) override;

	UPROPERTY()
	class UUIEventHub* UIEventHub;
	UPROPERTY()
	ABasePlayerCharacter* PlayerCharacter;
	
#if !UE_BUILD_SHIPPING
	void CreateSandboxUI()
	{		
		// const TSubclassOf<UUserWidget> UserWidgetClass = LoadClass<UUserWidget>(nullptr, TEXT("Blueprint'/Game/_GladiusTempora/Core/TESTING/BPUI_SandboxMainMenu.BPUI_SandboxMainMenu_C'"));
		// UUserWidget* UserWidget = CreateWidget<UUserWidget>(this, UserWidgetClass);
		// UserWidget->AddToViewport();
		// UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(this, UserWidget, EMouseLockMode::DoNotLock);
		// bShowMouseCursor = true;
	}
#endif
	
protected:	
	UPROPERTY(EditDefaultsOnly, Category="Gladius|Defaults")
	TArray<FGameplayTag> DefaultGameplayTags;	
	UPROPERTY(EditDefaultsOnly, Category="Gladius|Defaults")
	float OutlineTraceRange;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UQuestManagerComponent* QuestManager;
	
private:
	UFUNCTION()
	void HandlePlayerAimingChanged(const FPlayerAimingChangedPayload& PlayerAimingChangedPayload);
	bool IsActorAlive(UObject* InObject) const;
	bool ShouldOutlineInteractable(TScriptInterface<IInteractable> InInteractable) const;

	bool bShouldOutlineCombatants = false;
	FGameplayTagContainer GameplayTagContainer;
	TScriptInterface<IInteractable> CurrentInteractableActor;
	FNewActorTargeted NewActorTargeted;
};
