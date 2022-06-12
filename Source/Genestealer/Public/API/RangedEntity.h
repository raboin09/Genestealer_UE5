// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Types/EventDeclarations.h"
#include "UObject/Interface.h"
#include "RangedEntity.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class URangedEntity : public UInterface
{
	GENERATED_BODY()
};

class GENESTEALER_API IRangedEntity
{
	GENERATED_BODY()

public:
	virtual FAmmoAmountChanged& OnAmmoAmountChanged() = 0;
	virtual void BroadcastAmmoUsage() = 0;
	virtual void StartReload() PURE_VIRTUAL(IRangedEntity::StartReload,);
	virtual void StopReload() PURE_VIRTUAL(IRangedEntity::StopReload,);
	virtual void GiveAmmo(int32 AddAmount) PURE_VIRTUAL(IRangedEntity::GiveAmmo,);
	virtual int32 GetCurrentAmmo() PURE_VIRTUAL(IRangedEntity::GetCurrentAmmo, return 0;);
	virtual int32 GetCurrentAmmoInClip() PURE_VIRTUAL(IRangedEntity::GetCurrentAmmoInClip, return 0;)
	virtual int32 GetAmmoPerClip() PURE_VIRTUAL(IRangedEntity::GetAmmoPerClip, return 0;)
	virtual int32 GetMaxAmmo() PURE_VIRTUAL(IRangedEntity::GetMaxAmmo, return 0;);
	virtual bool HasInfiniteAmmo() PURE_VIRTUAL(IRangedEntity::HasInfiniteAmmo, return false;);
	virtual bool HasInfiniteClip() PURE_VIRTUAL(IRangedEntity::HasInfiniteClip, return false;);
	virtual UTexture2D* GetCrosshair() const PURE_VIRTUAL(IRangedEntity::GetCrosshair, return nullptr;);
};
