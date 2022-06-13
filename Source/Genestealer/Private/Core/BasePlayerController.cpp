// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/BasePlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "Characters/BasePlayerCharacter.h"
#include "Core/UIEventHub.h"
#include "Actors/BaseCoverActor.h"
#include "Genestealer/Genestealer.h"
#include "Kismet/KismetMathLibrary.h"
#include "Utils/CombatUtils.h"
#include "Utils/CoreUtils.h"

ABasePlayerController::ABasePlayerController()
{
	OutlineTraceRange = 3000.f;
}

void ABasePlayerController::BeginPlay()
{
	Super::BeginPlay();
#if !UE_BUILD_SHIPPING
	CreateSandboxUI();
#endif
}

void ABasePlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if(const TScriptInterface<IInteractable> NewActor = GetTargetedActor())
	{
		const bool bNewActorAlive = IsActorAlive(NewActor.GetObject());
		const bool bOldActorAlive = IsActorAlive(CurrentInteractableActor.GetObject());
		
		if(NewActor != CurrentInteractableActor)
		{
			if(CurrentInteractableActor)
			{
				CurrentInteractableActor->SwitchOutlineOnMesh(false);	
			}

			if(bNewActorAlive && ShouldOutlineInteractable(NewActor))
			{
				NewActor->SwitchOutlineOnMesh(true);
				CurrentInteractableActor = NewActor;
				NewActorTargeted.Broadcast(FNewActorTargetedPayload(CurrentInteractableActor));
			}
		} else {
			if(CurrentInteractableActor && bOldActorAlive)
			{
				CurrentInteractableActor->SwitchOutlineOnMesh(true);
			} else
			{
				CurrentInteractableActor->SwitchOutlineOnMesh(false);
			}
		}
	}
	else
	{
		if(CurrentInteractableActor.GetObject() && CurrentInteractableActor.GetObject()->IsValidLowLevel())
		{
			CurrentInteractableActor->SwitchOutlineOnMesh(false);
			CurrentInteractableActor = nullptr;
			NewActorTargeted.Broadcast(FNewActorTargetedPayload(nullptr));
		}
	}
}

TScriptInterface<IInteractable> ABasePlayerController::GetTargetedActor() const
{
	FVector CamLoc = FVector::ZeroVector;
	FRotator CamRot;	
	GetPlayerViewPoint(CamLoc, CamRot);
	const FVector AimDir = CamRot.Vector();
	if(GetPawn())
	{
		const FVector OutStartTrace = CamLoc + AimDir * ((GetPawn()->GetActorLocation() - CamLoc) | AimDir);
		const FVector OutEndTrace = OutStartTrace + AimDir * OutlineTraceRange;
		
		TArray<AActor*> IgnoreActors;
		IgnoreActors.Add(GetPawn());
		
		FHitResult Hit(ForceInit);
		UKismetSystemLibrary::SphereTraceSingle(this, OutStartTrace, OutEndTrace, UCoreUtils::GetPlayerControllerSphereTraceRadius(this), UEngineTypes::ConvertToTraceType(GENESTEALER_TRACE_INTERACTION), false, IgnoreActors, EDrawDebugTrace::None, Hit, true
			, FLinearColor::Red, FLinearColor::Green, 1.f);
		if(Hit.GetActor() && Hit.GetActor()->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
		{
			return Hit.GetActor();
		}
	}
	return nullptr;
}

void ABasePlayerController::HandlePlayerAimingChanged(const FPlayerAimingChangedPayload& PlayerAimingChangedPayload)
{
	bShouldOutlineCombatants = PlayerAimingChangedPayload.bIsAiming;
	if(!bShouldOutlineCombatants && CurrentInteractableActor)
	{
		CurrentInteractableActor->SwitchOutlineOnMesh(false);
		CurrentInteractableActor = nullptr;
		NewActorTargeted.Broadcast(FNewActorTargetedPayload(CurrentInteractableActor));
	}
}

bool ABasePlayerController::IsActorAlive(UObject* InObject) const
{
	if(const AActor* CastedActor = Cast<AActor>(InObject))
	{
		if(UHealthComponent* HealthComponent = CastedActor->FindComponentByClass<UHealthComponent>())
		{
			return HealthComponent->IsAlive();
		}
	}
	if(ABaseCoverActor* CoverPoint = Cast<ABaseCoverActor>(InObject))
	{
		FVector CamLoc = FVector::ZeroVector;
		FRotator CamRot;	
		GetPlayerViewPoint(CamLoc, CamRot);
		return !CoverPoint->HasOccupant() && UKismetMathLibrary::Vector_Distance(CamLoc, CoverPoint->GetActorLocation()) <= UCoreUtils::GetCoverPointValidDistance();
	}
	return false;
}

bool ABasePlayerController::ShouldOutlineInteractable(TScriptInterface<IInteractable> InInteractable) const
{
	if(UCombatUtils::IsActorNeutral(InInteractable))
	{
		return true;
	}

	if(!bShouldOutlineCombatants)
	{
		return false;
	}
	
	const bool bIsAlly = UCombatUtils::AreActorsAllies(InInteractable, PlayerCharacter);
	const bool bIsEnemy = UCombatUtils::AreActorsEnemies(InInteractable, PlayerCharacter);
	return bIsAlly || bIsEnemy;
}

void ABasePlayerController::CoverAction(const FInputActionValue& Value)
{
	if (PlayerCharacter)
	{
		PlayerCharacter->CoverDodgeAction();
	}
}

void ABasePlayerController::OnPossess(APawn* NewPawn)
{
	Super::OnPossess(NewPawn);
	PlayerCharacter = Cast<ABasePlayerCharacter>(NewPawn);
	UIEventHub = NewObject<UUIEventHub>(this, UUIEventHub::StaticClass());
	if(UIEventHub)
	{
		UIEventHub->InitEventHub(this);
	}
	
	if(PlayerCharacter)
	{
		PlayerCharacter->OnPlayerAimingChanged().AddDynamic(this, &ABasePlayerController::HandlePlayerAimingChanged);
	}
}
