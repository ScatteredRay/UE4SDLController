
#include "SDLControllerBlueprintLibrary.h"
#include "SDLController.h"

EGameControllerType USDLControllerBlurprintLibrary::GetControllerType(int Index)
{
	//const UEnum* ReflPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EGameControllerType"), true);
	//return ReflPtr->GetNameStringByIndex(SDL_GameControllerTypeForIndex(Index));
	return (EGameControllerType)SDL_GameControllerTypeForIndex(Index);
}