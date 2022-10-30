// Fill out your copyright notice in the Description page of Project Settings.


#include "Types/HealthTypes.h"

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

float FWoundContainer::TakeDamage(float Damage)
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
		return Delta;
	}
	CurrWound.CurrentHealth -= Damage;
	return Damage;
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