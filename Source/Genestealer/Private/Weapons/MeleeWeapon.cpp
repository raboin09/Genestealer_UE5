// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/MeleeWeapon.h"

#include "GameFramework/Character.h"
#include "Genestealer/Genestealer.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Utils/EffectUtils.h"
#include "Utils/GameplayTagUtils.h"

void AMeleeWeapon::Activate()
{
	if (!bIsActive) {
		HitActors.Empty();
		bIsActive = true;
		Internal_StartCollisionRaycastingTick();
	}
}

void AMeleeWeapon::Deactivate()
{
	if (bIsActive) {
		Internal_StopAttack();
	}
}

void AMeleeWeapon::EnableComboWindow()
{
	GetTagContainer().RemoveTag(TAG_STATE_COMBO_ACTIVATED);
	GetTagContainer().AddTag(TAG_STATE_COMBO_WINDOW_ENABLED);
}

void AMeleeWeapon::DisableComboWindow()
{
	GetTagContainer().RemoveTag(TAG_STATE_COMBO_WINDOW_ENABLED);
	if(!GetTagContainer().HasTag(TAG_STATE_COMBO_ACTIVATED))
	{
		ComboSectionIncrement = 1;
	}
}

void AMeleeWeapon::StartFire()
{	
	if (IsWeaponOnCooldown())
	{
		return;
	}
	
	if(!GetTagContainer().HasTag(TAG_STATE_ATTACK_COMMITTED))
	{
		Internal_StartAttack();
		return;
	}
	
	if(GetTagContainer().HasTag(TAG_STATE_COMBO_WINDOW_ENABLED))
	{
		GetTagContainer().AddTag(TAG_STATE_COMBO_ACTIVATED);
		GetTagContainer().RemoveTag(TAG_STATE_COMBO_WINDOW_ENABLED);
		if(ComboSectionIncrement >= MaxComboSections)
		{
			Internal_ResetComboCounter();
		} else
		{
			ComboSectionIncrement++;
		}
		Internal_StartAttack();
	}
}

void AMeleeWeapon::StopFire()
{
	
}

void AMeleeWeapon::BeginPlay()
{
	Super::BeginPlay();
	MeshComponentRef = GetWeaponMesh();
	ComboSectionIncrement = 1;
	bIsActive = false;
	HitActors.Empty();
	for(const FName& Socket : MeshComponentRef->GetAllSocketNames())
	{
		if(Socket.ToString().Contains(SocketPrefix))
		{
			Sockets.Add(Socket.ToString(), FVector::ZeroVector);
		}
	}
}

FAnimMontagePlayData AMeleeWeapon::Internal_GetPlayData() const
{
	FAnimMontagePlayData PlayData;	
	PlayData.MontageToPlay = FireAnim;
	if(OwningPawn && OwningPawn->IsPlayerControlled())
	{
		PlayData.bForceInPlace = true;
	}
	PlayData.MontageSection = Internal_GetNextMontageSection();
	return PlayData;
}

void AMeleeWeapon::Internal_StartCollisionRaycastingTick()
{
	if(!MeshComponentRef)
	{
		return;
	}

	Internal_SetCurrentSocketLocations();
	GetWorldTimerManager().SetTimer(Timer_Raycasting, this, &AMeleeWeapon::Internal_CheckForCollisionHit, .01f, true);
}

void AMeleeWeapon::Internal_StopCollisionRaycastingTick()
{
	GetWorldTimerManager().ClearTimer(Timer_Raycasting);
}

void AMeleeWeapon::Internal_CheckForCollisionHit()
{
	if(!MeshComponentRef)
	{
		return;
	}
	
	TArray<FString> Keys;
	Sockets.GetKeys(Keys);
	for(const FString& Key : Keys)
	{
		FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(WeaponTrace), true, GetInstigator());
		TraceParams.bReturnPhysicalMaterial = true;
		FHitResult Hit(ForceInit);
		TArray<AActor*> IgnoreActors = { GetInstigator(), this, GetOwner() };
		UKismetSystemLibrary::LineTraceSingle(this, *Sockets.Find(Key), MeshComponentRef->GetSocketLocation(FName(Key)), UEngineTypes::ConvertToTraceType(TRACE_WEAPON), false, IgnoreActors, EDrawDebugTrace::None, Hit, true, FLinearColor::Red, FLinearColor::Green, 1.f);
		if(Hit.bBlockingHit && bIsActive && !HitActors.Contains(Hit.GetActor()))
		{
			HitActors.Add(Hit.GetActor());
			UEffectUtils::ApplyEffectsToHitResult(WeaponEffects, Hit, GetInstigator());
			break;
		}
	}
	Internal_SetCurrentSocketLocations();
}

FName AMeleeWeapon::Internal_GetNextMontageSection() const
{
	return FName("Combo" + FString::FromInt(ComboSectionIncrement));
}

void AMeleeWeapon::Internal_SetCurrentSocketLocations()
{
	TArray<FString> Keys;
	Sockets.GetKeys(Keys);
	for(const FString& Key : Keys)
	{
		Sockets.Add(Key, MeshComponentRef->GetSocketLocation(FName(Key)));	
	}	
}

void AMeleeWeapon::Internal_StartAttack()
{
	GetTagContainer().AddTag(TAG_STATE_ATTACK_COMMITTED);
	const FAnimMontagePlayData PlayData = Internal_GetPlayData();
	PlayWeaponAnimation(PlayData);
	PlayWeaponSound(FireSound);
	LastFireTime = GetWorld()->GetTimeSeconds();
}

void AMeleeWeapon::Internal_StopAttack()
{
	GetTagContainer().RemoveTag(TAG_STATE_ATTACK_COMMITTED);
	bIsActive = false;
	HitActors.Empty();
	Internal_StopCollisionRaycastingTick();
}

void AMeleeWeapon::Internal_ResetComboCounter()
{
	ComboSectionIncrement = 1;
}
