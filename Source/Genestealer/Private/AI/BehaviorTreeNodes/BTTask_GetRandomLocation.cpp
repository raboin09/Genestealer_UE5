// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BehaviorTreeNodes/BTTask_GetRandomLocation.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"

UBTTask_GetRandomLocation::UBTTask_GetRandomLocation()
{
	NodeName = "Get Random Location";

	BlackboardKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_GetRandomLocation, BlackboardKey));
}

EBTNodeResult::Type UBTTask_GetRandomLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UWorld* World = GetWorld();
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);
	APawn* Pawn = OwnerComp.GetAIOwner()->GetPawn();

	if (NavSys && Pawn)
	{
		FSharedConstNavQueryFilter SharedFilter = nullptr;

		if (Filter)
		{
			const ANavigationData* NavData = NavSys->GetDefaultNavDataInstance(FNavigationSystem::DontCreate);
			if (NavData)
			{
				SharedFilter = UNavigationQueryFilter::GetQueryFilter(*NavData, World, Filter);
			}
		}

		const FVector Origin = Pawn->GetActorLocation();
		FNavLocation Destination;

		if (NavSys->GetRandomReachablePointInRadius(Origin, MaxDistance, Destination, nullptr, SharedFilter))
		{
			OwnerComp.GetBlackboardComponent()->SetValueAsVector(BlackboardKey.SelectedKeyName, Destination.Location);
			return EBTNodeResult::Succeeded;
		}
	}

	return EBTNodeResult::Failed;
}

FString UBTTask_GetRandomLocation::GetStaticDescription() const
{
	return FString::Printf(TEXT("Get Random Location\nMax Distance: %d\nFilter:%s"), FMath::RoundToInt(MaxDistance),
						   Filter ? *GetNameSafe(Filter.Get()) : TEXT("None"));
}
