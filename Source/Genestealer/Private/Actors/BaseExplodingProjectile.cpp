
#include "Actors/BaseExplodingProjectile.h"

#include "NiagaraFunctionLibrary.h"
#include "API/Attackable.h"
#include "Core/AudioManager.h"
#include "Genestealer/Genestealer.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Utils/EffectUtils.h"

ABaseExplodingProjectile::ABaseExplodingProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	ExplosionRadius = CreateDefaultSubobject<USphereComponent>(TEXT("ExplosionRadius"));
	ExplosionRadius->SetupAttachment(RootComponent);
	ExplosionRadius->SetSphereRadius(128.f);
	ExplosionRadius->SetCollisionResponseToAllChannels(ECR_Ignore);
	ExplosionRadius->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	ExplosionRadius->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	ExplosionRadius->SetGenerateOverlapEvents(true);
}

void ABaseExplodingProjectile::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if(!bExplodedAlready)
	{
		Internal_ExplodeAllActorsInRadius();
	}
	Super::EndPlay(EndPlayReason);
}

void ABaseExplodingProjectile::HandleActorDeath()
{
	if(!bExplodedAlready)
	{
		Internal_ExplodeAllActorsInRadius();
	}
	Super::HandleActorDeath();
}

void ABaseExplodingProjectile::K2_HandleImpact_Implementation(const FHitResult& HitResult)
{
	Internal_ExplodeAllActorsInRadius();
}

void ABaseExplodingProjectile::OnImpact(const FHitResult& HitResult)
{
	K2_HandleImpact_Implementation(HitResult);
	HandleActorDeath();
}

bool ABaseExplodingProjectile::Internal_TryTraceToOverlappedActor(const FHitResult& Impact, const FVector& StartTrace, AActor* TargetActor)
{
	if(TargetActor == Impact.GetActor())
	{
		UEffectUtils::ApplyEffectsToHitResult(ProjectileEffectsToApply, Impact, this);
		return true;
	}
	return false;
}

FHitResult ABaseExplodingProjectile::Internal_TraceToLocation(const FVector& StartTrace, const FVector& EndTrace) const
{
	FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(WeaponTrace), true, GetInstigator());
	TraceParams.bReturnPhysicalMaterial = true;
	FHitResult Hit(ForceInit);
	TArray<AActor*> IgnoreActors; 
	IgnoreActors.Add(GetInstigator());
	auto DrawDebugTrace = EDrawDebugTrace::None;
	UKismetSystemLibrary::LineTraceSingle(this, StartTrace, EndTrace,  UEngineTypes::ConvertToTraceType(GENESTEALER_TRACE_WEAPON), false, IgnoreActors, DrawDebugTrace, Hit, true, FLinearColor::Red, FLinearColor::Green, 10.f);
	return Hit;
}

void ABaseExplodingProjectile::Internal_ExplodeAllActorsInRadius()
{
	if(!ExplosionRadius)
	{
		return;
	}
	bExplodedAlready = true;
	TArray<AActor*> OverlappingActors;
	ExplosionRadius->GetOverlappingActors(OverlappingActors);
	
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetOwner(), ExplosionNiagara, GetActorLocation(), GetActorRotation());
	UAudioManager::SpawnSoundAtLocation(GetOwner(), ExplosionSound, GetActorLocation(), GetActorRotation());

	const FVector StartTrace = GetActorLocation();
	
	for(AActor* CurrActor : OverlappingActors)
	{
		if(!CurrActor)
		{
			continue;
		}
		
		if(const IAttackable* AttackableCast = Cast<IAttackable>(CurrActor))
		{
			const FVector& TargetPelvisTrace = AttackableCast->GetPelvisLocation();
			const FVector& TargetHeadTrace = AttackableCast->GetHeadLocation();
			if(const FHitResult& PelvisImpact = Internal_TraceToLocation(StartTrace, TargetPelvisTrace); PelvisImpact.bBlockingHit)
			{
				if(Internal_TryTraceToOverlappedActor(PelvisImpact, StartTrace, CurrActor)) {	continue; }
			}			
			if(const FHitResult& HeadImpact = Internal_TraceToLocation(StartTrace, TargetHeadTrace); HeadImpact.bBlockingHit)
			{
				Internal_TryTraceToOverlappedActor(HeadImpact, StartTrace, CurrActor);
			}
		}
	}
}
