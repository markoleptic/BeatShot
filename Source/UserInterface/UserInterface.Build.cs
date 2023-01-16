// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class UserInterface : ModuleRules
{
	public UserInterface(ReadOnlyTargetRules Target) : base(Target)
	{
		PublicDependencyModuleNames.AddRange(new[]
		{
			"Core", "CoreUObject", "Engine", "UMG", "Slate", "SlateCore", "WebBrowserWidget", "WebBrowser", "Global", "EasyFileDialog"
		});
		PrivateDependencyModuleNames.AddRange(new[]
			{ "ParallelcubeTaglib", "ParallelcubeAudioAnalyzer" });
		PrivateIncludePaths.Add("../Plugins/Marketplace/AudioAnalyzer/Source/AudioAnalyzer/Private");
		PrivateIncludePaths.Add("../Plugins/Marketplace/AudioAnalyzer/Source/Thirdparty/miniaudio/include");
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");
		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}