// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BaseAICharacter.h"

#include "AI/BaseAIController.h"
#include "Characters/InteractionComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Quest/QuestManagerComponent.h"
#include "Utils/WorldUtils.h"

ABaseAICharacter::ABaseAICharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	AIControllerClass = ABaseAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
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

void ABaseAICharacter::FireWeapon(bool bStartFiring)
{
	if(bStartFiring)
	{
		GL_HandleFireAction(false);	
	}
	GL_HandleFireAction(bStartFiring);
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
	UQuestManagerComponent::TryAddActorToActiveQuests(this);
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

	UWorldUtils::TryRemoveActorFromQuestableArray(this);
	
	Super::HandleDeathEvent(DeathEventPayload);
}
