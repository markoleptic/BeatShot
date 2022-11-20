// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class BeatShot : ModuleRules
{
    public BeatShot(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore",
            "UMG", "Slate", "SlateCore", "Niagara", "HTTP", "Json", "HttpLibrary", "JsonLibrary", "JsonUtilities",
            "WebBrowserWidget", "WebBrowser", "EasyFileDialog", "EnhancedInput" });
        PrivateDependencyModuleNames.AddRange(new string[] { "ParallelcubeTaglib", "ParallelcubeAudioAnalyzer",
            "Slate", "SlateCore", "EnhancedInput", "InputCore" });
        PrivateIncludePaths.Add("../Plugins/Marketplace/AudioAnalyzer/Source/AudioAnalyzer/Private");
        PrivateIncludePaths.Add("../Plugins/Marketplace/AudioAnalyzer/Source/AudioAnalyzer/Thirdparty/KissFFT_130");
        PrivateIncludePaths.Add("../Plugins/Marketplace/AudioAnalyzer/Source/AudioAnalyzer/Thirdparty/KissFFT_130/tools");
        PrivateIncludePaths.Add("../Plugins/Marketplace/AudioAnalyzer/Source/Thirdparty/miniaudio/include");
        PrivateIncludePaths.Add("../Plugins/Marketplace/AudioAnalyzer/Source/AudioAnalyzer/Thirdparty/stb");

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
