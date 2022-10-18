// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Requestable.generated.h"

UENUM()
enum class EGetRequest : uint8
{
	GET_AvailableCharacters
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class URequestable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GENESTEALER_API IRequestable
{
	GENERATED_BODY()

		
public:
	virtual void SendRequest_GET(EGetRequest GetRequest, const UObject* RequestingObject, const FName& CallbackFunctionName) = 0;	
};
