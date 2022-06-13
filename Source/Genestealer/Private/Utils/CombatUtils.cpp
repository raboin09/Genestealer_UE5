// Fill out your copyright notice in the Description page of Project Settings.


#include "Utils/CombatUtils.h"

#include "API/Attackable.h"
#include "Kismet/KismetSystemLibrary.h"

float UCombatUtils::GetHitImpulseValue(EHitReactType InHit)
{
	const float Multiplier = 30000.f;
	switch (InHit)
	{
	case EHitReactType::Knockback_VeryLight: return .5f * Multiplier;
	case EHitReactType::Knockback_Light: return .7f * Multiplier;
	case EHitReactType::Knockback_Medium: return .85f * Multiplier;
	case EHitReactType::Knockback_Heavy: return 1.f * Multiplier;
	case EHitReactType::Knockback_VeryHeavy: return 1.15f * Multiplier;
	case EHitReactType::Knockback_Huge: return 2.f * Multiplier;
	default: return 0.f;
	}
}

bool UCombatUtils::ShouldHitKnockback(EHitReactType InHit)
{
	switch (InHit)
	{
	case EHitReactType::Knockback_VeryLight:
	case EHitReactType::Knockback_Light:
	case EHitReactType::Knockback_Medium:
	case EHitReactType::Knockback_Heavy:
	case EHitReactType::Knockback_VeryHeavy:
	case EHitReactType::Knockback_Huge:
		return true;
	case EHitReactType::HitReact_Light:
	case EHitReactType::None:
	default:
		return false;
	}
}

float UCombatUtils::GetKnockbackRecoveryTime(EHitReactType InHit)
{
	switch (InHit)
	{
	case EHitReactType::Knockback_VeryLight:
		return 1.f;
	case EHitReactType::Knockback_Light:
		return 1.15f;
	case EHitReactType::Knockback_Medium:
		return 1.3f;
	case EHitReactType::Knockback_Heavy:
		return 1.5f;
	case EHitReactType::Knockback_VeryHeavy:
		return 1.75f;
	case EHitReactType::Knockback_Huge:
		return 2.f;
	case EHitReactType::HitReact_Light:
	default:
		return 0.f;
	}
}

bool UCombatUtils::IsBoneNameHead(const FName& InBone)
{
	TArray<FName> ValidBones;
	ValidBones.Add("head");
	ValidBones.Add("neck_01");
	ValidBones.Add("clavicle_l");
	ValidBones.Add("clavicle_r");
 	return ValidBones.Contains(InBone);
}

FName UCombatUtils::GetNearestValidBoneForImpact(FName InBone)
{
	const TArray<FName> LeftArmBones = {
		"upperarm_l",
		"lowerarm_l",
		"hand_l",
		"index_01_l",
		"index_02_l",
		"index_03_l",
		"middle_01_l",
		"middle_02_l",
		"middle_03_l",
		"pinky_01_l",
		"pinky_02_l",
		"pinky_03_l",
		"ring_01_l",
		"ring_02_l",
		"ring_03_l",
		"thumb_01_l",
		"thumb_02_l",
		"thumb_03_l"
	};

	const TArray<FName> RightArmBones = {
		"upperarm_r",
		"lowerarm_r",
		"hand_r",
		"index_01_r",
		"index_02_r",
		"index_03_r",
		"middle_01_r",
		"middle_02_r",
		"middle_03_r",
		"pinky_01_r",
		"pinky_02_r",
		"pinky_03_r",
		"ring_01_r",
		"ring_02_r",
		"ring_03_r",
		"thumb_01_r",
		"thumb_02_r",
		"thumb_03_r"
	};
	
	if(LeftArmBones.Contains(InBone))
	{
		return "clavicle_l";
	}

	if(RightArmBones.Contains(InBone)){
		return "clavicle_r";
	}

	if(InBone.IsEqual("foot_l"))
	{
		return "calf_l";
	}
	
	if(InBone.IsEqual("foot_r"))
	{
		return "calf_r";
	}

	if(InBone.IsEqual("head"))
	{
		return "spine_03";
	}
	return InBone;
}

FRotator UCombatUtils::GetRotationFromComponentHit(const FHitResult& Impact)
{
	if(!Impact.Component.Get() || !Impact.Component.IsValid())
	{
		return FRotator();
	}
	
	const FQuat RootQuat = Impact.Component->GetComponentQuat();
	const FVector UpVector = RootQuat.GetUpVector();
	FVector RotationAxis = FVector::CrossProduct(UpVector, Impact.ImpactNormal);
	RotationAxis.Normalize();

	const float DotProduct = FVector::DotProduct(UpVector, Impact.ImpactNormal);
	const float RotationAngle = acosf(DotProduct);

	const FQuat Quat = FQuat(RotationAxis, RotationAngle);

	const FQuat NewQuat = Quat * RootQuat;
	return NewQuat.Rotator();
}

bool UCombatUtils::AreActorsAllies(TScriptInterface<IInteractable> FirstActor, AActor* SecondActor)
{
	IAttackable* CastedOwner = Cast<IAttackable>(SecondActor);
	if(FirstActor && CastedOwner)
	{
		return FirstActor->GetInteractableAffiliation() == CastedOwner->GetAffiliation();
	}
	return false;
}

bool UCombatUtils::AreActorsEnemies(TScriptInterface<IInteractable> FirstActor, AActor* SecondActor)
{
	IAttackable* CastedOwner = Cast<IAttackable>(SecondActor);
	if(FirstActor && CastedOwner)
	{
		return FirstActor->GetInteractableAffiliation() != CastedOwner->GetAffiliation();
	}
	return false;
}

bool UCombatUtils::AreActorsAllies(AActor* FirstActor, AActor* SecondActor)
{
	IAttackable* CastedChar = Cast<IAttackable>(FirstActor);
	IAttackable* CastedOwner = Cast<IAttackable>(SecondActor);
	if(CastedChar && CastedOwner)
	{
		return CastedChar->GetAffiliation() == CastedOwner->GetAffiliation();
	}
	return false;
}

bool UCombatUtils::AreActorsEnemies(AActor* FirstActor, AActor* SecondActor)
{
	IAttackable* CastedChar = Cast<IAttackable>(FirstActor);
	IAttackable* CastedOwner = Cast<IAttackable>(SecondActor);
	if(CastedChar && CastedOwner)
	{
		return CastedChar->GetAffiliation() != CastedOwner->GetAffiliation();
	}
	return false;
}

bool UCombatUtils::IsActorNeutral(AActor* FirstActor)
{
	if(const IAttackable* CastedChar = Cast<IAttackable>(FirstActor))
	{
		return CastedChar->GetAffiliation() == EAffiliation::Neutral;
	}
	return false;
}

bool UCombatUtils::IsActorNeutral(TScriptInterface<IInteractable> FirstActor)
{
	if(FirstActor)
	{
		return FirstActor->GetInteractableAffiliation() == EAffiliation::Neutral;
	}
	return false;
}


int32 UCombatUtils::GetOutlineIntFromColor(const EOutlineColor InColor)
{
	switch (InColor)
	{
	case EOutlineColor::Green: return 252;
	case EOutlineColor::Red: return 254;
	case EOutlineColor::Gray: return 255;
	case EOutlineColor::Purple: return 253;
	default: return 255;
	}
}

int32 UCombatUtils::GetOutlineIntFromAffiliation(const EAffiliation InAffiliation)
{
	switch (InAffiliation)
	{
	case EAffiliation::Allies: return 252;
	case EAffiliation::Enemies: return 254;
	case EAffiliation::Neutral: return 255;
	default: return 255;
	}
}