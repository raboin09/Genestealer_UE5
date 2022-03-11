// Fill out your copyright notice in the Description page of Project Settings.


#include "Utils/CombatUtils.h"

#include "API/Attackable.h"

float UCombatUtils::GetHitImpulseValue(EHitReactType InHit)
{
	switch (InHit)
	{
	case EHitReactType::Knockback_VeryLight: return 25000.f;
	case EHitReactType::Knockback_Light: return 50000.f;
	case EHitReactType::Knockback_Medium: return 75000.f;
	case EHitReactType::Knockback_Heavy: return 100000.f;
	case EHitReactType::Knockback_VeryHeavy: return 125000.f;
	case EHitReactType::Knockback_Huge: return 150000.f;
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
		return 1.2f;
	case EHitReactType::Knockback_Light:
		return 1.15f;
	case EHitReactType::Knockback_Medium:
		return 1.1f;
	case EHitReactType::Knockback_Heavy:
		return 1.f;
	case EHitReactType::Knockback_VeryHeavy:
		return .95f;
	case EHitReactType::Knockback_Huge:
		return .9f;
	case EHitReactType::HitReact_Light:
	default:
		return 0.f;
	}
}

bool UCombatUtils::IsBoneNameHead(FName InBone)
{
	TArray<FName> ValidBones;
	ValidBones.Add("head");
	ValidBones.Add("neck_01");
	ValidBones.Add("clavicle_l");
	ValidBones.Add("clavicle_r");
	return ValidBones.Contains(InBone);
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
	IAttackable* CastedChar = Cast<IAttackable>(FirstActor);
	if(CastedChar)
	{
		return CastedChar->GetAffiliation() == EAffiliation::Neutral;
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