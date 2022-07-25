// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "API/Activatable.h"
#include "Weapons/BaseWeapon.h"
#include "MeleeWeapon.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable, AutoExpandCategories=("Genestealer"), PrioritizeCategories = "Genestealer")
class GENESTEALER_API AMeleeWeapon : public ABaseWeapon, public IActivatable
{
	GENERATED_BODY()
	
public:
	AMeleeWeapon();
	
	///////////////////////////////////////////
	/// IActivatable code
	///////////////////////////////////////////
	virtual void Activate(TArray<TSubclassOf<AActor>> ActivationEffects) override;
	virtual void Deactivate() override;
	virtual void EnableComboWindow() override;
	virtual void DisableComboWindow() override;
	virtual void ResetActivatable() override;

protected:
	///////////////////////////////////////////
	/// ABaseWeapon code
	///////////////////////////////////////////
	FORCEINLINE virtual void FireWeapon() override {}
	FORCEINLINE virtual void StopSimulatingWeaponFire() override {}
	FORCEINLINE virtual float SimulateWeaponFire() override { return 0.f; }
	virtual void StartFire() override;
	virtual void StopFire() override;
	virtual void OnUnEquip() override;
	virtual void OnEnterInventory(ACharacter* NewOwner) override;
	
	///////////////////////////////////////////
	/// AMeleeWeapon code
	///////////////////////////////////////////
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent)
	void K2_PlayHitEffects(FHitResult HitResult, int32 ComboCounter = 0, int32 MaxCombos = 0);
	UFUNCTION(BlueprintImplementableEvent)
	void K2_StartWeaponTrace();
	UFUNCTION(BlueprintImplementableEvent)
	void K2_StopWeaponTrace();

	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Fire")
	bool bCanHitMultipleEnemies = true;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Sound")
	USoundCue* FireSound;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Animation")
	FString SocketPrefix = "R_";
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Animation")
	UAnimMontage* FireAnim;


private:
	UFUNCTION()
	FAnimMontagePlayData Internal_GetPlayData() const;
	UFUNCTION()
	void Internal_StartCollisionRaycastingTick();
	UFUNCTION()
	void Internal_StopCollisionRaycastingTick();
	UFUNCTION()
	void Internal_CheckForCollisionHit();
	FName Internal_GetNextMontageSection() const;
	void Internal_SetCurrentSocketLocations();
	void Internal_StartAttack();
	void Internal_StopAttack();

	void Internal_ResetStateForCombo();
	void Internal_IncrementComboCounter();
	void Internal_ResetComboCounter();
	
	TMap<FString, FVector> Sockets;
	FTimerHandle Timer_Raycasting;

	bool bRecordedHit;
	int32 ComboSectionIncrement;
	int32 MaxComboSections;
	const FString ComboPrefix = "Combo";

	UPROPERTY()
	UMeshComponent* MeshComponentRef;
	UPROPERTY(Transient)
	float MontageLengthInGameTime;
	UPROPERTY(Transient)
	TArray<AActor*> HitActors;
	UPROPERTY(Transient) 
	TArray<TSubclassOf<AActor>> AdditionalEffectsToApply;
};
