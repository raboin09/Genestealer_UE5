// Copyright 2021 Adam Grodzki All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class FAGRPROModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
