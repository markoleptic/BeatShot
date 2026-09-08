// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "BSPlayerSettingsInterface.h"
#include "SaveGames/SaveGamePlayerSettings.h"

FPlayerSettings IBSPlayerSettingsInterface::LoadPlayerSettings()
{
	if (const USaveGamePlayerSettings* SaveGamePlayerSettings = USaveGamePlayerSettings::LoadFromSlot())
	{
		return SaveGamePlayerSettings->GetPlayerSettings();
	}
	return FPlayerSettings();
}

void IBSPlayerSettingsInterface::SavePlayerSettings(const FPlayerSettings_AudioAnalyzer& InSettingsStruct)
{
	if (USaveGamePlayerSettings* Settings = USaveGamePlayerSettings::LoadFromSlot())
	{
		Settings->SavePlayerSettings(InSettingsStruct);
		Settings->SaveToSlot();
		OnPlayerSettingsChangedDelegate_AudioAnalyzer.Broadcast(InSettingsStruct);
	}
}

void IBSPlayerSettingsInterface::SavePlayerSettings(const FPlayerSettings_CrossHair& InSettingsStruct)
{
	if (USaveGamePlayerSettings* Settings = USaveGamePlayerSettings::LoadFromSlot())
	{
		Settings->SavePlayerSettings(InSettingsStruct);
		Settings->SaveToSlot();
		OnPlayerSettingsChangedDelegate_CrossHair.Broadcast(InSettingsStruct);
	}
}

void IBSPlayerSettingsInterface::SavePlayerSettings(const FPlayerSettings_Game& InSettingsStruct)
{
	if (USaveGamePlayerSettings* Settings = USaveGamePlayerSettings::LoadFromSlot())
	{
		Settings->SavePlayerSettings(InSettingsStruct);
		Settings->SaveToSlot();
		OnPlayerSettingsChangedDelegate_Game.Broadcast(InSettingsStruct);
	}
}

void IBSPlayerSettingsInterface::SavePlayerSettings(const FPlayerSettings_User& InSettingsStruct)
{
	if (USaveGamePlayerSettings* Settings = USaveGamePlayerSettings::LoadFromSlot())
	{
		Settings->SavePlayerSettings(InSettingsStruct);
		Settings->SaveToSlot();
		OnPlayerSettingsChangedDelegate_User.Broadcast(InSettingsStruct);
	}
}
