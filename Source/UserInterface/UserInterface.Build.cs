// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class UserInterface : ModuleRules
{
	public UserInterface(ReadOnlyTargetRules Target) : base(Target)
	{
		PublicDependencyModuleNames.AddRange(new[]
		{
			"Core", "CoreUObject", "Engine", "UMG", "Slate", "SlateCore", "WebBrowserWidget", "WebBrowser", "BeatShotGlobal",
			"EasyFileDialog", "OnlineSubsystem", "OnlineSubsystemUtils", "Steamworks", "DLSSBlueprint", "NISBlueprint", 
			"CommonLoadingScreen"
		});

		PrivateDependencyModuleNames.AddRange(new[]
		{ 
			"ParallelcubeAudioAnalyzer", "ParallelcubeTaglib"
		});
		
		PublicIncludePaths.Add(Path.Combine(EngineDirectory, "Plugins/Marketplace/DLSS/Source/ThirdParty/NGX/Include"));
		
		PrivateIncludePaths.Add(Path.Combine(EngineDirectory, "Plugins/Marketplace/AudioAnalyzer/Source/AudioAnalyzer/Private"));
		
		DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");
		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}