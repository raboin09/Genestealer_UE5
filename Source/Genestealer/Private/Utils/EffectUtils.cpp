// Fill out your copyright notice in the Description page of Project Settings.


#include "Utils/EffectUtils.h"
#include "Characters/BaseCharacter.h"
#include "API/Attackable.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "API/Effectible.h"
#include "Characters//HealthComponent.h"
#include "Effects/BaseStatModifierEffect.h"
#include "Genestealer/Genestealer.h"

void UEffectUtils::ApplyEffectToActor(AActor* ReceivingActor, TSubclassOf<AActor> EffectToApply)
{
	if (!ReceivingActor || !EffectToApply)
	{
		return;
	}

	IEffectible* EffectibleCast = Cast<IEffectible>(ReceivingActor);
	if (!EffectibleCast)
	{
		return;
	}
	UEffectContainerComponent* EffectContainerComponent = EffectibleCast->GetEffectContainerComponent();
	EffectContainerComponent->TryApplyEffectToContainer(EffectToApply, ReceivingActor);
}

void UEffectUtils::ApplyEffectsToActor(TArray<TSubclassOf<AActor>> EffectsToApply, AActor* ReceivingActor)
{
	for (const TSubclassOf<AActor> EffectClass : EffectsToApply)
	{
		ApplyEffectToActor(ReceivingActor, EffectClass);
	}
}

void UEffectUtils::ApplyEffectsToHitResult(TArray<TSubclassOf<AActor>> EffectsToApply, const FHitResult& Impact, AActor* InstigatingActor, bool bShouldRotateHitResult)
{
	for(const TSubclassOf<AActor> EffectClass : EffectsToApply)
	{
		ApplyEffectToHitResult(EffectClass, Impact, InstigatingActor, bShouldRotateHitResult);
	}
}

void UEffectUtils::ApplyEffectToHitResult(TSubclassOf<AActor> BaseEffectClass, const FHitResult& Impact, AActor* InstigatingActor, bool bShouldRotateHitResult)
{
	if (!InstigatingActor || !BaseEffectClass)
	{
		return;
	}

	AActor* HitActor = Impact.GetActor();
	if (!HitActor || !HitActor->GetClass())
	{
		return;
	}

	if (!HitActor->GetClass()->ImplementsInterface(UEffectible::StaticClass()))
	{
		return;
	}

	IEffectible* EffectibleCast = Cast<IEffectible>(Impact.GetActor());
	if (!EffectibleCast)
	{
		return;
	}
	UEffectContainerComponent* EffectContainerComponent = EffectibleCast->GetEffectContainerComponent();
	EffectContainerComponent->TryApplyEffectToContainerFromHitResult(BaseEffectClass, Impact, InstigatingActor, bShouldRotateHitResult);
}

void UEffectUtils::TryAddMaxWoundsToActor(AActor* ReceivingActor, float MaxWoundsToAdd)
{
	if (IAttackable* Attackable = Cast<IAttackable>(ReceivingActor))
	{
		UHealthComponent* HealthComponent = Attackable->GetHealthComponent();
		if (!HealthComponent)
		{
			return;
		}
		HealthComponent->AddMaxWounds(MaxWoundsToAdd);
	}
}

void UEffectUtils::TryApplyHealToActor(AActor* ReceivingActor, AActor* InstigatingActor, float Heal)
{
	if (IAttackable* Attackable = Cast<IAttackable>(ReceivingActor))
	{
		UHealthComponent* HealthComponent = Attackable->GetHealthComponent();
		if (!HealthComponent)
		{
			return;
		}
		HealthComponent->ApplyHeal(Heal, ReceivingActor, InstigatingActor);
	}
}

void UEffectUtils::TryApplyDamageToActor(AActor* ReceivingActor, AActor* InstigatingActor, float Damage, const FDamageHitReactEvent& HitReactEvent)
{
	if (IAttackable* Attackable = Cast<IAttackable>(ReceivingActor))
	{
		UHealthComponent* HealthComponent = Attackable->GetHealthComponent();
		if (!HealthComponent)
		{
			return;
		}
		HealthComponent->TakeDamage(Damage, ReceivingActor, InstigatingActor, HitReactEvent);
	}
}

UFXSystemAsset* UEffectUtils::GetVFXAssetFromKey(const FDataTableRowHandle& RowHandle, UPhysicalMaterial* SurfaceMaterial, bool bIsValidHeadshot)
{
	if(RowHandle.IsNull() || RowHandle.RowName.IsNone())
	{
		return nullptr;
	}
	
	const FEffectImpactVFX* FoundRow = RowHandle.DataTable->FindRow<FEffectImpactVFX>(RowHandle.RowName, SurfaceMaterial->GetFullName());
	if(!FoundRow)
	{
		return nullptr;
	}	
	const FEffectImpactVFX& OutRow = *FoundRow;
	
	if (bIsValidHeadshot && OutRow.FleshHeadshotFX)
	{
		return OutRow.FleshHeadshotFX;
	}
	
	if(!IsValid(SurfaceMaterial))
	{
		return OutRow.DefaultFX;
	}

	UNiagaraSystem* SelectedParticle;
	switch (SurfaceMaterial->SurfaceType)
	{
	case GENESTEALER_SURFACE_Default: SelectedParticle = OutRow.DefaultFX;
		break;
	case GENESTEALER_SURFACE_Concrete: SelectedParticle = OutRow.ConcreteFX;
		break;
	case GENESTEALER_SURFACE_Dirt: SelectedParticle = OutRow.DirtFX;
		break;
	case GENESTEALER_SURFACE_Water: SelectedParticle = OutRow.WaterFX;
		break;
	case GENESTEALER_SURFACE_Metal: SelectedParticle = OutRow.MetalFX;
		break;
	case GENESTEALER_SURFACE_Wood: SelectedParticle = OutRow.WoodFX;
		break;
	case GENESTEALER_SURFACE_Grass: SelectedParticle = OutRow.GrassFX;
		break;
	case GENESTEALER_SURFACE_Glass: SelectedParticle = OutRow.GlassFX;
		break;
	case GENESTEALER_SURFACE_Flesh: SelectedParticle = OutRow.FleshFX;
		break;
	case GENESTEALER_SURFACE_Plastic: SelectedParticle = OutRow.PlasticFX;
		break;
	case GENESTEALER_SURFACE_Sand: SelectedParticle = OutRow.SandFX;
		break;
	case GENESTEALER_SURFACE_Ice: SelectedParticle = OutRow.FleshFX;
		break;
	default: SelectedParticle = OutRow.DefaultFX;
	}
	return SelectedParticle ? SelectedParticle : OutRow.DefaultFX;
}

USoundCue* UEffectUtils::GetSFXAssetFromKey(const FDataTableRowHandle& RowHandle, UPhysicalMaterial* SurfaceMaterial, bool bIsValidHeadshot)
{
	if(RowHandle.IsNull() || RowHandle.RowName.IsNone())
	{
		return nullptr;
	}
	
	const FEffectImpactSFX* FoundRow = RowHandle.DataTable->FindRow<FEffectImpactSFX>(RowHandle.RowName, SurfaceMaterial->GetFullName());
	if(!FoundRow)
	{
		return nullptr;
	}
	
	const FEffectImpactSFX& OutRow = *FoundRow;	
	if (bIsValidHeadshot && OutRow.FleshHeadshotSound)
	{
		return OutRow.FleshHeadshotSound;
	}

	if(!IsValid(SurfaceMaterial))
	{
		return OutRow.DefaultSound;
	}

	USoundCue* SelectedSound;
	switch (SurfaceMaterial->SurfaceType)
	{
	case GENESTEALER_SURFACE_Concrete: SelectedSound = OutRow.ConcreteSound;
		break;
	case GENESTEALER_SURFACE_Dirt: SelectedSound = OutRow.DirtSound;
		break;
	case GENESTEALER_SURFACE_Water: SelectedSound = OutRow.WaterSound;
		break;
	case GENESTEALER_SURFACE_Metal: SelectedSound = OutRow.MetalSound;
		break;
	case GENESTEALER_SURFACE_Wood: SelectedSound = OutRow.WoodSound;
		break;
	case GENESTEALER_SURFACE_Grass: SelectedSound = OutRow.GrassSound;
		break;
	case GENESTEALER_SURFACE_Glass: SelectedSound = OutRow.GlassSound;
		break;
	case GENESTEALER_SURFACE_Flesh: SelectedSound = OutRow.FleshSound;
		break;
	case GENESTEALER_SURFACE_Plastic: SelectedSound = OutRow.PlasticSound;
		break;
	case GENESTEALER_SURFACE_Sand: SelectedSound = OutRow.SandSound;
		break;
	case GENESTEALER_SURFACE_Ice: SelectedSound = OutRow.IceSound;
		break;
	default: SelectedSound = OutRow.DefaultSound;
	}
	return SelectedSound ? SelectedSound : OutRow.DefaultSound;
}

