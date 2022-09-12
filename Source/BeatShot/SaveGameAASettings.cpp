// Fill out your copyright notice in the Description page of Project Settings.

#include "SaveGameAASettings.h"
#include "GameModeActorStruct.h"

USaveGameAASettings::USaveGameAASettings()
{
}

void USaveGameAASettings::SaveAASettings(FAASettingsStruct PlayerSettingsToSave)
{
	AASettings = PlayerSettingsToSave;
}

FAASettingsStruct USaveGameAASettings::LoadAASettings()
{
	return AASettings;
}
