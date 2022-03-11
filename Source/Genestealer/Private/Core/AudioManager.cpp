// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/AudioManager.h"

#include "Kismet/GameplayStatics.h"

UAudioComponent* UAudioManager::SpawnSoundAttached(USoundBase* Sound, USceneComponent* ParentComponent)
{
	UAudioComponent* AC = nullptr;
	if (Sound)
	{
		AC = UGameplayStatics::SpawnSoundAttached(Sound, ParentComponent);
	}
	return AC;
}

void UAudioManager::PlaySoundAtLocation(const UObject* ContextObj, USoundBase* Sound, const FVector& Location)
{
	UGameplayStatics::PlaySoundAtLocation(ContextObj, Sound, Location);
}
