// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/ConeWeapon.h"

#include "AIController.h"
#include "API/Attackable.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Utils/EffectUtils.h"

AConeWeapon::AConeWeapon()
{
	ConeComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ConeComponent"));
	ConeComponent->SetStaticMesh(ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Engine/BasicShapes/Cone.Cone'")).Object);
	ConeComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	ConeComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	ConeComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
}

void AConeWeapon::FireWeapon()
{
	if(!ConeComponent)
	{
		return;
	}	
	TArray<AActor*> OverlappingActors;
	ConeComponent->GetOverlappingActors(OverlappingActors);
	for(AActor* CurrActor : OverlappingActors)
	{
		if(const IAttackable* AttackableCast = Cast<IAttackable>(CurrActor))
		{
			const FVector& AimDirection = GetAdjustedAim();
			const FVector& StartTrace = GetCameraDamageStartLocation(AimDirection);	
			const FVector& TargetPelvisTrace = AttackableCast->GetPelvisLocation();
			const FVector& TargetHeadTrace = AttackableCast->GetHeadLocation();
			if(const FHitResult& ChestImpact = WeaponTrace(StartTrace, TargetPelvisTrace, true); ChestImpact.bBlockingHit)
			{
				if(TryTraceToOverlappedActor(ChestImpact, StartTrace, CurrActor)) {	continue; }
			}			
			if(const FHitResult& HeadImpact = WeaponTrace(StartTrace, TargetHeadTrace, true); HeadImpact.bBlockingHit)
			{
				TryTraceToOverlappedActor(HeadImpact, StartTrace, CurrActor);
			}
		}
	}
}

void AConeWeapon::BeginPlay()
{
	Super::BeginPlay();
	if(!ConeComponent)
	{
		return;
	}
	
	ConeComponent->SetGenerateOverlapEvents(true);
	ConeComponent->IgnoreActorWhenMoving(this, true);
	ConeComponent->IgnoreActorWhenMoving(OwningPawn, true);
	ConeComponent->SetVisibility(false);
	if(const AController* CurrCon = GetInstigatorController(); CurrCon->IsA(AAIController::StaticClass()))
	{
		ConeComponent->SetRelativeScale3D(AIScaleOverride);
	}
}

void AConeWeapon::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if(!ConeComponent)
	{
		return;
	}
	ConeComponent->AttachToComponent(GetWeaponMesh(), FAttachmentTransformRules::KeepRelativeTransform, RaycastSourceSocketName); 
}

bool AConeWeapon::TryTraceToOverlappedActor(const FHitResult& Impact, const FVector& StartTrace, AActor* TargetActor)
{
	if(TargetActor == Impact.GetActor())
	{
		UEffectUtils::ApplyEffectsToHitResult(WeaponEffects, AdjustHitResultIfNoValidHitComponent(Impact), GetInstigator());
		return true;
	}
	return false;
}
