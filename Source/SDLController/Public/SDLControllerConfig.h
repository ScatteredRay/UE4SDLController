#pragma once

#include "SDLControllerConfig.generated.h"

UCLASS(config = Debug, defaultconfig)
class USDLControllerConfig : public UObject
{
	GENERATED_BODY()

public:
	USDLControllerConfig(const FObjectInitializer& ObjectInitializer);
	
	/** Controllers GUIDs that this plugin will ignore */
	UPROPERTY(Config, EditAnywhere, Category = "SDLController")
	TArray<FString> disabledControllerGUIDs = { "78696e70757401000000000000000000" };
};
