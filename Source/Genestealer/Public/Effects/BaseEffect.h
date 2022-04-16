// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "NiagaraSystem.h"
#include "GameFramework/Actor.h"
#include "API/Effect.h"
#include "Engine/DataTable.h"
#include "BaseEffect.generated.h"

class UFXSystemAsset;
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
	UNiagaraSystem* DefaultFX;
	UPROPERTY(EditDefaultsOnly, Category=Visual)
	UNiagaraSystem* ConcreteFX;
	UPROPERTY(EditDefaultsOnly, Category=Visual)
	UNiagaraSystem* DirtFX;
	UPROPERTY(EditDefaultsOnly, Category=Visual)
	UNiagaraSystem* WaterFX;
	UPROPERTY(EditDefaultsOnly, Category=Visual)
	UNiagaraSystem* MetalFX;
	UPROPERTY(EditDefaultsOnly, Category=Visual)
	UNiagaraSystem* WoodFX;
	UPROPERTY(EditDefaultsOnly, Category=Visual)
	UNiagaraSystem* GlassFX;
	UPROPERTY(EditDefaultsOnly, Category=Visual)
	UNiagaraSystem* GrassFX;
	UPROPERTY(EditDefaultsOnly, Category=Visual)
	UNiagaraSystem* FleshFX;
	UPROPERTY(EditDefaultsOnly, Category=Visual)
	UNiagaraSystem* NurgleFleshFX;
	UPROPERTY(EditDefaultsOnly, Category=Visual)
	UNiagaraSystem* SandFX;
	UPROPERTY(EditDefaultsOnly, Category=Visual)
	UNiagaraSystem* PlasticFX;
	UPROPERTY(EditDefaultsOnly, Category=Visual)
	UNiagaraSystem* IceFX;
	UPROPERTY(EditDefaultsOnly, Category=Visual)
	UNiagaraSystem* FleshHeadshotFX;
	UPROPERTY(EditDefaultsOnly, Category=Visual)
	UNiagaraSystem* NurgleFleshHeadshotFX;
};

USTRUCT(BlueprintType)
struct FEffectImpactSFX : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category=Defaults)
	USoundCue* DefaultSound;
	UPROPERTY(EditDefaultsOnly, Category=Sound)
	USoundCue* ConcreteSound;
	UPROPERTY(EditDefaultsOnly, Category=Sound)
	USoundCue* DirtSound;
	UPROPERTY(EditDefaultsOnly, Category=Sound)
	USoundCue* WaterSound;
	UPROPERTY(EditDefaultsOnly, Category=Sound)
	USoundCue* MetalSound;
	UPROPERTY(EditDefaultsOnly, Category=Sound)
	USoundCue* WoodSound;
	UPROPERTY(EditDefaultsOnly, Category=Sound)
	USoundCue* GlassSound;
	UPROPERTY(EditDefaultsOnly, Category=Sound)
	USoundCue* GrassSound;
	UPROPERTY(EditDefaultsOnly, Category=Sound)
	USoundCue* FleshSound;
	UPROPERTY(EditDefaultsOnly, Category=Sound)
	USoundCue* NurgleFleshSound;
	UPROPERTY(EditDefaultsOnly, Category=Sound)
	USoundCue* SandSound;
	UPROPERTY(EditDefaultsOnly, Category=Sound)
	USoundCue* PlasticSound;
	UPROPERTY(EditDefaultsOnly, Category=Sound)
	USoundCue* IceSound;
	UPROPERTY(EditDefaultsOnly, Category=Sound)
	USoundCue* FleshHeadshotSound;
	UPROPERTY(EditDefaultsOnly, Category=Sound)
	USoundCue* NurgleFleshHeadshotSound;
};

UCLASS(Abstract, Blueprintable, AutoExpandCategories=("Genestealer"), hideCategories = ("Material Parameters", "Optimization", "Skeletal Mesh", "HLOD", "Mobile", "Asset User Data", "Virtual Texture", "Compilation", "Clothing", "Master Post Component", "Skin Weights", "Clothing Simulation", "Override Parameters", "Activation", "Lighting", "Attachment", "Navigation", "Component Tick", "Physics", "Tags", "Component Replication", "Variable", "Parameters", "Randomness", "Rendering", "Replication", "Input", "Actor", "Actor Tick", "Collision", "Transform", "LOD", "Cooking"))
class GENESTEALER_API ABaseEffect : public AActor, public IEffect
{
	GENERATED_BODY()
	
public:	
	ABaseEffect();
	
	//////////////////////////
	/// IEffect overrides
	//////////////////////////
	FORCEINLINE virtual void SetEffectContext(const FEffectContext& InContext) override { EffectContext = InContext; };
	FORCEINLINE virtual const FEffectInitializationData& GetEffectInitializationData() override { return EffectDataObj->EffectData; };
	FORCEINLINE virtual const TArray<FGameplayTag>& GetBlockedTags() const override { return EffectDataObj->EffectData.ValidTargets.BlockedTags; }
	FORCEINLINE virtual const TArray<FGameplayTag>& GetRequiredTags() const override { return EffectDataObj->EffectData.ValidTargets.RequiredTags; }
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
	
	UPROPERTY(EditDefaultsOnly, Instanced, Category = "Weapon")
	UBaseEffectData* EffectDataObj;
	UPROPERTY(BlueprintReadOnly)
	FEffectContext EffectContext;
private:
	void Internal_PlayEffectSound();
	void Internal_PlayEffectParticleSystem();
	bool Internal_IsValidHeadshot() const;

	// Add and remove tags
	void Internal_AddAndRemoveTagsFromReceiver_Activation();
	// If Effect added tags and EffectDataObj has bShouldRemoveAppliedTagsWhenDestroyed, remove applied tags
	void Internal_AddAndRemoveTagsFromReceiver_Deactivation();
};
