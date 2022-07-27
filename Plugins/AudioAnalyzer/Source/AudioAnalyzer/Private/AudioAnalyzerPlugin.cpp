// Copyright 2018 Cristian Barrio, Parallelcube. All Rights Reserved.
#include "AudioAnalyzerPlugin.h"
#include "AudioAnalyzerConfig.h"

#include "CoreMinimal.h"
#include "HAL/PlatformProcess.h"
#include "Runtime/Core/Public/Misc/Paths.h"

#define STB_VORBIS_HEADER_ONLY
#include "stb_vorbis.c"    

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"


DEFINE_LOG_CATEGORY(LogParallelcubeAudioAnalyzerPlugin);

/**
* Base audio analyzer plugin class
*/
class FAudioAnalyzerPlugin : public IAudioAnalyzerPlugin
{
public:
	// IModuleInterface implementation
	/**
	* StartupModule IModuleInterface implementation
	*/
	virtual void StartupModule() override;

	/**
	* ShutdownModule IModuleInterface implementation
	*/
	virtual void ShutdownModule() override;
	// End of IModuleInterface implementation

};

IMPLEMENT_MODULE(FAudioAnalyzerPlugin, ParallelcubeAudioAnalyzer)

void FAudioAnalyzerPlugin::StartupModule()
{
	UE_LOG(LogParallelcubeAudioAnalyzerPlugin, Log, TEXT("Audio Analyzer Plugin Load"));
}

void FAudioAnalyzerPlugin::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

