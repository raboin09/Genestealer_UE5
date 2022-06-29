// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/BaseActor.h"
#include "Types/GenestealerTags.h"
#include "Utils/GameplayTagUtils.h"
#include "BaseOverlapActor.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class GENESTEALER_API ABaseOverlapActor : public ABaseActor
{
	GENERATED_BODY()

public:
	ABaseOverlapActor();

	UFUNCTION(BlueprintCallable)
	void Activate();
	UFUNCTION(BlueprintCallable)
	void Deactivate();
	bool ShouldActivateOnStart() const { return bActivateOnStart; };
	UFUNCTION(BlueprintNativeEvent)
	UMeshComponent* GetMesh() const;
	virtual UMeshComponent* GetMesh_Implementation() const PURE_VIRTUAL(ABaseOverlapActor::GetMesh, return nullptr;);
	UFUNCTION(BlueprintNativeEvent)
	UShapeComponent* GetCollisionComponent() const;
	virtual UShapeComponent* GetCollisionComponent_Implementation() const  PURE_VIRTUAL(ABaseOverlapActor::GetCollisionComponent, return nullptr;);
	
protected:
	FORCEINLINE bool IsActive() { return UGameplayTagUtils::ActorHasGameplayTag(this, TAG_STATE_ACTIVE); }
	
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintNativeEvent)
	void K2_HandleOverlapEvent(AActor* OtherActor, const FHitResult& HitResult);
	virtual void K2_HandleOverlapEvent_Implementation(AActor* OtherActor, const FHitResult& HitResult);
	UFUNCTION(BlueprintNativeEvent)
	void K2_HandleEndOverlapEvent(AActor* ExitingActor);
	virtual void K2_HandleEndOverlapEvent_Implementation(AActor* ExitingActor);
	
	virtual void HandleActorDeath();	
	UFUNCTION()
	virtual void ActorBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void ActorEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(EditDefaultsOnly, Category = "Genestealer")
	float DeathBuffer;
	UPROPERTY(EditAnywhere, Category = "Genestealer")
	TArray<FGameplayTag> BlockedOverlapTags;
	UPROPERTY(EditAnywhere, Category = "Genestealer")
	TArray<FGameplayTag> RequiredOverlapTags;
	UPROPERTY(EditAnywhere, Category = "Genestealer")
	bool bActivateOnStart;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genestealer")
	bool bDiesAfterOverlap;

	UPROPERTY()
	TArray<AActor*> HitActors;
};
