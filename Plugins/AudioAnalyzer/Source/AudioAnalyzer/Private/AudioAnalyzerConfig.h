// Copyright 2018 Cristian Barrio, Parallelcube. All Rights Reserved.
#ifndef AudioAnalyzerConfig_H
#define AudioAnalyzerConfig_H

#include "HAL/PlatformProcess.h"

#undef AUDIOANALYZER_OVR

// 0 Disable Library support
// 1 Enable Library support

#if PLATFORM_WINDOWS
	#define AUDIOANALYZER_OVR 1
#elif PLATFORM_ANDROID
	#define AUDIOANALYZER_OVR 1
#elif PLATFORM_LINUX
	#define AUDIOANALYZER_OVR 0
#elif PLATFORM_MAC
	#define AUDIOANALYZER_OVR 0
#elif PLATFORM_IOS
	#define AUDIOANALYZER_OVR 0
#else
	#define AUDIOANALYZER_OVR 0
#endif


#endif