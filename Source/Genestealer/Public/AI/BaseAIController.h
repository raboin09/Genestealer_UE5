// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "API/AIPawn.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "BaseAIController.generated.h"

/**
 * 
 */
UCLASS()
class GENESTEALER_API ABaseAIController : public AAIController
{
	GENERATED_BODY()

public:
	ABaseAIController();
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	virtual void UpdateControlRotation(float DeltaTime, bool bUpdatePawn = true) override;
	
	void SetEnemy(ACharacter* InEnemy);
	ACharacter* GetEnemy() const;

	UFUNCTION()
	void SetIsInCombat(const FCharacterInCombatChangedPayload& CharacterInCombatChangedPayload);
	bool IsInCombat() const;

	
private:
	UFUNCTION()
	virtual void Internal_OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors);
	void InitAIComponents(UBehaviorTree* BehaviorTree);
	void InitPerceptionComponents();

	virtual FVector GetUpdatedFocalPoint();
	virtual FVector GetUpdatedSourcePoint();
	
	UPROPERTY(Transient, VisibleDefaultsOnly)
	UAISenseConfig_Sight* Sight;
	UPROPERTY(Transient, VisibleDefaultsOnly)
	UAIPerceptionComponent* AIPerceptionComponent;

	
	int32 EnemyKeyID;
	int32 IsInCombatKeyID;
	int32 CanSeeEnemyID;
	
	UPROPERTY(Transient)
	UBlackboardComponent* BlackboardComponent;
	UPROPERTY(Transient)
	UBehaviorTreeComponent* BehaviorTreeComponent;
	UPROPERTY(Transient)
	TScriptInterface<IAIPawn> AIPawn;

};
