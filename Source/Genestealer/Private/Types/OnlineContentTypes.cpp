// Fill out your copyright notice in the Description page of Project Settings.


#include "Types/OnlineContentTypes.h"

void FUnitStats::ResolveRawToEnums()
{
	switch (RawArmorSave)
	{
	case 0:
		ArmorSave = EArmorSave::Default;
		break;
	case 1:
		ArmorSave = EArmorSave::Invulnerable;
		break;
	case 2:
		ArmorSave = EArmorSave::TwoPlus;
		break;
	case 3:
		ArmorSave = EArmorSave::ThreePlus;
		break;
	case 4:
		ArmorSave = EArmorSave::FourPlus;
		break;
	case 5:
		ArmorSave = EArmorSave::FivePlus;
		break;
	case 6:
		ArmorSave = EArmorSave::SixPlus;
		break;
	default:
		ArmorSave = EArmorSave::Default;
		break;
	}

	switch(RawBallisticSkill)
	{
	case 0:
		BallisticSkill = EBallisticSkill::Default;
		break;
	case 1:
		BallisticSkill = EBallisticSkill::FullAccuracy;
		break;
	case 2:
		BallisticSkill = EBallisticSkill::TwoPlus;
		break;
	case 3:
		BallisticSkill = EBallisticSkill::ThreePlus;
		break;
	case 4:
		BallisticSkill = EBallisticSkill::FourPlus;
		break;
	case 5:
		BallisticSkill = EBallisticSkill::FivePlus;
		break;
	case 6:
		BallisticSkill = EBallisticSkill::SixPlus;
		break;
	default:
		BallisticSkill = EBallisticSkill::Default;
		break;
	}
}
