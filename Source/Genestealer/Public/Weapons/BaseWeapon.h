#pragma once

#include "CoreMinimal.h"
#include "Actors/BaseActor.h"
#include "API/Animatable.h"
#include "API/Weapon.h"
#include "Components/SphereComponent.h"
#include "Effects/BaseEffect.h"

#include "BaseWeapon.generated.h"

class UMatineeCameraShake;
class UInventoryComponent;
class USkeletalMeshComponent;
class ABaseCharacter;
class USoundCue;

/**
 * 
 */
UCLASS(Abstract, NotBlueprintable, AutoExpandCategories=("Genestealer|Weapon"))
class GENESTEALER_API ABaseWeapon : public ABaseActor, public IWeapon
{
	GENERATED_BODY()
	
public:
	ABaseWeapon();
	
	///////////////////////////////
	// IWeapon overrides
	///////////////////////////////
	virtual void OnEquip(const TScriptInterface<IWeapon> LastWeapon) override;
	virtual void OnEquipFinished() override;
	virtual void OnUnEquip() override;
	virtual void StartReload() override;
	virtual void StopReload() override;
	virtual void StartFire() override;
	virtual void StopFire() override;
	virtual bool CanFire() const override;
	virtual void SetOwningPawn(ACharacter* IncomingCharacter) override;
	FORCEINLINE virtual FGameplayTag GetWeaponBasePose() const override { return WeaponBasePose; }
	FORCEINLINE virtual FGameplayTag GetWeaponOverlayPose() const override { return WeaponBasePose; }
	FORCEINLINE virtual void DestroyWeapon() override { Destroy(); }
	FORCEINLINE virtual FAmmoAmountChanged& OnAmmoAmountChanged() override { return AmmoAmountChanged; }
	FORCEINLINE virtual float GetWeaponRange() override { return  AI_UseRange; }

	UFUNCTION(BlueprintCallable)
    virtual int32 GetCurrentAmmo() override PURE_VIRTUAL(ABaseWeapon::GetCurrentAmmo, return 0;)
    UFUNCTION(BlueprintCallable)
    virtual int32 GetMaxAmmo() override PURE_VIRTUAL(ABaseWeapon::GetMaxAmmo, return 0;)
	virtual bool CanReload() override PURE_VIRTUAL(ABaseWeapon::CanReload, return false;)
	virtual void GiveAmmo(int32 AddAmount) override PURE_VIRTUAL(ABaseWeapon::GiveAmmo,);
	virtual void UseAmmo();
	
protected:

	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	virtual void OnBurstFinished();
	virtual void ApplyWeaponEffectsToActor(const FHitResult& Impact, const bool bShouldRotateHit = true);
	
	UAudioComponent* PlayWeaponSound(USoundCue* Sound) const;
    float PlayWeaponAnimation(const FAnimMontagePlayData& PlayData) const;
    void StopWeaponAnimation(UAnimMontage* AnimMontage) const;
	virtual void OnEnterInventory(ACharacter* NewOwner) override;
	virtual void OnLeaveInventory() override;
	virtual void PlayCameraShake();
	
	UFUNCTION()
	virtual void BroadcastAmmoUsage() override PURE_VIRTUAL(ABaseWeapon::BroadcastAmmoUsage,) 
	
	// Pure virtual functions
	UFUNCTION(BlueprintCallable)
    virtual int32 GetCurrentAmmoInClip() override PURE_VIRTUAL(ABaseWeapon::GetCurrentAmmoInClip, return 0;)
	UFUNCTION(BlueprintCallable)
    virtual int32 GetAmmoPerClip() override PURE_VIRTUAL(ABaseWeapon::GetAmmoPerClip, return 0;)
	UFUNCTION(BlueprintCallable)
	virtual bool HasInfiniteAmmo() override PURE_VIRTUAL(ABaseWeapon::HasInfiniteAmmo, return false;)
	UFUNCTION(BlueprintCallable)
    virtual bool HasInfiniteClip() override PURE_VIRTUAL(ABaseWeapon::HasInfiniteClip, return false;)
	virtual void FireWeapon() PURE_VIRTUAL(ABaseWeapon::FireWeapon,);
	virtual float SimulateWeaponFire() PURE_VIRTUAL(ABaseWeapon::SimulateWeaponFire, return 0.f; )
	virtual void StopSimulatingWeaponFire() PURE_VIRTUAL(ABaseWeapon::StopSimulatingWeaponFire,)	
	virtual void ReloadWeapon() PURE_VIRTUAL(ABaseWeapon::ReloadWeapon,)

	/////////////////////////////////
	// IWeapon overrides
	/////////////////////////////////
	
	UFUNCTION(BlueprintCallable)
    virtual UMeshComponent* GetWeaponMesh() const override;
	virtual UMeshComponent* GetSecondaryWeaponMesh() const override;
	FORCEINLINE virtual EWeaponState GetCurrentState() const override { return CurrentState; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE ACharacter* GetOwningPawn() const override { return OwningPawn; }

	UFUNCTION(BlueprintImplementableEvent)
	void BlueprintEquip(bool bEquipping);

	UPROPERTY()
	ACharacter* OwningPawn;
	EWeaponState CurrentState;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Genestealer|Weapon|Meshes")
	USphereComponent* WeaponRootComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Genestealer|Weapon|Meshes")
	UStaticMeshComponent* WeaponStaticMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Genestealer|Weapon|Meshes")
	USkeletalMeshComponent* WeaponSkeletalMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Genestealer|Weapon|Meshes")
	UStaticMeshComponent* SecondaryWeaponStaticMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Genestealer|Weapon|Meshes")
	USkeletalMeshComponent* SecondaryWeaponSkeletalMesh;
	
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Animation")
	float ReloadDurationIfNoAnim = 1.f;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Animation")
	UAnimMontage* ReloadAnim;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Animation")
	UAnimMontage* EquipAnim;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Animation")
	bool bLoopedFireAnim = true;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Animation")
	FGameplayTag WeaponBasePose;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Animation")
	FGameplayTag WeaponOverlayPose;
	
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Fire")
	float AI_UseRange = 500.f;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Fire")
	float TimeBetweenShots = .2f;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Effects", meta=(MustImplement="Effect"))
	TArray<TSubclassOf<AActor>> WeaponEffects;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Sound")
	USoundCue* EquipSound;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Sound")
	USoundCue* ReloadSound;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Sound")
	USoundCue* OutOfAmmoSound;


	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Camera")
	TSubclassOf<UCameraShakeBase> FireCameraShake;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Camera")
	float CameraShakeScale = 1.f;
private:
	void HandleFiring();
	void HandlePostBlendInFiring();
	void OnBurstStarted();
	void SetWeaponState(EWeaponState NewState);
	void DetermineWeaponState();
	void PlayWeaponMissEffectFX(const FHitResult& Impact, const bool bShouldRotateHit);

private:
	UPROPERTY()
	UInventoryComponent* OwningInventory;
	UPROPERTY()
	FAmmoAmountChanged AmmoAmountChanged;
	
	UPROPERTY(Transient)
	bool bPendingReload;
	UPROPERTY(Transient)
	bool bPendingEquip;
	UPROPERTY(Transient)
	bool bRefiring;
	UPROPERTY(Transient)
	bool bIsEquipped;
	UPROPERTY(Transient)
	bool bWantsToFire;
	
	UPROPERTY(Transient)
	float LastFireTime;
	
	UPROPERTY(Transient)
	int32 BurstCounter;

	FTimerHandle TimerHandle_OnEquipFinished;
	FTimerHandle TimerHandle_StopReload;
	FTimerHandle TimerHandle_ReloadWeapon;
	FTimerHandle TimerHandle_HandleFiring;
	FTimerHandle TimerHandle_FireBlendIn;
};