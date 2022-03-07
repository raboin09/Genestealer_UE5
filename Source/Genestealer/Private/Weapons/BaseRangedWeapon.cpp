// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/BaseRangedWeapon.h"
#include "AIController.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Characters//BaseCharacter.h"
#include "Components/AudioComponent.h"
#include "Characters//InventoryComponent.h"
#include "Core/BasePlayerController.h"
#include "GameFramework/Character.h"
#include "Genestealer/Genestealer.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Utils/CoreUtils.h"

bool ABaseRangedWeapon::CanReload()
{
	const bool bCanReload = OwningPawn != nullptr;
	const bool bGotAmmo = ( GetCurrentAmmoInClip() < GetRangedWeaponData().AmmoPerClip) && (CurrentAmmo - CurrentAmmoInClip > 0 || HasInfiniteClip());
	const bool bStateOKToReload = ( ( CurrentState ==  EWeaponState::Idle ) || ( CurrentState == EWeaponState::Firing) );
	return ( ( bCanReload == true ) && ( bGotAmmo == true ) && ( bStateOKToReload == true) );
}

void ABaseRangedWeapon::BeginPlay()
{
	Super::BeginPlay();
	if (GetRangedWeaponData().InitialClips > 0)
   	{
   		CurrentAmmoInClip = GetRangedWeaponData().AmmoPerClip;
   		CurrentAmmo = GetRangedWeaponData().AmmoPerClip * GetRangedWeaponData().InitialClips;
   	}
	BroadcastAmmoUsage();
}

void ABaseRangedWeapon::SimulateWeaponFire()
{
	if (GetRangedWeaponData().FireFX)
	{
		if (!GetRangedWeaponData().bLoopedMuzzleFX || FireFX == nullptr)
		{
			if(GetRangedWeaponData().FireFX->IsA(UParticleSystem::StaticClass()))
			{
				FireFX = UGameplayStatics::SpawnEmitterAttached(Cast<UParticleSystem>(GetRangedWeaponData().FireFX), GetWeaponMesh(), GetRangedWeaponData().MuzzleAttachPoint, FVector::ZeroVector, GetRangedWeaponData().AltFireRotation, EAttachLocation::SnapToTargetIncludingScale, true);
			} else if(GetRangedWeaponData().FireFX->IsA(UNiagaraSystem::StaticClass()))
			{
				FireFX = UNiagaraFunctionLibrary::SpawnSystemAttached(Cast<UNiagaraSystem>(GetRangedWeaponData().FireFX), GetWeaponMesh(), GetRangedWeaponData().MuzzleAttachPoint, FVector::ZeroVector, GetRangedWeaponData().AltFireRotation, EAttachLocation::SnapToTargetIncludingScale, true);
			}
		}
	}

	if (!GetRangedWeaponData().bLoopedFireAnim || !bPlayingFireAnim)
	{
		PlayWeaponAnimation(GetRangedWeaponData().FireAnim);
		bPlayingFireAnim = true;
	}

	if (GetRangedWeaponData().bLoopedFireSound)
	{
		if (FireAC == nullptr)
		{
			FireAC = PlayWeaponSound(GetRangedWeaponData().FireLoopSound);
		}
	}
	else
	{
		PlayWeaponSound(GetRangedWeaponData().FireSound);
	}
	PlayCameraShake();
}

void ABaseRangedWeapon::StopSimulatingWeaponFire()
{
	if (GetRangedWeaponData().bLoopedMuzzleFX)
	{
		if( FireFX != nullptr )
		{
			FireFX->Deactivate();
			FireFX = nullptr;
		}
	}

	if (GetRangedWeaponData().bLoopedFireAnim && bPlayingFireAnim)
	{
		StopWeaponAnimation(GetRangedWeaponData().FireAnim);
		bPlayingFireAnim = false;
	}

	if (FireAC)
	{
		FireAC->FadeOut(0.1f, 0.0f);
		FireAC = nullptr;

		PlayWeaponSound(GetRangedWeaponData().FireFinishSound);
	}
}

void ABaseRangedWeapon::ReloadWeapon()
{
	int32 ClipDelta = FMath::Min(GetRangedWeaponData().AmmoPerClip - CurrentAmmoInClip, CurrentAmmo - CurrentAmmoInClip);

	if (HasInfiniteClip())
	{
		ClipDelta = GetRangedWeaponData().AmmoPerClip - CurrentAmmoInClip;
	}

	if (ClipDelta > 0)
	{
		CurrentAmmoInClip += ClipDelta;
	}

	if (HasInfiniteClip())
	{
		CurrentAmmo = FMath::Max(CurrentAmmoInClip, CurrentAmmo);
	}
	BroadcastAmmoUsage();
}

void ABaseRangedWeapon::GiveAmmo(int AddAmount)
{
	const int32 MissingAmmo = FMath::Max(0, GetRangedWeaponData().MaxAmmo - CurrentAmmo);
	AddAmount = FMath::Min(AddAmount, MissingAmmo);
	CurrentAmmo += AddAmount;

	UInventoryComponent* InventoryComponent = UCoreUtils::GetInventoryComponentFromActor(OwningPawn);
	
	if (GetCurrentAmmoInClip() <= 0 && CanReload() && InventoryComponent && InventoryComponent->GetEquippedWeapon() == this) {
		StartReload();
	}
	BroadcastAmmoUsage();
}

void ABaseRangedWeapon::UseAmmo()
{
	if (!HasInfiniteAmmo())
	{
		CurrentAmmoInClip--;
	}

	if (!HasInfiniteAmmo() && !HasInfiniteClip())
	{
		CurrentAmmo--;
	}
	Super::UseAmmo();
}

void ABaseRangedWeapon::OnUnEquip()
{
	Super::OnUnEquip();
	if(!FireFX)
	{
		return;
	}

	FireFX->DeactivateImmediate();
}

FHitResult ABaseRangedWeapon::AdjustHitResultIfNoValidHitComponent(const FHitResult& Impact)
{
	if (Impact.bBlockingHit)
	{
		FHitResult UseImpact = Impact;
		if (!Impact.Component.IsValid())
		{
			const FVector StartTrace = Impact.ImpactPoint + Impact.ImpactNormal * 10.0f;
			const FVector EndTrace = Impact.ImpactPoint - Impact.ImpactNormal * 10.0f;
			FHitResult Hit = WeaponTrace(StartTrace, EndTrace);
			UseImpact = Hit;
			return UseImpact;
		}
	}
	return Impact;
}

FVector ABaseRangedWeapon::GetMuzzleLocation()
{
	if(OwningPawn)
	{
		return GetWeaponMesh()->GetSocketLocation(GetRangedWeaponData().MuzzleAttachPoint);
	}
	return FVector::ZeroVector;
}

FVector ABaseRangedWeapon::GetMuzzleDirection()
{
	if(OwningPawn)
	{
		return GetWeaponMesh()->GetSocketRotation(GetRangedWeaponData().MuzzleAttachPoint).Vector();
	}
	return FVector::ZeroVector;
}

FVector ABaseRangedWeapon::GetAdjustedAim()
{
	ABasePlayerController* const PlayerController = GetInstigator() ? Cast<ABasePlayerController>(GetInstigator()->Controller) : nullptr;
	FVector FinalAim = FVector::ZeroVector;
	if (PlayerController)
	{
		FVector CamLoc;
		FRotator CamRot;
		PlayerController->GetPlayerViewPoint(CamLoc, CamRot);
		FinalAim = CamRot.Vector();
	}
	else if (GetInstigator())
	{
		AAIController* AIController = OwningPawn ? Cast<AAIController>(OwningPawn->Controller) : nullptr;
		if(AIController != nullptr )
		{
			FinalAim = AIController->GetControlRotation().Vector();
		}
		else
		{			
			FinalAim = GetInstigator()->GetBaseAimRotation().Vector();
		}
	}
	return FinalAim;
}

FVector ABaseRangedWeapon::GetCameraAim() const
{
	ABasePlayerController* const PlayerController = GetInstigator() ? Cast<ABasePlayerController>(GetInstigator()->Controller) : nullptr;
	FVector FinalAim = FVector::ZeroVector;

	if (PlayerController)
	{
		FVector CamLoc;
		FRotator CamRot;
		PlayerController->GetPlayerViewPoint(CamLoc, CamRot);
		FinalAim = CamRot.Vector();
	}
	else if (GetInstigator())
	{
		FinalAim = GetInstigator()->GetBaseAimRotation().Vector();		
	}

	return FinalAim;
}

FVector ABaseRangedWeapon::GetCameraDamageStartLocation(const FVector& AimDir)
{
	ABasePlayerController* PlayerController = OwningPawn ? Cast<ABasePlayerController>(OwningPawn->GetController()) : nullptr;
	AAIController* AIPC = OwningPawn ? Cast<AAIController>(OwningPawn->Controller) : nullptr;
	FVector OutStartTrace = FVector::ZeroVector;

	if (PlayerController)
	{
		FRotator UnusedRot;
		PlayerController->GetPlayerViewPoint(OutStartTrace, UnusedRot);
		OutStartTrace = OutStartTrace + AimDir * ((GetInstigator()->GetActorLocation() - OutStartTrace) | AimDir);
	}

	else if (AIPC)
	{
		OutStartTrace = GetMuzzleLocation();
	}
	return OutStartTrace;
}

FHitResult ABaseRangedWeapon::WeaponTrace(const FVector& TraceFrom, const FVector& TraceTo, bool bSphereTrace) const
{
	FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(WeaponTrace), true, GetInstigator());
	TraceParams.bReturnPhysicalMaterial = true;
	FHitResult Hit(ForceInit);
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(GetOwner());
	if(bSphereTrace)
	{
		UKismetSystemLibrary::SphereTraceSingle(this, TraceFrom, TraceTo, 5.f, UEngineTypes::ConvertToTraceType(COLLISION_WEAPON), false, IgnoreActors, EDrawDebugTrace::None, Hit, true, FLinearColor::Red, FLinearColor::Green, 10.f);
	} else
	{
		UKismetSystemLibrary::LineTraceSingle(this, TraceFrom, TraceTo, UEngineTypes::ConvertToTraceType(COLLISION_WEAPON), false, IgnoreActors, EDrawDebugTrace::None, Hit, true, FLinearColor::Red, FLinearColor::Green, 10.f);
	}

	return Hit;
}

void ABaseRangedWeapon::BroadcastAmmoUsage()
{
	const int32 TotalAmmo = ((( CurrentAmmo - CurrentAmmoInClip ) / GetRangedWeaponData().AmmoPerClip) * GetRangedWeaponData().AmmoPerClip) + ( CurrentAmmo - CurrentAmmoInClip ) % GetRangedWeaponData().AmmoPerClip;
	AmmoAmountChanged.Broadcast(CurrentAmmoInClip, GetRangedWeaponData().AmmoPerClip, TotalAmmo);
}