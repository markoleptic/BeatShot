// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class BeatShot : ModuleRules
{
	public BeatShot(ReadOnlyTargetRules Target) : base(Target)
	{
		bEnableUndefinedIdentifierWarnings = false;
		bEnableExceptions = true;
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new[]
		{
			"Core", "CoreUObject", "Engine", "InputCore", "Niagara", "EnhancedInput", "UserInterface", "UMG", "Global", "NumCpp", "OnlineSubsystem", "OnlineSubsystemUtils", 
			"Steamworks", "GameplayAbilities", "GameplayTags", "GameplayTasks", "ModularGameplay", "GameFeatures", "NetCore", "PhysicsCore"
		});
		
		PrivateDependencyModuleNames.AddRange(new[]
		{ 
			"ParallelcubeAudioAnalyzer", "ParallelcubeTaglib"
		});
		
		PrivateIncludePaths.Add(Path.Combine(EngineDirectory, "Plugins/Marketplace/AudioAnalyzer/Source/AudioAnalyzer/Private"));

		DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");
		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}