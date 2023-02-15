// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryComponent.h"
#include "Types/EventDeclarations.h"
#include "API/Animatable.h"
#include "API/Attackable.h"
#include "API/Mountable.h"
#include "API/Taggable.h"
#include "Character/ALSCharacter.h"
#include "Character/Animation/ALSPlayerCameraBehavior.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Types/OnlineContentTypes.h"
#include "Weapons/BaseWeapon.h"
#include "BaseCharacter.generated.h"

class ABaseAIController;

UCLASS(Abstract, NotBlueprintable, config=Game, AutoExpandCategories=("Genestealer"), PrioritizeCategories = "Genestealer")
class GENESTEALER_API ABaseCharacter : public AALSCharacter, public IAttackable, public ITaggable, public IAnimatable
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
	FORCEINLINE virtual bool CanSprint() const override { return IsMounted() ? false : Super::CanSprint(); }
	FORCEINLINE virtual EALSGait GetAllowedGait() const override { return IsMounted() ?  EALSGait::Walking : Super::GetAllowedGait(); } 
	
	////////////////////////////////
	/// IAnimatable override
	////////////////////////////////
	virtual float TryPlayAnimMontage(const FAnimMontagePlayData& AnimMontageData) override;
	virtual float ForcePlayAnimMontage(const FAnimMontagePlayData& AnimMontageData) override;
	virtual void ForceStopAnimMontage(UAnimMontage* AnimMontage) override;
	FORCEINLINE virtual void SetCameraLookingMode() override { if(CameraBehavior) CameraBehavior->SetRotationMode(EALSRotationMode::LookingDirection); }
	FORCEINLINE virtual void SetCameraOverRightShoulder(bool bNewRightShoulder) override { SetRightShoulder(bNewRightShoulder); }
	FORCEINLINE virtual void SetAimingMode(bool bForce = true, bool bAffectsCamera = true) override { SetRotationMode(EALSRotationMode::Aiming, bForce, bAffectsCamera); }
	FORCEINLINE virtual void SetLookingMode(bool bForce = true, bool bAffectsCamera = true) override { SetRotationMode(EALSRotationMode::LookingDirection, bForce, bAffectsCamera); }
	FORCEINLINE virtual void SetVelocityMode(bool bForce = true, bool bAffectsCamera = true) override { SetRotationMode(EALSRotationMode::VelocityDirection, bForce, bAffectsCamera); }
	FORCEINLINE virtual void SetWalkingGait() override { SetDesiredGait(EALSGait::Walking); }
	FORCEINLINE virtual void SetRunningGait() override { SetDesiredGait(EALSGait::Running); }
	FORCEINLINE virtual void SetSprintingGait() override { SetDesiredGait(EALSGait::Sprinting); }
	FORCEINLINE virtual void SetStanding() override { SetStance(EALSStance::Standing); }
	FORCEINLINE virtual void SetCrouching() override { SetStance(EALSStance::Crouching); }
	FORCEINLINE virtual bool IsAiming() override { return UGameplayTagUtils::ActorHasGameplayTag(this, TAG_STATE_AIMING); }
	FORCEINLINE virtual bool IsFiring() override { return UGameplayTagUtils::ActorHasGameplayTag(this, TAG_STATE_FIRING); }
	FORCEINLINE virtual bool IsReady() override { return UGameplayTagUtils::ActorHasGameplayTag(this, TAG_STATE_READY); }
	FORCEINLINE virtual bool IsInCover() override { return UGameplayTagUtils::ActorHasGameplayTag(this, TAG_STATE_IN_COVER); }
	FORCEINLINE virtual bool IsRagdoll() override { return UGameplayTagUtils::ActorHasGameplayTag(this, TAG_STATE_RAGDOLL); }
	FORCEINLINE virtual UAnimMontage* GetCurrentPlayingMontage() const override { return GetCurrentMontage(); }
	FORCEINLINE virtual bool HasRightInput() override { return UGameplayTagUtils::ActorHasGameplayTag(this, TAG_INPUT_RIGHT_MOVEMENT); }
	FORCEINLINE virtual FTransform GetWeaponLeftHandPlacementLocation() const override { return InventoryComponent ? InventoryComponent->GetCurrentWeaponLeftHandSocketTransform() : FTransform(); }
	
	////////////////////////////////
	/// IAttackable override
	////////////////////////////////
	FORCEINLINE virtual EAbsoluteAffiliation GetAffiliation() const override { return AbsoluteAffiliation; }
	FORCEINLINE virtual UHealthComponent* GetHealthComponent() const override { return HealthComponent; }
	FORCEINLINE virtual FVector GetHeadLocation() const override { return GetMesh()->GetSocketLocation("head"); }
	FORCEINLINE virtual FVector GetChestLocation() const override { return GetMesh()->GetSocketLocation("spine_02");}
	FORCEINLINE virtual FVector GetPelvisLocation() const override { return GetMesh()->GetSocketLocation("pelvis");}
	FORCEINLINE virtual UEffectContainerComponent* GetEffectContainerComponent() const override { return EffectContainerComponent; }
	
	////////////////////////////////
	/// ITaggable override
	////////////////////////////////
	UFUNCTION()
	virtual FGameplayTagContainer& GetTagContainer() override { return GameplayTagContainer; }
	virtual void HandleTagChanged(const FGameplayTag& ChangedTag, bool bAdded) override;

	////////////////////////////////
	/// ABaseCharacter
	////////////////////////////////
	FORCEINLINE void AddTrackedAIController(ABaseAIController* TrackedCon) { if(IsAlive()) EnemyTrackers.AddUnique(TrackedCon); }
	FORCEINLINE void RemoveTrackedAIController(ABaseAIController* TrackedCon) { if(IsAlive()) EnemyTrackers.Remove(TrackedCon); } 
	UFUNCTION(BlueprintImplementableEvent)
	void K2_HandleDamageEvent(FHitResult DamageEvent, float DecalSize);
	UFUNCTION(BlueprintCallable, Category = "Genestealer")
	FORCEINLINE bool IsAlive() { return !UGameplayTagUtils::ActorHasGameplayTag(this, TAG_STATE_DEAD); }
	FORCEINLINE UInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }
	FORCEINLINE FCharacterInCombatChanged& OnCharacterInCombatChanged() { return CharacterInCombatChanged; }
	bool IsInCombat();
	void SetInCombat(bool bInNewState, AActor* DamageCauser);
	FORCEINLINE TScriptInterface<IMountable> GetCurrentMount() const { return CurrentMount; }

	UFUNCTION(BlueprintImplementableEvent, Category = "Genestealer")
	void K2_OnDeath();
	UFUNCTION(BlueprintNativeEvent, Category = "Genestealer")
	void K2_HandleTagAdded(const FGameplayTag& AddedTag);
	UFUNCTION(BlueprintNativeEvent, Category = "Genestealer")
	void K2_HandleTagRemoved(const FGameplayTag& RemovedTag);
	UFUNCTION(BlueprintCallable)
	void PlayAudioDialogue(USoundBase* SoundToPlay, bool bForce = false);
	
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
	void HandleNewWeaponAddedEvent(const FNewWeaponAddedPayload& EventPayload);
	UFUNCTION()
	void HandleCurrentWeaponChanged(const FCurrentWeaponChangedPayload& CurrentWeaponChangedPayload);
	UFUNCTION()
	virtual void HandleDeathEvent(const FActorDeathEventPayload& DeathEventPayload);
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	UHealthComponent* HealthComponent;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	UInventoryComponent* InventoryComponent;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Defaults")
	FString UnitName = "Placeholder";
	UPROPERTY(EditAnywhere, Category="Genestealer|Defaults")
	TArray<FGameplayTag> DefaultGameplayTags;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Defaults")
	float InCombatTime;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Defaults")
	TArray<TSubclassOf<AActor>> DefaultEffects;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Genestealer|Defaults")
	EAbsoluteAffiliation AbsoluteAffiliation;
	UPROPERTY(EditAnywhere, Category="Genestealer|Defaults", meta=(MustImplement="Weapon"))
	TSubclassOf<AActor> StartingPrimaryWeaponClass;
	// UPROPERTY(EditAnywhere, Category="Genestealer|Defaults", meta=(MustImplement="Weapon"))
	// TSubclassOf<AActor> StartingAlternateWeaponClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genestealer|Defaults")
	FHealthDefaults StartingHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genestealer|Defaults")
	EBallisticSkill BallisticSkill;
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

	FORCEINLINE bool IsMounted() const { return CurrentMount != nullptr; }
	FORCEINLINE bool CanGetInCover() { return !GetTagContainer().HasTag(TAG_STATE_CANNOT_MOUNT); }
	void Internal_CoverDodgeTryStart();
	void Internal_CoverDodgeTryEnd();

	void Internal_HideWeapons();
	void Internal_ShowWeapons();

	////////////////////////////////
	/// Knockbacks and Hit Reacts
	////////////////////////////////
	void Internal_ApplyCharacterKnockback(const FVector& Impulse, const float ImpulseScale, const FName BoneName, bool bVelocityChange);
	void Internal_TryStartCharacterKnockback(const FDamageHitReactEvent& HitReactEvent, bool bShouldRecoverFromKnockback = true);
	void Internal_TryCharacterKnockbackRecovery();
	void Internal_TryPlayHitReact(const FDamageHitReactEvent& HitReactEvent);
	void Internal_AssignNewMountable(UObject* InMountableObject, FHitResult InHitResult);
	
protected:
	
	////////////////////////////////
	/// ABaseCharacter Input
	////////////////////////////////
	virtual bool GL_IsForwardMovementAllowed(float Value) override;
	virtual bool GL_IsRightMovementAllowed(float Value) override;
	virtual void GL_HandleFireAction(bool bValue) override;
	virtual void GL_HandleReloadAction() override;
	virtual void GL_HandleCoverDodgeAction() override;
	virtual void GL_HandleAimAction(bool bValue) override;
	virtual void GL_HandleSprintAction(bool bValue) override;
	
	UPROPERTY(Transient)
	EHitReactType LastKnownHitReact;
	FGameplayTag Internal_GetHitDirectionTag(const FVector& OriginatingLocation) const;
	
private:
	FTimerHandle TimerHandle_HideWeapons;
	FTimerHandle TimerHandle_InCombat;
	FTimerHandle TimerHandle_Ragdoll;

	UPROPERTY(Transient)
	UAudioComponent* CurrentDialogue;
	
	UPROPERTY(Transient)
	TScriptInterface<IMountable> CurrentMount;

	// Array of AIControllers who have this marked as an enemy
	UPROPERTY()
	TArray<ABaseAIController*> EnemyTrackers = {};
};
