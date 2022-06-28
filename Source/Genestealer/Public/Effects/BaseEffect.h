// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "NiagaraSystem.h"
#include "GameFramework/Actor.h"
#include "API/Effect.h"
#include "Particles/ParticleSystemComponent.h"
#include "Engine/DataTable.h"
#include "BaseEffect.generated.h"

class USoundCue;

UCLASS(Blueprintable, BlueprintType, EditInlineNew, DefaultToInstanced)
class GENESTEALER_API UBaseEffectData : public UObject
{
	GENERATED_BODY()
	
public:
	UBaseEffectData()
	{
		ImpactVFXRowHandle.DataTable = LoadObject<UDataTable>(nullptr, UTF8_TO_TCHAR("DataTable'/Game/_Genestealer/Data/Genestealer_ImpactVFX.Genestealer_ImpactVFX'"));
		ImpactSFXRowHandle.DataTable = LoadObject<UDataTable>(nullptr, UTF8_TO_TCHAR("DataTable'/Game/_Genestealer/Data/Genestealer_ImpactSFX.Genestealer_ImpactSFX'"));
	}
	
	UPROPERTY(EditDefaultsOnly, Category="Genestealer")
	FEffectInitializationData EffectData;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer")
	bool bAttachVFXToActor = false;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer")
	FDataTableRowHandle ImpactVFXRowHandle;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer")
	FDataTableRowHandle ImpactSFXRowHandle;
	UPROPERTY(EditDefaultsOnly, Category="Genestealer|Modifiers")
	TMap<FGameplayTag, FModifierExpression> EffectModifiers;
};

USTRUCT(BlueprintType)
struct FEffectImpactVFX : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category=Defaults)
	UNiagaraSystem* DefaultFX = nullptr;
	UPROPERTY(EditDefaultsOnly, Category=Visual)
	UNiagaraSystem* ConcreteFX = nullptr;
	UPROPERTY(EditDefaultsOnly, Category=Visual)
	UNiagaraSystem* DirtFX = nullptr;
	UPROPERTY(EditDefaultsOnly, Category=Visual)
	UNiagaraSystem* WaterFX = nullptr;
	UPROPERTY(EditDefaultsOnly, Category=Visual)
	UNiagaraSystem* MetalFX = nullptr;
	UPROPERTY(EditDefaultsOnly, Category=Visual)
	UNiagaraSystem* WoodFX = nullptr;
	UPROPERTY(EditDefaultsOnly, Category=Visual)
	UNiagaraSystem* GlassFX = nullptr;
	UPROPERTY(EditDefaultsOnly, Category=Visual)
	UNiagaraSystem* GrassFX = nullptr;
	UPROPERTY(EditDefaultsOnly, Category=Visual)
	UNiagaraSystem* FleshFX = nullptr;
	UPROPERTY(EditDefaultsOnly, Category=Visual)
	UNiagaraSystem* NurgleFleshFX = nullptr;
	UPROPERTY(EditDefaultsOnly, Category=Visual)
	UNiagaraSystem* SandFX = nullptr;
	UPROPERTY(EditDefaultsOnly, Category=Visual)
	UNiagaraSystem* PlasticFX = nullptr;
	UPROPERTY(EditDefaultsOnly, Category=Visual)
	UNiagaraSystem* IceFX = nullptr;
	UPROPERTY(EditDefaultsOnly, Category=Visual)
	UNiagaraSystem* FleshHeadshotFX = nullptr;
	UPROPERTY(EditDefaultsOnly, Category=Visual)
	UNiagaraSystem* NurgleFleshHeadshotFX = nullptr;
};

USTRUCT(BlueprintType)
struct FEffectImpactSFX : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category=Defaults)
	USoundCue* DefaultSound = nullptr;
	UPROPERTY(EditDefaultsOnly, Category=Sound)
	USoundCue* ConcreteSound = nullptr;
	UPROPERTY(EditDefaultsOnly, Category=Sound)
	USoundCue* DirtSound = nullptr;
	UPROPERTY(EditDefaultsOnly, Category=Sound)
	USoundCue* WaterSound = nullptr;
	UPROPERTY(EditDefaultsOnly, Category=Sound)
	USoundCue* MetalSound = nullptr;
	UPROPERTY(EditDefaultsOnly, Category=Sound)
	USoundCue* WoodSound = nullptr;
	UPROPERTY(EditDefaultsOnly, Category=Sound)
	USoundCue* GlassSound = nullptr;
	UPROPERTY(EditDefaultsOnly, Category=Sound)
	USoundCue* GrassSound = nullptr;
	UPROPERTY(EditDefaultsOnly, Category=Sound)
	USoundCue* FleshSound = nullptr;
	UPROPERTY(EditDefaultsOnly, Category=Sound)
	USoundCue* NurgleFleshSound = nullptr;
	UPROPERTY(EditDefaultsOnly, Category=Sound)
	USoundCue* SandSound = nullptr;
	UPROPERTY(EditDefaultsOnly, Category=Sound)
	USoundCue* PlasticSound = nullptr;
	UPROPERTY(EditDefaultsOnly, Category=Sound)
	USoundCue* IceSound = nullptr;
	UPROPERTY(EditDefaultsOnly, Category=Sound)
	USoundCue* FleshHeadshotSound = nullptr;
	UPROPERTY(EditDefaultsOnly, Category=Sound)
	USoundCue* NurgleFleshHeadshotSound = nullptr;
};

UCLASS(Abstract, Blueprintable, AutoExpandCategories=("Genestealer"), PrioritizeCategories = "Genestealer")
class GENESTEALER_API ABaseEffect : public AActor, public IEffect
{
	GENERATED_BODY()
	
public:	
	ABaseEffect();
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	//////////////////////////
	/// IEffect overrides
	//////////////////////////
	FORCEINLINE virtual void SetEffectContext(const FEffectContext& InContext) override { EffectContext = InContext; };
	FORCEINLINE virtual const FEffectInitializationData& GetEffectInitializationData() override { return EffectDataObj->EffectData; };
	FORCEINLINE virtual const TArray<FGameplayTag>& GetBlockedTags() const override { return EffectDataObj->EffectData.ValidTargets.BlockedTags; }
	FORCEINLINE virtual const TArray<FGameplayTag>& GetEffectTags() const override { return EffectDataObj->EffectData.EffectTags; }
	FORCEINLINE virtual const TArray<FGameplayTag>& GetRequiredTags() const override { return EffectDataObj->EffectData.ValidTargets.RequiredTags; }
	FORCEINLINE virtual const TArray<FGameplayTag>& GetRemoveEffectTags() const override { return EffectDataObj->EffectData.RemoveEffectsWithTags; }
	virtual void PlayEffectFX() override;
	virtual void ActivateEffect() override;
	virtual void DestroyEffect() override;
	
protected:

	//////////////////////////
	/// BaseEffect code
	//////////////////////////
	
	// Must override in child classes
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void K2_ActivateEffect();
	virtual void K2_ActivateEffect_Implementation();

	// Optionally override one of these in child classes
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void K2_OnDestroyEffect();
	virtual void K2_OnDestroyEffect_Implementation();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UFXSystemAsset* K2_GetEffectParticleSystem();
	virtual UFXSystemAsset* K2_GetEffectParticleSystem_Implementation();
	
	// Optionally override one of these in child classes
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	USoundCue* K2_GetEffectSound();
	virtual USoundCue* K2_GetEffectSound_Implementation();
	
	UPROPERTY(EditDefaultsOnly, Instanced, Category = "Genestealer")
	UBaseEffectData* EffectDataObj;
	UPROPERTY(BlueprintReadOnly)
	FEffectContext EffectContext;

	UPROPERTY(Transient)
	UFXSystemComponent* EffectVFX;
private:
	void Internal_PlayEffectSound();
	void Internal_PlayEffectParticleSystem();
	bool Internal_IsValidHeadshot() const;

	// Add and remove tags
	void Internal_AddAndRemoveTagsFromReceiver_Activation();
	// If Effect added tags and EffectDataObj has bShouldRemoveAppliedTagsWhenDestroyed, remove applied tags
	void Internal_AddAndRemoveTagsFromReceiver_Deactivation();	
};
