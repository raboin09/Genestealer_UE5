// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/MeleeWeapon.h"

#include "API/AIPawn.h"
#include "Characters/LockOnComponent.h"
#include "Core/PlayerStatsComponent.h"
#include "GameFramework/Character.h"
#include "Genestealer/Genestealer.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Utils/CoreUtils.h"
#include "Utils/EffectUtils.h"
#include "Utils/GameplayTagUtils.h"

AMeleeWeapon::AMeleeWeapon()
{
	WeaponType = EWeaponType::Melee;
}

void AMeleeWeapon::BeginPlay()
{
	Super::BeginPlay();
	MeshComponentRef = GetWeaponMesh();
	ComboSectionIncrement = 1;
	UGameplayTagUtils::RemoveTagFromActor(this, TAG_STATE_ACTIVE);
	HitActors.Empty();
	for(const FName& Socket : MeshComponentRef->GetAllSocketNames())
	{
		if(Socket.ToString().Contains(SocketPrefix))
		{
			Sockets.Add(Socket.ToString(), FVector::ZeroVector);
		}
	}
	MaxComboSections = FireAnim ? FireAnim->CompositeSections.Num() : 1;
}

void AMeleeWeapon::Activate(TArray<TSubclassOf<AActor>> ActivationEffects)
{
	RecordStatsEvent(MeleeSwung);
	
	AdditionalEffectsToApply = ActivationEffects;
	if (!UGameplayTagUtils::ActorHasGameplayTag(this, TAG_STATE_ACTIVE)) {
		HitActors.Empty();
		UGameplayTagUtils::AddTagToActor(this, TAG_STATE_ACTIVE);
		Internal_StartCollisionRaycastingTick();
	}
}

void AMeleeWeapon::Deactivate()
{
	if(UGameplayTagUtils::ActorHasGameplayTag(this, TAG_STATE_ACTIVE))
	{
		Internal_StopAttack();
	}
}

void AMeleeWeapon::EnableComboWindow()
{
	UGameplayTagUtils::RemoveTagFromActor(this, TAG_STATE_COMBO_ACTIVATED);
	UGameplayTagUtils::AddTagToActor(this, TAG_STATE_COMBO_WINDOW_ENABLED);
}

void AMeleeWeapon::DisableComboWindow()
{
	UGameplayTagUtils::RemoveTagFromActor(this, TAG_STATE_COMBO_WINDOW_ENABLED);
}

void AMeleeWeapon::ResetActivatable()
{
	Internal_StopAttack();
	UGameplayTagUtils::RemoveTagFromActor(this, TAG_STATE_COMBO_WINDOW_ENABLED);
	UGameplayTagUtils::RemoveTagFromActor(this, TAG_STATE_COMBO_ACTIVATED);
	Internal_ResetComboCounter();
}

void AMeleeWeapon::StartFire()
{
	if(UGameplayTagUtils::ActorHasGameplayTag(this, TAG_STATE_COMBO_WINDOW_ENABLED))
	{
		UGameplayTagUtils::AddTagToActor(this, TAG_STATE_COMBO_ACTIVATED);
		UGameplayTagUtils::RemoveTagFromActor(this, TAG_STATE_COMBO_WINDOW_ENABLED);
		Internal_IncrementComboCounter();
		if(!UGameplayTagUtils::ActorHasGameplayTag(this, TAG_STATE_ACTIVE))
		{
			Internal_ResetStateForCombo();
			Internal_StartAttack();
		}
	}
	else if(!UGameplayTagUtils::ActorHasGameplayTag(this, TAG_STATE_ATTACK_COMMITTED) && !UGameplayTagUtils::ActorHasGameplayTag(this, TAG_STATE_ACTIVE))
	{
		Internal_StartAttack();
	}
}

void AMeleeWeapon::StopFire()
{
	
}

void AMeleeWeapon::OnUnEquip()
{
	Super::OnUnEquip();
	ResetActivatable();
	StopWeaponAnimation(FireAnim);
}

FAnimMontagePlayData AMeleeWeapon::Internal_GetPlayData() const
{
	FAnimMontagePlayData PlayData;	
	PlayData.MontageToPlay = FireAnim;
	PlayData.MontageSection = Internal_GetNextMontageSection();
	return PlayData;
}

void AMeleeWeapon::Internal_StartCollisionRaycastingTick()
{
	if(!MeshComponentRef)
	{
		return;
	}
	K2_StartWeaponTrace();

	Internal_SetCurrentSocketLocations();
	GetWorldTimerManager().SetTimer(Timer_Raycasting, this, &AMeleeWeapon::Internal_CheckForCollisionHit, .01f, true);
}

void AMeleeWeapon::Internal_StopCollisionRaycastingTick()
{
	GetWorldTimerManager().ClearTimer(Timer_Raycasting);
	K2_StopWeaponTrace();
}

void AMeleeWeapon::Internal_CheckForCollisionHit()
{
	if(!MeshComponentRef || !UGameplayTagUtils::ActorHasGameplayTag(this, TAG_STATE_ACTIVE))
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
		const FVector StartTrace = *Sockets.Find(Key);
		const FVector EndTrace = MeshComponentRef->GetSocketLocation(FName(Key));
		const float Radius = UKismetMathLibrary::Vector_Distance(StartTrace, EndTrace) / 2;
		UKismetSystemLibrary::SphereTraceSingle(this, StartTrace, EndTrace, Radius, UEngineTypes::ConvertToTraceType(GENESTEALER_TRACE_WEAPON), false, IgnoreActors, EDrawDebugTrace::None, Hit, true, FLinearColor::Red, FLinearColor::Green, 1.f);
		if(!Hit.bBlockingHit)
		{
			continue;
		}
		
		AActor* HitActor = Hit.GetActor();
		if(HitActor && !HitActors.Contains(HitActor) && HitActor->GetClass()->ImplementsInterface(UAttackable::StaticClass()))
		{
			if(!bRecordedHit)
			{
				bRecordedHit = true;
				RecordStatsEvent(MeleeHit, 1.f, HitActor);
			}
			K2_PlayHitEffects();
			HitActors.Add(Hit.GetActor());
			TArray<TSubclassOf<AActor>> EffectsToApply = WeaponEffects;
			EffectsToApply.Append(AdditionalEffectsToApply);
			UEffectUtils::ApplyEffectsToHitResult(EffectsToApply, Hit, GetInstigator());
		}
	}
	Internal_SetCurrentSocketLocations();
}

FName AMeleeWeapon::Internal_GetNextMontageSection() const
{
	return FName(ComboPrefix + FString::FromInt(ComboSectionIncrement));
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
	UGameplayTagUtils::AddTagToActor(this, TAG_STATE_ATTACK_COMMITTED);
	const FAnimMontagePlayData PlayData = Internal_GetPlayData();
	PlayWeaponAnimation(PlayData);
	PlayWeaponSound(FireSound);
	if(ULockOnComponent* LockOnComponent = GetOwningPawn()->FindComponentByClass<ULockOnComponent>(); IsWeaponPlayerControlled())
	{
		LockOnComponent->InterpToBestTargetForMeleeAttack();
	}
	K2_OnFireWeapon();
}

void AMeleeWeapon::Internal_StopAttack()
{
	UGameplayTagUtils::RemoveTagFromActor(this, TAG_STATE_ATTACK_COMMITTED);
	UGameplayTagUtils::RemoveTagFromActor(this, TAG_STATE_ACTIVE);
	HitActors.Empty();
	bRecordedHit = false;
	AdditionalEffectsToApply.Empty();
	Internal_StopCollisionRaycastingTick();
}

void AMeleeWeapon::Internal_ResetStateForCombo()
{
	HitActors.Empty();
	UGameplayTagUtils::RemoveTagFromActor(this, TAG_STATE_COMBO_WINDOW_ENABLED);
	UGameplayTagUtils::RemoveTagFromActor(this, TAG_STATE_COMBO_ACTIVATED);
	UGameplayTagUtils::RemoveTagFromActor(this, TAG_STATE_ACTIVE);
	Internal_StopCollisionRaycastingTick();
}

void AMeleeWeapon::Internal_IncrementComboCounter()
{
	if(ComboSectionIncrement >= MaxComboSections)
	{
		Internal_ResetComboCounter();
	} else
	{
		ComboSectionIncrement++;
	}
}

void AMeleeWeapon::Internal_ResetComboCounter()
{
	ComboSectionIncrement = 1;
}
