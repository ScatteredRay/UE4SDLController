// Copyright (c) 2016, Indy Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.

using UnrealBuildTool;

public class SDLController : ModuleRules
{
    public SDLController(TargetInfo Target)
    {

        PrivateIncludePathModuleNames.Add("TargetPlatform");
        
        PublicIncludePaths.AddRange(new string[]
        {
            "SDLController/Public"
        });
                
        
        PrivateIncludePaths.AddRange(new string[]
        {
            "SDLController/Private",
        });
            
        
        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "Engine",
            "SDL2Library",
            "Projects"
        });


        PrivateDependencyModuleNames.AddRange(new string[]
        {
			"InputDevice",
            "SlateCore"
        });


        DynamicallyLoadedModuleNames.AddRange(new string[]
        {
        });

        AddThirdPartyPrivateStaticDependencies(Target,
            "SDL2Library"
        );
    }
}
