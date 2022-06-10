// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/HealthComponent.h"
#include "Characters/BaseCharacter.h"
#include "Kismet/KismetSystemLibrary.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

float UHealthComponent::TakeDamage(const float RawDamage, AActor* ReceivingActor, AActor* InstigatingActor, const FDamageHitReactEvent& HitReactEvent)
{
	if(!IsAlive())
	{
		FCurrentWoundEventPayload WoundEventPayload;
		WoundEventPayload.DamageHitReactEvent = HitReactEvent;
		WoundEventPayload.bNaturalChange = true;
		CurrentHealthChanged.Broadcast(WoundEventPayload);	
		return 0.f;
	}

	const FWound OldWound = WoundContainer.GetCurrentWound();
	const float Delta = CalculateDamage(RawDamage);
	WoundContainer.TakeDamage(Delta);
	const FWound NewWound = WoundContainer.GetCurrentWound();
	if(WoundContainer.IsAlive())
	{
		FCurrentWoundEventPayload WoundEventPayload;
		WoundEventPayload.InstigatingActor = InstigatingActor;
		WoundEventPayload.ReceivingActor = ReceivingActor;
		WoundEventPayload.NewWound = NewWound;
		WoundEventPayload.OldWound = OldWound;
		WoundEventPayload.MaxWounds = WoundContainer.MaxWounds;
		WoundEventPayload.Delta = Delta;
		WoundEventPayload.Percentage = NewWound.GetWoundHealthPercent();
		WoundEventPayload.bNaturalChange = true;
		WoundEventPayload.bWasDamage = true;
		WoundEventPayload.DamageHitReactEvent = HitReactEvent;
		WoundEventPayload.DamageHitReactEvent.DamageTaken = Delta;
		CurrentHealthChanged.Broadcast(WoundEventPayload);		
	} else
	{
		FActorDeathEventPayload DeathEventPayload;
		DeathEventPayload.DyingActor = ReceivingActor;
		DeathEventPayload.KillingActor = InstigatingActor;
		DeathEventPayload.HitReactEvent = HitReactEvent;
		DeathEventPayload.HitResult = HitReactEvent.HitResult;
		ActorDeath.Broadcast(DeathEventPayload);
	}
	return Delta;
}

void UHealthComponent::ApplyHeal(const float RawHeal, AActor* ReceivingActor, AActor* InstigatingActor)
{
	const float Delta = RawHeal;
	const FWound OldWound = WoundContainer.GetCurrentWound();
	WoundContainer.ApplyHeal(Delta);
	const FWound NewWound = WoundContainer.GetCurrentWound();
	FCurrentWoundEventPayload WoundEventPayload;
	WoundEventPayload.InstigatingActor = InstigatingActor;
	WoundEventPayload.ReceivingActor = ReceivingActor;
	WoundEventPayload.NewWound = NewWound;
	WoundEventPayload.OldWound = OldWound;
	WoundEventPayload.bWasDamage = false;
	WoundEventPayload.MaxWounds = WoundContainer.MaxWounds;
	WoundEventPayload.Delta = Delta;
	WoundEventPayload.Percentage = NewWound.GetWoundHealthPercent();
	WoundEventPayload.bNaturalChange = true;
	CurrentHealthChanged.Broadcast(WoundEventPayload);
}

void UHealthComponent::OverrideWoundContainer(const FHealthDefaults& InRot)
{
	WoundContainer.InitWoundContainer(InRot);
}

void UHealthComponent::InitHealthComponent(const FHealthDefaults& HealthModel)
{
	WoundContainer.InitWoundContainer(HealthModel);
	if(CurrentHealthChanged.IsBound())
	{
		FCurrentWoundEventPayload WoundEventPayload;
		WoundEventPayload.NewWound = WoundContainer.GetCurrentWound();
		WoundEventPayload.OldWound = WoundContainer.GetCurrentWound();
		WoundEventPayload.MaxWounds = WoundContainer.MaxWounds;
		WoundEventPayload.Delta = 0.f;
		WoundEventPayload.Percentage = WoundContainer.GetCurrentWound().GetWoundHealthPercent();
		WoundEventPayload.bNaturalChange = false;
		CurrentHealthChanged.Broadcast(WoundEventPayload);
	}
}

void UHealthComponent::AddMaxWounds(float MaxWoundsToAdd)
{
	const float OldWounds = WoundContainer.MaxWounds;
	WoundContainer.AddToMaxWounds(MaxWoundsToAdd);
	const float NewWounds = WoundContainer.MaxWounds;
	FMaxWoundsEventPayload MaxWoundsEventPayload;
	MaxWoundsEventPayload.Delta = NewWounds - OldWounds;
	MaxWoundsEventPayload.NewMaxWounds = NewWounds;
	MaxWoundsEventPayload.OldMaxWounds = OldWounds;
	MaxWoundsChanged.Broadcast(MaxWoundsEventPayload);	
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
}

float UHealthComponent::CalculateDamage(const float RawDamage) const
{
	return RawDamage;
}

bool UHealthComponent::IsAlive()
{
	return WoundContainer.IsAlive();
}

void UHealthComponent::Execute(AActor* ExecutedActor, AActor* ExecutingActor)
{
	TakeDamage(WoundContainer.GetAllWoundsHealthSum(), ExecutedActor, ExecutingActor, FDamageHitReactEvent());
}

