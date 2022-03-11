// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "Effect.generated.h"

UENUM(BlueprintType)
enum class EModifierOperator : uint8 
{
	Add,
	Subtract,
	Multiply,
};

UENUM(BlueprintType)
enum class EModifierActor : uint8 
{
	Instigator,
	Receiver
};

USTRUCT(BlueprintType)
struct FModifierExpression
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	EModifierActor ModifierActor;
	UPROPERTY(EditAnywhere)
	EModifierOperator Operation;
	UPROPERTY(EditAnywhere)
	float ModifierOperand;
};

USTRUCT(BlueprintType)
struct FEffectContext
{
	GENERATED_BODY()

	FEffectContext()
	{
		InstigatingActor = nullptr;
		ReceivingActor = nullptr;
	}

	FEffectContext(AActor* InOriginator, AActor* InReceiver)
	{
		InstigatingActor = InOriginator;
		ReceivingActor = InReceiver;
	}

	// Actor that the effect is being applied to
	UPROPERTY(BlueprintReadOnly)
	AActor* ReceivingActor;
	// Actor that applied the effect
	UPROPERTY(BlueprintReadOnly)
	AActor* InstigatingActor;
	FHitResult SurfaceHit;
	FVector HitDirection;
};

UENUM()
enum class EEffectInterval : uint8
{
	Instant,
	Apply_Once,
	Apply_Every_Quarter_Second,
	Apply_Every_Half_Second,
	Apply_Every_Second,
	Apply_Every_Two_Seconds,
	Apply_Every_Three_Seconds,
	Apply_Every_Five_Seconds
};

USTRUCT()
struct FEffectValidTargets
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category=Defaults)
	TArray<FGameplayTag> RequiredTags;
	UPROPERTY(EditDefaultsOnly, Category=Defaults)
	TArray<FGameplayTag> BlockedTags;
};

USTRUCT()
struct FEffectInitializationData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category=Defaults, meta=(EditCondition = "EffectInterval != EEffectInterval::Instant", EditConditionHides))
	bool bInfinite = false;
	UPROPERTY(EditDefaultsOnly, Category=Defaults)
	FEffectValidTargets ValidTargets;
	UPROPERTY(EditDefaultsOnly, Category=Defaults, meta=(ClampMin="1.0", EditCondition = "bInfinite != true && EffectInterval != EEffectInterval::Instant", EditConditionHides))
	float EffectDuration = 1.f;
	UPROPERTY(EditDefaultsOnly, Category=Defaults)
	EEffectInterval EffectInterval = EEffectInterval::Instant;
	UPROPERTY(EditDefaultsOnly, Category=Defaults)
	TArray<FGameplayTag> TagsToApply;
	UPROPERTY(EditDefaultsOnly, Category=Defaults)
	bool bShouldReverseEffectsOnDestroy = true;
	UPROPERTY(EditDefaultsOnly, Category=Defaults)
	TArray<FGameplayTag> TagsToRemove;
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UEffect : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GENESTEALER_API IEffect
{
	GENERATED_BODY()

public:
	virtual void SetEffectContext(const FEffectContext& InContext) PURE_VIRTUAL(IEffect::SetEffectContext,)
	virtual const FEffectInitializationData& GetEffectInitializationData() = 0;
	virtual const TArray<FGameplayTag>& GetBlockedTags() const = 0;
	virtual const TArray<FGameplayTag>& GetRequiredTags() const= 0;
	virtual void PlayEffectFX() PURE_VIRTUAL(IEffect::PlayEffectFX,)
	virtual void ActivateEffect() PURE_VIRTUAL(IEffect::ActivateEffect,)
	virtual void DestroyEffect() PURE_VIRTUAL(IEffect::DestroyEffect,)
};
