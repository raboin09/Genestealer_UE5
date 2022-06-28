// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/AudioManager.h"

#include "Kismet/GameplayStatics.h"

UAudioComponent* UAudioManager::SpawnSoundAttached(USoundBase* Sound, USceneComponent* AttachToComponent, FName AttachPointName, FVector Location, FRotator Rotation, EAttachLocation::Type LocationType, bool bStopWhenAttachedToDestroyed,
	float VolumeMultiplier, float PitchMultiplier, float StartTime, USoundAttenuation* AttenuationSettings, USoundConcurrency* ConcurrencySettings, bool bAutoDestroy)
{
	return UGameplayStatics::SpawnSoundAttached(Sound, AttachToComponent, AttachPointName, Location, LocationType, bStopWhenAttachedToDestroyed, VolumeMultiplier, PitchMultiplier, StartTime, AttenuationSettings, ConcurrencySettings, bAutoDestroy);
}

UAudioComponent* UAudioManager::SpawnSoundAtLocation(const UObject* WorldContextObject, USoundBase* Sound, FVector Location, FRotator Rotation, float VolumeMultiplier, float PitchMultiplier, float StartTime, USoundAttenuation* AttenuationSettings,
                                                     USoundConcurrency* ConcurrencySettings, bool bAutoDestroy)
{
	return UGameplayStatics::SpawnSoundAtLocation(WorldContextObject, Sound, Location, Rotation, VolumeMultiplier, PitchMultiplier, StartTime, AttenuationSettings, ConcurrencySettings, bAutoDestroy);
}

void UAudioManager::StopSound(UAudioComponent* AudioToStop, bool bFadeOut, float FadeTime)
{
	if(!AudioToStop)
	{
		return;
	}
	
	if(bFadeOut)
	{
		AudioToStop->FadeOut(FadeTime, 0.f);
	} else
	{
		AudioToStop->Stop();
	}
}
