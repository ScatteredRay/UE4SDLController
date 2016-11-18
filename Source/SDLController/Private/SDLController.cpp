// Copyright (c) 2016, Indy Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.

#include "SDLControllerPrivatePCH.h"
#include "Core.h"
#include "ModuleManager.h"
#include "IPluginManager.h"
#include "InputDevice.h"

#define SDL_MAIN_HANDLED
#include "include/SDL.h"

DEFINE_LOG_CATEGORY_STATIC(LogSDLController, Log, All);

#define LOCTEXT_NAMESPACE "FSDLControllerModule"

void* SDL2Handle = nullptr;

namespace
{
	inline float ShortToNormalizedFloat(short AxisVal)
	{
			// normalize [-32768..32767] -> [-1..1]
			const float Norm = (AxisVal <= 0 ? 32768.f : 32767.f);
			return float(AxisVal) / Norm;
	}
};

void FSDLControllerModule::StartupModule()
{
	IInputDeviceModule::StartupModule();
	FString BaseDir = IPluginManager::Get().FindPlugin("SDLController")->GetBaseDir();

	FString LibraryPath;
#ifdef PLATFORM_WINDOWS
	LibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/SDL2Library/lib/x64/SDL2.dll"));
#endif // PLATFORM_WINDOWS

	SDL2Handle = !LibraryPath.IsEmpty() ? FPlatformProcess::GetDllHandle(*LibraryPath) : nullptr;

	if (SDL2Handle)
	{
	}
	else
	{
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("SDL2LibraryError", "Failed to load SDL2 third party library"));
	}

	SDL_Init(SDL_INIT_GAMECONTROLLER | SDL_INIT_JOYSTICK | SDL_INIT_EVENTS);
}

void FSDLControllerModule::ShutdownModule()
{
	SDL_Quit();
	FPlatformProcess::FreeDllHandle(SDL2Handle);
	SDL2Handle = nullptr;
}

class FSDLController : public IInputDevice
{
    TSharedRef<FGenericApplicationMessageHandler> MessageHandler;
    TArray<SDL_GameController*> controllers;
public:
    void AddJoystick(int32 joystickId)
    {
        static const char* disabledControllerGUIDs[] =
        {
            "78696e70757401000000000000000000"
        };
        const uint32_t numDisabledControllers = sizeof(disabledControllerGUIDs) / sizeof(char*);

        if(SDL_IsGameController(joystickId))
        {
            SDL_JoystickGUID joyGuid = SDL_JoystickGetDeviceGUID(joystickId);
            char guid[33];
            SDL_JoystickGetGUIDString(joyGuid, guid, sizeof(guid));
            bool disabled = false;
            for(int g = 0; g < numDisabledControllers; g++)
            {
                if(strcmp(guid, disabledControllerGUIDs[g]) == 0)
                    disabled = true;
            }

            SDL_GameController* controller = SDL_GameControllerFromInstanceID(joystickId);
            if(controller)
            {
                for(int32 i = 0; i < controllers.Num(); i++)
                {
                    if(controller == controllers[i])
                        disabled = true;
                }
            }

            if(!disabled)
            {

                int32 index = -1;

                for(int32 i = 0; i < controllers.Num(); i++)
                {
                    if(controllers[i] == nullptr)
                    {
                        index = i;
                        break;
                    }
                }

                if(index == -1)
                {
                    controllers.Add(nullptr);
                    index = controllers.Num() - 1;
                }

                controllers[index] = SDL_GameControllerOpen(joystickId);
                if(controllers[index] == nullptr)
                {
                    UE_LOG(LogSDLController, Error, TEXT("Error in SDL_GameControllerOpen: %s"), UTF8_TO_TCHAR(SDL_GetError()));
                }
                UE_LOG(LogSDLController, Log, TEXT("Initialized controller with GUID: %s with index %d"), UTF8_TO_TCHAR(guid), index);
            }
            else
            {
                UE_LOG(LogSDLController, Log, TEXT("Skipping controller with GUID: %s"), UTF8_TO_TCHAR(guid));
            }
        }
    }

    FSDLController(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler)
        : MessageHandler(InMessageHandler)
	{
        // SDL_CONTROLLERDEVICEADDED May get called for every joystick, so this may be redundant
        for(int32 i = 0; i < SDL_NumJoysticks(); i++)
        {
            AddJoystick(i);
        }
    }

    ~FSDLController()
	{
        for(int32 i = 0; i < controllers.Num(); i++)
        {
            if(controllers[i] != nullptr)
            {
                SDL_GameControllerClose(controllers[i]);
                controllers[i] = nullptr;
            }
        }
    }

    virtual void Tick(float DeltaTime) override
	{
    }

    virtual void SendControllerEvents() override
    {
        static const FGamepadKeyNames::Type ButtonMap[SDL_CONTROLLER_BUTTON_MAX] =
        {
            FGamepadKeyNames::FaceButtonBottom,
            FGamepadKeyNames::FaceButtonRight,
            FGamepadKeyNames::FaceButtonLeft,
            FGamepadKeyNames::FaceButtonTop,
            FGamepadKeyNames::SpecialLeft,
            FGamepadKeyNames::Invalid,
            FGamepadKeyNames::SpecialRight,
            FGamepadKeyNames::LeftThumb,
            FGamepadKeyNames::RightThumb,
            FGamepadKeyNames::LeftTriggerThreshold,
            FGamepadKeyNames::RightTriggerThreshold,
            FGamepadKeyNames::DPadUp,
            FGamepadKeyNames::DPadDown,
            FGamepadKeyNames::DPadLeft,
            FGamepadKeyNames::DPadRight,
        };

        static const FGamepadKeyNames::Type AxisMap[SDL_CONTROLLER_AXIS_MAX] =
        {
            FGamepadKeyNames::LeftAnalogX,
            FGamepadKeyNames::LeftAnalogY,
            FGamepadKeyNames::RightAnalogX,
            FGamepadKeyNames::RightAnalogY,
            FGamepadKeyNames::LeftShoulder,
            FGamepadKeyNames::RightShoulder,
        };

        static const float InvertMap[SDL_CONTROLLER_AXIS_MAX] =
        {
            1.f,
            -1.f,
            1.f,
            -1.f,
            1.f,
            1.f,
        };

        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
            case SDL_CONTROLLERAXISMOTION:
            case SDL_CONTROLLERBUTTONDOWN:
            case SDL_CONTROLLERBUTTONUP:
            {
                int32 index = -1;
                SDL_GameController* controller = SDL_GameControllerFromInstanceID(event.cdevice.which);
                for(int32 i = 0; i < controllers.Num(); i++)
                {
                    if(controller == controllers[i])
                        index = i;
                }

				if (index == -1)
					break;

                switch(event.type)
                {
                case SDL_CONTROLLERAXISMOTION:
                    MessageHandler->OnControllerAnalog(AxisMap[event.caxis.axis], index, InvertMap[event.caxis.axis] * ShortToNormalizedFloat(event.caxis.value));
                    break;
                case SDL_CONTROLLERBUTTONDOWN:
                {
                    FGamepadKeyNames::Type button = ButtonMap[event.cbutton.button];
                    if(button != FGamepadKeyNames::Invalid)
                        MessageHandler->OnControllerButtonPressed(button, index, false);
                    break;
                }
                case SDL_CONTROLLERBUTTONUP:
                {
                    FGamepadKeyNames::Type button = ButtonMap[event.cbutton.button];
                    if(button != FGamepadKeyNames::Invalid)
                        MessageHandler->OnControllerButtonReleased(button, index, false);
                    break;
                }
                }
                break;
            }
            case SDL_CONTROLLERDEVICEADDED:
            {
                AddJoystick(event.cdevice.which);
                break;
            }
            case SDL_CONTROLLERDEVICEREMOVED:
            {
                int32 index = 0;
                SDL_GameController* controller = SDL_GameControllerFromInstanceID(event.cdevice.which);
                for(int32 i = 0; i < controllers.Num(); i++)
                {
                    if(controller == controllers[i])
                        index = i;
                }

                if(controllers[index] != nullptr)
                {
                    SDL_GameControllerClose(controllers[index]);
                    controllers[index] = nullptr;
                }
                break;
            }
            }
        }
    }

    virtual void SetChannelValue(int32 ControllerId, FForceFeedbackChannelType ChannelType, float Value) override
    {
    }

    virtual void SetChannelValues(int32 ControllerId, const FForceFeedbackValues& Values) override
    {
    }

    virtual void SetMessageHandler(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler) override
    {
        MessageHandler = InMessageHandler;
    }

    virtual bool Exec(UWorld* InWorld, const TCHAR* cmd, FOutputDevice& ar) override
    {
        return false;
    }
};

TSharedPtr<class IInputDevice> FSDLControllerModule::CreateInputDevice(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler)
{
    return TSharedPtr<class IInputDevice>(new FSDLController(InMessageHandler));
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSDLControllerModule, SDLController)
