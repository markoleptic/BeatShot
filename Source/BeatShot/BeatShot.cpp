// Copyright Epic Games, Inc. All Rights Reserved.

#include "BeatShot.h"
#include "Internationalization/StringTableRegistry.h"
#include "Modules/ModuleManager.h"

void FBeatShot::StartupModule()
{
	FDefaultGameModuleImpl::StartupModule();

	LOCTABLE_FROMFILE_GAME("WebBrowserOverlayStrings","WebBrowserOverlayStrings","NonAssets/WebBrowserOverlay.csv");
	LOCTABLE_FROMFILE_GAME("TooltipStrings","TooltipStrings","NonAssets/Tooltips.csv");
	LOCTABLE_FROMFILE_GAME("LoginStrings","LoginStrings","NonAssets/Login.csv");

	// Trying to create a new one didn't work either
	LOCTABLE_NEW("CodeStringTable","CodeStringTable");
	LOCTABLE_SETSTRING("CodeStringTable","HelloWorld","Hello World!");
	LOCTABLE_SETMETA("CodeStringTable","HelloWorld","Comment","This is a comment about hello world");
	LOCTABLE_SETSTRING("CodeStringTable","GoodbyeWorld","Goodbye World!");
	LOCTABLE_SETMETA("CodeStringTable","GoodbyeWorld","Comment","This is a comment about goodbye world");
}

void FBeatShot::ShutdownModule()
{
	FDefaultGameModuleImpl::ShutdownModule();
}

IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, BeatShot, "BeatShot" );