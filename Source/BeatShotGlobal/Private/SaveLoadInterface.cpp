// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#include "SaveLoadInterface.h"
#include "BSGameModeDataAsset.h"
#include "SaveGameCustomGameMode.h"
#include "SaveGamePlayerScore.h"
#include "SaveGamePlayerSettings.h"
#include "Kismet/GameplayStatics.h"

FPlayerSettings ISaveLoadInterface::LoadPlayerSettings() const
{
	if (UGameplayStatics::DoesSaveGameExist(TEXT("SettingsSlot"), 0))
	{
		if (USaveGamePlayerSettings* SaveGamePlayerSettings = Cast<USaveGamePlayerSettings>(UGameplayStatics::LoadGameFromSlot(TEXT("SettingsSlot"), 0)))
		{
			return SaveGamePlayerSettings->PlayerSettings;
		}
	}
	else
	{
		if (USaveGamePlayerSettings* SaveGamePlayerSettings = Cast<USaveGamePlayerSettings>(UGameplayStatics::CreateSaveGameObject(USaveGamePlayerSettings::StaticClass())))
		{
			return SaveGamePlayerSettings->PlayerSettings;
		}
	}
	return FPlayerSettings();
}

void ISaveLoadInterface::SavePlayerSettings(const FPlayerSettings_Game& InGameSettings)
{
	if (USaveGamePlayerSettings* SaveGameObject = Cast<USaveGamePlayerSettings>(UGameplayStatics::CreateSaveGameObject(USaveGamePlayerSettings::StaticClass())))
	{
		SaveGameObject->PlayerSettings = LoadPlayerSettings();
		SaveGameObject->PlayerSettings.Game = InGameSettings;
		UGameplayStatics::SaveGameToSlot(SaveGameObject, TEXT("SettingsSlot"), 0);
	}
	OnPlayerSettingsChangedDelegate_Game.Broadcast(InGameSettings);
}

void ISaveLoadInterface::SavePlayerSettings(const FPlayerSettings_AudioAnalyzer& InAudioAnalyzerSettings)
{
	if (USaveGamePlayerSettings* SaveGameObject = Cast<USaveGamePlayerSettings>(UGameplayStatics::CreateSaveGameObject(USaveGamePlayerSettings::StaticClass())))
	{
		SaveGameObject->PlayerSettings = LoadPlayerSettings();
		SaveGameObject->PlayerSettings.AudioAnalyzer = InAudioAnalyzerSettings;
		UGameplayStatics::SaveGameToSlot(SaveGameObject, TEXT("SettingsSlot"), 0);
	}
	OnPlayerSettingsChangedDelegate_AudioAnalyzer.Broadcast(InAudioAnalyzerSettings);
}

void ISaveLoadInterface::SavePlayerSettings(const FPlayerSettings_User& InUserSettings)
{
	if (USaveGamePlayerSettings* SaveGameObject = Cast<USaveGamePlayerSettings>(UGameplayStatics::CreateSaveGameObject(USaveGamePlayerSettings::StaticClass())))
	{
		SaveGameObject->PlayerSettings = LoadPlayerSettings();
		SaveGameObject->PlayerSettings.User = InUserSettings;
		UGameplayStatics::SaveGameToSlot(SaveGameObject, TEXT("SettingsSlot"), 0);
	}
	OnPlayerSettingsChangedDelegate_User.Broadcast(InUserSettings);
}

void ISaveLoadInterface::SavePlayerSettings(const FPlayerSettings_CrossHair& InCrossHairSettings)
{
	if (USaveGamePlayerSettings* SaveGameObject = Cast<USaveGamePlayerSettings>(UGameplayStatics::CreateSaveGameObject(USaveGamePlayerSettings::StaticClass())))
	{
		SaveGameObject->PlayerSettings = LoadPlayerSettings();
		SaveGameObject->PlayerSettings.CrossHair = InCrossHairSettings;
		UGameplayStatics::SaveGameToSlot(SaveGameObject, TEXT("SettingsSlot"), 0);
	}
	OnPlayerSettingsChangedDelegate_CrossHair.Broadcast(InCrossHairSettings);
}

void ISaveLoadInterface::SavePlayerSettings(const FPlayerSettings_VideoAndSound& InVideoAndSoundSettings)
{
	if (USaveGamePlayerSettings* SaveGameObject = Cast<USaveGamePlayerSettings>(UGameplayStatics::CreateSaveGameObject(USaveGamePlayerSettings::StaticClass())))
	{
		SaveGameObject->PlayerSettings = LoadPlayerSettings();
		SaveGameObject->PlayerSettings.VideoAndSound = InVideoAndSoundSettings;
		UGameplayStatics::SaveGameToSlot(SaveGameObject, TEXT("SettingsSlot"), 0);
	}
	OnPlayerSettingsChangedDelegate_VideoAndSound.Broadcast(InVideoAndSoundSettings);
}

void ISaveLoadInterface::SavePlayerSettings(const FPlayerSettings& InPlayerSettings)
{
	if (USaveGamePlayerSettings* SaveGameObject = Cast<USaveGamePlayerSettings>(UGameplayStatics::CreateSaveGameObject(USaveGamePlayerSettings::StaticClass())))
	{
		SaveGameObject->PlayerSettings = InPlayerSettings;
		UGameplayStatics::SaveGameToSlot(SaveGameObject, TEXT("SettingsSlot"), 0);
	}
}

TArray<FBSConfig> ISaveLoadInterface::LoadCustomGameModes() const
{
	if (UGameplayStatics::DoesSaveGameExist(TEXT("CustomGameModesSlot"), 3))
	{
		if (USaveGameCustomGameMode* SaveGameCustomGameMode = Cast<USaveGameCustomGameMode>(UGameplayStatics::LoadGameFromSlot(TEXT("CustomGameModesSlot"), 3)))
		{
			return SaveGameCustomGameMode->CustomGameModes;
		}
	}
	else
	{
		if (USaveGameCustomGameMode* SaveGameCustomGameMode = Cast<USaveGameCustomGameMode>(UGameplayStatics::CreateSaveGameObject(USaveGameCustomGameMode::StaticClass())))
		{
			return SaveGameCustomGameMode->CustomGameModes;
		}
	}
	return TArray<FBSConfig>();
}

void ISaveLoadInterface::SaveCustomGameMode(const FBSConfig& ConfigToSave)
{
	TArray<FBSConfig> CustomGameModesArray = LoadCustomGameModes();
	if (const int32 Index = CustomGameModesArray.Find(ConfigToSave); Index != INDEX_NONE)
	{
		CustomGameModesArray[Index] = ConfigToSave;
	}
	else
	{
		CustomGameModesArray.Add(ConfigToSave);
	}
	if (USaveGameCustomGameMode* SaveCustomGameModeObject = Cast<USaveGameCustomGameMode>(UGameplayStatics::CreateSaveGameObject(USaveGameCustomGameMode::StaticClass())))
	{
		SaveCustomGameModeObject->CustomGameModes = CustomGameModesArray;
		UGameplayStatics::SaveGameToSlot(SaveCustomGameModeObject, TEXT("CustomGameModesSlot"), 3);
	}
}

int32 ISaveLoadInterface::RemoveCustomGameMode(const FBSConfig& ConfigToRemove)
{
	TArray<FBSConfig> CustomGameModesArray = LoadCustomGameModes();
	const int32 NumRemoved = CustomGameModesArray.Remove(ConfigToRemove);
	CustomGameModesArray.Shrink();
	if (USaveGameCustomGameMode* SaveCustomGameModeObject = Cast<USaveGameCustomGameMode>(UGameplayStatics::CreateSaveGameObject(USaveGameCustomGameMode::StaticClass())))
	{
		SaveCustomGameModeObject->CustomGameModes = CustomGameModesArray;
		UGameplayStatics::SaveGameToSlot(SaveCustomGameModeObject, TEXT("CustomGameModesSlot"), 3);
	}
	return NumRemoved;
}

void ISaveLoadInterface::RemoveAllCustomGameModes()
{
	const TArray<FBSConfig> Empty = TArray<FBSConfig>();
	if (USaveGameCustomGameMode* SaveCustomGameModeObject = Cast<USaveGameCustomGameMode>(UGameplayStatics::CreateSaveGameObject(USaveGameCustomGameMode::StaticClass())))
	{
		SaveCustomGameModeObject->CustomGameModes = Empty;
		UGameplayStatics::SaveGameToSlot(SaveCustomGameModeObject, TEXT("CustomGameModesSlot"), 3);
	}
}

FBSConfig ISaveLoadInterface::FindPresetGameMode(const FString& GameModeName, const EGameModeDifficulty& Difficulty) const
{
	EBaseGameMode BaseGameMode = EBaseGameMode::None;
	
	for (const EBaseGameMode& Preset : TEnumRange<EBaseGameMode>())
	{
		if (GameModeName.Equals(UEnum::GetDisplayValueAsText(Preset).ToString(), ESearchCase::IgnoreCase))
		{
			BaseGameMode = Preset;
			break;
		}
	}

	if (BaseGameMode != EBaseGameMode::None)
	{
		const FBS_DefiningConfig DefiningConfig = FBSConfig::GetConfigForPreset(BaseGameMode, Difficulty);
		TMap<FBS_DefiningConfig, FBSConfig> Map = GetGameModeDataAsset()->GetDefaultGameModesMap();
		FBSConfig* Found = Map.Find(DefiningConfig);
		if (Found)
		{
			return *Found;
		}
	}
	
	return FBSConfig();
}

FBSConfig ISaveLoadInterface::FindPresetGameMode(const EBaseGameMode& BaseGameMode, const EGameModeDifficulty& Difficulty) const
{
	if (BaseGameMode != EBaseGameMode::None)
	{
		const FBS_DefiningConfig DefiningConfig = FBSConfig::GetConfigForPreset(BaseGameMode, Difficulty);
		TMap<FBS_DefiningConfig, FBSConfig> Map = GetGameModeDataAsset()->GetDefaultGameModesMap();
		FBSConfig* Found = Map.Find(DefiningConfig);
		if (Found)
		{
			return *Found;
		}
	}
	
	return FBSConfig();
}

FBSConfig ISaveLoadInterface::FindCustomGameMode(const FString& CustomGameModeName) const
{
	for (const FBSConfig& Mode : LoadCustomGameModes())
	{
		if (Mode.DefiningConfig.CustomGameModeName.Equals(CustomGameModeName))
		{
			return Mode;
		}
	}
	return FBSConfig();
}

bool ISaveLoadInterface::IsPresetGameMode(const FString& GameModeName) const
{
	for (const EBaseGameMode& Preset : TEnumRange<EBaseGameMode>())
	{
		if (GameModeName.Equals(UEnum::GetDisplayValueAsText(Preset).ToString(), ESearchCase::IgnoreCase))
		{
			return true;
		}
	}
	return false;
}

bool ISaveLoadInterface::IsCustomGameMode(const FString& GameModeName) const
{
	for (const FBSConfig& GameMode : LoadCustomGameModes())
	{
		if (GameMode.DefiningConfig.CustomGameModeName.Equals(GameModeName, ESearchCase::IgnoreCase))
		{
			return true;
		}
	}
	return false;
}

TArray<FPlayerScore> ISaveLoadInterface::LoadPlayerScores() const
{
	if (UGameplayStatics::DoesSaveGameExist(TEXT("ScoreSlot"), 1))
	{
		if (USaveGamePlayerScore* SaveGamePlayerScore = Cast<USaveGamePlayerScore>(UGameplayStatics::LoadGameFromSlot(TEXT("ScoreSlot"), 1)))
		{
			return SaveGamePlayerScore->PlayerScoreArray;
		}
	}
	else
	{
		if (USaveGamePlayerScore* SaveGamePlayerScore = Cast<USaveGamePlayerScore>(UGameplayStatics::CreateSaveGameObject(USaveGamePlayerScore::StaticClass())))
		{
			return SaveGamePlayerScore->PlayerScoreArray;
		}
	}
	return TArray<FPlayerScore>();
}

void ISaveLoadInterface::SavePlayerScores(const TArray<FPlayerScore>& PlayerScoreArrayToSave)
{
	if (USaveGamePlayerScore* SaveGamePlayerScores = Cast<USaveGamePlayerScore>(UGameplayStatics::CreateSaveGameObject(USaveGamePlayerScore::StaticClass())))
	{
		SaveGamePlayerScores->PlayerScoreArray = PlayerScoreArrayToSave;
		if (UGameplayStatics::SaveGameToSlot(SaveGamePlayerScores, TEXT("ScoreSlot"), 1))
		{
			UE_LOG(LogTemp, Warning, TEXT("SavePlayerScores Succeeded"));
		}
	}
}

TArray<FPlayerScore> ISaveLoadInterface::GetMatchingPlayerScores(const FPlayerScore& PlayerScore) const
{
	return LoadPlayerScores().FilterByPredicate([&PlayerScore] (const FPlayerScore& ComparePlayerScore) {
		if (ComparePlayerScore == PlayerScore)
		{
			return true;
		}
		return false;
	});
}

TMap<FBS_DefiningConfig, FCommonScoreInfo> ISaveLoadInterface::LoadCommonScoreInfo() const
{
	if (UGameplayStatics::DoesSaveGameExist(TEXT("ScoreSlot"), 1))
	{
		if (USaveGamePlayerScore* SaveGamePlayerScore = Cast<USaveGamePlayerScore>(UGameplayStatics::LoadGameFromSlot(TEXT("ScoreSlot"), 1)))
		{
			return SaveGamePlayerScore->CommonScoreInfo;
		}
	}
	else
	{
		if (USaveGamePlayerScore* SaveGamePlayerScore = Cast<USaveGamePlayerScore>(UGameplayStatics::CreateSaveGameObject(USaveGamePlayerScore::StaticClass())))
		{
			return SaveGamePlayerScore->CommonScoreInfo;
		}
	}
	return TMap<FBS_DefiningConfig, FCommonScoreInfo>();
}

FCommonScoreInfo ISaveLoadInterface::GetScoreInfoFromDefiningConfig(const FBS_DefiningConfig& DefiningConfig) const
{
	TMap<FBS_DefiningConfig, FCommonScoreInfo> Map = LoadCommonScoreInfo();
	if (Map.Find(DefiningConfig) == nullptr)
	{
		return Map.Add(DefiningConfig);
	}
	return *Map.Find(DefiningConfig);
}

void ISaveLoadInterface::SaveCommonScoreInfo(const FBS_DefiningConfig& DefiningConfig, const FCommonScoreInfo& CommonScoreInfoToSave)
{
	TMap<FBS_DefiningConfig, FCommonScoreInfo> Map = LoadCommonScoreInfo();
	FCommonScoreInfo FoundOrAddedScoreInfo = Map.FindOrAdd(DefiningConfig);
	FoundOrAddedScoreInfo.UpdateCommonValues(CommonScoreInfoToSave.TotalHits, CommonScoreInfoToSave.TotalSpawns);
	UpdateCommonScoreInfo(Map);
}

void ISaveLoadInterface::UpdateCommonScoreInfo(const TMap<FBS_DefiningConfig, FCommonScoreInfo>& MapToSave) const
{
	if (USaveGamePlayerScore* SaveGamePlayerScores = Cast<USaveGamePlayerScore>(UGameplayStatics::CreateSaveGameObject(USaveGamePlayerScore::StaticClass())))
	{
		SaveGamePlayerScores->CommonScoreInfo = MapToSave;
		if (UGameplayStatics::SaveGameToSlot(SaveGamePlayerScores, TEXT("ScoreSlot"), 1))
		{
			UE_LOG(LogTemp, Warning, TEXT("UpdateCommonScoreInfo Succeeded"));
		}
	}
}
