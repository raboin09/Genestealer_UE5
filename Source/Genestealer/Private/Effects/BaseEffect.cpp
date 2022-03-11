// Fill out your copyright notice in the Description page of Project Settings.


#include "Effects/BaseEffect.h"



#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "API/Taggable.h"
#include "Core/AudioManager.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Utils/CombatUtils.h"
#include "Utils/EffectUtils.h"

ABaseEffect::ABaseEffect()
{
 	PrimaryActorTick.bCanEverTick = false;
	SetAutoDestroyWhenFinished(false);
}

void ABaseEffect::K2_ActivateEffect_Implementation()
{
	check(false)
}

void ABaseEffect::K2_OnDestroyEffect_Implementation()
{
	
}

void ABaseEffect::Internal_PlayEffectSound()
{
	if(K2_GetEffectSound())
	{
		UAudioManager::PlaySoundAtLocation(this, K2_GetEffectSound(), GetActorLocation());
	}
}

void ABaseEffect::Internal_PlayEffectParticleSystem()
{
	if(UNiagaraSystem* CastedNiagaraSystem = Cast<UNiagaraSystem>(K2_GetEffectParticleSystem()))
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, CastedNiagaraSystem, GetActorLocation(), GetActorRotation());
	}
	else if(UParticleSystem* CastedParticleSystem = Cast<UParticleSystem>(K2_GetEffectParticleSystem()))
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, CastedParticleSystem, GetActorLocation(), GetActorRotation());
	}
}

void ABaseEffect::Internal_AddAndRemoveTagsFromReceiver_Activation()
{
	ITaggable* TaggableReceiver = Cast<ITaggable>(EffectContext.ReceivingActor);
	if(!TaggableReceiver)
	{
		return;
	}
	TaggableReceiver->GetTagContainer().AppendTags(FGameplayTagContainer::CreateFromArray(GetEffectInitializationData().TagsToApply));
	TaggableReceiver->GetTagContainer().RemoveTags(FGameplayTagContainer::CreateFromArray(GetEffectInitializationData().TagsToRemove));
}

void ABaseEffect::Internal_AddAndRemoveTagsFromReceiver_Deactivation()
{
	if(!EffectDataObj->EffectData.bShouldReverseEffectsOnDestroy)
	{
		return;
	}
	
	ITaggable* TaggableReceiver = Cast<ITaggable>(EffectContext.ReceivingActor);
	if(!TaggableReceiver)
	{
		return;
	}
	TaggableReceiver->GetTagContainer().RemoveTags(FGameplayTagContainer::CreateFromArray(GetEffectInitializationData().TagsToApply));
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
	K2_OnDestroyEffect();
	if(IsValid(this))
	{
		Destroy();
	}
}

UFXSystemAsset* ABaseEffect::K2_GetEffectParticleSystem_Implementation()
{
	if(!EffectDataObj || EffectDataObj->ImpactVFXRowHandle.IsNull())
	{
		return nullptr;
	}
	UPhysicalMaterial* PhysicalMaterial = EffectContext.SurfaceHit.PhysMaterial.Get();
	return UEffectUtils::GetVFXAssetFromKey(EffectDataObj->ImpactVFXRowHandle, PhysicalMaterial, Internal_IsValidHeadshot());
}

USoundCue* ABaseEffect::K2_GetEffectSound_Implementation()
{
	if(!EffectDataObj || EffectDataObj->ImpactSFXRowHandle.IsNull())
	{
		return nullptr;
	}
	UPhysicalMaterial* PhysicalMaterial = EffectContext.SurfaceHit.PhysMaterial.Get();
	return UEffectUtils::GetSFXAssetFromKey(EffectDataObj->ImpactSFXRowHandle, PhysicalMaterial, Internal_IsValidHeadshot());
}