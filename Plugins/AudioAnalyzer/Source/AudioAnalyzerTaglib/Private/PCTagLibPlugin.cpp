// Copyright 2018 Cristian Barrio, Parallelcube. All Rights Reserved.
#include "PCTaglibPlugin.h"

#include "CoreMinimal.h"

DEFINE_LOG_CATEGORY(LogParallelcubeTaglibPlugin);

/**
* Base audio analyzer plugin class
*/
class FPCTaglibPlugin : public IPCTaglibPlugin
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

IMPLEMENT_MODULE(FPCTaglibPlugin, ParallelcubeTaglib)

void FPCTaglibPlugin::StartupModule()
{
	UE_LOG(LogParallelcubeTaglibPlugin, Log, TEXT("Taglib Load"));
}


void FPCTaglibPlugin::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

