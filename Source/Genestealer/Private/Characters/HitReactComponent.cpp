
#include "Characters/HitReactComponent.h"

UHitReactComponent::UHitReactComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UHitReactComponent::BeginPlay()
{
	Super::BeginPlay();	
}
