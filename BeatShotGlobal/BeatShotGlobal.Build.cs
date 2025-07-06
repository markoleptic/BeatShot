// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class BeatShotGlobal : ModuleRules
{
	public BeatShotGlobal(ReadOnlyTargetRules Target) : base(Target)
	{
		PublicDependencyModuleNames.AddRange([
			"Core", "CoreUObject", "InputCore", "Engine", "HTTP", "Json", "JsonUtilities",
			"DLSSBlueprint", "NISBlueprint", "StreamlineBlueprint", "StreamlineDLSSGBlueprint",
			"StreamlineReflexBlueprint", "GameplayTags", "UMG", "EnhancedInput",
			"PhysicsCore", "Slate", "SlateCore", "AudioMixer"
		]);

		PrivateDependencyModuleNames.AddRange([
			"DeveloperSettings", "AudioModulation"
		]);
	}
}