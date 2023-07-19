// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class BeatShotGlobal : ModuleRules
{
	public BeatShotGlobal(ReadOnlyTargetRules Target) : base(Target)
	{
		PublicDependencyModuleNames.AddRange(new[]
		{ 
			"Core", "CoreUObject", "Engine", "HTTP", "HttpLibrary", "Json", "JsonLibrary", "JsonUtilities", "DLSSBlueprint", "NISBlueprint", "StreamlineBlueprint", "GameplayTags"
		});

		PublicIncludePaths.Add(Path.Combine(EngineDirectory, "Plugins/Marketplace/DLSS/Source/ThirdParty/NGX/Include"));
		PublicIncludePaths.Add(Path.Combine(EngineDirectory, "Plugins/Marketplace/Streamline/Source/ThirdParty/Streamline/include"));
	}
}