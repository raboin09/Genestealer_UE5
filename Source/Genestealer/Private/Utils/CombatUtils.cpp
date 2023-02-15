// Fill out your copyright notice in the Description page of Project Settings.


#include "Utils/CombatUtils.h"

#include "API/Attackable.h"
#include "Characters/BasePlayerCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Utils/CoreUtils.h"
#include "Utils/WorldUtils.h"

AActor* UCombatUtils::GetRandomEnemy(EAbsoluteAffiliation EnemyAffiliation)
{
	int32 NumEnemies;
	switch (EnemyAffiliation) {
	case EAbsoluteAffiliation::GenestealerCult:
		NumEnemies = UKismetMathLibrary::RandomInteger(UWorldUtils::GenestealerCultActors.Num() - 1);
		return UWorldUtils::GenestealerCultActors[NumEnemies];
	case EAbsoluteAffiliation::ChaosCult:
		NumEnemies = UKismetMathLibrary::RandomInteger(UWorldUtils::ChaosCultActors.Num() - 1);
		return UWorldUtils::ChaosCultActors[NumEnemies];
	case EAbsoluteAffiliation::Neutral:
	case EAbsoluteAffiliation::Imperium:
	case EAbsoluteAffiliation::Orks:
	case EAbsoluteAffiliation::Destructible:
	default:
		return nullptr;
	}
}

float UCombatUtils::GetArmorSaveReduction(EArmorSave InArmorSave)
{
	switch (InArmorSave) {
		case EArmorSave::Default: return 1.f;
		case EArmorSave::Invulnerable: return 0.f;
		case EArmorSave::TwoPlus: return .33f;
		case EArmorSave::ThreePlus: return .5f;
		case EArmorSave::FourPlus: return .67f;
		case EArmorSave::FivePlus: return .83f;
		case EArmorSave::SixPlus: return 1.f;
		default: return 1.f;
	}
}

float UCombatUtils::GetBallisticSkillValueFromEnum(EBallisticSkill InSkill)
{
	switch(InSkill)
	{
	case EBallisticSkill::Default: return 0.f;
	case EBallisticSkill::FullAccuracy: return 0.f;
	case EBallisticSkill::TwoPlus: return .25f;
	case EBallisticSkill::ThreePlus: return .5f;
	case EBallisticSkill::FourPlus: return .75f;
	case EBallisticSkill::FivePlus: return 1.f;
	case EBallisticSkill::SixPlus: return 3.5f;
	default: return 0.f;
	}
}

float UCombatUtils::GetHitImpulseValue(EHitReactType InHit)
{
	const float Multiplier = 30000.f;
	switch (InHit)
	{
	case EHitReactType::Knockback_Tiny: return .25f * Multiplier;
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
	case EHitReactType::Knockback_Tiny:
	case EHitReactType::Knockback_VeryLight:
	case EHitReactType::Knockback_Light:
	case EHitReactType::Knockback_Medium:
	case EHitReactType::Knockback_Heavy:
	case EHitReactType::Knockback_VeryHeavy:
	case EHitReactType::Knockback_Huge:
		return true;
	case EHitReactType::HitReact_Light:
	case EHitReactType::HitReact_Chainsaw:
	case EHitReactType::None:
	default:
		return false;
	}
}

float UCombatUtils::GetKnockbackRecoveryTime(EHitReactType InHit)
{
	switch (InHit)
	{
	case EHitReactType::Knockback_Tiny:
		return .5f;
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
	if(InBone.IsEqual(NAME_None))
	{
		return "spine_03";
	}
	
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

	if(InBone.IsEqual("root"))
	{
		return "pelvis";
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
		if(IsActorNeutral(FirstActor))
		{
			return false;
		}

		if(IsActorNeutral(SecondActor))
		{
			return false;
		}

		if(IsActorDestructible(FirstActor))
		{
			return false;
		}

		if(IsActorDestructible(SecondActor))
		{
			return false;
		}
		
		return FirstActor->GetInteractableAffiliation() == CastedOwner->GetAffiliation();
	}
	return false;
}

bool UCombatUtils::AreActorsEnemies(TScriptInterface<IInteractable> FirstActor, AActor* SecondActor)
{
	IAttackable* CastedOwner = Cast<IAttackable>(SecondActor);
	if(FirstActor && CastedOwner)
	{
		if(IsActorNeutral(FirstActor))
		{
			return false;
		}

		if(IsActorNeutral(SecondActor))
		{
			return false;
		}

		if(IsActorDestructible(FirstActor))
		{
			return false;
		}

		if(IsActorDestructible(SecondActor))
		{
			return false;
		}
		
		return FirstActor->GetInteractableAffiliation() != CastedOwner->GetAffiliation();
	}
	return false;
}

bool UCombatUtils::AreActorsAllies(AActor* FirstActor, AActor* SecondActor)
{
	const IAttackable* CastedChar = Cast<IAttackable>(FirstActor);
	const IAttackable* CastedOwner = Cast<IAttackable>(SecondActor);
	if(CastedChar && CastedOwner)
	{
		if(IsActorNeutral(FirstActor))
		{
			return false;
		}
  
		if(IsActorNeutral(SecondActor))
		{
			return false;
		}

		if(IsActorDestructible(FirstActor))
		{
			return false;
		}

		if(IsActorDestructible(SecondActor))
		{
			return false;
		}
		
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
		if(IsActorNeutral(FirstActor))
		{
			return false;
		}

		if(IsActorNeutral(SecondActor))
		{
			return false;
		}

		if(IsActorDestructible(FirstActor))
		{
			return false;
		}

		if(IsActorDestructible(SecondActor))
		{
			return false;
		}
		
		return CastedChar->GetAffiliation() != CastedOwner->GetAffiliation();
	}
	return false;
}

bool UCombatUtils::IsActorNeutral(AActor* FirstActor)
{
	if(const IAttackable* CastedChar = Cast<IAttackable>(FirstActor))
	{
		return CastedChar->GetAffiliation() == EAbsoluteAffiliation::Neutral;
	}
	return false;
}

bool UCombatUtils::IsActorNeutral(TScriptInterface<IInteractable> FirstActor)
{
	if(FirstActor)
	{
		return FirstActor->GetInteractableAffiliation() == EAbsoluteAffiliation::Neutral;
	}
	return false;
}

bool UCombatUtils::IsActorDestructible(AActor* FirstActor)
{
	if(const IAttackable* CastedChar = Cast<IAttackable>(FirstActor))
	{
		return CastedChar->GetAffiliation() == EAbsoluteAffiliation::Destructible;
	}
	return false;
}


bool UCombatUtils::IsActorDestructible(TScriptInterface<IInteractable> FirstActor)
{
	if(FirstActor)
	{
		return FirstActor->GetInteractableAffiliation() == EAbsoluteAffiliation::Destructible;
	}
	return false;
}

EAffectedAffiliation UCombatUtils::GetAffiliationRelatedToPlayerCharacter(AActor* ContextActor)
{
	if(IsActorNeutral(ContextActor))
	{
		return EAffectedAffiliation::Neutral;
	}

	if(IsActorDestructible(ContextActor))
	{
		return EAffectedAffiliation::Enemies;
	}

	AActor* PlayerCharacter = UCoreUtils::GetPlayerCharacter(ContextActor);
	if(AreActorsAllies(PlayerCharacter, ContextActor))
	{
		return EAffectedAffiliation::Allies;
	}

	if(AreActorsEnemies(PlayerCharacter, ContextActor))
	{
		return EAffectedAffiliation::Enemies;
	}
	return EAffectedAffiliation::Neutral;
}

int32 UCombatUtils::GetOutlineInt(AActor* InActor)
{
	switch(GetAffiliationRelatedToPlayerCharacter(InActor))
	{
		case EAffectedAffiliation::Allies: return OUTLINE_COLOR_GREEN;
		case EAffectedAffiliation::Enemies: return OUTLINE_COLOR_RED;
		case EAffectedAffiliation::Neutral: return OUTLINE_COLOR_GRAY;
		default: return OUTLINE_COLOR_GRAY;
	}
}