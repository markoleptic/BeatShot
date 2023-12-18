// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class LocalBoost : ModuleRules
{
	public LocalBoost(ReadOnlyTargetRules Target) : base(Target)
	{
		Type = ModuleType.External;


		string[] BoostLibraries = { "atomic", "chrono", "iostreams", "program_options", "python39", "regex", "system", "thread", "date_time" };
		
		// string BoostVersion = "1_81_1";
		// string BoostVersionDir = "boost-" + BoostVersion;
		
		string BoostPath = Path.Combine(ModuleDirectory);
		
		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			string BoostToolsetVersion = "vc143";
			string BoostLibPath = Path.Combine(BoostPath, "lib");
			string BoostVersionShort = "1_81";
			foreach (string BoostLib in BoostLibraries)
			{
				string BoostLibName = "boost_" + BoostLib + "-" + BoostToolsetVersion + "-mt-x64" + "-" + BoostVersionShort;
				PublicAdditionalLibraries.Add(Path.Combine(BoostLibPath, BoostLibName + ".lib"));
				RuntimeDependencies.Add(Path.Combine("$(TargetOutputDir)", BoostLibName + ".dll"), Path.Combine(BoostLibPath, BoostLibName + ".dll"));
			}

			PublicDefinitions.Add("BOOST_LIB_TOOLSET=\"" + BoostToolsetVersion + "\"");
			PublicDefinitions.Add("BOOST_ALL_NO_LIB");
		}
		else if (Target.Platform == UnrealTargetPlatform.Mac)
		{
			string BoostLibPath = Path.Combine(BoostPath, "lib", "Mac");

			foreach (string BoostLib in BoostLibraries)
			{
				// Note that these file names identify the universal binaries
				// that support both x86_64 and arm64.
				string BoostLibName = "libboost_" + BoostLib + "-mt";
				PublicAdditionalLibraries.Add(Path.Combine(BoostLibPath, BoostLibName + ".a"));
				RuntimeDependencies.Add(Path.Combine("$(TargetOutputDir)", BoostLibName + ".dylib"), Path.Combine(BoostLibPath, BoostLibName + ".dylib"));
			}
		}
	}
}