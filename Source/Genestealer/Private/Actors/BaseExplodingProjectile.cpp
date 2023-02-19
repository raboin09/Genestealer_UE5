
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

void ABaseExplodingProjectile::Internal_ExplodeAllActorsInRadius()
{
	bExplodedAlready = true;
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetOwner(), ExplosionNiagara, GetActorLocation(), GetActorRotation());
	UAudioManager::SpawnSoundAtLocation(GetOwner(), ExplosionSound, GetActorLocation(), GetActorRotation());
	UEffectUtils::ApplyEffectsToHitResultsInRadius(GetOwner(), ProjectileEffectsToApply, GetActorLocation(), ExplosionRadius, UEngineTypes::ConvertToTraceType(GENESTEALER_TRACE_WEAPON), bFriendlyFire ? EAffectedAffiliation::All : EAffectedAffiliation::Enemies, true, GetActorLocation());
}
