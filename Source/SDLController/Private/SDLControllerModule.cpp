
#include "SDLControllerModule.h"
#include "SDLController.h"
#include "Developer/Settings/Public/ISettingsModule.h"
#include "Developer/Settings/Public/ISettingsSection.h"
#include "SDLControllerConfig.h"

void FSDLControllerModule::StartupModule()
{
	IInputDeviceModule::StartupModule();
	FString BaseDir = IPluginManager::Get().FindPlugin("SDLController")->GetBaseDir();

	FString LibraryPath;
#ifdef PLATFORM_WINDOWS
	LibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/SDL/VisualC/SDL/x64/Release/SDL2.dll"));
#endif // PLATFORM_WINDOWS

	SDL2Handle = !LibraryPath.IsEmpty() ? FPlatformProcess::GetDllHandle(*LibraryPath) : nullptr;

	if (!SDL2Handle)
	{
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("SDL2LibraryError", "Failed to load SDL2 third party library"));
	}

	SDL_Init(SDL_INIT_GAMECONTROLLER | SDL_INIT_JOYSTICK | SDL_INIT_EVENTS);

	RegisterSettings();
}

void FSDLControllerModule::ShutdownModule()
{
	SDL_Quit();
	FPlatformProcess::FreeDllHandle(SDL2Handle);
	SDL2Handle = nullptr;
	if (UObjectInitialized()) {
		UnregisterSettings();
	}
}

TSharedPtr<class IInputDevice> FSDLControllerModule::CreateInputDevice(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler)
{
	return TSharedPtr<class IInputDevice>(new FSDLController(InMessageHandler));
}


void FSDLControllerModule::RegisterSettings()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings")) {

		ISettingsSectionPtr SettingsSection = SettingsModule->RegisterSettings("Project", "Plugins", "SDLContoller",
			LOCTEXT("SDLContollerSettingsName", "SDLContoller"),
			LOCTEXT("SDLContollerSettingsDescription", "Configure the SDLContoller plug-in."),
			GetMutableDefault<USDLControllerConfig>()
		);

		// Register the save handler to settings
		if (SettingsSection.IsValid()) {
			SettingsSection->OnModified().BindRaw(this, &FSDLControllerModule::HandleSettingsSaved);
		}	
	}
}

void FSDLControllerModule::UnregisterSettings()
{
	ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");

	if (SettingsModule != nullptr)
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "SDLContoller");
	}
}

bool FSDLControllerModule::HandleSettingsSaved()
{
	return true;
}


#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FSDLControllerModule, SDLController)
