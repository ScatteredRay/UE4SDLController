
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SDLControllerBlueprintLibrary.generated.h"

UENUM(BlueprintType)
enum class EGameControllerType : uint8 {
	SDL_CONTROLLER_TYPE_UNKNOWN = 0,
	SDL_CONTROLLER_TYPE_XBOX360,
	SDL_CONTROLLER_TYPE_XBOXONE,
	SDL_CONTROLLER_TYPE_PS3,
	SDL_CONTROLLER_TYPE_PS4,
	SDL_CONTROLLER_TYPE_NINTENDO_SWITCH_PRO,
	SDL_CONTROLLER_TYPE_VIRTUAL,
	SDL_CONTROLLER_TYPE_PS5
};

UCLASS()
class USDLControllerBlurprintLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
    /** Starts an analytics session without any custom attributes specified */
    UFUNCTION(BlueprintCallable, Category="SDLController")
    static EGameControllerType GetControllerType(int Index);
};

