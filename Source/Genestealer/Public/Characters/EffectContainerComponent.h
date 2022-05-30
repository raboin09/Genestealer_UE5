// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "API/Effect.h"
#include "API/Taggable.h"
#include "Components/ActorComponent.h"
#include "EffectContainerComponent.generated.h"

USTRUCT()
struct FTickingEffect
{
	GENERATED_BODY()

	FTickingEffect()
	{
		TickingEffect = nullptr;
		TickModulus = -1;
		ExpirationTime = -1;
		TickID = -1;
	}

	FTickingEffect(TScriptInterface<IEffect> IncomingEffect, int32 InTickModulus, int32 InExpirationTime, int32 InTickID)
	{
		TickModulus = InTickModulus;
		ExpirationTime = InExpirationTime;
		TickingEffect = IncomingEffect;
		TickID = InTickID;
	}

	TScriptInterface<IEffect> TickingEffect;
	int32 TickModulus;
	float ExpirationTime;
	int32 TickID;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GENESTEALER_API UEffectContainerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEffectContainerComponent();
	
	void TryApplyEffectToContainerFromHitResult(TSubclassOf<AActor> BaseEffectClass, const FHitResult& Impact, AActor* InstigatingActor, bool bShouldRotateHitResult = true);
	void TryApplyEffectToContainer(TSubclassOf<AActor> BaseEffectClass, AActor* InstigatingActor);
	
	// This is static so that projectile/hitscan misses can still create the Sound/Visual FX on the ground 
	static TScriptInterface<IEffect> CreateEffectInstanceFromHitResult(UObject* ContextObject, TSubclassOf<AActor> BaseEffectClass, const FHitResult& Impact, AActor* InstigatingActor, bool bShouldRotateHitResult = true);
	
protected:
	virtual void BeginPlay() override;
	
private:
	TScriptInterface<IEffect> CreateEffectInstance(TSubclassOf<AActor> BaseEffectClass, AActor* InstigatingActor) const;

	void TryActivateEffect(TScriptInterface<IEffect> IncomingEffect);
	void Internal_ApplyEffect(TScriptInterface<IEffect> IncomingEffect);
	void Internal_RemoveEffectsWithTags(const TArray<FGameplayTag>& InTags, TScriptInterface<IEffect> IncomingEffect);
	
	void TickEffects();
	void Internal_TickEffect(const int32 CurrentTickingEffectKey);
	void StartTicking();
	void StopTicking();
	
	bool CanApplyEffect(TScriptInterface<IEffect> IncomingEffect) const;
	void AddEffectToTickContainer(TScriptInterface<IEffect> IncomingEffect);
	void ActivateEffect(TScriptInterface<IEffect> IncomingEffect) const;
	void DestroyEffect(TScriptInterface<IEffect> IncomingEffect, int32 TickID);

	TMap<int32, FTickingEffect> EffectsToTick;
	TScriptInterface<ITaggable> TaggableOwner;
	FTimerHandle Timer_EffectTicker;
	
	const float QuarterSecondTick = .25f;
	const float HalfSecondTick = .5f;
	const float FullSecondTick = 1.f;

	const float MaxCycles = 5;
	const float TotalTicksPerCycle = 4 * MaxCycles;
	
	int32 TickCounter = 1;
	int32 TickEntityIDCounter = 0;
	bool bIsTicking;
};