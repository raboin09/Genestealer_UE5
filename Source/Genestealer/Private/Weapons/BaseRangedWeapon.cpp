// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/BaseRangedWeapon.h"
#include "AIController.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Characters//BaseCharacter.h"
#include "Components/AudioComponent.h"
#include "Characters//InventoryComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "Utils/CoreUtils.h"
#include "Utils/FeedbackUtils.h"

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

void ABaseRangedWeapon::PlayMuzzleFX()
{
	if (FireFXClass && bSpawnMuzzleFX)
	{
		if (!bLoopedMuzzleFX || !FireVFXSystem)
		{
			if(FireVFXSystem && !bLoopedMuzzleFX)
			{
				FireVFXSystem->DeactivateImmediate();
			}

			if(FireFXClass->IsA(UParticleSystem::StaticClass()))
			{
				FireVFXSystem = Internal_PlayParticleFireEffects();
			} else if(FireFXClass->IsA(UNiagaraSystem::StaticClass()))
			{
				FireVFXSystem = PlayNiagaraFireEffects();
			}
		}
	}
}

float ABaseRangedWeapon::SimulateWeaponFire()
{
	const FAnimMontagePlayData PlayData = GetPlayData();
	PlayMuzzleFX();
	
	if(bSpawnShellFX && IsWeaponPlayerControlled())
	{
		Internal_PlayShellEffects();
	}
	
	if(!CurrentMontage || !bLoopedFireAnim)
	{
		PlayWeaponAnimation(PlayData);
		CurrentMontage = PlayData.MontageToPlay;
	}

	if (bLoopedFireSound)
	{
		if (!FireAudio)
		{
			FireAudio = PlayWeaponSound(FireSound);
		}
	}
	else
	{
		PlayWeaponSound(FireSound);
	}
	UFeedbackUtils::TryPlayCameraShake(GetOwningPawn(), FireCameraShake, CameraShakeScale);
	bSecondaryWeaponsTurn = !bSecondaryWeaponsTurn;
	if(PlayData.MontageToPlay)
	{
		return PlayData.MontageToPlay->BlendIn.GetBlendTime();
	}
	return 0.f;
}

void ABaseRangedWeapon::StopSimulatingWeaponFire()
{	
	if(FireVFXSystem != nullptr)
	{
		if(bDeactivateVFXImmediately)
		{
			FireVFXSystem->DeactivateImmediate();
		} else
		{
			FireVFXSystem->Deactivate();
		}
		FireVFXSystem = nullptr;
	}

	if (bLoopedFireAnim && CurrentMontage)
	{
		StopWeaponAnimation(CurrentMontage);
		CurrentMontage = nullptr;
	}
	
	if (FireAudio)
	{
		StopWeaponSound(FireAudio, true, 0.3f);
		FireAudio = nullptr;
		PlayWeaponSound(FireFinishSound);
	}
	
	CurrentFiringSpread = 0.f;
}

void ABaseRangedWeapon::Internal_PlayShellEffects() const
{
	if(!GetWeaponMesh())
	{
		return;
	}
	
	const FTransform SpawnTransform = GetWeaponMesh()->GetSocketTransform(ShellSpawnSocket);	
	if(ShellFXClass->IsA(UParticleSystem::StaticClass()))
	{
		if(UParticleSystemComponent* ShellFXSystem = UGameplayStatics::SpawnEmitterAtLocation(this, Cast<UParticleSystem>(ShellFXClass), SpawnTransform.GetLocation(), SpawnTransform.Rotator()))
		{
			ShellFXSystem->OnParticleCollide.AddDynamic(this, &ABaseRangedWeapon::HandleShellParticleCollision);
		}
	} else if(ShellFXClass->IsA(UNiagaraSystem::StaticClass()))
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, Cast<UNiagaraSystem>(ShellFXClass), SpawnTransform.GetLocation(), SpawnTransform.Rotator());
	}
}

void ABaseRangedWeapon::HandleShellParticleCollision(FName EventName, float EmitterTime, int32 ParticleTime, FVector Location, FVector Velocity, FVector Direction, FVector Normal, FName BoneName, UPhysicalMaterial* PhysMat)
{
	UAudioManager::SpawnSoundAtLocation(this, ShellImpactSound, Location);
}

UFXSystemComponent* ABaseRangedWeapon::Internal_PlayParticleFireEffects()
{
	UParticleSystemComponent* ParticleFX = UGameplayStatics::SpawnEmitterAttached(Cast<UParticleSystem>(FireFXClass), GetWeaponMesh(), RaycastSourceSocketName, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTargetIncludingScale, true);
	return ParticleFX;
} 

UFXSystemComponent* ABaseRangedWeapon::PlayNiagaraFireEffects()
{
	UNiagaraComponent* NiagaraFX = UNiagaraFunctionLibrary::SpawnSystemAttached(Cast<UNiagaraSystem>(FireFXClass), GetWeaponMesh(), RaycastSourceSocketName, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTargetIncludingScale, true);

	if(NiagaraFX && AdjustVFX != EWeaponVFXAdjustmentType::NeverAdjust)
	{		
		if(AdjustVFX == EWeaponVFXAdjustmentType::AdjustOnImpact)
		{			
			FVector MaxAdjustedScale = MaxVFXScaleAdjust;
			const FVector& AimDirection = GetAdjustedAim();
			const FVector& StartTrace = GetCameraDamageStartLocation(AimDirection);	
			const FVector ShootDirection = GetShootDirection(AimDirection);
			const FVector& EndTrace = StartTrace + ShootDirection * TraceRange;
			const FHitResult& Impact = WeaponTrace(StartTrace, EndTrace, true);
			if(Impact.bBlockingHit)
			{
				MaxAdjustedScale.Z = UKismetMathLibrary::Vector_Distance(StartTrace, Impact.Location);
				MaxAdjustedScale.Z /= UKismetMathLibrary::Max(ParticleMeshZ, 1);
			}
			NiagaraFX->SetNiagaraVariableVec3(MaxSpawnScaleName, MaxAdjustedScale);
			NiagaraFX->SetNiagaraVariableVec3(MinSpawnScaleName, MinVFXScaleAdjust);
			return NiagaraFX;
		}

		if(AdjustVFX == EWeaponVFXAdjustmentType::AdjustOnAIUse)
		{
			if(IsWeaponPlayerControlled())
			{
				return NiagaraFX;
			}
			NiagaraFX->SetNiagaraVariableVec3(MaxSpawnScaleName, MaxVFXScaleAdjust);
			NiagaraFX->SetNiagaraVariableVec3(MinSpawnScaleName, MinVFXScaleAdjust);
		}
	}
	return NiagaraFX;
}

FAnimMontagePlayData ABaseRangedWeapon::GetPlayData()
{	
	FAnimMontagePlayData PlayData;	
	PlayData.MontageToPlay = FireAnim;
	return PlayData;
}

bool ABaseRangedWeapon::Internal_IsInCover()
{
	if(IAnimatable* AnimOwner = Cast<IAnimatable>(OwningPawn))
	{
		return AnimOwner->IsInCover();
	}
	return false;
}

bool ABaseRangedWeapon::Internal_HasRightInput()
{
	if(IAnimatable* AnimOwner = Cast<IAnimatable>(OwningPawn))
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
		StopWeaponAnimation(ReloadAnim);
		DetermineWeaponState();
	}
	K2_HandleStopReload();
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

	const UInventoryComponent* InventoryComponent = UCoreUtils::GetInventoryComponentFromActor(OwningPawn);
	if(!InventoryComponent)
	{
		return;
	}
	
	if(InventoryComponent->GetEquippedWeapon() == this)
	{
		if (GetCurrentAmmoInClip() <= 0) {
			StartReload();
		}
	} else
	{
		if(CanReload())
		{
			ReloadWeapon();	
		}
	}

	if(GetCurrentAmmo() > 0)
	{
		K2_HandleOutOfAmmo(false);
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

	if(GetCurrentAmmo() <= 0 && GetCurrentAmmoInClip() <= 0)
	{
		K2_HandleOutOfAmmo(true);
	} else
	{
		K2_HandleOutOfAmmo(false);	
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
	if(!FireVFXSystem)
	{
		return;
	}

	FireVFXSystem->DeactivateImmediate();
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
		if(!GetWeaponMesh())
		{
			return FVector::ZeroVector;
		}
		return GetWeaponMesh()->GetSocketLocation(RaycastSourceSocketName);
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
	return !IsWeaponPlayerControlled();
}

FVector ABaseRangedWeapon::GetRaycastOriginRotation()
{
	if(bRaycastFromWeaponMeshInsteadOfPawnMesh)
	{
		if(!GetWeaponMesh())
		{
			return FVector::ZeroVector;
		}
		return GetWeaponMesh()->GetSocketRotation(RaycastSourceSocketName).Vector();
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
	if(!GetWeaponMesh())
	{
		return FRotator::ZeroRotator;
	}
	return GetWeaponMesh()->GetSocketRotation(RaycastSourceSocketName);
}

FVector ABaseRangedWeapon::GetAdjustedAim() const
{
	if(!GetInstigator())
	{
		return FVector::ZeroVector;
	}

	if(const APlayerController* PlayerController = Cast<APlayerController>(GetInstigator()->Controller))
	{
		FVector CamLoc;
		FRotator CamRot;
		PlayerController->GetPlayerViewPoint(CamLoc, CamRot);
		return CamRot.Vector();
	}

	if(const AAIController* AIController = Cast<AAIController>(GetInstigator()->Controller))
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

	bool bShouldEyeTrace = false;
	if (IAnimatable* AnimationOwner = Cast<IAnimatable>(OwningPawn))
	{
		bShouldEyeTrace = AnimationOwner->IsAiming() && bAimOriginIsPlayerEyesInsteadOfWeapon;
	}
	
	if(!bShouldEyeTrace)
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

FHitResult ABaseRangedWeapon::WeaponTrace(const FVector& StartTrace, const FVector& EndTrace, bool bLineTrace, float CircleRadius) const
{
	FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(WeaponTrace), true, GetInstigator());
	TraceParams.bReturnPhysicalMaterial = true;
	FHitResult Hit(ForceInit);
	TArray<AActor*> IgnoreActors; 
	IgnoreActors.Add(GetInstigator());
	auto DrawDebugTrace = EDrawDebugTrace::None;
	auto WeaponTraceType = UEngineTypes::ConvertToTraceType(ECC_Visibility); //bUseWeaponTraceType ? UEngineTypes::ConvertToTraceType(GENESTEALER_TRACE_WEAPON) : UEngineTypes::ConvertToTraceType(ECC_Visibility);
	if(bLineTrace)
	{
		UKismetSystemLibrary::LineTraceSingle(this, StartTrace, EndTrace, WeaponTraceType, false, IgnoreActors, DrawDebugTrace, Hit, true, FLinearColor::Red, FLinearColor::Green, 10.f);
	} else
	{
		UKismetSystemLibrary::SphereTraceSingle(this, StartTrace, EndTrace, CircleRadius, WeaponTraceType, false, IgnoreActors, DrawDebugTrace, Hit, true, FLinearColor::Red, FLinearColor::Green, 10.f);	
	}
	return Hit;
}

void ABaseRangedWeapon::StartReload()
{
	if (CanReload())
	{
		bPendingReload = true;
		DetermineWeaponState();
		StopWeaponAnimation(FireAnim);
		if(ReloadAnim)
		{
			FAnimMontagePlayData PlayData;
			PlayData.MontageToPlay = ReloadAnim; 
			const float AnimDuration = PlayWeaponAnimation(PlayData);
			K2_HandleStartReload(AnimDuration);
			GetWorldTimerManager().SetTimer(TimerHandle_StopReload, this, &ABaseRangedWeapon::StopReload, AnimDuration, false);
			GetWorldTimerManager().SetTimer(TimerHandle_ReloadWeapon, this, &ABaseRangedWeapon::ReloadWeapon, FMath::Max(0.1f, AnimDuration - 0.1f), false);
		} else
		{
			K2_HandleStartReload(ReloadDurationIfNoAnim);
			GetWorldTimerManager().SetTimer(TimerHandle_StopReload, this, &ABaseRangedWeapon::StopReload, ReloadDurationIfNoAnim, false);
			GetWorldTimerManager().SetTimer(TimerHandle_ReloadWeapon, this, &ABaseRangedWeapon::ReloadWeapon, ReloadDurationIfNoAnim - .1f, false);
		}

		ReloadAudio = PlayWeaponSound(ReloadSound);
	}
}

void ABaseRangedWeapon::BroadcastAmmoUsage()
{
	const int32 TotalAmmo = ((( CurrentAmmo - CurrentAmmoInClip ) / AmmoPerClip) * AmmoPerClip) + ( CurrentAmmo - CurrentAmmoInClip ) % AmmoPerClip;
	OnAmmoAmountChanged().Broadcast(FAmmoAmountChangedPayload(CurrentAmmoInClip, AmmoPerClip, TotalAmmo));
}

void ABaseRangedWeapon::HandleFiring()
{
	if (GetCurrentAmmo() == 0 && !bRefiring)
	{
		PlayWeaponSound(OutOfAmmoSound);
		if(BurstCounter > 0)
		{
			OnBurstFinished();
		}
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

void ABaseRangedWeapon::OnEnterInventory(ACharacter* NewOwner)
{
	Super::OnEnterInventory(NewOwner);
	UGameplayStatics::PrimeSound(FireSound);
}

void ABaseRangedWeapon::OnLeaveInventory()
{
	Super::OnLeaveInventory();
	if(FireVFXSystem)
	{
		FireVFXSystem->DeactivateImmediate();
	}
	if(ReloadAudio)
	{
		ReloadAudio->Stop();
	}
	if(FireAudio)
	{
		FireAudio->Stop();
	}
}

void ABaseRangedWeapon::DetermineWeaponState()
{
	EWeaponState NewState = EWeaponState::Idle;
	if (bIsEquipped)
	{
		if(bPendingReload)
		{
			if(!CanReload())
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
