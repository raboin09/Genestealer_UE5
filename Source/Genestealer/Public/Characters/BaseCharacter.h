// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HealthComponent.h"
#include "InventoryComponent.h"
#include "API/Attackable.h"
#include "API/Effectible.h"
#include "API/Taggable.h"
#include "Character/ALSCharacter.h"
#include "GameFramework/Character.h"
#include "Weapons/BaseWeapon.h"
#include "BaseCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPlayerInCombatChanged, bool, bIsInCombat, AActor*, DamageCauser);

UCLASS(Abstract, NotBlueprintable, config=Game)
class GENESTEALER_API ABaseCharacter : public AALSCharacter, public IAttackable, public ITaggable, public IEffectible
{
	GENERATED_BODY()
	
public:
	////////////////////////////
	// ACharacter overrides
	////////////////////////////
	ABaseCharacter(const FObjectInitializer& ObjectInitializer);
	virtual void Tick(float DeltaSeconds) override;

	////////////////////////////////
	/// IAttackable override
	////////////////////////////////
	FORCEINLINE virtual EAffiliation GetAffiliation() override { return CurrentAffiliation; }
	FORCEINLINE virtual UHealthComponent* GetHealthComponent() override { return HealthComponent; }

	////////////////////////////////
	/// ITaggable override
	////////////////////////////////
	FORCEINLINE virtual FGameplayTagContainer& GetTagContainer() override { return GameplayTagContainer; }

	////////////////////////////////
	/// IEffectible override
	////////////////////////////////
	FORCEINLINE virtual class UEffectContainerComponent* GetEffectContainerComponent() const override { return EffectContainerComponent; }

	////////////////////////////////
	/// ABaseCharacter
	////////////////////////////////
	FORCEINLINE UInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }
	UFUNCTION()
	virtual void Die(FDeathEventPayload DeathEventPayload);
	
protected:
	////////////////////////////
	// ACharacter overrides
	////////////////////////////
	virtual void BeginPlay() override;
	virtual void PreInitializeComponents() override;
	virtual void PostInitializeComponents() override;
	
	////////////////////////////////
	/// ALSCharacter overrides
	////////////////////////////////
	virtual void ForwardMovementAction_Implementation(float Value) override;
	virtual void RightMovementAction_Implementation(float Value) override;
	virtual void AimAction_Implementation(bool bValue) override;
	virtual void JumpAction_Implementation(bool bValue) override;
	virtual void OnOverlayStateChanged(EALSOverlayState PreviousState) override;

	////////////////////////////////
	/// ABaseCharacter
	////////////////////////////////
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS|Input")
	void FireAction(bool bFiring);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS|Input")
	void TargetingAction(bool bTargeting);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS|Input")
	void ReloadAction();
	
	UFUNCTION()
	void HandleCurrentWeaponChanged(TScriptInterface<IWeapon> NewWeapon, TScriptInterface<IWeapon> PreviousWeapon);
	UFUNCTION()
	virtual void HandleDeathEvent(const FDeathEventPayload& DeathEventPayload);
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	UHealthComponent* HealthComponent;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	UInventoryComponent* InventoryComponent;
	UPROPERTY(EditAnywhere, Category="Genestealer|Defaults")
	TArray<FGameplayTag> DefaultGameplayTags;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Defaults")
	TArray<TSubclassOf<AActor>> DefaultEffects;
	UPROPERTY(EditAnywhere, Category="Genestealer|Defaults")
	TSubclassOf<ABaseWeapon> StartingPistolClass;
	UPROPERTY(EditAnywhere, Category="Genestealer|Defaults")
	TSubclassOf<ABaseWeapon> StartingRifleClass;
	UPROPERTY(EditAnywhere, Category="Genestealer|Defaults")
	TSubclassOf<ABaseWeapon> StartingMeleeClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genestealer|Defaults")
	FHealthDefaults StartingHealth;

	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Sounds") 
	USoundCue* DeathSound;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Anims")
	UAnimMontage* DeathAnimation;
	
	UPROPERTY()
	UEffectContainerComponent* EffectContainerComponent;
	
	FGameplayTagContainer GameplayTagContainer;
	
private:
	void Internal_StopAllAnimMontages() const;
	void Internal_AddDefaultTagsToContainer();

	FTimerHandle TimerHandle_InCombat;
	FTimerHandle DestroyTimerHandle;
	FTimerHandle TimerHandle;
	
	FPlayerInCombatChanged PlayerInCombatChanged;
	
	EAffiliation CurrentAffiliation;
};
