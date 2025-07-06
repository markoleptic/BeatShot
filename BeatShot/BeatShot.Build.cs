// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class BeatShot : ModuleRules
{
	public BeatShot(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PrecompileForTargets = PrecompileTargetsType.Any;

		PublicDependencyModuleNames.AddRange([
			"Core", "CoreUObject", "Engine", "InputCore", "Niagara", "EnhancedInput", "UserInterface", "UMG",
			"BeatShotGlobal", "OnlineSubsystem", "OnlineSubsystemUtils",
			"Steamworks", "GameplayAbilities", "GameplayTags", "GameplayTasks", "NetCore", "PhysicsCore",
			"ModularGameplay", "Text3D", "DLSSBlueprint", "NISBlueprint", "StreamlineDLSSGBlueprint",
			"StreamlineReflexBlueprint",
			"StreamlineBlueprint", "Slate", "SlateCore", "MoviePlayer", "NumCpp", "Synthesis",
			"RuntimeAudioImporter", "MetasoundEngine", "MetasoundFrontend", "AudioMixer"
		]);

		PrivateDependencyModuleNames.AddRange([
			"ParallelcubeAudioAnalyzer", "ParallelcubeTaglib", "EnhancedInput", "MoviePlayer", "DeveloperSettings",
			"AudioModulation"
		]);

		PublicIncludePaths.AddRange([
			"../Plugins/AudioAnalyzer/Source/AudioAnalyzer/Private",
			"../Plugins/AudioAnalyzer/Source/AudioAnalyzer/Thirdparty/KissFFT_130",
			"../Plugins/AudioAnalyzer/Source/AudioAnalyzer/Thirdparty/KissFFT_130/tools",
			"../Plugins/AudioAnalyzer/Source/Thirdparty/miniaudio/include",
			"../Plugins/AudioAnalyzer/Source/AudioAnalyzer/Thirdparty/stb"
		]);

		DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");
	}
}