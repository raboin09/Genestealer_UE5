// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "API/Activatable.h"
#include "Weapons/BaseWeapon.h"
#include "MeleeWeapon.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class GENESTEALER_API AMeleeWeapon : public ABaseWeapon, public IActivatable
{
	GENERATED_BODY()
	
public:
	///////////////////////////////////////////
	/// IActivatable code
	///////////////////////////////////////////
	virtual void Activate() override;
	virtual void Deactivate() override;
	virtual void EnableComboWindow() override;
	virtual void DisableComboWindow() override;

protected:
	///////////////////////////////////////////
	/// ABaseWeapon code
	///////////////////////////////////////////
	FORCEINLINE virtual int32 GetCurrentAmmo() override { return 0.f; };
	FORCEINLINE virtual int32 GetMaxAmmo() override { return 0.f; }
	FORCEINLINE virtual bool CanReload() override { return false; }
	FORCEINLINE virtual void GiveAmmo(int32 AddAmount) override {}
	FORCEINLINE virtual void BroadcastAmmoUsage() override {}
	FORCEINLINE virtual int32 GetCurrentAmmoInClip() override{ return 1.f; }
	FORCEINLINE virtual int32 GetAmmoPerClip() override { return 1.f; }
	FORCEINLINE virtual bool HasInfiniteAmmo() override { return true; }
	FORCEINLINE virtual bool HasInfiniteClip() override { return true; }
	FORCEINLINE virtual void ReloadWeapon() override {}
	FORCEINLINE virtual void FireWeapon() override {}
	FORCEINLINE virtual void StopSimulatingWeaponFire() override {}
	FORCEINLINE virtual float SimulateWeaponFire() override { return 0.f; }
	virtual void StartFire() override;
	virtual void StopFire() override;
	
	///////////////////////////////////////////
	/// AMeleeWeapon code
	///////////////////////////////////////////
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Sound")
	USoundCue* FireSound;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Animation")
	FString SocketPrefix = "R_";
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Animation")
	UAnimMontage* FireAnim;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Animation")
	int32 MaxComboSections = 2;

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
	void Internal_ResetComboCounter();
	void Internal_AttemptRaycast(const FHitResult& HitResult, FVector StartLocation, FVector EndLocation);
	
	TMap<FString, FVector> Sockets;
	FTimerHandle Timer_Raycasting;
	
	int32 ComboSectionIncrement;
	
	UPROPERTY()
	UMeshComponent* MeshComponentRef;
	UPROPERTY(Transient)
	bool bIsActive;
	UPROPERTY(Transient)
	TArray<AActor*> HitActors;
};
