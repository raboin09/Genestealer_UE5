// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTDecorator_HasLineOfSight.h"
#include "AI/BaseAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "GameFramework/Character.h"
#include "Genestealer/Genestealer.h"
#include "Utils/CombatUtils.h"

bool UBTDecorator_HasLineOfSight::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const UBlackboardComponent* MyBlackboard = OwnerComp.GetBlackboardComponent();
	AAIController* MyController = OwnerComp.GetAIOwner();
	if(!MyBlackboard || !MyController)
	{
		return false;
	}

	const auto KeyID = MyBlackboard->GetKeyID(EnemyKey.SelectedKeyName);
	const auto TargetKeyType = MyBlackboard->GetKeyType(KeyID);
	AActor* EnemyActor = Cast<AActor>(MyBlackboard->GetValue<UBlackboardKeyType_Object>(KeyID));
	FVector EndLocation = FVector::ZeroVector;
	
	if (TargetKeyType == UBlackboardKeyType_Object::StaticClass())
	{
		if(!EnemyActor)
		{
			return false;
		}
		EndLocation = EnemyActor->GetActorLocation();	
	} else if (TargetKeyType == UBlackboardKeyType_Vector::StaticClass())
	{
		EndLocation = MyBlackboard->GetValue<UBlackboardKeyType_Vector>(KeyID);
	}

	if(EndLocation != FVector::ZeroVector)
	{
		return LOSTrace(OwnerComp.GetOwner(), EnemyActor, EndLocation);
	}
	
	return false;
}

bool UBTDecorator_HasLineOfSight::LOSTrace(AActor* InActor, AActor* InEnemyActor, const FVector& EndLocation) const
{

	ABaseAIController* MyController = Cast<ABaseAIController>(InActor);
	ACharacter* MyBot = MyController ? Cast<ACharacter>(MyController->GetPawn()) : nullptr; 

	if(!MyBot)
	{
		return false;
	}

	FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(AILosTrace), true, InActor);			
	TraceParams.bReturnPhysicalMaterial = true;
	TraceParams.AddIgnoredActor(MyBot);
	const FVector StartLocation = MyBot->GetActorLocation();
	FHitResult Hit(ForceInit);
	GetWorld()->LineTraceSingleByChannel(Hit, StartLocation, EndLocation, TRACE_WEAPON, TraceParams);
	if(!Hit.bBlockingHit)
	{
		return false;
	}

	if(!InEnemyActor)
	{
		FVector HitDelta = Hit.ImpactPoint - StartLocation;
		FVector TargetDelta = EndLocation - StartLocation;
		if (TargetDelta.SizeSquared() < HitDelta.SizeSquared())
		{
			return true;
		}
	}

	return Hit.GetActor() == InEnemyActor;
}