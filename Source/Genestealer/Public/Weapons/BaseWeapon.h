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
	virtual void StartFire() override;
	virtual void StopFire() override;
	virtual bool CanFire() const override;
	virtual void SetOwningPawn(ACharacter* IncomingCharacter) override;
	virtual void StartWeaponRagdoll() override;
	virtual bool IsWeaponOnCooldown() const override;
	FORCEINLINE virtual bool ShouldForceAimOnFire() const override { return bForceAimOnFire; }
	FORCEINLINE virtual EWeaponType GetWeaponType() const override { return WeaponType; }
	FORCEINLINE virtual EALSOverlayState GetWeaponOverlay() override { return WeaponOverlayState; }
	FORCEINLINE virtual void DestroyWeapon() override { Destroy(); }
	FORCEINLINE virtual float GetWeaponRange() override { return  AI_UseRange; }
	
	virtual bool CanReload() override PURE_VIRTUAL(ABaseWeapon::CanReload, return false;)
	virtual bool CheckChildFireCondition() PURE_VIRTUAL(ABaseWeapon::CheckChildFireCondition, return false;)
protected:
	
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	virtual void HandleFiring();
	virtual void OnBurstFinished();
	virtual void OnBurstStarted();
	virtual void ApplyWeaponEffectsToActor(const FHitResult& Impact, const bool bShouldRotateHit = true);

	bool IsWeaponPlayerControlled() const;
	void SetWeaponState(EWeaponState NewState);
	UAudioComponent* PlayWeaponSound(USoundCue* Sound) const;
    float PlayWeaponAnimation(const FAnimMontagePlayData& PlayData) const;
    void StopWeaponAnimation(UAnimMontage* AnimMontage) const;
	virtual void OnEnterInventory(ACharacter* NewOwner) override;
	virtual void OnLeaveInventory() override;
	virtual void PlayCameraShake();
	virtual void DetermineWeaponState();
	
	virtual void FireWeapon() PURE_VIRTUAL(ABaseWeapon::FireWeapon,);
	virtual float SimulateWeaponFire() PURE_VIRTUAL(ABaseWeapon::SimulateWeaponFire, return 0.f; )
	virtual void StopSimulatingWeaponFire() PURE_VIRTUAL(ABaseWeapon::StopSimulatingWeaponFire,)

	UFUNCTION(BlueprintImplementableEvent)
	void K2_OnBurstStarted();
	UFUNCTION(BlueprintImplementableEvent)
	void K2_OnBurstFinished();
	UFUNCTION(BlueprintImplementableEvent)
	void K2_OnUnEquip();
	UFUNCTION(BlueprintImplementableEvent)
	void K2_OnEquip();
	
	/////////////////////////////////
	// IWeapon overrides
	/////////////////////////////////
	
	UFUNCTION(BlueprintCallable)
    virtual UMeshComponent* GetWeaponMesh() const override;
	virtual UMeshComponent* GetSecondaryWeaponMesh() const override;
	FORCEINLINE virtual EWeaponState GetCurrentState() const override { return CurrentState; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE ACharacter* GetOwningPawn() const override { return OwningPawn; }
	virtual FTransform GetLeftHandSocketTransform() const override;

	UPROPERTY()
	ACharacter* OwningPawn;
	EWeaponState CurrentState;
	
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon")
	EWeaponType WeaponType = EWeaponType::Rifle;
	
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
	UAnimMontage* EquipAnim;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Animation")
	EALSOverlayState WeaponOverlayState;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Animation")
	FName ik_hand_l_Socket = "ik_hand_l";

	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Fire", meta = (ClampMin="0", EditCondition = "WeaponType != EWeaponType::Melee", EditConditionHides))
	float FireWarmUpTime = 0.f;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Fire", meta=(ClampMin="0"))
	float AI_UseRange = 500.f;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Fire")
	bool bForceAimOnFire = true;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Fire", meta=(ClampMin="0"))
	float TimeBetweenShots = .2f;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Effects", meta=(MustImplement="Effect"))
	TArray<TSubclassOf<AActor>> WeaponEffects;

	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Sound", meta = (EditCondition = "WeaponType != EWeaponType::Melee && FireWarmUpTime > 0", EditConditionHides))
	USoundCue* FireWarmupSound;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Sound")
	USoundCue* EquipSound;
	
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Camera", meta = (EditCondition = "WeaponType != EWeaponType::Melee", EditConditionHides))
	TSubclassOf<UCameraShakeBase> FireCameraShake;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Weapon|Camera", meta = (ClampMin="0", EditCondition = "FireCameraShake != nullptr || WeaponType != EWeaponType::Melee", EditConditionHides))
	float CameraShakeScale = 1.f;

	UPROPERTY(Transient)
	float LastFireTime;
	UPROPERTY(Transient)
	bool bRefiring;
	UPROPERTY(Transient)
	bool bIsEquipped;
	UPROPERTY(Transient)
	bool bWantsToFire;
	UPROPERTY(Transient)
	bool bPendingEquip;
	
	FTimerHandle TimerHandle_HandleFiring;
	
private:	
	void PlayWeaponMissEffectFX(const FHitResult& Impact, const bool bShouldRotateHit);
	void Internal_StartMeshRagdoll(UMeshComponent* InMeshComp) const;
	void InitWeaponMesh(UMeshComponent* InMeshComp);
	
	UPROPERTY()
	UInventoryComponent* OwningInventory;
	UPROPERTY(Transient)
	UAudioComponent* FireStartAudio;	
	UPROPERTY(Transient)
	int32 BurstCounter;

	FTimerHandle TimerHandle_OnEquipFinished;
	FTimerHandle TimerHandle_FireBlendIn;
};