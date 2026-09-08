// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SaveGames/SaveGamePlayerSettings.h"
#include "Utilities/SaveLoadCommon.h"

namespace
{
constexpr int PlayerSettingsSlotIndex = 0;
constexpr auto PlayerSettingsSlotName = TEXT("SettingsSlot");
}

USaveGamePlayerSettings* USaveGamePlayerSettings::LoadFromSlot()
{
	return SaveLoadCommon::LoadFromSlot<USaveGamePlayerSettings>(PlayerSettingsSlotName, PlayerSettingsSlotIndex);
}

void USaveGamePlayerSettings::SaveToSlot()
{
	SaveLoadCommon::SaveToSlot(this, PlayerSettingsSlotName, PlayerSettingsSlotIndex);
}

FPlayerSettings USaveGamePlayerSettings::GetPlayerSettings() const
{
	return PlayerSettings;
}

void USaveGamePlayerSettings::SavePlayerSettings(const FPlayerSettings_Game& InGameSettings)
{
	PlayerSettings.Game = InGameSettings;
}

void USaveGamePlayerSettings::SavePlayerSettings(const FPlayerSettings_AudioAnalyzer& InAudioAnalyzerSettings)
{
	PlayerSettings.AudioAnalyzer = InAudioAnalyzerSettings;
}

void USaveGamePlayerSettings::SavePlayerSettings(const FPlayerSettings_User& InUserSettings)
{
	PlayerSettings.User = InUserSettings;
}

void USaveGamePlayerSettings::SavePlayerSettings(const FPlayerSettings_CrossHair& InCrossHairSettings)
{
	PlayerSettings.CrossHair = InCrossHairSettings;
}
