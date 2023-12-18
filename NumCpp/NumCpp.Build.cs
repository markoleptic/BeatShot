// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

using UnrealBuildTool;
public class NumCpp : ModuleRules
{
	public NumCpp(ReadOnlyTargetRules Target) : base(Target)
	{
		bEnableExceptions = true;
		Type = ModuleType.External;
		PublicDependencyModuleNames.AddRange(new[]
		{
			"Core", "CoreUObject", "Engine", "LocalBoost"
		});
	}
}