#pragma once

#include "SDL2Library/include/SDL.h"
#include "IInputDeviceModule.h"

class FSDLController : public IInputDevice
{

	public:

	void AddJoystick(int32 joystickId);
	FSDLController(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler);
	virtual void Tick(float DeltaTime) override;
	virtual void SendControllerEvents() override;
	virtual void SetChannelValue(int32 ControllerId, FForceFeedbackChannelType ChannelType, float Value) override;
	virtual void SetChannelValues(int32 ControllerId, const FForceFeedbackValues& Values) override;
	virtual void SetMessageHandler(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler) override;
	virtual bool Exec(UWorld* InWorld, const TCHAR* cmd, FOutputDevice& ar) override;

	public:

	TSharedRef<FGenericApplicationMessageHandler> MessageHandler;
	TArray<SDL_GameController*> controllers;
	TMap<TPair<int, FName>, float> AxisValues;
	TPair<int, FName> axis;


	FORCEINLINE float ShortToNormalizedFloat(short AxisVal)
	{
		// normalize [-32768..32767] -> [-1..1]
		const float Norm = (AxisVal <= 0 ? 32768.f : 32767.f);
		return float(AxisVal) / Norm;
	}
};