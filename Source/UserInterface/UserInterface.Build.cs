// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

using UnrealBuildTool;

public class UserInterface : ModuleRules
{
	public UserInterface(ReadOnlyTargetRules Target) : base(Target)
	{
		PublicDependencyModuleNames.AddRange(new[]
		{
			"Core", "CoreUObject", "Engine", "UMG", "Slate", "SlateCore", "WebBrowserWidget", "WebBrowser", "Global", "EasyFileDialog", "OnlineSubsystem", "OnlineSubsystemUtils", "Steamworks", 
			"DLSSBlueprint", "NISBlueprint", "ParallelcubeAudioAnalyzer", "ParallelcubeTaglib"
		});
		PrivateDependencyModuleNames.AddRange(new[]
			{ "OnlineSubsystem", "DLSSBlueprint", "NISBlueprint" });
		PrivateIncludePaths.Add("../Plugins/Marketplace/AudioAnalyzer/Source/AudioAnalyzer/Private");
		PrivateIncludePaths.Add("../Plugins/Marketplace/AudioAnalyzer/Source/Thirdparty/miniaudio/include");
		PublicIncludePaths.Add("U:/Epic Games/UE_5.1/Engine/Plugins/Marketplace/DLSS/Source/ThirdParty/NGX/Include");

		DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");
		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}