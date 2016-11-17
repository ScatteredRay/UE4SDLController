// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ModuleManager.h"
#include "IInputDeviceModule.h"

class FSDLControllerModule : public IInputDeviceModule
{
public:

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	virtual TSharedPtr<class IInputDevice> CreateInputDevice(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler) override;
};
