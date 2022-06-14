// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/BaseWeaponPickup.h"

#include "Characters/InventoryComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Utils/CoreUtils.h"

ABaseWeaponPickup::ABaseWeaponPickup()
{
	SummonedStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SummonedStaticMesh"));
	SummonedStaticMesh->SetupAttachment(RootComponent);
	SummonedStaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SummonedStaticMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	
	SummonedSkelMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SummonedSkeletalMesh"));
	SummonedSkelMesh->SetupAttachment(SummonedStaticMesh);
	SummonedSkelMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SummonedSkelMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
}

UMeshComponent* ABaseWeaponPickup::GetMesh() const
{
	if(SummonedSkelMesh && SummonedSkelMesh->SkeletalMesh != nullptr)
	{
		return SummonedSkelMesh;
	}
	return SummonedStaticMesh;
}

void ABaseWeaponPickup::ConsumePickup(ACharacter* ConsumingChar)
{
	if(ConsumingChar)
	{
		if(UInventoryComponent* InventoryComponent = UCoreUtils::GetInventoryComponentFromActor(ConsumingChar))
		{
			InventoryComponent->GiveWeaponClassAmmo(WeaponPickupClass, 20);
		}
	}
}

bool ABaseWeaponPickup::CanPickup(ACharacter* PotentialChar)
{
	if(!UGameplayTagUtils::ActorHasGameplayTag(this, TAG_STATE_ACTIVE))
	{
		return false;
	}
	
	if(const UInventoryComponent* InventoryComponent = UCoreUtils::GetInventoryComponentFromActor(PotentialChar))
	{
		return InventoryComponent->HasWeapon(WeaponPickupClass);
	}
	return false;
}