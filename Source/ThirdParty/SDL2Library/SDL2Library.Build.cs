// Fill out your copyright notice in the Description page of Project Settings.

using System.IO;
using UnrealBuildTool;

public class SDL2Library : ModuleRules
{
	public SDL2Library(ReadOnlyTargetRules Target) : base(Target)
	{
		Type = ModuleType.External;

        PublicIncludePaths.Add("ThirdParty/SDL2Library/include");

		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			PublicLibraryPaths.Add(Path.Combine(ModuleDirectory, "lib", "x64"));
			PublicAdditionalLibraries.Add("SDL2.lib");

			PublicDelayLoadDLLs.Add("SDL2.dll");
		}
	}
}
