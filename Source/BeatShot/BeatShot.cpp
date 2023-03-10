// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#include "BeatShot.h"
#include "Internationalization/StringTableRegistry.h"
#include "Modules/ModuleManager.h"

void FBeatShot::StartupModule()
{
	FDefaultGameModuleImpl::StartupModule();
}

void FBeatShot::ShutdownModule()
{
	FDefaultGameModuleImpl::ShutdownModule();
}

IMPLEMENT_PRIMARY_GAME_MODULE(FDefaultGameModuleImpl, BeatShot, "BeatShot");
