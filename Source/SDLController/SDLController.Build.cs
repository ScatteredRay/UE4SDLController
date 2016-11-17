// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

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
