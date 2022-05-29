// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/BaseRangedWeapon.h"
#include "AIController.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Characters//BaseCharacter.h"
#include "Components/AudioComponent.h"
#include "Characters//InventoryComponent.h"
#include "GameFramework/Character.h"
#include "Genestealer/Genestealer.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystemComponent.h"
#include "Utils/CoreUtils.h"

bool ABaseRangedWeapon::CanReload()
{
	const bool bCanReload = OwningPawn != nullptr;
	const bool bGotAmmo = ( GetCurrentAmmoInClip() < AmmoPerClip) && (CurrentAmmo - CurrentAmmoInClip > 0 || HasInfiniteClip());
	const bool bStateOKToReload = ( ( CurrentState ==  EWeaponState::Idle ) || ( CurrentState == EWeaponState::Firing) );
	return ( ( bCanReload == true ) && ( bGotAmmo == true ) && ( bStateOKToReload == true) );
}

void ABaseRangedWeapon::BeginPlay()
{
	Super::BeginPlay();
	if (InitialClips > 0)
   	{
   		CurrentAmmoInClip = AmmoPerClip;
   		CurrentAmmo = AmmoPerClip * InitialClips;
   	}
	BroadcastAmmoUsage();
}

void ABaseRangedWeapon::OnEquipFinished()
{
	Super::OnEquipFinished();
	if (OwningPawn)
	{
		if (GetCurrentAmmoInClip() <= 0 && CanReload())
		{
			StartReload();
		}
	}
	BroadcastAmmoUsage();
}

void ABaseRangedWeapon::OnEquip(const TScriptInterface<IWeapon> LastWeapon)
{
	Super::OnEquip(LastWeapon);
	BroadcastAmmoUsage();
}

float ABaseRangedWeapon::SimulateWeaponFire()
{
	const FAnimMontagePlayData PlayData = Internal_GetPlayData();
	Internal_AlternateFiringMesh();
	if (FireFXClass)
	{
		if (!bLoopedMuzzleFX || !FireFXSystem)
		{
			if(FireFXSystem && !bLoopedMuzzleFX)
			{
				FireFXSystem->DeactivateImmediate();
			}

			if(FireFXClass->IsA(UParticleSystem::StaticClass()))
			{
				FireFXSystem = Internal_PlayParticleFireEffects();
			} else if(FireFXClass->IsA(UNiagaraSystem::StaticClass()))
			{
				FireFXSystem = Internal_PlayNiagaraFireEffects();
			}
		}
	}
	
	if(!CurrentMontage || !bLoopedFireAnim)
	{
		PlayWeaponAnimation(PlayData);
		CurrentMontage = PlayData.MontageToPlay;
	}

	if (bLoopedFireSound)
	{
		if (FireAC == nullptr)
		{
			FireAC = PlayWeaponSound(FireSound);
		}
	}
	else
	{
		PlayWeaponSound(FireSound);
	}
	PlayCameraShake();
	bSecondaryWeaponsTurn = !bSecondaryWeaponsTurn;
	if(PlayData.MontageToPlay)
	{
		return PlayData.MontageToPlay->BlendIn.GetBlendTime();
	}
	return 0.f;
}

void ABaseRangedWeapon::StopSimulatingWeaponFire()
{
	if(FireFXSystem != nullptr)
	{
		FireFXSystem->DeactivateImmediate();
		FireFXSystem = nullptr;
	}

	if (bLoopedFireAnim && CurrentMontage)
	{
		StopWeaponAnimation(CurrentMontage);
		CurrentMontage = nullptr;
	}

	if (FireAC)
	{
		FireAC->FadeOut(0.3f, 0.0f);
		FireAC = nullptr;

		PlayWeaponSound(FireFinishSound);
	}
	
	CurrentFiringSpread = 0.f;
}

UFXSystemComponent* ABaseRangedWeapon::Internal_PlayParticleFireEffects()
{
	ParticleFX = UGameplayStatics::SpawnEmitterAttached(Cast<UParticleSystem>(FireFXClass), NextFiringMesh, RaycastSourceSocketName, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTargetIncludingScale, true);
	return ParticleFX;
} 

UFXSystemComponent* ABaseRangedWeapon::Internal_PlayNiagaraFireEffects()
{
	NiagaraFX = UNiagaraFunctionLibrary::SpawnSystemAttached(Cast<UNiagaraSystem>(FireFXClass), NextFiringMesh, RaycastSourceSocketName, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTargetIncludingScale, true);

	if(NiagaraFX && bAdjustVFXScaleOnSpawn)
	{
		const FVector& AimDirection = GetAdjustedAim();
		const FVector& StartTrace = GetCameraDamageStartLocation(AimDirection);	
		const FVector ShootDirection = GetShootDirection(AimDirection);
		const FVector& EndTrace = StartTrace + ShootDirection * TraceRange;
		const FHitResult& Impact = WeaponTrace(StartTrace, EndTrace, true);
		
		FVector MaxAdjustedScale = MaxVFXScaleAdjust;
		if(Impact.bBlockingHit)
		{
			MaxAdjustedScale.Z = UKismetMathLibrary::Vector_Distance(StartTrace, Impact.Location);
			MaxAdjustedScale.Z /= UKismetMathLibrary::Max(ParticleMeshZ, 1);
		}
		NiagaraFX->SetNiagaraVariableVec3(MaxSpawnScaleName, MaxAdjustedScale);
	}
	return NiagaraFX;
}

FAnimMontagePlayData ABaseRangedWeapon::Internal_GetPlayData() const
{
	// if not akimbo, if in cover return 
	
	FAnimMontagePlayData PlayData;	
	if(bAkimbo)
	{
		if(Internal_IsInCover())
		{
			PlayData.MontageToPlay = Internal_HasRightInput() ? CoverFireRightAnim : CoverFireLeftAnim;
		} else
		{
			PlayData.MontageToPlay = FireAnim;
			PlayData.MontageSection = bSecondaryWeaponsTurn ? "AltFire" : "MainFire";
		}

	} else
	{
		PlayData.MontageToPlay = FireAnim;
	}
	return PlayData;
}

void ABaseRangedWeapon::Internal_AlternateFiringMesh()
{
	if(bAkimbo)
	{
		if(Internal_IsInCover())
		{
			if(Internal_HasRightInput())
			{
				NextFiringMesh = GetWeaponMesh();
			} else
			{
				NextFiringMesh = GetSecondaryWeaponMesh();
			}
		} else
		{
			if(bSecondaryWeaponsTurn)
			{
				NextFiringMesh = GetSecondaryWeaponMesh();
			} else
			{
				NextFiringMesh = GetWeaponMesh();
			}	
		}
	} else
	{
		NextFiringMesh = GetWeaponMesh();
	}
}

bool ABaseRangedWeapon::Internal_IsInCover() const
{
	if(const IAnimatable* AnimOwner = Cast<IAnimatable>(OwningPawn))
	{
		return AnimOwner->IsInCover();
	}
	return false;
}

bool ABaseRangedWeapon::Internal_HasRightInput() const
{
	if(const IAnimatable* AnimOwner = Cast<IAnimatable>(OwningPawn))
	{
		return AnimOwner->HasRightInput();
	}
	return false;
}

void ABaseRangedWeapon::ReloadWeapon()
{
	int32 ClipDelta = FMath::Min(AmmoPerClip - CurrentAmmoInClip, CurrentAmmo - CurrentAmmoInClip);

	if (HasInfiniteClip())
	{
		ClipDelta = AmmoPerClip - CurrentAmmoInClip;
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

void ABaseRangedWeapon::StopReload()
{
	if (CurrentState == EWeaponState::Reloading)
	{
		bPendingReload = false;
		CurrentState = EWeaponState::Idle;
		DetermineWeaponState();
		StopWeaponAnimation(ReloadAnim);
	}
	BroadcastAmmoUsage();
}

void ABaseRangedWeapon::StopFire()
{
	Super::StopFire();
	if(ReloadAudio)
	{
		ReloadAudio->Deactivate();
		ReloadAudio = nullptr;
	}
}

void ABaseRangedWeapon::GiveAmmo(int AddAmount)
{
	const int32 MissingAmmo = FMath::Max(0, MaxAmmo - CurrentAmmo);
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
	if (!HasInfiniteClip())
	{
		CurrentAmmoInClip--;
	}

	if (!HasInfiniteAmmo())
	{
		CurrentAmmo--;
	}
	BroadcastAmmoUsage();
}

void ABaseRangedWeapon::OnUnEquip()
{
	Super::OnUnEquip();
	if (bPendingReload)
	{
		StopWeaponAnimation(ReloadAnim);
		bPendingReload = false;

		GetWorldTimerManager().ClearTimer(TimerHandle_StopReload);
		GetWorldTimerManager().ClearTimer(TimerHandle_ReloadWeapon);
	}
	if(!FireFXSystem)
	{
		return;
	}

	FireFXSystem->DeactivateImmediate();
}

bool ABaseRangedWeapon::CanFire() const
{
	if(bPendingReload)
	{
		return false;
	}
	return Super::CanFire();
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
			FHitResult Hit = WeaponTrace(StartTrace, EndTrace, ShouldLineTrace());
			UseImpact = Hit;
			return UseImpact;
		}
	}
	return Impact;
}

FVector ABaseRangedWeapon::GetRaycastOriginLocation()
{
	if(bRaycastFromWeaponMeshInsteadOfPawnMesh)
	{
		if(!NextFiringMesh)
		{
			return FVector::ZeroVector;
		}
		return NextFiringMesh->GetSocketLocation(RaycastSourceSocketName);
	}
	
	if(!GetInstigator())
	{
		return FVector::ZeroVector;
	}

	if(const USkeletalMeshComponent* InstigatorMesh = GetInstigator()->FindComponentByClass<USkeletalMeshComponent>())
	{
		return InstigatorMesh->GetBoneLocation(RaycastSourceSocketName);
	}

	if (const UStaticMeshComponent* InstigatorMesh = GetInstigator()->FindComponentByClass<UStaticMeshComponent>())
	{
		return InstigatorMesh->GetSocketLocation(RaycastSourceSocketName);
	}

	return FVector::ZeroVector;
}

FVector ABaseRangedWeapon::GetShootDirection(const FVector& AimDirection)
{
	if(bHasFiringSpread)
	{
		const int32 RandomSeed = FMath::Rand();
		const FRandomStream WeaponRandomStream(RandomSeed);
		const float CurrentSpread = GetCurrentSpread();
		const float ConeHalfAngle = FMath::DegreesToRadians(CurrentSpread * 0.5f);
		CurrentFiringSpread = FMath::Min(FiringSpreadMax, CurrentFiringSpread + FiringSpreadIncrement);
		return WeaponRandomStream.VRandCone(AimDirection, ConeHalfAngle, ConeHalfAngle);
	}
		const int32 RandomSeed = FMath::Rand();
		const FRandomStream WeaponRandomStream(RandomSeed);
		const float ConeHalfAngle = FMath::DegreesToRadians(0.f);
		return WeaponRandomStream.VRandCone(AimDirection, ConeHalfAngle, ConeHalfAngle);
}

float ABaseRangedWeapon::GetCurrentSpread() const
{
	return TraceSpread + CurrentFiringSpread;
}

float ABaseRangedWeapon::GetCurrentFiringSpreadPercentage() const
{
	float FinalSpread = CurrentFiringSpread;
	// if (IInteractableInterface* CastedInterface = Cast<IInteractableInterface>(OwningPawn))
	// {
	// 	if(CastedInterface->IsTargeting())
	// 	{
	// 		FinalSpread *= TargetingSpreadMod;	
	// 	}
	// 	FinalSpread *= CastedInterface->GetSpreadModification();
	// }

	return FinalSpread / FiringSpreadMax;
}

bool ABaseRangedWeapon::ShouldLineTrace() const
{
	if(GetInstigator() && GetInstigator()->Controller)
	{
		// Not a line trace if this is a player
		return !GetInstigator()->Controller->IsA(ABasePlayerController::StaticClass());
	}
	return true;
}

FVector ABaseRangedWeapon::GetRaycastOriginRotation()
{
	if(bRaycastFromWeaponMeshInsteadOfPawnMesh)
	{
		if(!NextFiringMesh)
		{
			return FVector::ZeroVector;
		}
		return NextFiringMesh->GetSocketRotation(RaycastSourceSocketName).Vector();
	}
	
	if(!GetInstigator())
	{
		return FVector::ZeroVector;
	}

	if(const UMeshComponent* InstigatorMesh = GetInstigator()->FindComponentByClass<UMeshComponent>())
	{
		return InstigatorMesh->GetSocketLocation(RaycastSourceSocketName);
	}
	return FVector::ZeroVector;
}

FRotator ABaseRangedWeapon::GetRaycastSocketRotation() const
{
	if(!NextFiringMesh)
	{
		return FRotator::ZeroRotator;
	}
	return NextFiringMesh->GetSocketRotation(RaycastSourceSocketName);
}

FVector ABaseRangedWeapon::GetAdjustedAim() const
{
	if(!GetInstigator())
	{
		return FVector::ZeroVector;
	}

	if(const APlayerController* const PlayerController = Cast<APlayerController>(GetInstigator()->Controller))
	{
		FVector CamLoc;
		FRotator CamRot;
		PlayerController->GetPlayerViewPoint(CamLoc, CamRot);
		return CamRot.Vector();
	}

	if(const AAIController* const AIController = Cast<AAIController>(GetInstigator()->Controller))
	{
		return AIController->GetControlRotation().Vector();
	}
	return GetInstigator()->GetBaseAimRotation().Vector();
}

FVector ABaseRangedWeapon::GetCameraDamageStartLocation(const FVector& AimDirection)
{
	APlayerController* PlayerController = GetInstigator() ? Cast<APlayerController>(GetInstigator()->GetController()) : nullptr;
	AAIController* AIPC = GetInstigator() ? Cast<AAIController>(GetInstigator()->Controller) : nullptr;
	FVector OutStartTrace = FVector::ZeroVector;

	if(!bAimOriginIsPlayerEyesInsteadOfWeapon)
	{
		OutStartTrace = GetRaycastOriginLocation();
	}
	else if (PlayerController)
	{
		FRotator UnusedRot;
		PlayerController->GetPlayerViewPoint(OutStartTrace, UnusedRot);
		OutStartTrace = OutStartTrace + AimDirection * ((GetInstigator()->GetActorLocation() - OutStartTrace) | AimDirection);
	}
	else if (AIPC)
	{
		OutStartTrace = GetRaycastOriginLocation();
	}
	return OutStartTrace;
}

FHitResult ABaseRangedWeapon::AdjustHitResultIfNoValidHitComponent(const FHitResult& Impact) const
{
	if (Impact.bBlockingHit)
	{
		FHitResult UseImpact = Impact;
		if (!Impact.Component.IsValid())
		{
			const FVector StartTrace = Impact.ImpactPoint + Impact.ImpactNormal * 10.0f;
			const FVector EndTrace = Impact.ImpactPoint - Impact.ImpactNormal * 10.0f;
			FHitResult Hit = WeaponTrace(StartTrace, EndTrace, ShouldLineTrace());
			UseImpact = Hit;
			return UseImpact;
		}
	}
	return Impact;
}

FHitResult ABaseRangedWeapon::WeaponTrace(const FVector& StartTrace, const FVector& EndTrace, bool bLineTrace) const
{
	FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(WeaponTrace), true, GetInstigator());
	TraceParams.bReturnPhysicalMaterial = true;
	FHitResult Hit(ForceInit);
	TArray<AActor*> IgnoreActors; 
	IgnoreActors.Add(GetInstigator());
	auto DrawDebugTrace = EDrawDebugTrace::None;
	if(bLineTrace)
	{
		UKismetSystemLibrary::LineTraceSingle(this, StartTrace, EndTrace,  UEngineTypes::ConvertToTraceType(TRACE_WEAPON), false, IgnoreActors, DrawDebugTrace, Hit, true, FLinearColor::Red, FLinearColor::Green, 10.f);
	} else
	{
		UKismetSystemLibrary::SphereTraceSingle(this, StartTrace, EndTrace, 5.f, UEngineTypes::ConvertToTraceType(TRACE_WEAPON), false, IgnoreActors, DrawDebugTrace, Hit, true, FLinearColor::Red, FLinearColor::Green, 10.f);	
	}
	return Hit;
}

void ABaseRangedWeapon::StartReload()
{
	if (CanReload())
	{
		bPendingReload = true;
		DetermineWeaponState();
		if(ReloadAnim)
		{
			FAnimMontagePlayData PlayData;
			PlayData.MontageToPlay = ReloadAnim;
			const float AnimDuration = PlayWeaponAnimation(PlayData);	
			GetWorldTimerManager().SetTimer(TimerHandle_StopReload, this, &ABaseRangedWeapon::StopReload, AnimDuration, false);
			GetWorldTimerManager().SetTimer(TimerHandle_ReloadWeapon, this, &ABaseRangedWeapon::ReloadWeapon, FMath::Max(0.1f, AnimDuration - 0.1f), false);
		} else
		{
			GetWorldTimerManager().SetTimer(TimerHandle_StopReload, this, &ABaseRangedWeapon::StopReload, ReloadDurationIfNoAnim, false);
			GetWorldTimerManager().SetTimer(TimerHandle_ReloadWeapon, this, &ABaseRangedWeapon::ReloadWeapon, ReloadDurationIfNoAnim - .1f, false);
		}

		ReloadAudio = PlayWeaponSound(ReloadSound);
	}
}

void ABaseRangedWeapon::BroadcastAmmoUsage()
{
	const int32 TotalAmmo = ((( CurrentAmmo - CurrentAmmoInClip ) / AmmoPerClip) * AmmoPerClip) + ( CurrentAmmo - CurrentAmmoInClip ) % AmmoPerClip;
	OnAmmoAmountChanged().Broadcast(CurrentAmmoInClip, AmmoPerClip, TotalAmmo);
}

void ABaseRangedWeapon::HandleFiring()
{
	if (GetCurrentAmmo() == 0 && !bRefiring)
	{
		PlayWeaponSound(OutOfAmmoSound);
	}	
	else if (GetCurrentAmmoInClip() <= 0 && CanReload())
	{
		StartReload();
	} else
	{
		Super::HandleFiring();
		UseAmmo();
	}
}

void ABaseRangedWeapon::DetermineWeaponState()
{
	EWeaponState NewState = EWeaponState::Idle;
	if (bIsEquipped)
	{
		if(bPendingReload)
		{
			if(CanReload() == false )
			{
				NewState = CurrentState;
			}
			else
			{
				NewState = EWeaponState::Reloading;
			}
		}		
		else
		{
			if(bWantsToFire && CanFire())
			{
				NewState = EWeaponState::Firing;	
			}
		}
	}
	else if (bPendingEquip)
	{
		NewState = EWeaponState::Equipping;
	}
	SetWeaponState(NewState);
}
