// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "API/AIPawn.h"
#include "API/Interactable.h"
#include "API/Questable.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Characters/BaseCharacter.h"
#include "BaseAICharacter.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable, AutoExpandCategories=("Genestealer"), PrioritizeCategories = "Genestealer")
class GENESTEALER_API ABaseAICharacter : public ABaseCharacter, public IAIPawn, public IInteractable, public IQuestable
{
	GENERATED_BODY()

public:
	ABaseAICharacter(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable)
	void IncrementMinorWaypoint()
	{
		if(MinorWaypointIndex < 3)
		{
			MinorWaypointIndex++;
		}
	}

	UFUNCTION(BlueprintCallable)
	int32 GetMajorWaypoint() const { return MajorWaypointIndex; }
	UFUNCTION(BlueprintCallable)
	int32 GetMinorWaypoint() const { return MinorWaypointIndex; }

	////////////////////////////////
	/// IAIPawn override
	////////////////////////////////
	FORCEINLINE virtual EAbsoluteAffiliation GetPawnAffiliation() const override { return AbsoluteAffiliation; }
	FORCEINLINE virtual UBehaviorTree* GetDefaultBehavior() const override { return DefaultBehaviorTree; }
	FORCEINLINE virtual UBehaviorTree* GetAttackBehavior() const override { return AttackBehaviorTree; }
	FORCEINLINE virtual FCharacterInCombatChanged& OnCombatStateChanged() override { return CharacterInCombatChanged; }
	FORCEINLINE virtual bool IsAIFiring() override { return IsFiring(); }
	FORCEINLINE virtual EBallisticSkill GetBallisticSkill() const override { return BallisticSkill; }
	FORCEINLINE virtual FVector GetSocketLocation(FName SocketName, bool bWeaponMesh = false) const override;
	virtual void FireWeapon(bool bStartFiring) override;
	virtual void Aim(bool bStartAiming) override;
	virtual float GetWeaponRange() const override;

	////////////////////////////////
	// IInteractable overrides
	////////////////////////////////
	virtual void InteractWithActor(AActor* InstigatingActor) override;
	virtual void SwitchOutlineOnMesh(bool bShouldOutline) override;
	virtual EAbsoluteAffiliation GetInteractableAffiliation() const override { return AbsoluteAffiliation; }
	virtual FString GetInteractionText() const override { return InteractionText; }
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Genestealer")
	FString InteractionText;
	
	////////////////////////////////
	// IQuestable overrides
	////////////////////////////////
	FORCEINLINE virtual FQuestObjectiveEvent& OnQuestObjectiveEvent() override { return QuestObjectiveEvent; }
	
protected:
	virtual void BeginPlay() override;
	virtual void HandleDeathEvent(const FActorDeathEventPayload& DeathEventPayload) override;

	UPROPERTY(EditAnywhere, Category="Genestealer|Defaults")
	bool bIsASquadMember = false;
	UPROPERTY(EditAnywhere, Category="Genestealer|Defaults")
	UBehaviorTree* AttackBehaviorTree;
	UPROPERTY(EditAnywhere, Category="Genestealer|Defaults")
	UBehaviorTree* DefaultBehaviorTree;
	
private:
	UPROPERTY()
	class UInteractionComponent* InteractionComponent;

	FQuestObjectiveEvent QuestObjectiveEvent;

	int32 MajorWaypointIndex;
	int32 MinorWaypointIndex;
};
