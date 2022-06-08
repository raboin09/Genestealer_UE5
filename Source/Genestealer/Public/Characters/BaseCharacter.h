// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HealthComponent.h"
#include "InventoryComponent.h"
#include "Types/EventDeclarations.h"
#include "API/Animatable.h"
#include "API/Attackable.h"
#include "API/CoverPoint.h"
#include "API/Effectible.h"
#include "API/Taggable.h"
#include "Character/ALSCharacter.h"
#include "Utils/GameplayTagUtils.h"
#include "Weapons/BaseWeapon.h"
#include "BaseCharacter.generated.h"

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
	/// ALSCharacter overrides
	////////////////////////////////
	virtual void RagdollEnd() override;
	virtual void RagdollStart() override;
	virtual void OnOverlayStateChanged(EALSOverlayState PreviousState) override;
	virtual bool CanSprint() const override;
	virtual EALSGait GetAllowedGait() const override;
	
	////////////////////////////////
	/// IAnimatable override
	////////////////////////////////
	virtual float TryPlayAnimMontage(const FAnimMontagePlayData& AnimMontageData) override;
	virtual float ForcePlayAnimMontage(const FAnimMontagePlayData& AnimMontageData) override;
	virtual void ForceStopAnimMontage(UAnimMontage* AnimMontage) override;
	FORCEINLINE virtual bool IsAiming() const override { return GameplayTagContainer.HasTag(TAG_STATE_AIMING); }
	FORCEINLINE virtual bool IsFiring() const override { return GameplayTagContainer.HasTag(TAG_STATE_FIRING); }
	FORCEINLINE virtual bool IsReady() const override { return GameplayTagContainer.HasTag(TAG_STATE_READY); }
	FORCEINLINE virtual bool IsInCover() const override { return GameplayTagContainer.HasTag(TAG_STATE_IN_COVER); }
	FORCEINLINE virtual bool IsRagdoll() const override { return GameplayTagContainer.HasTag(TAG_STATE_RAGDOLL); }
	FORCEINLINE virtual UAnimMontage* GetCurrentPlayingMontage() const override { return GetCurrentMontage(); }
	FORCEINLINE virtual bool HasRightInput() const override { return bHasRightInput; }
	FORCEINLINE virtual FTransform GetFABRIKHandL() const override { return InventoryComponent ? InventoryComponent->GetCurrentWeaponLeftHandSocketTransform() : FTransform(); }
	
	////////////////////////////////
	/// IAttackable override
	////////////////////////////////
	FORCEINLINE virtual EAffiliation GetAffiliation() const override { return CurrentAffiliation; }
	FORCEINLINE virtual UHealthComponent* GetHealthComponent() const override { return HealthComponent; }
	FORCEINLINE virtual FVector GetHeadLocation() const override { return GetMesh()->GetSocketLocation("head"); }
	FORCEINLINE virtual FVector GetChestLocation() const override { return GetMesh()->GetSocketLocation("spine_02");}
	FORCEINLINE virtual FVector GetPelvisLocation() const override { return GetMesh()->GetSocketLocation("pelvis");}

	////////////////////////////////
	/// ITaggable override
	////////////////////////////////
	UFUNCTION()
	virtual FGameplayTagContainer& GetTagContainer() override { return GameplayTagContainer; }

	////////////////////////////////
	/// IEffectible override
	////////////////////////////////
	FORCEINLINE virtual UEffectContainerComponent* GetEffectContainerComponent() const override { return EffectContainerComponent; }

	////////////////////////////////
	/// ABaseCharacter
	////////////////////////////////
	UFUNCTION(BlueprintCallable, Category = "Genestealer")
	FORCEINLINE bool IsAlive() const { return !GameplayTagContainer.HasTag(TAG_STATE_DEAD); }
	FORCEINLINE UInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }
	FORCEINLINE FCharacterInCombatChanged& OnCharacterInCombatChanged() { return CharacterInCombatChanged; }
	bool IsInCombat();
	void SetInCombat(bool bInNewState, AActor* DamageCauser);

	UFUNCTION(BlueprintImplementableEvent)
	void K2_OnDeath();
	
protected:
	////////////////////////////
	// ACharacter overrides
	////////////////////////////
	virtual void BeginPlay() override;
	virtual void PreInitializeComponents() override;
	virtual void PostInitializeComponents() override;
	
	UFUNCTION(BlueprintImplementableEvent)
	UAnimMontage* K2_GetHitReactAnimation(const FGameplayTag& HitReactDirection) const;

	////////////////////////////////
	/// Event bindings
	////////////////////////////////
	UFUNCTION()
	void HandleCurrentWoundChangedEvent(const FCurrentWoundEventPayload& EventPayload);
	UFUNCTION()
	void HandleCurrentWeaponChanged(const FCurrentWeaponChangedPayload& CurrentWeaponChangedPayload);
	UFUNCTION()
	virtual void HandleDeathEvent(const FDeathEventPayload& DeathEventPayload);
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	UHealthComponent* HealthComponent;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	UInventoryComponent* InventoryComponent;
	UPROPERTY(EditAnywhere, Category="Genestealer|Defaults")
	TArray<FGameplayTag> DefaultGameplayTags;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Defaults")
	float InCombatTime;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Defaults")
	TArray<TSubclassOf<AActor>> DefaultEffects;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Defaults")
	EAffiliation CurrentAffiliation;
	UPROPERTY(EditAnywhere, Category="Genestealer|Defaults", meta=(MustImplement="Weapon"))
	TSubclassOf<AActor> StartingPrimaryWeaponClass;
	UPROPERTY(EditAnywhere, Category="Genestealer|Defaults", meta=(MustImplement="Weapon"))
	TSubclassOf<AActor> StartingAlternateWeaponClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genestealer|Defaults")
	FHealthDefaults StartingHealth;
	
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Sounds") 
	USoundCue* DeathSound;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Anims")
	UAnimMontage* DeathAnimation;
	
	UPROPERTY()
	UEffectContainerComponent* EffectContainerComponent;

	FCharacterInCombatChanged CharacterInCombatChanged;
	
private:
	void Internal_StopAllAnimMontages() const;
	float Internal_PlayMontage(const FAnimMontagePlayData& AnimMontagePlayData);
	
	void Internal_AddDefaultTagsToContainer();	
	void Internal_SetOutOfCombat();
	
	void InitCapsuleCollisionDefaults() const;
	void InitMeshCollisionDefaults() const;
	
	FORCEINLINE bool Internal_CanGetInCover() { return !GetTagContainer().HasTag(TAG_STATE_CANNOT_GET_IN_COVER); }
	void Internal_CoverDodgeTryStart();
	void Internal_CoverDodgeTryEnd();

	////////////////////////////////
	/// Knockbacks and Hit Reacts
	////////////////////////////////
	void Internal_ApplyCharacterKnockback(const FVector& Impulse, const float ImpulseScale, const FName BoneName, bool bVelocityChange);
	void Internal_TryStartCharacterKnockback(const FDamageHitReactEvent& HitReactEvent, bool bShouldRecoverFromKnockback = true);
	void Internal_TryCharacterKnockbackRecovery();
	void Internal_TryPlayHitReact(const FDamageHitReactEvent& HitReactEvent);
	FGameplayTag Internal_GetHitDirectionTag(const FVector& OriginatingLocation) const;
protected:
	
	////////////////////////////////
	/// ABaseCharacter Input
	////////////////////////////////
	virtual bool GL_IsForwardMovementAllowed(float Value) override;
	virtual bool GL_IsRightMovementAllowed(float Value) override;
	virtual void GL_HandleFireAction(bool bValue) override;
	virtual void GL_HandleCoverDodgeAction() override;
	virtual void GL_HandleAimAction(bool bValue) override;
	virtual void GL_HandleSprintAction(bool bValue) override;
	
private:
	FTimerHandle TimerHandle_InCombat;
	FTimerHandle TimerHandle_Destroy;
	FTimerHandle TimerHandle_DeathRagoll;
	FTimerHandle TimerHandle_Ragdoll;

	UPROPERTY(Transient)
	EHitReactType LastKnownHitReact;
	UPROPERTY(Transient)
	bool bHasRightInput;
	UPROPERTY(Transient)
	TScriptInterface<ICoverPoint> CurrentCoverPoint;
};
