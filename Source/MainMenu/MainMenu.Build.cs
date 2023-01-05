// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MainMenu : ModuleRules
{
	public MainMenu(ReadOnlyTargetRules Target) : base(Target)
	{
		PublicDependencyModuleNames.AddRange(new[] { "Core"});
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");
		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}