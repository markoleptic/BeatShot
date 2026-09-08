// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SaveGames/SaveGamePlayerSettings.h"
#include "Utilities/SaveLoadCommon.h"

namespace
{
constexpr int SlotIndex = 0;
constexpr auto SlotName = TEXT("SettingsSlot");
}

USaveGamePlayerSettings* USaveGamePlayerSettings::LoadFromSlot()
{
	return SaveLoadCommon::LoadFromSlot<USaveGamePlayerSettings>(SlotName, SlotIndex);
}

void USaveGamePlayerSettings::SaveToSlot()
{
	SaveLoadCommon::SaveToSlot(this, SlotName, SlotIndex);
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
