// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#include "SaveLoadInterface.h"
#include "BSGameModeDataAsset.h"
#include "SaveGameCustomGameMode.h"
#include "SaveGamePlayerScore.h"
#include "SaveGamePlayerSettings.h"
#include "Kismet/GameplayStatics.h"

USaveGamePlayerScore* ISaveLoadInterface::LoadFromSlot_SaveGamePlayerScore()
{
	return LoadFromSlot<USaveGamePlayerScore>("ScoreSlot", 1);
	/*if (UGameplayStatics::DoesSaveGameExist(TEXT("ScoreSlot"), 1))
	{
		if (USaveGamePlayerScore* SaveGamePlayerScore = Cast<USaveGamePlayerScore>(UGameplayStatics::LoadGameFromSlot(TEXT("ScoreSlot"), 1)))
		{
			return SaveGamePlayerScore;
		}
	}
	else
	{
		if (USaveGamePlayerScore* SaveGamePlayerScore = Cast<USaveGamePlayerScore>(UGameplayStatics::CreateSaveGameObject(USaveGamePlayerScore::StaticClass())))
		{
			return SaveGamePlayerScore;
		}
	}
	return nullptr;*/
}

USaveGameCustomGameMode* ISaveLoadInterface::LoadFromSlot_SaveGameCustomGameMode()
{
	return LoadFromSlot<USaveGameCustomGameMode>("CustomGameModesSlot", 3);
	/*if (UGameplayStatics::DoesSaveGameExist(TEXT("CustomGameModesSlot"), 3))
	{
		if (USaveGameCustomGameMode* SaveGameCustomGameMode = Cast<USaveGameCustomGameMode>(UGameplayStatics::LoadGameFromSlot(TEXT("CustomGameModesSlot"), 3)))
		{
			return SaveGameCustomGameMode;
		}
	}
	else
	{
		if (USaveGameCustomGameMode* SaveGameCustomGameMode = Cast<USaveGameCustomGameMode>(UGameplayStatics::CreateSaveGameObject(USaveGameCustomGameMode::StaticClass())))
		{
			return SaveGameCustomGameMode;
		}
	}
	return nullptr;*/
}

USaveGamePlayerSettings* ISaveLoadInterface::LoadFromSlot_SaveGamePlayerSettings()
{
	return LoadFromSlot<USaveGamePlayerSettings>("SettingsSlot", 0);
	/*if (UGameplayStatics::DoesSaveGameExist(TEXT("SettingsSlot"), 0))
	{
		if (USaveGamePlayerSettings* SaveGamePlayerSettings = Cast<USaveGamePlayerSettings>(UGameplayStatics::LoadGameFromSlot(TEXT("SettingsSlot"), 0)))
		{
			return SaveGamePlayerSettings;
		}
	}
	else
	{
		if (USaveGamePlayerSettings* SaveGamePlayerSettings = Cast<USaveGamePlayerSettings>(UGameplayStatics::CreateSaveGameObject(USaveGamePlayerSettings::StaticClass())))
		{
			return SaveGamePlayerSettings;
		}
	}
	return nullptr;*/
}

bool ISaveLoadInterface::SaveToSlot(USaveGamePlayerSettings* SaveGamePlayerSettings)
{
	if (UGameplayStatics::SaveGameToSlot(SaveGamePlayerSettings, TEXT("SettingsSlot"), 0))
	{
		UE_LOG(LogTemp, Warning, TEXT("SavePlayerSettings Succeeded"));
		return true;
	}
	return false;
}

bool ISaveLoadInterface::SaveToSlot(USaveGamePlayerScore* SaveGamePlayerScore)
{
	if (UGameplayStatics::SaveGameToSlot(SaveGamePlayerScore, TEXT("ScoreSlot"), 1))
	{
		UE_LOG(LogTemp, Warning, TEXT("SavePlayerScores Succeeded"));
		return true;
	}
	return false;
}

bool ISaveLoadInterface::SaveToSlot(USaveGameCustomGameMode* SaveGameCustomGameMode)
{
	if (UGameplayStatics::SaveGameToSlot(SaveGameCustomGameMode, TEXT("CustomGameModesSlot"), 3))
	{
		UE_LOG(LogTemp, Warning, TEXT("SaveGameCustomGameModes Succeeded"));
		return true;
	}
	return false;
}

/* --------------------------- */
/* ----- Player Settings------ */
/* --------------------------- */

FPlayerSettings ISaveLoadInterface::LoadPlayerSettings()
{
	if (const USaveGamePlayerSettings* SaveGamePlayerSettings = LoadFromSlot_SaveGamePlayerSettings())
	{
		return SaveGamePlayerSettings->GetPlayerSettings();
	}
	return FPlayerSettings();
}

void ISaveLoadInterface::SavePlayerSettings(const FPlayerSettings_Game& InGameSettings)
{
	if (USaveGamePlayerSettings* SaveGamePlayerSettings = LoadFromSlot_SaveGamePlayerSettings())
	{
		SaveGamePlayerSettings->SavePlayerSettings(InGameSettings);
		SaveToSlot(SaveGamePlayerSettings);
		OnPlayerSettingsChangedDelegate_Game.Broadcast(InGameSettings);
	}
}

void ISaveLoadInterface::SavePlayerSettings(const FPlayerSettings_AudioAnalyzer& InAudioAnalyzerSettings)
{
	if (USaveGamePlayerSettings* SaveGamePlayerSettings = LoadFromSlot_SaveGamePlayerSettings())
	{
		SaveGamePlayerSettings->SavePlayerSettings(InAudioAnalyzerSettings);
		SaveToSlot(SaveGamePlayerSettings);
		OnPlayerSettingsChangedDelegate_AudioAnalyzer.Broadcast(InAudioAnalyzerSettings);
	}
}

void ISaveLoadInterface::SavePlayerSettings(const FPlayerSettings_User& InUserSettings)
{
	if (USaveGamePlayerSettings* SaveGamePlayerSettings = LoadFromSlot_SaveGamePlayerSettings())
	{
		SaveGamePlayerSettings->SavePlayerSettings(InUserSettings);
		SaveToSlot(SaveGamePlayerSettings);
		OnPlayerSettingsChangedDelegate_User.Broadcast(InUserSettings);
	}
}

void ISaveLoadInterface::SavePlayerSettings(const FPlayerSettings_CrossHair& InCrossHairSettings)
{
	if (USaveGamePlayerSettings* SaveGamePlayerSettings = LoadFromSlot_SaveGamePlayerSettings())
	{
		SaveGamePlayerSettings->SavePlayerSettings(InCrossHairSettings);
		SaveToSlot(SaveGamePlayerSettings);
		OnPlayerSettingsChangedDelegate_CrossHair.Broadcast(InCrossHairSettings);
	}
}

void ISaveLoadInterface::SavePlayerSettings(const FPlayerSettings_VideoAndSound& InVideoAndSoundSettings)
{
	if (USaveGamePlayerSettings* SaveGamePlayerSettings = LoadFromSlot_SaveGamePlayerSettings())
	{
		SaveGamePlayerSettings->SavePlayerSettings(InVideoAndSoundSettings);
		SaveToSlot(SaveGamePlayerSettings);
		OnPlayerSettingsChangedDelegate_VideoAndSound.Broadcast(InVideoAndSoundSettings);
	}
}

/* --------------------------- */
/* ---- Custom Game Modes ---- */
/* --------------------------- */

TArray<FBSConfig> ISaveLoadInterface::LoadCustomGameModes()
{
	if (const USaveGameCustomGameMode* SaveGameCustomGameMode = LoadFromSlot_SaveGameCustomGameMode())
	{
		return SaveGameCustomGameMode->GetCustomGameModes();
	}
	return TArray<FBSConfig>();
}

FBSConfig ISaveLoadInterface::FindCustomGameMode(const FString& CustomGameModeName)
{
	if (const USaveGameCustomGameMode* SaveGameCustomGameMode = LoadFromSlot_SaveGameCustomGameMode())
	{
		return SaveGameCustomGameMode->FindCustomGameMode(CustomGameModeName);
	}
	return FBSConfig();
}

void ISaveLoadInterface::SaveCustomGameMode(const FBSConfig& ConfigToSave)
{
	if (USaveGameCustomGameMode* SaveGameCustomGameMode = LoadFromSlot_SaveGameCustomGameMode())
	{
		SaveGameCustomGameMode->SaveCustomGameMode(ConfigToSave);
		SaveToSlot(SaveGameCustomGameMode);
	}
}

int32 ISaveLoadInterface::RemoveCustomGameMode(const FBSConfig& ConfigToRemove)
{
	int32 NumCustomGameModesRemoved = 0;
	if (USaveGameCustomGameMode* SaveGameCustomGameMode = LoadFromSlot_SaveGameCustomGameMode())
	{
		NumCustomGameModesRemoved = SaveGameCustomGameMode->RemoveCustomGameMode(ConfigToRemove);
		SaveToSlot(SaveGameCustomGameMode);
	}
	if (USaveGamePlayerScore* SaveGamePlayerScore = LoadFromSlot_SaveGamePlayerScore())
	{
		const int32 NumCommonScoreInfosRemoved = SaveGamePlayerScore->RemoveCommonScoreInfo(ConfigToRemove.DefiningConfig);
		UE_LOG(LogTemp, Display, TEXT("%d Common Score Infos removed when removing a custom game mode."), NumCommonScoreInfosRemoved);
		SaveToSlot(SaveGamePlayerScore);
	}
	return NumCustomGameModesRemoved;
}

int32 ISaveLoadInterface::RemoveAllCustomGameModes()
{
	int32 NumCustomGameModesRemoved = 0;
	if (USaveGameCustomGameMode* SaveGameCustomGameMode = LoadFromSlot_SaveGameCustomGameMode())
	{
		NumCustomGameModesRemoved = SaveGameCustomGameMode->RemoveAll();
		SaveToSlot(SaveGameCustomGameMode);
	}
	if (USaveGamePlayerScore* SaveGamePlayerScore = LoadFromSlot_SaveGamePlayerScore())
	{
		const int32 NumCommonScoreInfosRemoved = SaveGamePlayerScore->RemoveAllCustomGameModeCommonScoreInfo();
		UE_LOG(LogTemp, Display, TEXT("%d Common Score Infos removed when removing all custom game modes."), NumCommonScoreInfosRemoved);
		SaveToSlot(SaveGamePlayerScore);
	}
	return NumCustomGameModesRemoved;
}

bool ISaveLoadInterface::IsCustomGameMode(const FString& GameModeName)
{
	if (const USaveGameCustomGameMode* SaveGameCustomGameMode = LoadFromSlot_SaveGameCustomGameMode())
	{
		return SaveGameCustomGameMode->IsCustomGameMode(GameModeName);
	}
	return false;
}

bool ISaveLoadInterface::DoesCustomGameModeMatchConfig(const FString& CustomGameModeName, const FBSConfig& InConfig)
{
	if (!IsCustomGameMode(CustomGameModeName))
	{
		return false;
	}
	const FBSConfig CustomMode = FindCustomGameMode(CustomGameModeName);
	if (CustomMode.DefiningConfig.CustomGameModeName != CustomGameModeName)
	{
		return false;
	}
	if (CustomMode.AIConfig == InConfig.AIConfig && CustomMode.GridConfig == InConfig.GridConfig && CustomMode.
		TargetConfig == InConfig.TargetConfig && CustomMode.DynamicTargetScaling == InConfig.DynamicTargetScaling &&
		CustomMode.DynamicSpawnAreaScaling == InConfig.DynamicSpawnAreaScaling)
	{
		return true;
	}
	return false;
}

FBSConfig ISaveLoadInterface::ImportCustomGameMode(const FString& InImportString)
{
	FBSConfig OutConfig = FBSConfig();

	if (!InImportString.Contains("DefiningConfig") || !InImportString.Contains("TargetConfig"))
	{
		return OutConfig;
	}

	UScriptStruct* Struct = FBSConfig::StaticStruct();
	const FString StringCopy = FString(InImportString);
	const TCHAR* Result = Struct->ImportText(*StringCopy, &OutConfig, nullptr,
		(PPF_ExportsNotFullyQualified | PPF_Copy | PPF_Delimited | PPF_IncludeTransient), nullptr, "FBSConfig");
	if (!Result)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to import custom game mode"));
	}
	return OutConfig;
}

FString ISaveLoadInterface::ExportCustomGameMode(const FBSConfig& InGameMode)
{
	UScriptStruct* Struct = FBSConfig::StaticStruct();
	FString Output = TEXT("");
	Struct->ExportText(Output, &InGameMode, nullptr, nullptr,
		(PPF_ExportsNotFullyQualified | PPF_Copy | PPF_Delimited | PPF_IncludeTransient), nullptr);
	return Output;
}

/* --------------------------- */
/* ---- Preset Game Modes ---- */
/* --------------------------- */

FBSConfig ISaveLoadInterface::FindPresetGameMode(const FString& GameModeName,
	const EGameModeDifficulty& Difficulty) const
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
		const TMap<FBS_DefiningConfig, FBSConfig> Map = GetGameModeDataAsset()->GetDefaultGameModesMap();
		const FBSConfig* Found = Map.Find(DefiningConfig);
		if (Found)
		{
			return *Found;
		}
	}
	return FBSConfig();
}

FBSConfig ISaveLoadInterface::FindPresetGameMode(const EBaseGameMode& BaseGameMode,
	const EGameModeDifficulty& Difficulty) const
{
	if (BaseGameMode != EBaseGameMode::None)
	{
		const FBS_DefiningConfig DefiningConfig = FBSConfig::GetConfigForPreset(BaseGameMode, Difficulty);
		const TMap<FBS_DefiningConfig, FBSConfig> Map = GetGameModeDataAsset()->GetDefaultGameModesMap();
		const FBSConfig* Found = Map.Find(DefiningConfig);
		if (Found)
		{
			return *Found;
		}
	}

	return FBSConfig();
}

bool ISaveLoadInterface::IsPresetGameMode(const FString& GameModeName)
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

/* --------------------------- */
/* --------- Scoring --------- */
/* --------------------------- */

TArray<FPlayerScore> ISaveLoadInterface::LoadPlayerScores()
{
	if (const USaveGamePlayerScore* SaveGamePlayerScore = LoadFromSlot_SaveGamePlayerScore())
	{
		return SaveGamePlayerScore->GetPlayerScores();
	}
	return TArray<FPlayerScore>();
}

TArray<FPlayerScore> ISaveLoadInterface::LoadPlayerScores_UnsavedToDatabase()
{
	if (const USaveGamePlayerScore* SaveGamePlayerScore = LoadFromSlot_SaveGamePlayerScore())
	{
		return SaveGamePlayerScore->GetPlayerScores_UnsavedToDatabase();
	}
	return TArray<FPlayerScore>();
}

void ISaveLoadInterface::SetAllPlayerScoresSavedToDatabase()
{
	if (USaveGamePlayerScore* SaveGamePlayerScore = LoadFromSlot_SaveGamePlayerScore())
	{
		SaveGamePlayerScore->SetAllScoresSavedToDatabase();
		SaveToSlot(SaveGamePlayerScore);
	}
}

TArray<FPlayerScore> ISaveLoadInterface::GetMatchingPlayerScores(const FPlayerScore& PlayerScore)
{
	return LoadPlayerScores().FilterByPredicate([&PlayerScore](const FPlayerScore& ComparePlayerScore)
	{
		if (ComparePlayerScore == PlayerScore)
		{
			return true;
		}
		return false;
	});
}

void ISaveLoadInterface::SavePlayerScoreInstance(const FPlayerScore& PlayerScoreToSave)
{
	if (USaveGamePlayerScore* SaveGamePlayerScore = LoadFromSlot_SaveGamePlayerScore())
	{
		SaveGamePlayerScore->AddPlayerScoreInstance(PlayerScoreToSave);
		SaveToSlot(SaveGamePlayerScore);
	}
}

/* --------------------------- */
/* ---- Common Score Info ---- */
/* --------------------------- */

TMap<FBS_DefiningConfig, FCommonScoreInfo> ISaveLoadInterface::LoadCommonScoreInfoMap()
{
	if (const USaveGamePlayerScore* SaveGamePlayerScore = LoadFromSlot_SaveGamePlayerScore())
	{
		return SaveGamePlayerScore->GetCommonScoreInfo();
	}
	return TMap<FBS_DefiningConfig, FCommonScoreInfo>();
}

FCommonScoreInfo ISaveLoadInterface::FindCommonScoreInfo(const FBS_DefiningConfig& DefiningConfig)
{
	if (USaveGamePlayerScore* SaveGamePlayerScore = LoadFromSlot_SaveGamePlayerScore())
	{
		return SaveGamePlayerScore->FindCommonScoreInfo(DefiningConfig);
	}
	return FCommonScoreInfo();
}

void ISaveLoadInterface::SaveCommonScoreInfo(const FBS_DefiningConfig& DefiningConfig,
	const FCommonScoreInfo& CommonScoreInfoToSave)
{
	if (USaveGamePlayerScore* SaveGamePlayerScore = LoadFromSlot_SaveGamePlayerScore())
	{
		SaveGamePlayerScore->FindOrAddCommonScoreInfo(DefiningConfig, CommonScoreInfoToSave);
		SaveToSlot(SaveGamePlayerScore);
	}
}

int32 ISaveLoadInterface::RemoveCommonScoreInfo(const FBS_DefiningConfig& DefiningConfig)
{
	if (USaveGamePlayerScore* SaveGamePlayerScore = LoadFromSlot_SaveGamePlayerScore())
	{
		const int32 NumRemoved = SaveGamePlayerScore->RemoveCommonScoreInfo(DefiningConfig);
		if (SaveToSlot(SaveGamePlayerScore)) return NumRemoved;
	}
	return 0;
}

int32 ISaveLoadInterface::ResetQTable(const FBS_DefiningConfig& DefiningConfig)
{
	if (USaveGamePlayerScore* SaveGamePlayerScore = LoadFromSlot_SaveGamePlayerScore())
	{
		const int32 NumCleared = SaveGamePlayerScore->ResetQTable(DefiningConfig);
		if (SaveToSlot(SaveGamePlayerScore)) return NumCleared;
	}
	return 0;
}
