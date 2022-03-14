// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HealthComponent.h"
#include "InventoryComponent.h"
#include "API/Animatable.h"
#include "API/Attackable.h"
#include "API/Effectible.h"
#include "API/Taggable.h"
#include "Character/ALSCharacter.h"
#include "GameFramework/Character.h"
#include "Utils/GameplayTagUtils.h"
#include "Weapons/BaseWeapon.h"
#include "BaseCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPlayerInCombatChanged, bool, bIsInCombat, AActor*, DamageCauser);

UCLASS(Abstract, NotBlueprintable, config=Game)
class GENESTEALER_API ABaseCharacter : public AALSCharacter, public IAttackable, public ITaggable, public IEffectible, public IAnimatable
{
	GENERATED_BODY()
	
public:
	////////////////////////////
	// ACharacter overrides
	////////////////////////////
	ABaseCharacter(const FObjectInitializer& ObjectInitializer);
	virtual void Tick(float DeltaSeconds) override;

	////////////////////////////////
	/// IAnimatable override
	////////////////////////////////
	virtual float PlayWeaponAnimation(EWeaponAnimArchetype WeaponArchetype, EWeaponAnimAction WeaponAction) override;
	virtual void StopWeaponAnimation(EWeaponAnimArchetype WeaponArchetype, EWeaponAnimAction WeaponAction) override;
	virtual float TryPlayAnimMontage(const FAnimMontagePlayData& AnimMontageData) override;
	virtual float ForcePlayAnimMontage(const FAnimMontagePlayData& AnimMontageData) override;
	virtual void ChangeOverlayState(EALSOverlayState InOverlayState) override;
	
	////////////////////////////////
	/// IAttackable override
	////////////////////////////////
	FORCEINLINE virtual EAffiliation GetAffiliation() override { return CurrentAffiliation; }
	FORCEINLINE virtual UHealthComponent* GetHealthComponent() override { return HealthComponent; }

	////////////////////////////////
	/// ITaggable override
	////////////////////////////////
	FORCEINLINE virtual FGameplayTagContainer GetTagContainer() override { return GameplayTagContainer; }

	////////////////////////////////
	/// IEffectible override
	////////////////////////////////
	FORCEINLINE virtual UEffectContainerComponent* GetEffectContainerComponent() const override { return EffectContainerComponent; }

	////////////////////////////////
	/// ABaseCharacter
	////////////////////////////////
	UFUNCTION(BlueprintCallable, Category = "BaseCharacter")
	FORCEINLINE bool IsDying() const { return GameplayTagContainer.HasTag(GameplayTag::State::Dead); }
	UFUNCTION(BlueprintCallable, Category = "BaseCharacter")
	FORCEINLINE bool IsAlive() const { return !GameplayTagContainer.HasTag(GameplayTag::State::Dead); }
	UFUNCTION(BlueprintCallable, Category = "BaseCharacter")
	FORCEINLINE bool IsAiming() const { return !GameplayTagContainer.HasTag(GameplayTag::State::Aiming); }
	FORCEINLINE UInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }
	UFUNCTION()
	virtual void Die(FDeathEventPayload DeathEventPayload);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Genestealer|Input")
	void K2_HandleCoverDodgeAction();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Genestealer|Input")
	void K2_HandleFireAction(bool bFiring);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Genestealer|Input")
	void K2_HandleAimAction(bool bTargeting);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Genestealer|Input")
	void K2_HandleReloadAction();
	
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
	virtual void OnOverlayStateChanged(EALSOverlayState PreviousState) override;
	virtual void RagdollEnd() override;

	////////////////////////////////
	/// ABaseCharacter Inputs
	////////////////////////////////
	virtual void K2_HandleCoverDodgeAction_Implementation();
	virtual void K2_HandleFireAction_Implementation(bool bFiring);
	virtual void K2_HandleAimAction_Implementation(bool bTargeting);
	virtual void K2_HandleReloadAction_Implementation();
	
	UFUNCTION(BlueprintImplementableEvent)
	UAnimMontage* K2_GetWeaponFireAnimation(EWeaponAnimArchetype WeaponArchetype) const;
	UFUNCTION(BlueprintImplementableEvent)
	UAnimMontage* K2_GetWeaponReloadAnimation(EWeaponAnimArchetype WeaponArchetype) const;
	UFUNCTION(BlueprintImplementableEvent)
	UAnimMontage* K2_GetWeaponEquipAnimation(EWeaponAnimArchetype WeaponArchetype) const;
	
	UFUNCTION(BlueprintImplementableEvent)
	UAnimMontage* K2_GetHitReactAnimation(const FGameplayTag& HitReactDirection) const;

	////////////////////////////////
	/// Event bindings
	////////////////////////////////
	UFUNCTION()
	void HandleCurrentWoundChangedEvent(const FCurrentWoundEventPayload& EventPayload);
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
	UAnimMontage* Internal_GetWeaponAnimation(EWeaponAnimArchetype WeaponArchetype, EWeaponAnimAction WeaponAction) const;
	void Internal_StopAllAnimMontages() const;
	float Internal_PlayMontage(const FAnimMontagePlayData& AnimMontagePlayData);
	void Internal_AddDefaultTagsToContainer();

	////////////////////////////////
	/// Knockbacks and Hit Reacts
	////////////////////////////////
	virtual void Internal_ApplyCharacterKnockback(const FVector& Impulse, const float ImpulseScale, const FName BoneName, bool bVelocityChange, float KnockdownDuration);
	virtual void Internal_TryStartCharacterKnockback(const FDamageHitReactEvent& HitReactEvent);
	virtual void Internal_TryCharacterKnockbackRecovery();
	virtual void Internal_TryPlayHitReact(const FDamageHitReactEvent& HitReactEvent);
	FGameplayTag Internal_GetHitDirectionTag(const FVector& OriginatingLocation) const;

	FTimerHandle TimerHandle_InCombat;
	FTimerHandle TimerHandle_Destroy;
	FTimerHandle TimerHandle_DeathRagoll;
	FTimerHandle TimerHandle_Ragdoll;
	
	FPlayerInCombatChanged PlayerInCombatChanged;

	UPROPERTY(Transient)
	EHitReactType LastKnownHitReact;
	
	EAffiliation CurrentAffiliation;
};
