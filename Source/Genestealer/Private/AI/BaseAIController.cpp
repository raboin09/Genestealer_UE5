// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BaseAIController.h"

#include "Actors/BaseOverlapProjectile.h"
#include "Characters/BaseAICharacter.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "GameFramework/PawnMovementComponent.h"
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
	Sight->PeripheralVisionAngleDegrees = 90.f;
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
	Super::UpdateControlRotation(DeltaTime, bUpdatePawn);
	// const FVector FocalPoint = GetUpdatedFocalPoint();
	// const FVector SourcePoint = GetUpdatedSourcePoint();
	// APawn* const MyPawn = GetPawn();
	// // Look toward focus
	// if (!FocalPoint.IsZero() && MyPawn)
	// {
	// 	if(UKismetMathLibrary::Vector_Distance(FocalPoint, SourcePoint) <= 250)
	// 	{
	// 		Super::UpdateControlRotation(DeltaTime, bUpdatePawn);
	// 		return;
	// 	}
	// 	
	// 	const FVector Direction = FocalPoint - SourcePoint;
	// 	FRotator NewControlRotation = Direction.Rotation();
	//
	// 	NewControlRotation.Yaw = FRotator::ClampAxis(NewControlRotation.Yaw);
	// 	SetControlRotation(NewControlRotation);
	//
	// 	if (bUpdatePawn)
	// 	{
	// 		// UKismetSystemLibrary::DrawDebugLine(this, SourcePoint, FocalPoint, FLinearColor::Red, .1f, 1.f);
	// 		MyPawn->FaceRotation(NewControlRotation, DeltaTime);
	// 	}	
	// }
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
	if(UGameplayTagUtils::ActorHasGameplayTag(this, TAG_STATE_ALOOF))
	{
		return;
	}
	
	if (BlackboardComponent)
	{
		BlackboardComponent->SetValue<UBlackboardKeyType_Object>(EnemyKeyID, InEnemy);
		if(ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(InEnemy))
		{
			SetFocus(BaseCharacter);
			if(ABaseCharacter* AICast = Cast<ABaseCharacter>(InEnemy))
			{
				AICast->GetMovementComponent()->StopActiveMovement();
			}
			
			if(AIPawn->GetAttackBehavior() != AIPawn->GetDefaultBehavior())
			{
				InitAIComponents(AIPawn->GetAttackBehavior());	
			}
			BlackboardComponent->SetValue<UBlackboardKeyType_Bool>(IsInCombatKeyID, true);
			if(const auto AIChar = Cast<ABaseAICharacter>(GetPawn()))
			{
				const bool bHasMelee = AIChar->GetInventoryComponent()->GetCurrentWeaponType() == EWeaponType::Melee;
				if(!UGameplayTagUtils::ActorHasGameplayTag(AIChar, TAG_STATE_CANNOT_SPRINT) && bHasMelee)
				{
					AIChar->SetDesiredGait(EALSGait::Sprinting);	
				}
			}
			AIPawn->NewEnemyAcquired();
			EnemyRef = BaseCharacter;
			EnemyRef->AddTrackedAIController(this);
		} else
		{
			if(EnemyRef)
			{
				EnemyRef->RemoveTrackedAIController(this);	
			}
			EnemyRef = nullptr;
			ClearFocus(EAIFocusPriority::Gameplay);
			if(AIPawn->GetAttackBehavior() != AIPawn->GetDefaultBehavior())
			{
				InitAIComponents(AIPawn->GetDefaultBehavior());
			}
			if(const auto AIChar = Cast<ABaseAICharacter>(GetPawn()))
			{
				if(!UGameplayTagUtils::ActorHasGameplayTag(AIChar, TAG_STATE_CANNOT_SPRINT))
				{
					AIChar->SetDesiredGait(EALSGait::Running);	
				} else
				{
					AIChar->SetDesiredGait(EALSGait::Walking);
				}
			}
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
	if(UGameplayTagUtils::ActorHasGameplayTag(this, TAG_STATE_ALOOF) || !CharacterInCombatChangedPayload.bIsInCombat)
	{
		ClearFocus(EAIFocusPriority::Gameplay);
		SetEnemy(nullptr);
		return;
	}
	
	if(BlackboardComponent)
	{
		BlackboardComponent->SetValue<UBlackboardKeyType_Bool>(IsInCombatKeyID, CharacterInCombatChangedPayload.bIsInCombat);
		if(!GetEnemy())
		{
			if(UCombatUtils::IsActorDestructible(CharacterInCombatChangedPayload.DamageCauser))
			{
				return;
			}
			
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
	if(GetEnemy())
	{
		return;
	}
	
	if(BlackboardComponent)
	{
		for (AActor* Actor : UpdatedActors) {
			ABaseCharacter* CastedChar = Cast<ABaseCharacter>(Actor);
			if(!CastedChar)
			{
				return;
			}
			
			if (UCombatUtils::AreActorsEnemies(Actor, GetPawn()) && CastedChar->IsAlive())
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
