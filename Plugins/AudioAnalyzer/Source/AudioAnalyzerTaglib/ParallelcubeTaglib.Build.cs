// Copyright 2018 Cristian Barrio, Parallelcube. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class ParallelcubeTaglib : ModuleRules
{
    public ParallelcubeTaglib(ReadOnlyTargetRules Target) : base(Target)
    {
		bEnableUndefinedIdentifierWarnings = false;
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        bUseRTTI = true;
        bEnableExceptions = true;
        PrecompileForTargets = PrecompileTargetsType.Any;

        PrivateIncludePaths.Add("AudioAnalyzerTaglib/Thirdparty/utf8-cpp");

        PrivateIncludePaths.Add("AudioAnalyzerTaglib/Thirdparty/Taglib");
        PrivateIncludePaths.Add("AudioAnalyzerTaglib/Thirdparty/Taglib/toolkit");
        PrivateIncludePaths.Add("AudioAnalyzerTaglib/Thirdparty/Taglib/flac");
        PrivateIncludePaths.Add("AudioAnalyzerTaglib/Thirdparty/Taglib/ape");
        PrivateIncludePaths.Add("AudioAnalyzerTaglib/Thirdparty/Taglib/mpeg");
        PrivateIncludePaths.Add("AudioAnalyzerTaglib/Thirdparty/Taglib/mpeg/id3v1");
        PrivateIncludePaths.Add("AudioAnalyzerTaglib/Thirdparty/Taglib/mpeg/id3v2");
        PrivateIncludePaths.Add("AudioAnalyzerTaglib/Thirdparty/Taglib/mpeg/id3v2/frames");
        PrivateIncludePaths.Add("AudioAnalyzerTaglib/Thirdparty/Taglib/ogg");
        PrivateIncludePaths.Add("AudioAnalyzerTaglib/Thirdparty/Taglib/ogg/vorbis");
        PrivateIncludePaths.Add("AudioAnalyzerTaglib/Thirdparty/Taglib/riff");
        PrivateIncludePaths.Add("AudioAnalyzerTaglib/Thirdparty/Taglib/riff/wav");

        PublicDependencyModuleNames.AddRange(new string[] {"Core", "CoreUObject", "Engine", "Projects"});
	}
}


