// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class BeatShotTarget : TargetRules
{
	public BeatShotTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_2;
		ExtraModuleNames.AddRange(new[] { "BeatShot", "UserInterface", "BeatShotGlobal"});
	}
}