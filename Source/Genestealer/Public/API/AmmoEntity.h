// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Types/EventDeclarations.h"
#include "UObject/Interface.h"
#include "AmmoEntity.generated.h"


// This class does not need to be modified.
UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UAmmoEntity : public UInterface
{
	GENERATED_BODY()
};

class GENESTEALER_API IAmmoEntity
{
	GENERATED_BODY()

public:
	virtual FAmmoAmountChanged& OnAmmoAmountChanged() = 0;
	virtual void BroadcastAmmoUsage() = 0;
	virtual void StartReload() PURE_VIRTUAL(IAmmoEntity::StartReload,);
	virtual void StopReload() PURE_VIRTUAL(IAmmoEntity::StopReload,);
	virtual void GiveAmmo(int32 AddAmount) PURE_VIRTUAL(IAmmoEntity::GiveAmmo,);
	virtual int32 GetCurrentAmmo() PURE_VIRTUAL(IAmmoEntity::GetCurrentAmmo, return 0;);
	virtual int32 GetCurrentAmmoInClip() PURE_VIRTUAL(IAmmoEntity::GetCurrentAmmoInClip, return 0;)
	virtual int32 GetAmmoPerClip() PURE_VIRTUAL(IAmmoEntity::GetAmmoPerClip, return 0;)
	virtual int32 GetMaxAmmo() PURE_VIRTUAL(IAmmoEntity::GetMaxAmmo, return 0;);
	virtual bool HasInfiniteAmmo() PURE_VIRTUAL(IAmmoEntity::HasInfiniteAmmo, return false;);
	virtual bool HasInfiniteClip() PURE_VIRTUAL(IAmmoEntity::HasInfiniteClip, return false;);
};
