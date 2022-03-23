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
	ABaseCharacter();
	virtual void Tick(float DeltaSeconds) override;

	////////////////////////////////
	/// IAnimatable override
	////////////////////////////////
	virtual float PlayWeaponAnimation(EWeaponAnimArchetype WeaponArchetype, EWeaponAnimAction WeaponAction) override;
	virtual void StopWeaponAnimation(EWeaponAnimArchetype WeaponArchetype, EWeaponAnimAction WeaponAction) override;
	virtual float TryPlayAnimMontage(const FAnimMontagePlayData& AnimMontageData) override;
	virtual float ForcePlayAnimMontage(const FAnimMontagePlayData& AnimMontageData) override;
	// virtual void ChangeOverlayState(EALSOverlayState InOverlayState) override;
	
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
	FORCEINLINE bool IsDying() const { return GameplayTagContainer.HasTag(GameplayTag::State::Dead); }
	UFUNCTION(BlueprintCallable, Category = "BaseCharacter")
	FORCEINLINE bool IsAlive() const { return !GameplayTagContainer.HasTag(GameplayTag::State::Dead); }
	UFUNCTION(BlueprintCallable, Category = "BaseCharacter")
	FORCEINLINE bool IsAiming() const { return !GameplayTagContainer.HasTag(GameplayTag::State::Aiming); }
	FORCEINLINE UInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }
	UFUNCTION()
	virtual void Die(FDeathEventPayload DeathEventPayload);

	////////////////////////////////
	/// ABaseCharacter Input
	////////////////////////////////
	void Input_ForwardMovement(float Value);
	void Input_RightMovement(float Value);
	void Input_CameraUp(float Value);
	void Input_CameraRight(float Value);
	
	////////////////////////////////
	/// ABaseCharacter Animation
	////////////////////////////////

	// TODO player facing right
	bool bPlayerFacingRight;
	// TODO true when firing
	bool bFiring;
	// TODO true when cover exit
	bool bExitingCover;

	float MovingSidewaysInput;
	float MovingForwardInput;
	
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Defaults")
	float SprintSpeed = 650.f;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Defaults")
	float JogMaxSpeed = 370.f;
	
	float CalculateAimOffsetYaw(const float CurrentAimYaw, const float Alpha) const;
	float CalculateAimOffsetPitch(const float CurrentAimPitch) const;
	float CalculateCurrentInputLocalAngle() const;
	void CalculateMovementInputScale(float& MoveForwardScale, float& MoveRightScale) const;
	float CalculateCurrentMovingLocalAngle(bool bLastFrame) const;
	
protected:
	////////////////////////////
	// ACharacter overrides
	////////////////////////////
	virtual void BeginPlay() override;
	virtual void PreInitializeComponents() override;
	virtual void PostInitializeComponents() override;
	
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
	
	UPROPERTY()
	UEffectContainerComponent* EffectContainerComponent;
	UPROPERTY(BlueprintReadOnly)
	FGameplayTagContainer GameplayTagContainer;
	
private:
	UAnimMontage* Internal_GetWeaponAnimation(EWeaponAnimArchetype WeaponArchetype, EWeaponAnimAction WeaponAction) const;
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
	void Internal_TraceCameraAim();
	float Internal_DotProductWithForwardAndRightVector(FVector InputVector) const;

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
	
	EAffiliation CurrentAffiliation;

	///////////////////////////////
	/// Animation Variables
	///////////////////////////////
	FVector CameraTraceEnd;
};
