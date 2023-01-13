// Copyright Epic Games, Inc. All Rights Reserved.

#include "BeatShot.h"
#include "Internationalization/StringTableRegistry.h"
#include "Modules/ModuleManager.h"

void FBeatShot::StartupModule()
{
	FDefaultGameModuleImpl::StartupModule();

	LOCTABLE_FROMFILE_GAME("ST_WebBrowserOverlay","ST_WebBrowserOverlay","/Game/StringTables/ST_WebBrowserOverlay");
	LOCTABLE_FROMFILE_GAME("ST_GameModesWidget","ST_GameModesWidget","/Game/ST_GameModesWidget");
	LOCTABLE_FROMFILE_GAME("ST_Login","ST_Login","/Game/StringTables/ST_Login");
}

void FBeatShot::ShutdownModule()
{
	FDefaultGameModuleImpl::ShutdownModule();
}

IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, BeatShot, "BeatShot" );