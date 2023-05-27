// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class BeatShotGlobal : ModuleRules
{
	public BeatShotGlobal(ReadOnlyTargetRules Target) : base(Target)
	{
		PublicDependencyModuleNames.AddRange(new[]
		{ 
			"Core", "CoreUObject", "Engine", "HTTP", "Json", "HttpLibrary", "JsonLibrary", "JsonUtilities", "OnlineSubsystem", 
			"OnlineSubsystemUtils", "Steamworks", "NumCpp", "DLSSBlueprint", "NISBlueprint", "GameplayTags"
		});

		PublicIncludePaths.Add(Path.Combine(EngineDirectory, "Plugins/Marketplace/DLSS/Source/ThirdParty/NGX/Include"));
		
		DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");
		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}