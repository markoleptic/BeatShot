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
		PrecompileForTargets = PrecompileTargetsType.Any;

		PublicDependencyModuleNames.AddRange(new[]
		{
			"Core", "CoreUObject", "Engine", "InputCore", "Niagara", "EnhancedInput", "UserInterface", "UMG",
			"BeatShotGlobal", "NumCpp", "OnlineSubsystem", "OnlineSubsystemUtils",
			"Steamworks", "GameplayAbilities", "GameplayTags", "GameplayTasks", "NetCore", "PhysicsCore",
			"ModularGameplay", "CommonLoadingScreen", "Text3D", "DLSSBlueprint", "NISBlueprint",
			"StreamlineBlueprint"
		});

		PrivateDependencyModuleNames.AddRange(new[]
		{
			"ParallelcubeAudioAnalyzer", "ParallelcubeTaglib"
		});

		PublicIncludePaths.AddRange(new[]
		{
			"../Plugins/BeatShotAudioAnalyzer/Source/AudioAnalyzer/Private",
			"../Plugins/BeatShotAudioAnalyzer/Source/AudioAnalyzer/Thirdparty/KissFFT_130",
			"../Plugins/BeatShotAudioAnalyzer/Source/AudioAnalyzer/Thirdparty/KissFFT_130/tools",
			"../Plugins/BeatShotAudioAnalyzer/Source/Thirdparty/miniaudio/include",
			"../Plugins/BeatShotAudioAnalyzer/Source/Thirdparty/miniaudio",
			"../Plugins/BeatShotAudioAnalyzer/Source/AudioAnalyzer/Thirdparty/stb"
		});

		PublicIncludePaths.Add(Path.Combine(EngineDirectory, "Plugins/Marketplace/DLSS/Source/ThirdParty/NGX/Include"));
		PublicIncludePaths.Add(Path.Combine(EngineDirectory,
			"Plugins/Marketplace/Streamline/Source/ThirdParty/Streamline/include"));

		DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");
	}
}