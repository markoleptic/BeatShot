// Copyright 2018 Cristian Barrio, Parallelcube. All Rights Reserved.

#define AUDIOANALYZER_WIN_OVR
#define AUDIOANALYZER_ANDROID_OVR

using System.IO;
using UnrealBuildTool;

public class ParallelcubeAudioAnalyzer : ModuleRules
{
    public ParallelcubeAudioAnalyzer(ReadOnlyTargetRules Target) : base(Target)
    {
		bEnableUndefinedIdentifierWarnings = false;
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        PrecompileForTargets = PrecompileTargetsType.Any;

        //PrivateIncludePaths.Add("AudioAnalyzer/Private");
        //PrivateIncludePaths.Add("AudioAnalyzer/Thirdparty/KissFFT_130");
        //PrivateIncludePaths.Add("AudioAnalyzer/Thirdparty/KissFFT_130/tools");

        //PrivateIncludePaths.Add("Thirdparty/miniaudio/include");
        //PrivateIncludePaths.Add("AudioAnalyzer/Thirdparty/stb");

        //Nativization workaround		
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "../AudioAnalyzer/Private"));
		PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "../AudioAnalyzer/Thirdparty/KissFFT_130"));
		PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "../AudioAnalyzer/Thirdparty/KissFFT_130/tools"));

		PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "../Thirdparty/miniaudio/include"));
		PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "../AudioAnalyzer/Thirdparty/stb"));

		PublicDefinitions.AddRange(
		new string[] {
				"DR_WAV_IMPLEMENTATION=1",
				"DR_MP3_IMPLEMENTATION=1",
				"DR_FLAC_IMPLEMENTATION=1",
				"MA_NO_AAUDIO=1"
		});

        PublicDependencyModuleNames.AddRange(new string[] {"Core", "CoreUObject", "Engine", "Projects"});

        PrivateDependencyModuleNames.AddRange(new string[] { "ParallelcubeTaglib" });
		
        //var basePath = Path.GetDirectoryName(RulesCompiler.GetFileNameFromType(GetType()));
		//string thirdPartyPath = Path.Combine(basePath, "..", "Thirdparty");

		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
#if AUDIOANALYZER_WIN_OVR
			PublicDependencyModuleNames.AddRange(new[] { "LibOVRPlatform" });
			PublicDelayLoadDLLs.AddRange(new string[] { "LibOVRPlatform64_1.dll" });
#endif
		}
		else if (Target.Platform == UnrealTargetPlatform.Android)
		{
#if AUDIOANALYZER_ANDROID_OVR
			PublicDependencyModuleNames.AddRange(new[] { "LibOVRPlatform" });
#endif
		}
		else if (Target.Platform == UnrealTargetPlatform.Linux)
		{
		}
		else if (Target.Platform == UnrealTargetPlatform.Mac)
		{
        } 
		else if (Target.Platform == UnrealTargetPlatform.IOS)
		{
			PublicDefinitions.AddRange(
			new string[] {
				"MA_NO_RUNTIME_LINKING=1"
			});

			PublicFrameworks.AddRange(
				new string[] {
					 "CoreFoundation",
					 "CoreAudio",
					 "AudioToolbox"
				});
		}
		else
		{
			//throw new System.Exception(System.String.Format("Unsupported platform {0}", Target.Platform.ToString()));
		}
	}
}


