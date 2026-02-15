// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

using UnrealBuildTool;

public class UserInterface : ModuleRules
{
	public UserInterface(ReadOnlyTargetRules Target) : base(Target)
	{
		PublicDependencyModuleNames.AddRange([
			"Core", "ApplicationCore", "CoreUObject", "Engine", "UMG", "Slate", "SlateCore", "WebBrowserWidget",
			"WebBrowser", "BeatShotGlobal", "GameplayTags", "InputCore", "CommonUI", "MoviePlayer"
		]);

		PrivateDependencyModuleNames.AddRange([
			"ParallelcubeAudioAnalyzer", "ParallelcubeTaglib", "EnhancedInput", "Charter"
		]);

		PublicIncludePaths.AddRange([
			"../Plugins/AudioAnalyzer/Source/AudioAnalyzer/Private",
			"../Plugins/AudioAnalyzer/Source/AudioAnalyzer/Thirdparty/KissFFT_130",
			"../Plugins/AudioAnalyzer/Source/AudioAnalyzer/Thirdparty/KissFFT_130/tools",
			"../Plugins/AudioAnalyzer/Source/Thirdparty/miniaudio/include",
			"../Plugins/AudioAnalyzer/Source/AudioAnalyzer/Thirdparty/stb"
		]);
	}
}