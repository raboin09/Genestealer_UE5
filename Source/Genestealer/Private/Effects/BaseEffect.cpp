// Fill out your copyright notice in the Description page of Project Settings.


#include "Effects/BaseEffect.h"
#include "NiagaraComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "API/Taggable.h"
#include "Core/AudioManager.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Utils/CombatUtils.h"
#include "Utils/EffectUtils.h"
#include "Utils/GameplayTagUtils.h"

ABaseEffect::ABaseEffect()
{
 	PrimaryActorTick.bCanEverTick = false;
	SetAutoDestroyWhenFinished(false);
	// if(!EffectDataObj)
	// {
	// 	EffectDataObj = Cast<UBaseEffectData>(UBaseEffectData::StaticClass()->GetDefaultObject());
	// }
}

void ABaseEffect::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	K2_OnDestroyEffect();
	Super::EndPlay(EndPlayReason);
}

void ABaseEffect::K2_ActivateEffect_Implementation()
{
	check(false)
}

void ABaseEffect::K2_OnDestroyEffect_Implementation()
{
	if(EffectVFX)
	{
		EffectVFX->Deactivate();
	}
}

void ABaseEffect::Internal_PlayEffectSound()
{
	if(K2_GetEffectSound())
	{
		UAudioManager::SpawnSoundAtLocation(EffectContext.InstigatingActor, K2_GetEffectSound(), GetActorLocation());
	}
}

void ABaseEffect::Internal_PlayEffectParticleSystem()
{
	const bool bReceivingActorIsPawn = EffectContext.ReceivingActor ? EffectContext.ReceivingActor->IsA(APawn::StaticClass()) : false;
	const bool bShouldAttachVFX = EffectDataObj ? EffectDataObj->bAttachVFXToActor : false; 
	if(!bShouldAttachVFX || !bReceivingActorIsPawn)
	{
		if(UNiagaraSystem* CastedNiagaraSystem = Cast<UNiagaraSystem>(K2_GetEffectParticleSystem()))
		{
			EffectVFX = UNiagaraFunctionLibrary::SpawnSystemAtLocation(EffectContext.InstigatingActor, CastedNiagaraSystem, GetActorLocation(), GetActorRotation());
		}
		else if(UParticleSystem* CastedParticleSystem = Cast<UParticleSystem>(K2_GetEffectParticleSystem()))
		{
			EffectVFX = UGameplayStatics::SpawnEmitterAtLocation(EffectContext.InstigatingActor, CastedParticleSystem, GetActorLocation(), GetActorRotation());
		}
	} else
	{
		if(UMeshComponent* ActorMesh = EffectContext.ReceivingActor->FindComponentByClass<USkeletalMeshComponent>())
		{
			if(UNiagaraSystem* CastedNiagaraSystem = Cast<UNiagaraSystem>(K2_GetEffectParticleSystem()))
			{
				EffectVFX = UNiagaraFunctionLibrary::SpawnSystemAttached(CastedNiagaraSystem, ActorMesh, "spine_03", FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTargetIncludingScale, true);
			}
			else if(UParticleSystem* CastedParticleSystem = Cast<UParticleSystem>(K2_GetEffectParticleSystem()))
			{
				EffectVFX = UGameplayStatics::SpawnEmitterAttached(CastedParticleSystem, ActorMesh, "spine_03", FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTargetIncludingScale, true);
			}
		}
	}
}

void ABaseEffect::Internal_AddAndRemoveTagsFromReceiver_Activation()
{
	const ITaggable* TaggableReceiver = Cast<ITaggable>(EffectContext.ReceivingActor);
	if(!TaggableReceiver)
	{
		return;
	}
	UGameplayTagUtils::AddTagsToActor(EffectContext.ReceivingActor, GetEffectInitializationData().TagsToApply);
	UGameplayTagUtils::RemoveTagsFromActor(EffectContext.ReceivingActor, GetEffectInitializationData().TagsToRemove);
}

void ABaseEffect::Internal_AddAndRemoveTagsFromReceiver_Deactivation()
{
	if(!EffectDataObj || !EffectDataObj->EffectData.bShouldReverseEffectsOnDestroy)
	{
		return;
	}
	
	const ITaggable* TaggableReceiver = Cast<ITaggable>(EffectContext.ReceivingActor);
	if(!TaggableReceiver)
	{
		return;
	}
	UGameplayTagUtils::RemoveTagsFromActor(EffectContext.ReceivingActor, GetEffectInitializationData().TagsToApply);
}

bool ABaseEffect::Internal_IsValidHeadshot() const
{
	bool bPlayerControlled = false;	
	if(const APawn* CastedPawn = Cast<APawn>(EffectContext.SurfaceHit.GetActor()))
	{
		bPlayerControlled = CastedPawn->IsPlayerControlled();	
	}
	return UCombatUtils::IsBoneNameHead(EffectContext.SurfaceHit.BoneName) && !bPlayerControlled;
}

void ABaseEffect::PlayEffectFX()
{
	Internal_PlayEffectSound();
	Internal_PlayEffectParticleSystem();
}

void ABaseEffect::ActivateEffect()
{
	PlayEffectFX();
	Internal_AddAndRemoveTagsFromReceiver_Activation();
	K2_ActivateEffect();
}

void ABaseEffect::DestroyEffect()
{
	Internal_AddAndRemoveTagsFromReceiver_Deactivation();
	if(IsValid(this))
	{
		SetLifeSpan(1.f);
	}
}

UFXSystemAsset* ABaseEffect::K2_GetEffectParticleSystem_Implementation()
{
	if(!EffectDataObj || EffectDataObj->ImpactVFXRowHandle.IsNull())
	{
		return nullptr;
	}
	const UPhysicalMaterial* PhysicalMaterial = EffectContext.SurfaceHit.PhysMaterial.Get();
	return UEffectUtils::GetVFXAssetFromKey(EffectDataObj->ImpactVFXRowHandle, PhysicalMaterial, Internal_IsValidHeadshot());
}

USoundCue* ABaseEffect::K2_GetEffectSound_Implementation()
{
	if(!EffectDataObj || EffectDataObj->ImpactSFXRowHandle.IsNull())
	{
		return nullptr;
	}
	const UPhysicalMaterial* PhysicalMaterial = EffectContext.SurfaceHit.PhysMaterial.Get();
	return UEffectUtils::GetSFXAssetFromKey(EffectDataObj->ImpactSFXRowHandle, PhysicalMaterial, Internal_IsValidHeadshot());
}