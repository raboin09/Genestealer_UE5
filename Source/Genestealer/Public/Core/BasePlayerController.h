// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "API/AIPawn.h"
#include "API/Interactable.h"
#include "Blueprint/UserWidget.h"
#include "Character/ALSPlayerController.h"
#include "Characters/BasePlayerCharacter.h"
#include "BasePlayerController.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable, AutoExpandCategories=("Genestealer"), PrioritizeCategories = "Genestealer")
class GENESTEALER_API ABasePlayerController : public AALSPlayerController
{
	GENERATED_BODY()

public:
	ABasePlayerController();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	void AddNewAISquadMember(TScriptInterface<IAIPawn> InAIPawn);
	FORCEINLINE class UPlayerStatsComponent* GetPlayerStatsComponent() const { return PlayerStats; }
	FORCEINLINE class UQuestManagerComponent* GetQuestManager() const { return QuestManager; }
	FORCEINLINE FNewActorTargeted& OnNewActorTargeted() { return NewActorTargeted; }
	TScriptInterface<IInteractable> GetTargetedActor() const;
	
	virtual void OnPossess(APawn* NewPawn) override;

	UPROPERTY()
	class UUIEventHub* UIEventHub;
	UPROPERTY()
	ABasePlayerCharacter* PlayerCharacter;
	
#if WITH_EDITOR
	void CreateSandboxUI()
	{
		if(!bSandboxMode)
			return;
		// const TSubclassOf<UUserWidget> UserWidgetClass = LoadClass<UUserWidget>(nullptr, TEXT("WidgetBlueprint'/Game/_Genestealer/_TESTING/GL_DEV_SandboxMainMenu.GL_DEV_SandboxMainMenu_C'"));
		// UUserWidget* UserWidget = CreateWidget<UUserWidget>(this, UserWidgetClass);
		// UserWidget->AddToViewport(1);
		// UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(this, UserWidget, EMouseLockMode::DoNotLock);
		// bShowMouseCursor = true;
	}
#endif
	
protected:
	UFUNCTION()
	void CoverAction(const FInputActionValue& Value);
	UFUNCTION()
	void InteractAction(const FInputActionValue& Value);
	UFUNCTION()
	void FireAction(const FInputActionValue& Value);
	UFUNCTION()
	void SecureLocationOrder(const FInputActionValue& Value);
	
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Defaults")
	bool bSandboxMode = false;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Defaults")
	TArray<FGameplayTag> DefaultGameplayTags;	
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Defaults")
	float OutlineTraceRange;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UQuestManagerComponent* QuestManager;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UPlayerStatsComponent* PlayerStats;
	
private:
	UFUNCTION()
	void HandlePlayerAimingChanged(const FPlayerAimingChangedPayload& PlayerAimingChangedPayload);
	bool IsActorAlive(UObject* InObject) const;
	bool ShouldOutlineInteractable(TScriptInterface<IInteractable> InInteractable) const;

	bool bShouldOutlineCombatants = false;
	FGameplayTagContainer GameplayTagContainer;
	TScriptInterface<IInteractable> CurrentInteractableActor;
	FNewActorTargeted NewActorTargeted;

	TArray<TScriptInterface<IAIPawn>> AISquadMembers;
};
