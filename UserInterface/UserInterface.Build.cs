// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class UserInterface : ModuleRules
{
	public UserInterface(ReadOnlyTargetRules Target) : base(Target)
	{
		PublicDependencyModuleNames.AddRange(new[]
		{
			"Core", "ApplicationCore", "CoreUObject", "Engine", "UMG", "Slate", "SlateCore", "WebBrowserWidget",
			"WebBrowser", "BeatShotGlobal",
			"EasyFileDialog", "DLSSBlueprint", "NISBlueprint", "StreamlineBlueprint", "CommonLoadingScreen",
			"GameplayTags", "InputCore", "CommonUI"
		});

		PrivateDependencyModuleNames.AddRange(new[]
		{
			"ParallelcubeAudioAnalyzer", "ParallelcubeTaglib", "EnhancedInput"
		});

		PublicIncludePaths.AddRange(new[]
		{
			"../Plugins/BeatShotAudioAnalyzer/Source/AudioAnalyzer/Private",
			"../Plugins/BeatShotAudioAnalyzer/Source/AudioAnalyzer/Thirdparty/KissFFT_130",
			"../Plugins/BeatShotAudioAnalyzer/Source/AudioAnalyzer/Thirdparty/KissFFT_130/tools",
			"../Plugins/BeatShotAudioAnalyzer/Source/Thirdparty/miniaudio/include",
			"../Plugins/BeatShotAudioAnalyzer/Source/AudioAnalyzer/Thirdparty/stb"
		});

		PublicIncludePaths.Add(Path.Combine(EngineDirectory, "Plugins/Marketplace/DLSS/Source/ThirdParty/NGX/Include"));
		PublicIncludePaths.Add(Path.Combine(EngineDirectory,
			"Plugins/Marketplace/Streamline/Source/ThirdParty/Streamline/include"));
	}
}