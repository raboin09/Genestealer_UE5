// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/EffectContainerComponent.h"

#include "API/Taggable.h"
#include "Effects/BaseEffect.h"
#include "Kismet/GameplayStatics.h"
#include "Utils/CombatUtils.h"
#include "Utils/SpawnUtils.h"

DECLARE_CYCLE_STAT(TEXT("Genestealer_EffectContainerTick"), STAT_Genestealer_StatsEffectContainerTicks, STATGROUP_StatSystem);

UEffectContainerComponent::UEffectContainerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UEffectContainerComponent::TryApplyEffectToContainerFromHitResult(TSubclassOf<AActor> BaseEffectClass, const FHitResult& Impact, AActor* InstigatingActor, bool bShouldRotateHitResult)
{
	if(!BaseEffectClass || !BaseEffectClass->ImplementsInterface(UEffect::StaticClass()))
	{
		return;
	} 
	const TScriptInterface<IEffect> BaseEffect = CreateEffectInstanceFromHitResult(this, BaseEffectClass, Impact, InstigatingActor, bShouldRotateHitResult);
	Internal_ApplyEffect(BaseEffect);
}

void UEffectContainerComponent::TryApplyEffectToContainer(TSubclassOf<AActor> BaseEffectClass, AActor* InstigatingActor)
{
	if(!BaseEffectClass || !BaseEffectClass->ImplementsInterface(UEffect::StaticClass()))
	{
		return;
	} 
	const TScriptInterface<IEffect> BaseEffect = CreateEffectInstance(BaseEffectClass, InstigatingActor);
	Internal_ApplyEffect(BaseEffect);
}

void UEffectContainerComponent::BeginPlay()
{
	Super::BeginPlay();
	AActor* OwnerActor = GetOwner();
	if (OwnerActor && OwnerActor->GetClass()->ImplementsInterface(UTaggable::StaticClass()))
	{
		TScriptInterface<ITaggable> TaggableVar;
		TaggableVar.SetObject(GetOwner());
		TaggableVar.SetInterface(Cast<ITaggable>(GetOwner()));
		TaggableOwner = TaggableVar;
	}
}

TScriptInterface<IEffect> UEffectContainerComponent::CreateEffectInstanceFromHitResult(UObject* ContextObject, TSubclassOf<AActor> BaseEffectClass, const FHitResult& Impact, AActor* InstigatingActor, bool bShouldRotateHitResult)
{
	if(!ContextObject)
	{
		return nullptr;
	}
	
	FTransform SpawnTransform = FTransform(Impact.Location);
	if (bShouldRotateHitResult)
	{
		SpawnTransform = FTransform(UCombatUtils::GetRotationFromComponentHit(Impact), Impact.ImpactPoint);
	}

	ABaseEffect* EffectActor = USpawnUtils::SpawnActorToWorld_Deferred<ABaseEffect>(ContextObject, BaseEffectClass);
	if (!EffectActor)
	{
		return nullptr;
	}
		
	FEffectContext EffectContext;
	EffectContext.InstigatingActor = InstigatingActor;
	EffectContext.ReceivingActor = Impact.GetActor();
	EffectContext.SurfaceHit = Impact;
	EffectContext.HitDirection = Impact.ImpactNormal;
	EffectActor->SetEffectContext(EffectContext);
	USpawnUtils::FinishSpawningActor_Deferred(EffectActor, SpawnTransform);
	return EffectActor;
}

TScriptInterface<IEffect> UEffectContainerComponent::CreateEffectInstance(TSubclassOf<AActor> BaseEffectClass, AActor* InstigatingActor) const
{
	if (!GetOwner())
	{
		return nullptr;
	}

	const FTransform& SpawnTransform = GetOwner()->GetActorTransform();

	ABaseEffect* EffectActor = USpawnUtils::SpawnActorToWorld_Deferred<ABaseEffect>(GetOwner(), BaseEffectClass);
	if (!EffectActor)
	{
		return nullptr;
	}

	FEffectContext EffectContext;
	EffectContext.InstigatingActor = InstigatingActor;
	EffectContext.ReceivingActor = GetOwner();
	EffectActor->SetEffectContext(EffectContext);
	USpawnUtils::FinishSpawningActor_Deferred(EffectActor, SpawnTransform);
	return EffectActor;
}

void UEffectContainerComponent::Internal_ApplyEffect(TScriptInterface<IEffect> IncomingEffect)
{
	if (!IncomingEffect)
	{
		return;
	}
	const FEffectInitializationData& EffectInitData = IncomingEffect->GetEffectInitializationData();

	if(EffectInitData.bInfinite)
	{
		AddEffectToTickContainer(IncomingEffect);
	} else
	{
		switch (EffectInitData.EffectInterval)
		{
		case EEffectInterval::Instant:
			{
				TryActivateEffect(IncomingEffect);
				DestroyEffect(IncomingEffect, -1);
				break;
			}
		case EEffectInterval::Apply_Once:
		case EEffectInterval::Apply_Every_Quarter_Second:
		case EEffectInterval::Apply_Every_Half_Second:
		case EEffectInterval::Apply_Every_Second:
		case EEffectInterval::Apply_Every_Two_Seconds:
		case EEffectInterval::Apply_Every_Three_Seconds:
		case EEffectInterval::Apply_Every_Five_Seconds:
			{
				AddEffectToTickContainer(IncomingEffect);
				break;
			}
		default: ;
		}
	}
}

void UEffectContainerComponent::Internal_RemoveEffectsWithTags(const TArray<FGameplayTag>& InTags, TScriptInterface<IEffect> IncomingEffect)
{
	for(const FGameplayTag& CurrTag : InTags)
	{
		TArray<int32> EffectKeys;
		EffectsToTick.GetKeys(EffectKeys);
		for(const int32 Key : EffectKeys)
		{
			if(const auto Effect = EffectsToTick.Find(Key); Effect->TickingEffect)
			{
				if(Effect->TickingEffect->GetEffectTags().Contains(CurrTag) && Effect->TickingEffect != IncomingEffect)
				{
					Effect->TickingEffect->DestroyEffect();
					EffectsToTick.Remove(Key);
				}
			}
		}
	}
}

void UEffectContainerComponent::TryActivateEffect(TScriptInterface<IEffect> IncomingEffect)
{
	if(!IncomingEffect)
	{
		return;
	}
	
	Internal_RemoveEffectsWithTags(IncomingEffect->GetRemoveEffectTags(), IncomingEffect);
	if (CanApplyEffect(IncomingEffect))
	{
		IncomingEffect->ActivateEffect();
	}
}

void UEffectContainerComponent::TickEffects()
{
	SCOPE_CYCLE_COUNTER(STAT_Genestealer_StatsEffectContainerTicks);
	if (++TickCounter > TotalTicksPerCycle)
	{
		TickCounter = 1;
	}

	TArray<int32> MapKeys;
	EffectsToTick.GetKeys(MapKeys);
	if (!GetWorld())
	{
		return;
	}

	for (const int32 CurrentTickingEffectKey : MapKeys)
	{
		Internal_TickEffect(CurrentTickingEffectKey);
	}
}

void UEffectContainerComponent::Internal_TickEffect(const int32 CurrentTickingEffectKey)
{
	if(!EffectsToTick.Contains(CurrentTickingEffectKey))
	{
		return;
	}
	
	const FTickingEffect CurrentTickingEffect = *EffectsToTick.Find(CurrentTickingEffectKey);
	const TScriptInterface<IEffect> CurrentEffect = CurrentTickingEffect.TickingEffect;
	
	if(!CurrentEffect)
	{
		return;
	}
	
	const bool bIsInfinite = CurrentEffect->GetEffectInitializationData().bInfinite;
	if (!bIsInfinite)
	{
		if(GetWorld()->GetTimeSeconds() >= CurrentTickingEffect.ExpirationTime)
		{
			DestroyEffect(CurrentEffect, CurrentTickingEffectKey);
			return;
		}
	}
	
	if (TickCounter % CurrentTickingEffect.TickModulus == 0){
			TryActivateEffect(CurrentEffect);
	}
}

void UEffectContainerComponent::StartTicking()
{
	if(bIsTicking)
	{
		return;
	}
	
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	World->GetTimerManager().SetTimer(Timer_EffectTicker, this, &UEffectContainerComponent::TickEffects, QuarterSecondTick, true);
	bIsTicking = true;
}

void UEffectContainerComponent::StopTicking()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	World->GetTimerManager().ClearTimer(Timer_EffectTicker);
	bIsTicking = false;
}

bool UEffectContainerComponent::CanApplyEffect(TScriptInterface<IEffect> IncomingEffect) const
{
	if (!IncomingEffect)
	{
		return false;
	}
	
	const FGameplayTagContainer BlockedTagContainer = FGameplayTagContainer::CreateFromArray(IncomingEffect->GetBlockedTags());
	if (TaggableOwner->GetTagContainer().HasAny(BlockedTagContainer))
	{
		return false;
	}

	const FGameplayTagContainer RequiredTagsContainer = FGameplayTagContainer::CreateFromArray(IncomingEffect->GetRequiredTags());
	if (!TaggableOwner->GetTagContainer().HasAll(RequiredTagsContainer))
	{
		return false;
	}
	return true;
}

void UEffectContainerComponent::AddEffectToTickContainer(TScriptInterface<IEffect> IncomingEffect)
{
	if (!IncomingEffect || !GetWorld())
	{
		return;
	}

	IncomingEffect->ActivateEffect();

	FTickingEffect TickingEffect;
	TickingEffect.TickID = ++TickEntityIDCounter;
	switch (IncomingEffect->GetEffectInitializationData().EffectInterval)
	{
	case EEffectInterval::Apply_Every_Quarter_Second:
		TickingEffect.TickModulus = 1;
		break;
	case EEffectInterval::Apply_Every_Half_Second:
		TickingEffect.TickModulus = 2;
		break;
	case EEffectInterval::Apply_Every_Second:
		TickingEffect.TickModulus = 4;
		break;
	case EEffectInterval::Apply_Every_Two_Seconds:
		TickingEffect.TickModulus = 8;
		break;
	case EEffectInterval::Apply_Every_Three_Seconds:
		TickingEffect.TickModulus = 12;
		break;
	case EEffectInterval::Apply_Every_Five_Seconds:
		TickingEffect.TickModulus = 20;
		break;
	default:
		TickingEffect.TickModulus = -1;
	}
	if(!IncomingEffect->GetEffectInitializationData().bInfinite)
	{
		TickingEffect.ExpirationTime = GetWorld()->GetTimeSeconds() + IncomingEffect->GetEffectInitializationData().EffectDuration;	
	}
	TickingEffect.TickingEffect = IncomingEffect;
	EffectsToTick.Add(TickingEffect.TickID, TickingEffect);
	StartTicking();
}

void UEffectContainerComponent::ActivateEffect(TScriptInterface<IEffect> IncomingEffect) const
{
	if (!IncomingEffect)
	{
		return;
	}
	IncomingEffect->ActivateEffect();
}

void UEffectContainerComponent::DestroyEffect(TScriptInterface<IEffect> IncomingEffect, int32 TickID)
{
	if (!IncomingEffect)
	{
		return;
	}

	if (EffectsToTick.Contains(TickID))
	{
		EffectsToTick.Remove(TickID);
	}

	if (EffectsToTick.Num() <= 0)
	{
		StopTicking();
	}

	IncomingEffect->DestroyEffect();
}
