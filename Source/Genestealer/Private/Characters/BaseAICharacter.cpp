// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BaseAICharacter.h"

#include "AI/BaseAIController.h"
#include "Characters/InteractionComponent.h"
#include "Core/BasePlayerController.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Quest/QuestManagerComponent.h"
#include "Utils/CombatUtils.h"
#include "Utils/CoreUtils.h"
#include "Utils/WorldUtils.h"

ABaseAICharacter::ABaseAICharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	AIControllerClass = ABaseAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));

	MajorWaypointIndex = -1;
	MinorWaypointIndex = -1;
}

FVector ABaseAICharacter::GetSocketLocation(FName SocketName, bool bWeaponMesh) const
{
	if(bWeaponMesh)
	{
		if(!InventoryComponent || !InventoryComponent->GetEquippedWeapon()
			|| !InventoryComponent->GetEquippedWeapon()->GetWeaponMesh()
			|| !InventoryComponent->GetEquippedWeapon()->GetWeaponMesh()->DoesSocketExist(SocketName))
		{
			return GetMesh()->GetSocketLocation("head");
		}
		return InventoryComponent->GetEquippedWeapon()->GetWeaponMesh()->GetSocketLocation(SocketName);
	}
	return GetMesh()->GetSocketLocation(SocketName);
}

void ABaseAICharacter::NewEnemyAcquired()
{
	K2_HandleNewEnemyAcquired();
}

void ABaseAICharacter::FireWeapon(bool bStartFiring)
{	
	if(bStartFiring && !UGameplayTagUtils::ActorHasGameplayTag(this, TAG_STATE_STUNNED) && !GetCurrentPlayingMontage())
	{
		GL_HandleFireAction(true);	
	} else if(!bStartFiring)
	{
		GL_HandleFireAction(false);	
	}
}

void ABaseAICharacter::Aim(bool bStartAiming)
{
	AimAction(bStartAiming);
}

float ABaseAICharacter::GetWeaponRange() const
{
	if(!InventoryComponent || !InventoryComponent->GetEquippedWeapon())
	{
		return -1.f;
	}
	return InventoryComponent->GetEquippedWeapon()->GetWeaponRange();
}

void ABaseAICharacter::InteractWithActor(AActor* InstigatingActor)
{
	if(QuestObjectiveEvent.IsBound())
	{
		QuestObjectiveEvent.Broadcast(FQuestObjectiveEventPayload(this, EQuestObjectiveAction::Interact));
	}
}

void ABaseAICharacter::SwitchOutlineOnMesh(bool bShouldOutline)
{
	if(IsAlive() && InteractionComponent)
	{
		InteractionComponent->SwitchOutlineOnAllMeshes(bShouldOutline);
	}
}

void ABaseAICharacter::BeginPlay()
{
	Super::BeginPlay();
	UWorldUtils::TryAddActorToQuestableArray(this);
	//Add emptying of array in GameInstance
	UWorldUtils::TryAddActorToTeamArray(this, AbsoluteAffiliation);
	UQuestManagerComponent::TryAddActorToActiveQuests(this);

	if(bIsASquadMember && UCombatUtils::GetAffiliationRelatedToPlayerCharacter(this) == EAffectedAffiliation::Allies)
	{
		if(ABasePlayerController* BasePlayerController = UCoreUtils::GetBasePlayerController(this))
		{
			TScriptInterface<IAIPawn> NewAIPawn;
			NewAIPawn.SetInterface(Cast<IAIPawn>(this));
			NewAIPawn.SetObject(this);
			BasePlayerController->AddNewAISquadMember(NewAIPawn);
		}
	}
	MajorWaypointIndex = UKismetMathLibrary::RandomIntegerInRange(1, 5);
	MinorWaypointIndex = 1;
}

void ABaseAICharacter::HandleDeathEvent(const FActorDeathEventPayload& DeathEventPayload)
{
	if(InventoryComponent)
	{
		InventoryComponent->DestroyInventory(true, true);
	}

	if(QuestObjectiveEvent.IsBound())
	{
		QuestObjectiveEvent.Broadcast(FQuestObjectiveEventPayload(this, EQuestObjectiveAction::Death));
	}

	if(InteractionComponent)
	{
		InteractionComponent->SwitchOutlineOnAllMeshes(false);
	}

	UWorldUtils::TryRemoveActorFromQuestableArray(this);
	
	Super::HandleDeathEvent(DeathEventPayload);
}
