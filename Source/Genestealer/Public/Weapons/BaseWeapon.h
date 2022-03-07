#pragma once

#include "CoreMinimal.h"
#include "Actors/BaseActor.h"
#include "API/Weapon.h"
#include "Components/CapsuleComponent.h"
#include "Effects/BaseEffect.h"

#include "BaseWeapon.generated.h"

class UMatineeCameraShake;
class UInventoryComponent;
class USkeletalMeshComponent;
class ABaseCharacter;
class USoundCue;

USTRUCT(BlueprintType)
struct FWeaponData
{
	GENERATED_BODY()

	FWeaponData()
	{
		ReloadAnim = nullptr;
		EquipAnim = nullptr;
		OutOfAmmoSound = nullptr;
		ReloadSound = nullptr;
		EquipSound = nullptr;
		WeaponOverlayState = EALSOverlayState::Default;
	}

	UPROPERTY(EditDefaultsOnly, Category="Weapon|ALS")
    EALSOverlayState WeaponOverlayState;
	UPROPERTY(EditDefaultsOnly, Category="Weapon|WeaponStat")
	float AI_UseRange = 500.f;
	UPROPERTY(EditDefaultsOnly, Category="Weapon|WeaponStat")
	float TimeBetweenShots = .2f;
	UPROPERTY(EditDefaultsOnly, Category="Weapon|Config", meta=(MustImplement="Effect"))
	TArray<TSubclassOf<AActor>> WeaponEffects;
	UPROPERTY(EditDefaultsOnly, Category="Weapon|Sound")
	USoundCue* EquipSound;
	UPROPERTY(EditDefaultsOnly, Category="Weapon|Sound")
    USoundCue* ReloadSound;
	UPROPERTY(EditDefaultsOnly, Category="Weapon|Sound")
	USoundCue* OutOfAmmoSound;

	UPROPERTY(EditDefaultsOnly, Category="Weapon|Animation")
	UAnimMontage* EquipAnim;
	UPROPERTY(EditDefaultsOnly, Category="Weapon|Animation")
	UAnimMontage* ReloadAnim;
	UPROPERTY(EditDefaultsOnly, Category="Weapon|Animation")
	float ReloadDurationIfNoAnim = 1.f;

	UPROPERTY(EditDefaultsOnly, Category="Weapon|Camera")
	TSubclassOf<UCameraShakeBase> FireCameraShake;
	UPROPERTY(EditDefaultsOnly, Category="Weapon|Camera")
	float CameraShakeScale = 1.f;
};

/**
 * 
 */
UCLASS(Abstract, NotBlueprintable)
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
	FORCEINLINE virtual void DestroyWeapon() override { Destroy(); }
	FORCEINLINE virtual USceneComponent* GetWeaponRoot() const override { return GetRootComponent(); }
	FORCEINLINE virtual EALSOverlayState GetWeaponOverlay() override { return GetWeaponData().WeaponOverlayState; };
	FORCEINLINE virtual EWeaponState GetWeaponState() override { return CurrentState; }
	FORCEINLINE virtual FAmmoAmountChanged& OnAmmoAmountChanged() override { return AmmoAmountChanged; }
	FORCEINLINE virtual float GetWeaponRange() override { return  GetWeaponData().AI_UseRange; }
	FORCEINLINE virtual FVector GetEquipOffset() const override { return AttachOffset; }
	FORCEINLINE virtual FRotator GetEquipRotation() const override { return AttachRotation; }

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
    float PlayWeaponAnimation(UAnimMontage* Animation) const;
    void StopWeaponAnimation(UAnimMontage* Animation) const;
	virtual void OnEnterInventory(ACharacter* NewOwner) override;
	virtual void OnLeaveInventory() override;
	virtual void PlayCameraShake();
	
	UFUNCTION()
	virtual void BroadcastAmmoUsage() override PURE_VIRTUAL(ABaseWeapon::BroadcastAmmoUsage,) 
	
	// Pure virtual function
	UFUNCTION(BlueprintCallable)
    virtual int32 GetCurrentAmmoInClip() override PURE_VIRTUAL(ABaseWeapon::GetCurrentAmmoInClip, return 0;)
	UFUNCTION(BlueprintCallable)
    virtual int32 GetAmmoPerClip() override PURE_VIRTUAL(ABaseWeapon::GetAmmoPerClip, return 0;)
	UFUNCTION(BlueprintCallable)
	virtual bool HasInfiniteAmmo() override PURE_VIRTUAL(ABaseWeapon::HasInfiniteAmmo, return false;)
	UFUNCTION(BlueprintCallable)
    virtual bool HasInfiniteClip() override PURE_VIRTUAL(ABaseWeapon::HasInfiniteClip, return false;)
	virtual void FireWeapon() PURE_VIRTUAL(ABaseWeapon::FireWeapon,);
	virtual void SimulateWeaponFire() PURE_VIRTUAL(ABaseWeapon::SimulateWeaponFire,)
	virtual void StopSimulatingWeaponFire() PURE_VIRTUAL(ABaseWeapon::StopSimulatingWeaponFire,)	
	virtual void ReloadWeapon() PURE_VIRTUAL(ABaseWeapon::ReloadWeapon,)
	virtual FWeaponData GetWeaponData() PURE_VIRTUAL(ABaseWeapon::GetWeaponData, return FWeaponData(););

	/////////////////////////////////
	// IWeapon overrides
	/////////////////////////////////
	
	UFUNCTION(BlueprintCallable)
    virtual UMeshComponent* GetWeaponMesh() const override;
	virtual void SetWeaponMesh(UMeshComponent* InMesh) override;
	FORCEINLINE virtual EWeaponState GetCurrentState() const override { return CurrentState; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE ACharacter* GetOwningPawn() const override { return OwningPawn; }

	UFUNCTION(BlueprintImplementableEvent)
	void BlueprintEquip(bool bEquipping);
	
	UPROPERTY()
	ACharacter* OwningPawn;
	UPROPERTY()
	UInventoryComponent* OwningInventory;
	UPROPERTY(EditDefaultsOnly, Category="Weapon|Config")
	FVector AttachOffset;
	UPROPERTY(EditDefaultsOnly, Category="Weapon|Config")
	FRotator AttachRotation;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category="Weapon|Config")
	UStaticMeshComponent* WeaponStaticMesh;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category="Weapon|Config")
	USkeletalMeshComponent* WeaponSkeletalMesh;
	UPROPERTY()
	FAmmoAmountChanged AmmoAmountChanged;

	EWeaponState CurrentState;
private:
	
	void HandleFiring();
	void OnBurstStarted();
	void SetWeaponState(EWeaponState NewState);
	void DetermineWeaponState();
	void PlayWeaponMissEffectFX(const FHitResult& Impact, const bool bShouldRotateHit);

	// Private variables
	
private:
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
};