// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BehaviorTreeNodes/BTTask_FireSelectedWeapon.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "API/AIPawn.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

EBTNodeResult::Type UBTTask_FireSelectedWeapon::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	APawn* CurrPawn = OwnerComp.GetAIOwner()->GetPawn();
	AActor* SelectedActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetActorKey.SelectedKeyName));
	if(!SelectedActor || !CurrPawn)
	{
		return EBTNodeResult::Failed;
	}
	
	if(IAIPawn* AIPawn = Cast<IAIPawn>(CurrPawn))
	{
		// FRotator Rot = UKismetMathLibrary::FindLookAtRotation(CurrPawn->GetActorLocation(), SelectedActor->GetActorLocation());
		// CurrPawn->SetActorRotation({SelectedActor->GetActorRotation().Pitch, Rot.Yaw, SelectedActor->GetActorRotation().Roll});
		// AIPawn->Aim(true);
		// if(!AIPawn->IsAIFiring())
		// {
		// 	AIPawn->FireWeapon(true);
		// }
		AIPawn->FireWeapon(true);
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}
