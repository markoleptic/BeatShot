// Copyright 2018 Cristian Barrio, Parallelcube. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IPluginManager.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"

/**
* Plugin Log category
*/
DECLARE_LOG_CATEGORY_EXTERN(LogParallelcubeAudioAnalyzerPlugin, Log, All);


/**
* The public interface to this module
*/
class IAudioAnalyzerPlugin : public IModuleInterface
{

public:

	/**
	* Singleton-like access to this module's interface.  This is just for convenience!
	* Beware of calling this during the shutdown phase, though.  Your module might have been unloaded already.
	*
	* @return Returns singleton instance, loading the module on demand if needed
	*/
	static inline IAudioAnalyzerPlugin& Get()
	{
		return FModuleManager::LoadModuleChecked<IAudioAnalyzerPlugin>("AudioAnalyzerPlugin");
	}

	/**
	* Checks to see if this module is loaded and ready.  It is only valid to call Get() if IsAvailable() returns true.
	*
	* @return True if the module is loaded and ready to use
	*/
	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("AudioAnalyzerPlugin");
	}
};
