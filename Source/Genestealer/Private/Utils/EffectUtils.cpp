// Fill out your copyright notice in the Description page of Project Settings.


#include "Utils/EffectUtils.h"
#include "Characters/BaseCharacter.h"
#include "API/Attackable.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Engine/DataTable.h"
#include "Characters//HealthComponent.h"
#include "Characters/EffectContainerComponent.h"
#include "Effects/BaseStatModifierEffect.h"
#include "Genestealer/Genestealer.h"
#include "Utils/WorldUtils.h"

void UEffectUtils::ApplyEffectsToHitResultsInRadius(AActor* InstigatingActor, TArray<TSubclassOf<AActor>> EffectsToApply, FVector TraceLocation, float TraceRadius, ETraceTypeQuery ValidationTraceType, bool bValidateHit, FVector ValidationTraceStartLocation, FName HitValidationBone)
{
	if(EffectsToApply.IsEmpty() || !InstigatingActor || TraceRadius < 1.f || TraceLocation.IsZero())
	{
		return;
	}

	TArray<FHitResult> HitResults;
	UKismetSystemLibrary::SphereTraceMulti(InstigatingActor, TraceLocation, TraceLocation, TraceRadius, UEngineTypes::ConvertToTraceType(GENESTEALER_TRACE_WEAPON), true, {}, EDrawDebugTrace::ForDuration, HitResults, true, FLinearColor::Red, FLinearColor::Green, 1.f);

	TArray<AActor*> AllHitActors;
	for(FHitResult SumHit : HitResults)
	{
		AllHitActors.AddUnique(SumHit.GetActor());
	}

	if(HitResults.Num() <= 0)
	{
		return;
	}
	
	if(bValidateHit)
	{
		TArray<AActor*> ValidatedHitActors;
		for(FHitResult ValidationHit : HitResults)
		{
			AActor* CurrActor = ValidationHit.GetActor(); 
			if(!CurrActor)
			{
				continue;
			}
		
			if(ValidatedHitActors.Contains(CurrActor))
			{
				continue;
			}

			USkeletalMeshComponent* MeshComponent = CurrActor->FindComponentByClass<USkeletalMeshComponent>();
			if(!MeshComponent || !MeshComponent->DoesSocketExist(HitValidationBone))
			{
				continue;
			}

			AllHitActors.Remove(CurrActor);

			FHitResult ValidationLineTraceHit;
			UKismetSystemLibrary::LineTraceSingle(InstigatingActor, ValidationTraceStartLocation, MeshComponent->GetSocketLocation(HitValidationBone), ValidationTraceType, true, AllHitActors, EDrawDebugTrace::ForDuration, ValidationLineTraceHit, true, FLinearColor::Red, FLinearColor::Green, 1.f);
			if(ValidationLineTraceHit.bBlockingHit)
			{
				ApplyEffectsToHitResult(EffectsToApply, ValidationLineTraceHit, InstigatingActor);
			}
			
			ValidatedHitActors.AddUnique(CurrActor);
			AllHitActors.AddUnique(CurrActor);
		}
	} else
	{
		for(FHitResult Hit : HitResults)
		{
			ApplyEffectsToHitResult(EffectsToApply, Hit, InstigatingActor);
		}
	}
}

void UEffectUtils::ApplyEffectAtLocation(AActor* InstigatingActor, TSubclassOf<AActor> EffectToApply, FVector Location, bool bActivateImmediately)
{
	FTransform SpawnTransform = FTransform();
	SpawnTransform.SetLocation(Location);

	ABaseEffect* EffectActor = UWorldUtils::SpawnActorToWorld_Deferred<ABaseEffect>(InstigatingActor, EffectToApply, InstigatingActor, Cast<APawn>(InstigatingActor));
	UWorldUtils::FinishSpawningActor_Deferred(EffectActor, SpawnTransform);
	if(bActivateImmediately)
	{
		EffectActor->ActivateEffect();
	}
}

void UEffectUtils::ApplyEffectToActor(AActor* ReceivingActor, TSubclassOf<AActor> EffectToApply)
{
	if (!ReceivingActor || !EffectToApply)
	{
		return;
	}

	IAttackable* EffectibleCast = Cast<IAttackable>(ReceivingActor);
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
	if (!InstigatingActor)
	{
		UKismetSystemLibrary::PrintString(InstigatingActor, "Bad Inst");	
	}

	if (!BaseEffectClass)
	{
		UKismetSystemLibrary::PrintString(InstigatingActor, "Bad Class");	
	}
	
	if (!InstigatingActor || !BaseEffectClass)
	{
		return;
	}

	AActor* HitActor = Impact.GetActor();
	if (!HitActor || !HitActor->GetClass())
	{
		return;
	}

	if (!HitActor->GetClass()->ImplementsInterface(UAttackable::StaticClass()))
	{
		return;
	}

	IAttackable* EffectibleCast = Cast<IAttackable>(Impact.GetActor());
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

UFXSystemAsset* UEffectUtils::GetVFXAssetFromKey(const FDataTableRowHandle& RowHandle, const UPhysicalMaterial* SurfaceMaterial, bool bIsValidHeadshot)
{
	if(RowHandle.IsNull() || RowHandle.RowName.IsNone())
	{
		return nullptr;
	}
	
	const FEffectImpactVFX* FoundRow = RowHandle.DataTable->FindRow<FEffectImpactVFX>(RowHandle.RowName, RowHandle.RowName.ToString());
	if(!FoundRow)
	{
		return nullptr;
	}	
	const FEffectImpactVFX& OutRow = *FoundRow;
	
	if (bIsValidHeadshot && (OutRow.FleshHeadshotFX || OutRow.NurgleFleshHeadshotFX))
	{
		if(SurfaceMaterial && SurfaceMaterial->SurfaceType == GENESTEALER_SURFACE_NurgleFlesh)
		{
			return OutRow.NurgleFleshHeadshotFX ? OutRow.NurgleFleshHeadshotFX : OutRow.FleshHeadshotFX;
		}		
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
	case GENESTEALER_SURFACE_NurgleFlesh: SelectedParticle = OutRow.NurgleFleshFX;
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

USoundCue* UEffectUtils::GetSFXAssetFromKey(const FDataTableRowHandle& RowHandle, const UPhysicalMaterial* SurfaceMaterial, bool bIsValidHeadshot)
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

	if(!IsValid(SurfaceMaterial))
	{
		return FoundRow->DefaultSound;
	}
	
	const FEffectImpactSFX& OutRow = *FoundRow;	
	if (bIsValidHeadshot && (OutRow.FleshHeadshotSound || OutRow.NurgleFleshHeadshotSound))
	{
		if(SurfaceMaterial && SurfaceMaterial->SurfaceType == GENESTEALER_SURFACE_NurgleFlesh)
		{
			return OutRow.NurgleFleshHeadshotSound ? OutRow.NurgleFleshHeadshotSound : OutRow.FleshHeadshotSound;
		}	
		return OutRow.FleshHeadshotSound;
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
	case GENESTEALER_SURFACE_NurgleFlesh: SelectedSound = OutRow.NurgleFleshSound;
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

