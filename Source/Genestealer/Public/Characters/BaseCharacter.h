// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HealthComponent.h"
#include "InventoryComponent.h"
#include "API/Animatable.h"
#include "API/Attackable.h"
#include "API/Effectible.h"
#include "API/Taggable.h"
#include "Components/AGRAnimMasterComponent.h"
#include "GameFramework/Character.h"
#include "Utils/GameplayTagUtils.h"
#include "Weapons/BaseWeapon.h"
#include "BaseCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPlayerInCombatChanged, bool, bIsInCombat, AActor*, DamageCauser);

UCLASS(Abstract, NotBlueprintable, config=Game)
class GENESTEALER_API ABaseCharacter : public ACharacter, public IAttackable, public ITaggable, public IEffectible, public IAnimatable
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
	virtual float TryPlayAnimMontage(const FAnimMontagePlayData& AnimMontageData) override;
	virtual float ForcePlayAnimMontage(const FAnimMontagePlayData& AnimMontageData) override;
	virtual void ForceStopAnimMontage(UAnimMontage* AnimMontage) override;
	virtual bool IsAiming() const override { return GameplayTagContainer.HasTag(TAG_STATE_AIMING); }
	virtual bool IsFiring() const override { return GameplayTagContainer.HasTag(TAG_STATE_FIRING); }
	virtual bool IsInCover() const override { return GameplayTagContainer.HasTag(TAG_COVER_MIDDLE); }
	virtual UAnimMontage* GetCurrentPlayingMontage() const override { return GetCurrentMontage(); }
	virtual bool HasRightInput() const override { return bHasRightInput; }
	
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
	FORCEINLINE virtual UEffectContainerComponent* GetEffectContainerComponent() const override { return EffectContainerComponent; }

	////////////////////////////////
	/// ABaseCharacter
	////////////////////////////////
	UFUNCTION(BlueprintCallable, Category = "BaseCharacter")
	FORCEINLINE bool IsDying() const { return GameplayTagContainer.HasTag(TAG_STATE_DEAD); }
	UFUNCTION(BlueprintCallable, Category = "BaseCharacter")
	FORCEINLINE bool IsAlive() const { return !GameplayTagContainer.HasTag(TAG_STATE_DEAD); }
	FORCEINLINE UInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }
	UFUNCTION()
	virtual void Die(FDeathEventPayload DeathEventPayload);
	void SetCurrentCoverPoint(class ABaseCoverPoint* InCoverPoint) { CurrentCoverPoint = InCoverPoint; }

	////////////////////////////////
	/// ABaseCharacter Input
	////////////////////////////////
	void Input_ForwardMovement(float Value);
	void Input_RightMovement(float Value);
	void Input_CameraUp(float Value);
	void Input_CameraRight(float Value);
	void Input_Fire();
	void Input_StopFiring();
	void Input_CoverAction();
	void Input_Aim();
	void Input_StopAiming();

	UFUNCTION(BlueprintImplementableEvent)
	void K2_TryGetInCover();
	UFUNCTION(BlueprintImplementableEvent)
	void K2_VacateCover();
	void SetAimOffset(EAGR_AimOffsets InOffset);	
	UFUNCTION(BlueprintImplementableEvent)
	void K2_Aim();	
	UFUNCTION(BlueprintImplementableEvent)
	void K2_StopAiming();
	
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
	void HandleCurrentWeaponChanged(TScriptInterface<IWeapon> NewWeapon, TScriptInterface<IWeapon> PreviousWeapon);
	UFUNCTION()
	virtual void HandleDeathEvent(const FDeathEventPayload& DeathEventPayload);
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	UHealthComponent* HealthComponent;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	UInventoryComponent* InventoryComponent;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	UAGRAnimMasterComponent* AnimComponent;
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

	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Anims")
	float BaseTurnRate;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Anims")
	float BaseLookupRate;
	
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Sounds") 
	USoundCue* DeathSound;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Anims")
	UAnimMontage* DeathAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ABaseCoverPoint* CurrentCoverPoint;
	UPROPERTY()
	UEffectContainerComponent* EffectContainerComponent;
	UPROPERTY(BlueprintReadOnly)
	FGameplayTagContainer GameplayTagContainer;
	
private:
	void Internal_StopAllAnimMontages() const;
	float Internal_PlayMontage(const FAnimMontagePlayData& AnimMontagePlayData);
	void Internal_AddDefaultTagsToContainer();

	void InitAGRDefaults();

	////////////////////////////////
	/// Knockbacks and Hit Reacts
	////////////////////////////////
	void Internal_ApplyCharacterKnockback(const FVector& Impulse, const float ImpulseScale, const FName BoneName, bool bVelocityChange, float KnockdownDuration);
	void Internal_TryStartCharacterKnockback(const FDamageHitReactEvent& HitReactEvent);
	void Internal_TryCharacterKnockbackRecovery();
	void Internal_TryPlayHitReact(const FDamageHitReactEvent& HitReactEvent);
	FGameplayTag Internal_GetHitDirectionTag(const FVector& OriginatingLocation) const;

	FTimerHandle TimerHandle_InCombat;
	FTimerHandle TimerHandle_Destroy;
	FTimerHandle TimerHandle_DeathRagoll;
	FTimerHandle TimerHandle_Ragdoll;
	
	FPlayerInCombatChanged PlayerInCombatChanged;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArm;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* ThirdPersonCamera;

	UPROPERTY(Transient)
	EHitReactType LastKnownHitReact;
	UPROPERTY(Transient)
	bool bHasRightInput;
	
	EAffiliation CurrentAffiliation;
};
