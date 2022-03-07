// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapons/HitscanWeapon.h"
#include "NiagaraFunctionLibrary.h"
#include "Characters/BaseCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "Utils/CoreUtils.h"

void AHitscanWeapon::FireWeapon()
{
	const int32 RandomSeed = FMath::Rand();
	const FRandomStream WeaponRandomStream(RandomSeed);
	const float CurrentSpread = GetCurrentSpread();
	const float ConeHalfAngle = FMath::DegreesToRadians(CurrentSpread * 0.5f);

	const FVector AimDir = GetAdjustedAim();
	const FVector StartTrace = GetCameraDamageStartLocation(AimDir);
	FVector ShootDir = WeaponRandomStream.VRandCone(AimDir, ConeHalfAngle, ConeHalfAngle);
	const FVector EndTrace = StartTrace + ShootDir * RangedWeaponInstant.WeaponRange;
	const FHitResult Impact = WeaponTrace(StartTrace, EndTrace);
	ProcessInstantHit(Impact, StartTrace, ShootDir, RandomSeed, CurrentSpread);
	// DebugFire(StartTrace, EndTrace, FColor::Blue);
	CurrentFiringSpread = FMath::Min(RangedWeaponInstant.FiringSpreadMax, CurrentFiringSpread + RangedWeaponInstant.FiringSpreadIncrement);
}

void AHitscanWeapon::OnHit(const FHitResult& Impact, FVector_NetQuantizeNormal ShootDir, int32 RandomSeed, float ReticleSpread)
{
	const float WeaponAngleDot = FMath::Abs(FMath::Sin(ReticleSpread * PI / 180.f));

	if (GetInstigator() && (Impact.GetActor() || Impact.bBlockingHit))
	{
		const FVector Origin = GetMuzzleLocation();
		const FVector ViewDir = (Impact.Location - Origin).GetSafeNormal();
		const float ViewDotHitDir = FVector::DotProduct(GetInstigator()->GetViewRotation().Vector(), ViewDir);
		if (ViewDotHitDir > RangedWeaponInstant.AllowedViewDotHitDir - WeaponAngleDot)
		{
			if (CurrentState != EWeaponState::Idle)
			{
				if (Impact.GetActor() == nullptr)
				{
					if (Impact.bBlockingHit)
					{
						ProcessInstantHit_Confirmed(Impact, Origin, ShootDir, RandomSeed, ReticleSpread);
					}
				}
				else if (Impact.GetActor()->IsRootComponentStatic() || Impact.GetActor()->IsRootComponentStationary())
				{
					ProcessInstantHit_Confirmed(Impact, Origin, ShootDir, RandomSeed, ReticleSpread);
				}
				else
				{
					const FBox HitBox = Impact.GetActor()->GetComponentsBoundingBox();

					FVector BoxExtent = 0.5 * (HitBox.Max - HitBox.Min);
					BoxExtent *= 200.f;
					BoxExtent.X = FMath::Max(20.0f, BoxExtent.X);
					BoxExtent.Y = FMath::Max(20.0f, BoxExtent.Y);
					BoxExtent.Z = FMath::Max(20.0f, BoxExtent.Z);

					// Get the box center
					const FVector BoxCenter = (HitBox.Min + HitBox.Max) * 0.5;

					// if we are within client tolerance
					if (FMath::Abs(Impact.Location.Z - BoxCenter.Z) < BoxExtent.Z &&
						FMath::Abs(Impact.Location.X - BoxCenter.X) < BoxExtent.X &&
						FMath::Abs(Impact.Location.Y - BoxCenter.Y) < BoxExtent.Y)
					{
						ProcessInstantHit_Confirmed(Impact, Origin, ShootDir, RandomSeed, ReticleSpread);
					}
				}
			}
		}
	}
}

void AHitscanWeapon::OnMiss(FVector_NetQuantizeNormal ShootDir, int32 RandomSeed, float ReticleSpread)
{
	const FVector Origin = GetMuzzleLocation();

	// play FX on remote clients
	HitNotify.Origin = Origin;
	HitNotify.RandomSeed = RandomSeed;
	HitNotify.ReticleSpread = ReticleSpread;
	const FVector EndTrace = Origin + ShootDir * RangedWeaponInstant.WeaponRange;
	SpawnTrailEffect(EndTrace);
}

void AHitscanWeapon::ProcessInstantHit(const FHitResult& Impact, const FVector& Origin, FVector& ShootDir, int32 RandomSeed, float ReticleSpread)
{
	if (OwningPawn && OwningPawn->IsLocallyControlled() && GetNetMode() == NM_Client)
	{
		// if we're a client and we've hit something that is being controlled by the server
		if (Impact.GetActor() && Impact.GetActor()->GetRemoteRole() == ROLE_Authority)
		{
			// notify the server of the hit
			OnHit(Impact, ShootDir, RandomSeed, ReticleSpread);
		}
		else if (Impact.GetActor() == nullptr)
		{
			if (Impact.bBlockingHit)
			{
				// notify the server of the hit
				OnHit(Impact, ShootDir, RandomSeed, ReticleSpread);
			}
			else
			{
				// notify server of the miss
				OnMiss(ShootDir, RandomSeed, ReticleSpread);
			}
		}
	}

	// process a confirmed hit
	ProcessInstantHit_Confirmed(Impact, Origin, ShootDir, RandomSeed, ReticleSpread);
}

void AHitscanWeapon::ProcessInstantHit_Confirmed(const FHitResult& Impact, const FVector& Origin, FVector& ShootDir, int32 RandomSeed, float ReticleSpread)
{	
	HitNotify.Origin = Origin;
	HitNotify.RandomSeed = RandomSeed;
	HitNotify.ReticleSpread = ReticleSpread;
	const FVector EndTrace = Origin + ShootDir * RangedWeaponInstant.WeaponRange;
	const FVector EndPoint = Impact.GetActor() ? Impact.ImpactPoint : EndTrace;
	SpawnTrailEffect(EndPoint);
	SpawnFlybySound(Impact, ShootDir);
	ApplyWeaponEffectsToActor(AdjustHitResultIfNoValidHitComponent(Impact));
}

void AHitscanWeapon::OnBurstFinished()
{
	Super::OnBurstFinished();
	CurrentFiringSpread = 0.0f;
}

float AHitscanWeapon::GetCurrentSpread() const
{
	return RangedWeaponInstant.WeaponSpread + CurrentFiringSpread;
}

float AHitscanWeapon::GetCurrentFiringSpreadPercentage() const
{
	return CurrentFiringSpread / RangedWeaponInstant.FiringSpreadMax;
}

void AHitscanWeapon::SpawnTrailEffect(const FVector& EndPoint)
{
	if (RangedWeaponInstant.TrailFX)
	{
		const FVector Origin = GetMuzzleLocation();
		
		if (UParticleSystemComponent* TrailPSC = UGameplayStatics::SpawnEmitterAtLocation(this, RangedWeaponInstant.TrailFX, Origin))
		{
			TrailPSC->SetVectorParameter(RangedWeaponInstant.TrailTargetParam, EndPoint);
		}
	}
}

void AHitscanWeapon::SpawnFlybySound(const FHitResult& Impact, FVector& ShootDir) const
{
	// Translated to C++ from here https://www.youtube.com/watch?v=w7zGmwuRX_Q
	ABaseCharacter* CurrChar = UCoreUtils::GetPlayerCharacter(this);
	if(!CurrChar || CurrChar == GetOwningPawn()|| !RangedWeaponInstant.DegreesCurve || !RangedWeaponInstant.FlyBySound)
		return;
	
	FVector DiffVector = CurrChar->GetActorLocation() - GetOwningPawn()->GetActorLocation();
	DiffVector.Normalize();
	ShootDir.Normalize();
	const float LengthPlayer = CurrChar->GetDistanceTo(GetOwningPawn());
	const float ACOSd = UKismetMathLibrary::DegAcos(FVector::DotProduct(DiffVector, ShootDir));
	const float TraceLength = (Impact.ImpactPoint - Impact.TraceStart).Size();
	if(ACOSd <= RangedWeaponInstant.DegreesCurve->GetFloatValue(LengthPlayer))
	{
		const FVector WorldLoc = GetOwningPawn()->GetRootComponent()->GetComponentLocation();
		FVector DirectionVector = (UKismetMathLibrary::GetDirectionUnitVector(WorldLoc, Impact.ImpactPoint) * LengthPlayer) + WorldLoc;
		FTimerHandle PlayFlybyHandle;
		GetWorld()->GetTimerManager().SetTimer(PlayFlybyHandle, [this, DirectionVector]() {
			UGameplayStatics::PlaySoundAtLocation(this, RangedWeaponInstant.FlyBySound, DirectionVector, FRotator::ZeroRotator);
		}, LengthPlayer / 31200, false);
	}
}