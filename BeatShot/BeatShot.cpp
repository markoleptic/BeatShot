// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#include "BeatShot.h"
#include "Modules/ModuleManager.h"

IMPLEMENT_PRIMARY_GAME_MODULE(FDefaultGameModuleImpl, BeatShot, "BeatShot");

void FBeatShot::StartupModule()
{
	FDefaultGameModuleImpl::StartupModule();
}

void FBeatShot::ShutdownModule()
{
	FDefaultGameModuleImpl::ShutdownModule();
}
