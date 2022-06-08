// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayTagUtils.generated.h"

#define TAG_ANIMATION_MONTAGE_PLAYING FGameplayTag::RequestGameplayTag("Animation.MontagePlaying")
#define TAG_HITREACT_LEFT FGameplayTag::RequestGameplayTag("Animation.HitReact.Left")
#define TAG_HITREACT_FRONT FGameplayTag::RequestGameplayTag("Animation.HitReact.Front")
#define TAG_HITREACT_RIGHT FGameplayTag::RequestGameplayTag("Animation.HitReact.Right")
#define TAG_HITREACT_BACK FGameplayTag::RequestGameplayTag("Animation.HitReact.Back")

#define TAG_ACTOR_PLAYER FGameplayTag::RequestGameplayTag("ActorType.Player")
#define TAG_ACTOR_AI FGameplayTag::RequestGameplayTag("ActorType.AI")
#define TAG_ACTOR_DESTRUCTIBLE FGameplayTag::RequestGameplayTag("ActorType.Destructible")
#define TAG_ACTOR_WEAPON FGameplayTag::RequestGameplayTag("ActorType.Weapon")
#define TAG_ACTOR_COVER FGameplayTag::RequestGameplayTag("ActorType.Cover")

#define TAG_STATE_STUNNED FGameplayTag::RequestGameplayTag("State.Stunned")
#define TAG_STATE_IMMOVABLE FGameplayTag::RequestGameplayTag("State.Immovable")
#define TAG_STATE_LOADING FGameplayTag::RequestGameplayTag("State.Loading")
#define TAG_STATE_DEAD FGameplayTag::RequestGameplayTag("State.Dead")
#define TAG_STATE_AIMING FGameplayTag::RequestGameplayTag("State.Aiming")
#define TAG_STATE_FIRING FGameplayTag::RequestGameplayTag("State.Firing")
#define TAG_STATE_READY FGameplayTag::RequestGameplayTag("State.Ready")
#define TAG_STATE_RAGDOLL FGameplayTag::RequestGameplayTag("State.Ragdoll")
#define TAG_STATE_IN_COVER FGameplayTag::RequestGameplayTag("State.InCover")
#define TAG_STATE_CANNOT_GET_IN_COVER FGameplayTag::RequestGameplayTag("State.CannotGetInCover")
#define TAG_STATE_IN_COMBAT FGameplayTag::RequestGameplayTag("State.InCombat")
#define TAG_STATE_ATTACK_COMMITTED FGameplayTag::RequestGameplayTag("State.AttackCommitted")

#define TAG_STATE_COMBO_WINDOW_ENABLED FGameplayTag::RequestGameplayTag("State.ComboWindowEnabled")
#define TAG_STATE_COMBO_ACTIVATED FGameplayTag::RequestGameplayTag("State.ComboActivated")

#define TAG_COVER_LEFTPEEK FGameplayTag::RequestGameplayTag("State.InCover.LeftPeek")
#define TAG_COVER_LEFTEDGE FGameplayTag::RequestGameplayTag("State.InCover.LeftEdge")
#define TAG_COVER_RIGHTPEEK FGameplayTag::RequestGameplayTag("State.InCover.RightPeek")
#define TAG_COVER_RIGHTEDGE FGameplayTag::RequestGameplayTag("State.InCover.RightEdge")
#define TAG_COVER_ROLLEDOUT FGameplayTag::RequestGameplayTag("State.InCover.RolledOut")

#define TAG_BASEPOSE_MOCAP_UNARMED FGameplayTag::RequestGameplayTag("Animation.BasePose.MocapUnarmed")
#define TAG_BASEPOSE_RIFLE_STANDING FGameplayTag::RequestGameplayTag("Animation.BasePose.RifleStanding")

#define TAG_OVERLAYPOSE_2H_PISTOL FGameplayTag::RequestGameplayTag("Animation.Overlay.2H_Pistol")

UCLASS()
class GENESTEALER_API UGameplayTagUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Genestealer|GameplayTagUtils")
	static bool ActorHasAnyGameplayTags(AActor* InActor, TArray<FGameplayTag> InTags, bool bExact = false);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Genestealer|GameplayTagUtils")
	static bool ActorHasGameplayTag(AActor* InActor, FGameplayTag InTag, bool bExact = false);
	UFUNCTION(BlueprintCallable, Category="Genestealer|GameplayTagUtils")
	static void AddTagToActor(AActor* InActor, FGameplayTag InTag);
	UFUNCTION(BlueprintCallable, Category="Genestealer|GameplayTagUtils")
	static void RemoveTagsFromActor(AActor* InActor, TArray<FGameplayTag> InTags);
	UFUNCTION(BlueprintCallable, Category="Genestealer|GameplayTagUtils")
	static void RemoveTagFromActor(AActor* InActor, FGameplayTag InTag);
};
