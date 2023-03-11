// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

using UnrealBuildTool;

public class BeatShot : ModuleRules
{
	public BeatShot(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new[]
		{
			"Core", "CoreUObject", "Engine", "InputCore", "Niagara", "EnhancedInput", "UserInterface", "Global", "OnlineSubsystem", "OnlineSubsystemUtils", "Steamworks", "GameplayAbilities"
		});
		PrivateDependencyModuleNames.AddRange(new[]
			{ "ParallelcubeTaglib", "ParallelcubeAudioAnalyzer", "EnhancedInput", "InputCore", "GameplayAbilities", "GameplayTags", "GameplayTasks" });
		PrivateIncludePaths.Add("../Plugins/Marketplace/AudioAnalyzer/Source/AudioAnalyzer/Private");
		PrivateIncludePaths.Add("../Plugins/Marketplace/AudioAnalyzer/Source/AudioAnalyzer/Thirdparty/KissFFT_130");
		PrivateIncludePaths.Add(
			"../Plugins/Marketplace/AudioAnalyzer/Source/AudioAnalyzer/Thirdparty/KissFFT_130/tools");
		PrivateIncludePaths.Add("../Plugins/Marketplace/AudioAnalyzer/Source/Thirdparty/miniaudio/include");
		PrivateIncludePaths.Add("../Plugins/Marketplace/AudioAnalyzer/Source/AudioAnalyzer/Thirdparty/stb");

		DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}