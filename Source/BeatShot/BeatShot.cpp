// Copyright Epic Games, Inc. All Rights Reserved.

#include "BeatShot.h"
#include "Internationalization/StringTableRegistry.h"
#include "Modules/ModuleManager.h"

void FBeatShot::StartupModule()
{
	FDefaultGameModuleImpl::StartupModule();

	LOCTABLE_FROMFILE_GAME("WebBrowserOverlayStrings","WebBrowserOverlayStrings","/Game/NonAssets/WebBrowserOverlay.csv");
	LOCTABLE_FROMFILE_GAME("TooltipsStrings","TooltipsStrings","/Game/NonAssets/Tooltips.csv");
	LOCTABLE_FROMFILE_GAME("LoginStrings","LoginStrings","/Game/NonAssets/Login.csv");
}

void FBeatShot::ShutdownModule()
{
	FDefaultGameModuleImpl::ShutdownModule();
}

IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, BeatShot, "BeatShot" );