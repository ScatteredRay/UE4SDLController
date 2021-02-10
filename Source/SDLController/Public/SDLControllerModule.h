#pragma once


class FSDLControllerModule : public IInputDeviceModule
{
	public:

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	virtual TSharedPtr<class IInputDevice> CreateInputDevice(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler) override;

	private:

	void RegisterSettings();
	void UnregisterSettings();
	bool HandleSettingsSaved();

	void* SDL2Handle = nullptr;
};