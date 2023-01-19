// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Global : ModuleRules
{
	public Global(ReadOnlyTargetRules Target) : base(Target)
	{
		PublicDependencyModuleNames.AddRange(new[]
			{ "Core", "CoreUObject", "Engine", "HTTP", "Json", "HttpLibrary", "JsonLibrary", "JsonUtilities" });
		// Uncomment if you are using online features
		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}