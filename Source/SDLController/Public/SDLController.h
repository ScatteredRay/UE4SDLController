// Copyright (c) 2016, Indy Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.

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
