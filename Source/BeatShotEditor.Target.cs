// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class BeatShotEditorTarget : TargetRules
{
	public BeatShotEditorTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange( new string[] { "BeatShot", "UserInterface", "Global" } );
	}
}
