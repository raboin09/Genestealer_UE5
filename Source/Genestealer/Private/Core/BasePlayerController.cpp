// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/BasePlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "Characters/BasePlayerCharacter.h"
#include "Core/UIEventHub.h"
#include "Actors/BaseCoverActor.h"
#include "Genestealer/Genestealer.h"
#include "Kismet/KismetMathLibrary.h"
#include "Quest/QuestManagerComponent.h"
#include "Utils/CombatUtils.h"
#include "Utils/CoreUtils.h"

ABasePlayerController::ABasePlayerController()
{
	OutlineTraceRange = 3000.f;
	QuestManager = CreateDefaultSubobject<UQuestManagerComponent>(TEXT("QuestManager"));
	PlayerStats = CreateDefaultSubobject<UPlayerStatsComponent>(TEXT("PlayerStats"));
}

void ABasePlayerController::BeginPlay()
{
	Super::BeginPlay();
#if WITH_EDITOR
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
			if(CurrentInteractableActor)
			{
				CurrentInteractableActor->SwitchOutlineOnMesh(bOldActorAlive);
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

void ABasePlayerController::AddNewAISquadMember(TScriptInterface<IAIPawn> InAIPawn)
{
	AISquadMembers.AddUnique(InAIPawn);
}

TScriptInterface<IInteractable> ABasePlayerController::GetTargetedActor() const
{
	// Remember to add a new class type if it needs to be considered alive via IsActorAlive()
	FVector StartLocation = FVector::ZeroVector;
	FRotator CamRot;
	GetPlayerViewPoint(StartLocation, CamRot);
	const FVector AimDir = CamRot.Vector();
	if(PlayerCharacter)
	{
		if(!PlayerCharacter->IsAiming() && !PlayerCharacter->IsInCover())
		{
			StartLocation = PlayerCharacter->GetHeadLocation();
		}
		
		const FVector OutStartTrace = StartLocation + AimDir * ((PlayerCharacter->GetActorLocation() - StartLocation) | AimDir);
		const FVector OutEndTrace = OutStartTrace + AimDir * OutlineTraceRange;
		
		TArray<AActor*> IgnoreActors;
		IgnoreActors.Add(PlayerCharacter);
		
		FHitResult Hit(ForceInit);
		auto DrawDebug = EDrawDebugTrace::None;
		UKismetSystemLibrary::SphereTraceSingle(this, OutStartTrace, OutEndTrace, UCoreUtils::GetPlayerControllerSphereTraceRadius(this), UEngineTypes::ConvertToTraceType(GENESTEALER_TRACE_INTERACTION), false, IgnoreActors, DrawDebug, Hit, true
			, FLinearColor::Red, FLinearColor::Green, 1.f);
		AActor* HitActor = Hit.GetActor();
		if(HitActor && Hit.GetActor()->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
		{
			if(HitActor->IsA(ABaseWeaponPickup::StaticClass()))
			{
				return HitActor->GetDistanceTo(PlayerCharacter) <= 1500.f ? HitActor : nullptr;
			}

			if(HitActor->IsA(ABaseWeaponPickup::StaticClass()))
			{
				return HitActor->GetDistanceTo(PlayerCharacter) <= UCoreUtils::GetCoverPointValidDistance() ? HitActor : nullptr;
			}

			return HitActor;
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
	if(ABaseOverlapPickup* Pickup = Cast<ABaseOverlapPickup>(InObject))
	{
		return true;
	}
	return false;
}

bool ABasePlayerController::ShouldOutlineInteractable(TScriptInterface<IInteractable> InInteractable) const
{
	if(const UObject* InterObj = InInteractable.GetObject())
	{
		if(InterObj->IsA(ABaseOverlapPickup::StaticClass()))
		{
			return true;
		}	
	}

	if(const UObject* InterObj = InInteractable.GetObject())
	{
		if(InterObj->IsA(ABaseCoverActor::StaticClass()))
		{
			return true;
		}	
	}
	
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

	if(ABaseHUD* BaseHUD = UCoreUtils::GetBaseHUD(this))
	{
		BaseHUD->InitHUDOnNewPawnPossessed();
	}
}

void ABasePlayerController::CoverAction(const FInputActionValue& Value)
{
	if (PlayerCharacter)
	{
		PlayerCharacter->CoverDodgeAction();
	}
}

void ABasePlayerController::InteractAction(const FInputActionValue& Value)
{
	if(CurrentInteractableActor)
	{
		CurrentInteractableActor->InteractWithActor(PlayerCharacter);
	}
}

void ABasePlayerController::FireAction(const FInputActionValue& Value)
{
	if (PossessedCharacter)
	{
		PossessedCharacter->FireAction(Value.Get<bool>());
	}
}

void ABasePlayerController::SecureLocationOrder(const FInputActionValue& Value)
{
	for(TScriptInterface<IAIPawn> Act : AISquadMembers)
	{
		UKismetSystemLibrary::PrintString(this, Act.GetObject()->GetName() + " is securing location");		
	}
}