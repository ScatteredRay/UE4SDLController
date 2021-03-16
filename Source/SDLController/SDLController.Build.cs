// Copyright (c) 2016, Indy Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.

using UnrealBuildTool;

public class SDLController : ModuleRules
{
    public SDLController(ReadOnlyTargetRules Target) : base(Target)
    {

        PrivateIncludePathModuleNames.Add("TargetPlatform");

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "SDLLibrary",
            "Projects",
            "ApplicationCore"
        });


        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "InputDevice",
            "SlateCore"
        });


        DynamicallyLoadedModuleNames.AddRange(new string[]
        {
        });

        AddEngineThirdPartyPrivateStaticDependencies(Target, "SDLLibrary");
    }
}
