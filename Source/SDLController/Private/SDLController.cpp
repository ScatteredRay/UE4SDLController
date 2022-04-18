
#include "SDLController.h"
#include "Core.h"
#include "Interfaces/IPluginManager.h"
#include "SDLControllerConfig.h"

DEFINE_LOG_CATEGORY_STATIC(LogSDLController, Log, All);

void FSDLController::AddJoystick(int32 joystickId)
{
	TArray<FString> disabledControllerGUIDs = GetDefault<USDLControllerConfig>()->disabledControllerGUIDs;

	if (SDL_IsGameController(joystickId))
	{
		SDL_JoystickGUID joyGuid = SDL_JoystickGetDeviceGUID(joystickId);
		char guid[33];
		SDL_JoystickGetGUIDString(joyGuid, guid, sizeof(guid));
		bool disabled = false;

		for (auto& disabledControllerGUID : disabledControllerGUIDs)
		{
			if (disabledControllerGUID.Equals(guid, ESearchCase::IgnoreCase)) {
				disabled = true;
				return;
			}
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

FSDLController::FSDLController(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler)
        : MessageHandler(InMessageHandler)
	{
        // SDL_CONTROLLERDEVICEADDED May get called for every joystick, so this may be redundant
        for(int32 i = 0; i < SDL_NumJoysticks(); i++)
        {
            AddJoystick(i);
        }
    }

void FSDLController::Tick(float DeltaTime)
	{
		//if not done that way it will send multiple analog inputs between frames as their summ 
		for (const TPair<TPair<int, FName>, float>& pair : AxisValues)
		{
			MessageHandler->OnControllerAnalog(pair.Key.Value, pair.Key.Key, pair.Value);
		}
    }

void FSDLController::SendControllerEvents()
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
            FGamepadKeyNames::LeftShoulder,
            FGamepadKeyNames::RightShoulder,
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
            FGamepadKeyNames::LeftTriggerAnalog,
            FGamepadKeyNames::RightTriggerAnalog,
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
					axis.Key = index;
					axis.Value = AxisMap[event.caxis.axis];
					if (AxisValues.Contains(axis)) {
						float& value = AxisValues.Emplace(axis);
						value = InvertMap[event.caxis.axis] * ShortToNormalizedFloat(event.caxis.value);
					}
					else {
						AxisValues.Add(axis, InvertMap[event.caxis.axis] * ShortToNormalizedFloat(event.caxis.value));
					}
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

void FSDLController::SetChannelValue(int32 ControllerId, FForceFeedbackChannelType ChannelType, float Value)
    {
    }

void FSDLController::SetChannelValues(int32 ControllerId, const FForceFeedbackValues& Values)
    {
    }

void FSDLController::SetMessageHandler(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler)
    {
        MessageHandler = InMessageHandler;
    }

bool FSDLController::Exec(UWorld* InWorld, const TCHAR* cmd, FOutputDevice& ar)
    {
        return false;
    }
