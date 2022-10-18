// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BaseAIController.h"

#include "Actors/BaseOverlapProjectile.h"
#include "Characters/BaseAICharacter.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "Kismet/KismetMathLibrary.h"
#include "Perception/AIPerceptionComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Utils/CombatUtils.h"

ABaseAIController::ABaseAIController()
{
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackBoardComp"));
	BrainComponent = BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorComp"));

	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(FName("AIPerceptionComp"));
	Sight = CreateDefaultSubobject<UAISenseConfig_Sight>(FName("Sight Config"));
	Sight->SightRadius = 2000.f;
	Sight->LoseSightRadius = 2500.f;
	Sight->PeripheralVisionAngleDegrees = 60.f;
	Sight->DetectionByAffiliation.bDetectEnemies = true;
	Sight->DetectionByAffiliation.bDetectFriendlies = true;
	Sight->DetectionByAffiliation.bDetectNeutrals = true;
	AIPerceptionComponent->ConfigureSense(*Sight);
}

void ABaseAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if(!InPawn || !UKismetSystemLibrary::DoesImplementInterface(InPawn, UAIPawn::StaticClass()))
	{
		return;
	}
	
	AIPawn.SetObject(InPawn);
	AIPawn.SetInterface(Cast<IAIPawn>(InPawn));
	if(!AIPawn)
	{
		return;
	}
	AIPawn->OnCombatStateChanged().AddDynamic(this, &ABaseAIController::SetIsInCombat);	
	InitAIComponents(AIPawn->GetDefaultBehavior());
	InitPerceptionComponents();
}

void ABaseAIController::OnUnPossess()
{
	BehaviorTreeComponent->StopTree();
	Super::OnUnPossess();
}

void ABaseAIController::UpdateControlRotation(float DeltaTime, bool bUpdatePawn)
{
	const FVector FocalPoint = GetUpdatedFocalPoint();
	const FVector SourcePoint = GetUpdatedSourcePoint();
	APawn* const MyPawn = GetPawn();
	// Look toward focus
	if (!FocalPoint.IsZero() && MyPawn)
	{
		if(UKismetMathLibrary::Vector_Distance(FocalPoint, SourcePoint) <= 250)
		{
			Super::UpdateControlRotation(DeltaTime, bUpdatePawn);
			return;
		}
		
		const FVector Direction = FocalPoint - SourcePoint;
		FRotator NewControlRotation = Direction.Rotation();

		NewControlRotation.Yaw = FRotator::ClampAxis(NewControlRotation.Yaw);
		SetControlRotation(NewControlRotation);

		if (bUpdatePawn)
		{
			// UKismetSystemLibrary::DrawDebugLine(this, SourcePoint, FocalPoint, FLinearColor::Red, .1f, 1.f);
			MyPawn->FaceRotation(NewControlRotation, DeltaTime);
		}

	}
}

FVector ABaseAIController::GetUpdatedFocalPoint()
{
	if (GetEnemy())
	{
		return GetEnemy()->GetMesh()->GetSocketLocation("spine_03");
	}
	return GetFocalPoint();
}

FVector ABaseAIController::GetUpdatedSourcePoint()
{
	if(!AIPawn)
	{
		return FVector::ZeroVector;
	}
	return AIPawn->GetSocketLocation("Muzzle", true);
}

void ABaseAIController::SetEnemy(ACharacter* InEnemy)
{
	if (BlackboardComponent)
	{
		BlackboardComponent->SetValue<UBlackboardKeyType_Object>(EnemyKeyID, InEnemy);
		if(InEnemy)
		{
			SetFocus(InEnemy);
			InitAIComponents(AIPawn->GetAttackBehavior());
			BlackboardComponent->SetValue<UBlackboardKeyType_Bool>(IsInCombatKeyID, true);
		} else
		{
			ClearFocus(EAIFocusPriority::Gameplay);
			InitAIComponents(AIPawn->GetDefaultBehavior());
			BlackboardComponent->SetValue<UBlackboardKeyType_Bool>(IsInCombatKeyID, false);
		}
	}
}

ACharacter* ABaseAIController::GetEnemy() const
{
	if (Blackboard)
	{
		return Cast<ACharacter>(BlackboardComponent->GetValue<UBlackboardKeyType_Object>(EnemyKeyID));
	}
	return nullptr;
}

void ABaseAIController::SetIsInCombat(const FCharacterInCombatChangedPayload& CharacterInCombatChangedPayload)
{
	if(BlackboardComponent)
	{
		BlackboardComponent->SetValue<UBlackboardKeyType_Bool>(IsInCombatKeyID, CharacterInCombatChangedPayload.bIsInCombat);
		if(!GetEnemy())
		{
			if(const IWeapon* Weapon = Cast<IWeapon>(CharacterInCombatChangedPayload.DamageCauser))
			{
				SetEnemy(Weapon->GetOwningPawn());
			} else if(const ABaseOverlapProjectile* Proj = Cast<ABaseOverlapProjectile>(CharacterInCombatChangedPayload.DamageCauser))
			{
				if(const IWeapon* OwnerWeapon = Cast<IWeapon>(Proj->GetOwner()))
				{
					if(UCombatUtils::AreActorsEnemies(GetPawn(), OwnerWeapon->GetOwningPawn()))
					{
						SetEnemy(OwnerWeapon->GetOwningPawn());
					}
				}
			} else if (ACharacter* CastedChar = Cast<ACharacter>(CharacterInCombatChangedPayload.DamageCauser))
			{
				SetEnemy(CastedChar);
			}	
		}
	}
	
	if(!CharacterInCombatChangedPayload.bIsInCombat)
	{
		ClearFocus(EAIFocusPriority::Gameplay);
		SetEnemy(nullptr);
	}
}

bool ABaseAIController::IsInCombat() const
{
	if (BlackboardComponent)
	{
		return BlackboardComponent->GetValue<UBlackboardKeyType_Bool>(IsInCombatKeyID);
	}
	return nullptr;	
}

void ABaseAIController::Internal_OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	if(UpdatedActors.Contains(GetEnemy()))
	{
		return;
	}
	
	if(BlackboardComponent)
	{
		for (AActor* Actor : UpdatedActors) {
			ACharacter* CastedChar = Cast<ACharacter>(Actor);
			if (UCombatUtils::AreActorsEnemies(Actor, GetPawn()))
			{
				SetEnemy(CastedChar);
			}
		}
	}
}

void ABaseAIController::InitAIComponents(UBehaviorTree* BehaviorTree)
{
	if(!AIPawn || !BehaviorTree)
	{
		return;
	}
	BehaviorTreeComponent->StopTree();
	if (BehaviorTree->BlackboardAsset)
	{
		BlackboardComponent->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
		EnemyKeyID = BlackboardComponent->GetKeyID("Enemy");
		IsInCombatKeyID = BlackboardComponent->GetKeyID("IsInCombat");
	}	
	BehaviorTreeComponent->StartTree(*BehaviorTree);
}

void ABaseAIController::InitPerceptionComponents()
{
	if(AIPerceptionComponent)
	{
		AIPerceptionComponent->OnPerceptionUpdated.RemoveAll(this);
		AIPerceptionComponent->OnPerceptionUpdated.AddDynamic(this, &ABaseAIController::Internal_OnPerceptionUpdated);
	}
}
