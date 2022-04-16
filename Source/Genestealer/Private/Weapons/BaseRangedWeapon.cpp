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

UFXSystemComponent* ABaseRangedWeapon::Internal_PlayParticleFireEffects()
{
	ParticleFX = UGameplayStatics::SpawnEmitterAttached(Cast<UParticleSystem>(FireFXClass), NextFiringMesh, RaycastSourceSocketName, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTargetIncludingScale, true);
	return ParticleFX;
} 

UFXSystemComponent* ABaseRangedWeapon::Internal_PlayNiagaraFireEffects()
{
	NiagaraFX = UNiagaraFunctionLibrary::SpawnSystemAttached(Cast<UNiagaraSystem>(FireFXClass), NextFiringMesh, RaycastSourceSocketName, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTargetIncludingScale, true);
	// TODO handle Niagara collisions
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

void ABaseRangedWeapon::StopSimulatingWeaponFire()
{
	if (bLoopedMuzzleFX)
	{
		if( FireFXSystem != nullptr )
		{
			FireFXSystem->Deactivate();
			FireFXSystem = nullptr;
		}
	}

	if (bLoopedFireAnim && CurrentMontage)
	{
		StopWeaponAnimation(CurrentMontage);
		CurrentMontage = nullptr;
	}

	if (FireAC)
	{
		FireAC->FadeOut(0.1f, 0.0f);
		FireAC = nullptr;

		PlayWeaponSound(FireFinishSound);
	}
	
	CurrentFiringSpread = 0.f;
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
	if(!FireFXSystem)
	{
		return;
	}

	FireFXSystem->DeactivateImmediate();
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

FVector ABaseRangedWeapon::GetRaycastOriginLocation()
{
	if(bRaycastFromWeapon)
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
	const int32 RandomSeed = FMath::Rand();
	const FRandomStream WeaponRandomStream(RandomSeed);
	const float CurrentSpread = GetCurrentSpread();
	const float ConeHalfAngle = FMath::DegreesToRadians(CurrentSpread * 0.5f);
	CurrentFiringSpread = FMath::Min(FiringSpreadMax, CurrentFiringSpread + FiringSpreadIncrement);
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


FVector ABaseRangedWeapon::GetRaycastOriginRotation()
{
	if(bRaycastFromWeapon)
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

	if (PlayerController)
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
			FHitResult Hit = WeaponTrace(StartTrace, EndTrace);
			UseImpact = Hit;
			return UseImpact;
		}
	}
	return Impact;
}

FHitResult ABaseRangedWeapon::WeaponTrace(const FVector& StartTrace, const FVector& EndTrace) const
{
	FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(WeaponTrace), true, GetInstigator());
	TraceParams.bReturnPhysicalMaterial = true;
	FHitResult Hit(ForceInit);
	TArray<AActor*> IgnoreActors; 
	IgnoreActors.Add(GetInstigator());
	UKismetSystemLibrary::SphereTraceSingle(this, StartTrace, EndTrace, 5.f, UEngineTypes::ConvertToTraceType(TRACE_WEAPON), false, IgnoreActors, EDrawDebugTrace::None, Hit, true, FLinearColor::Red, FLinearColor::Green, 10.f);
	return Hit;
}

void ABaseRangedWeapon::BroadcastAmmoUsage()
{
	const int32 TotalAmmo = ((( CurrentAmmo - CurrentAmmoInClip ) / AmmoPerClip) * AmmoPerClip) + ( CurrentAmmo - CurrentAmmoInClip ) % AmmoPerClip;
	OnAmmoAmountChanged().Broadcast(CurrentAmmoInClip, AmmoPerClip, TotalAmmo);
}