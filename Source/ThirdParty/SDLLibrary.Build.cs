// Copyright (c) 2016, Indy Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.

using System.IO;
using UnrealBuildTool;

public class SDLLibrary : ModuleRules
{
    public SDLLibrary(ReadOnlyTargetRules Target) : base(Target)
    {
        Type = ModuleType.External;

        PublicIncludePaths.Add(Path.Combine(PluginDirectory, "Source", "ThirdParty", "SDL", "include"));

        string BuildConfig = "Release";
        switch(Target.Configuration) {
            case UnrealTargetConfiguration.Development:
            case UnrealTargetConfiguration.Shipping:
            case UnrealTargetConfiguration.Test:
            case UnrealTargetConfiguration.DebugGame:
                BuildConfig = "Release";
                break;
            case UnrealTargetConfiguration.Debug:
                BuildConfig = "Debug";
                break;
            default:
                throw new BuildException("Invalid Build Configuration '{0}", Target.Configuration);
        }

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            string LibPath = Path.Combine(PluginDirectory, "Binaries", "Win64");
            if(!Directory.Exists(LibPath)) {
                //Directory.CreateDirectory(LibPath);
            }

            PublicLibraryPaths.Add(LibPath);

            PublicAdditionalLibraries.Add("SDL2.lib");
            PublicDelayLoadDLLs.Add("SDL2.dll");
        }
    }
}
