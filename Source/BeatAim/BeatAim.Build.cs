// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class BeatAim : ModuleRules
{
	public BeatAim(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

		PrivateDependencyModuleNames.AddRange(new string[] { "ParallelcubeTaglib", "ParallelcubeAudioAnalyzer" });

		PrivateIncludePaths.Add("../Plugins/AudioAnalyzer/Source/AudioAnalyzer/Private");
		PrivateIncludePaths.Add("../Plugins/AudioAnalyzer/Source/AudioAnalyzer/Thirdparty/KissFFT_130");
		PrivateIncludePaths.Add("../Plugins/AudioAnalyzer/Source/AudioAnalyzer/Thirdparty/KissFFT_130/tools");
		PrivateIncludePaths.Add("../Plugins/AudioAnalyzer/Source/Thirdparty/miniaudio/include");
		PrivateIncludePaths.Add("../Plugins/AudioAnalyzer/Source/AudioAnalyzer/Thirdparty/stb");

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
