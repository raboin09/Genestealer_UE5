
#include "Core/PlayerStatsComponent.h"

#include "Utils/CoreUtils.h"

UPlayerStatsComponent::UPlayerStatsComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPlayerStatsComponent::RecordStatsEvent(const UObject* ContextObject, EStatsEvent Event, float Modifier, AActor* NonPlayerActorInstigator)
{
	if(UPlayerStatsComponent* PlayerStatsComponent = UCoreUtils::GetPlayerStatsComponent(ContextObject))
	{
		PlayerStatsComponent->HandleStatsEvent(Event, Modifier, NonPlayerActorInstigator);
	}
}

void UPlayerStatsComponent::HandleStatsEvent(EStatsEvent Event, float Modifier, AActor* NonPlayerActorInstigator)
{
	switch (Event)
	{
	case Headshot:
		{
			PlayerStats.Headshots += Modifier;
			//UKismetSystemLibrary::PrintString(this, "HEADSHOT%: " + FString::SanitizeFloat(PlayerStats.Headshots / PlayerStats.ShotsHit));
			break;
		}
	case ShotFired:
		{
			PlayerStats.ShotsFired += Modifier;
			float Accuracy = PlayerStats.ShotsHit / PlayerStats.ShotsFired;
			//UKismetSystemLibrary::PrintString(this, "ACCURACY%: " + FString::SanitizeFloat(Accuracy));
			break;
		}
	case ShotHit:
		{
			PlayerStats.ShotsHit += Modifier;
			float Accuracy = PlayerStats.ShotsHit / PlayerStats.ShotsFired;
			//UKismetSystemLibrary::PrintString(this, "ACCURACY%: " + FString::SanitizeFloat(Accuracy));
			break;
		}
	case MeleeHit:
		{
			PlayerStats.MeleeHit += Modifier;
			float Accuracy = PlayerStats.MeleeHit / PlayerStats.MeleeSwung;
			//UKismetSystemLibrary::PrintString(this, "MELEE%: " + FString::SanitizeFloat(Accuracy));
			break;
		}
	case MeleeSwung:
		{
			PlayerStats.MeleeSwung += Modifier;
			float Accuracy = PlayerStats.MeleeHit / PlayerStats.MeleeSwung;
			//UKismetSystemLibrary::PrintString(this, "MELEE%: " + FString::SanitizeFloat(Accuracy));
			break;
		}
	case DamageGiven:
		{
			if(!PlayerStats.DamageTakenMap.Contains(NonPlayerActorInstigator))
			{
				PlayerStats.DamageTakenMap.Add(NonPlayerActorInstigator, Modifier);

			} else
			{
				const float OldMod = *PlayerStats.DamageTakenMap.Find(NonPlayerActorInstigator);
				const float NewMod = OldMod + Modifier;
				PlayerStats.DamageTakenMap.Add(NonPlayerActorInstigator, NewMod);	
			}		

			PlayerStats.PlayerTotalDamageGiven += Modifier;
			break;
		}
	case DamageTaken:
		{
			PlayerStats.PlayerTotalDamageTaken += Modifier;
			break;
		}
	default: break;
	}
}
