// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/HealthComponent.h"
#include "Characters/BaseCharacter.h"

void FWoundContainer::ZeroizeWoundContainer()
{
	for(FWound& CurrWound : Wounds)
	{
		CurrWound.CurrentHealth = 0;
	}
}

void FWoundContainer::MaximizeWoundContainer()
{
	for(FWound& CurrWound : Wounds)
	{
		CurrWound.CurrentHealth = MaxHealthPerWound;
		CurrWound.MaxHealth = MaxHealthPerWound;
	}
}

void FWoundContainer::TakeDamage(float Damage)
{
	FWound& CurrWound = GetCurrentWound();
	if(Damage > CurrWound.CurrentHealth)
	{
		const float Delta = Damage - CurrWound.CurrentHealth;
		CurrWound.CurrentHealth = 0;
		if(CurrWound.WoundArrayIndex + 1 < Wounds.Num())
		{
			Wounds[CurrWound.WoundArrayIndex + 1].CurrentHealth -= Delta;
		}
	} else
	{
		CurrWound.CurrentHealth -= Damage;
	}
}

void FWoundContainer::ApplyHeal(float HealAmount)
{
	FWound& CurrWound = GetCurrentWound();
	if(HealAmount + CurrWound.CurrentHealth > CurrWound.MaxHealth)
	{
		const float Delta = CurrWound.MaxHealth - HealAmount + CurrWound.CurrentHealth;
		CurrWound.CurrentHealth = CurrWound.MaxHealth;
		Wounds[CurrWound.WoundArrayIndex - 1].CurrentHealth += Delta;
	} else
	{
		CurrWound.CurrentHealth += HealAmount;
	}
}

bool FWoundContainer::IsAlive()
{
	if(Wounds.Num() <= 0)
		return false;
	return Wounds[Wounds.Num() - 1].CurrentHealth > 0;
}

float FWoundContainer::GetAllWoundsHealthSum()
{
	float TempSum = 0.f;
	for(const FWound& CurrWound : Wounds)
	{
		if(CurrWound.CurrentHealth > 0)
		{
			TempSum += CurrWound.CurrentHealth;
		}
	}
	return TempSum;
}

FWound& FWoundContainer::GetCurrentWound()
{	
	// Check if dead
	if(Wounds[Wounds.Num() - 1].CurrentHealth <= 0)
	{
		return Wounds[Wounds.Num() - 1];
	}
	
	for(FWound& CurrWound : Wounds)
	{
		// If a Wound has 0 < X < 100 health, it's the Current Wound
		if(CurrWound.CurrentHealth > 0 && !CurrWound.IsFullHealth())
		{
			return CurrWound;
		}

		// If a Wound has 100 HP but the previous has 0, then it's the Current Wound (ex. 0|0|100|100 , then the third is our Current Wound)
		if(CurrWound.WoundArrayIndex > 0 && Wounds[CurrWound.WoundArrayIndex - 1].CurrentHealth <= 0 && CurrWound.CurrentHealth > 0)
		{
			return CurrWound;
		}
	}
	
	// None of the conditions fit, default to full HP
	return Wounds[0];
}

void FWoundContainer::InitWoundContainer(const FHealthDefaults& InData)
{
	Wounds.Empty();
	for(int i=0; i<InData.MaxWounds; i++)
	{
		FWound TempWound = FWound(InData, i);
		Wounds.Add(TempWound);
	}
	MaxHealthPerWound = InData.MaxHealthPerWound;
	MaxWounds = InData.MaxWounds;
}

void FWoundContainer::AddToMaxWounds(int32 MaxWoundsToAdd)
{
	for(int i=Wounds.Num() - 1; i < Wounds.Num() + MaxWoundsToAdd - 1; i++)
	{
		FWound TempWound = FWound();
		TempWound.CurrentHealth = MaxHealthPerWound;
		TempWound.MaxHealth = MaxHealthPerWound;
		Wounds.Add(TempWound);
	}
	MaxWounds += MaxHealthPerWound;
}

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
		if(CurrentHealthChanged.IsBound())
		{
			CurrentHealthChanged.Broadcast(WoundEventPayload);	
		}
		return 0.f;
	}
	
	const FWound OldWound = WoundContainer.GetCurrentWound();
	const float Delta = CalculateDamage(RawDamage);
	WoundContainer.TakeDamage(Delta);
	const FWound NewWound = WoundContainer.GetCurrentWound();
	const bool bIsAlive = WoundContainer.IsAlive();
	if(bIsAlive && CurrentHealthChanged.IsBound())
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
		if(CurrentHealthChanged.IsBound())
		{
			CurrentHealthChanged.Broadcast(WoundEventPayload);
		}
		
	} else if (!bIsAlive && ActorDeath.IsBound())
	{
		FDeathEventPayload DeathEventPayload;
		DeathEventPayload.DyingActor = ReceivingActor;
		DeathEventPayload.KillingActor = InstigatingActor;
		DeathEventPayload.HitReactEvent = HitReactEvent;
		if(ActorDeath.IsBound())
		{
			ActorDeath.Broadcast(DeathEventPayload);	
		}
	}
	return Delta;
}

void UHealthComponent::ApplyHeal(const float RawHeal, AActor* ReceivingActor, AActor* InstigatingActor)
{
	const float Delta = RawHeal;
	const FWound OldWound = WoundContainer.GetCurrentWound();
	WoundContainer.ApplyHeal(Delta);
	const FWound NewWound = WoundContainer.GetCurrentWound();
	
	if(!CurrentHealthChanged.IsBound())
	{
		return;
	}
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
	if(CurrentHealthChanged.IsBound())
	{
		CurrentHealthChanged.Broadcast(WoundEventPayload);		
	}
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
		if(CurrentHealthChanged.IsBound())
		{
			CurrentHealthChanged.Broadcast(WoundEventPayload);	
		}
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
	if(MaxWoundsChanged.IsBound())
	{
		MaxWoundsChanged.Broadcast(MaxWoundsEventPayload);	
	}
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
	if(!IsValidChecked(this))
	{
		return WoundContainer.IsAlive();	
	}
	return false;
}

void UHealthComponent::Execute(AActor* ExecutedActor, AActor* ExecutingActor)
{
	TakeDamage(WoundContainer.GetAllWoundsHealthSum(), ExecutedActor, ExecutingActor, FDamageHitReactEvent());
}

