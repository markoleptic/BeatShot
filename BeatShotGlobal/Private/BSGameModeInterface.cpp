// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "BSGameModeInterface.h"
#include "BSGameModeConfig/BSConfig.h"
#include "BSGameModeConfig/BSGameModeDataAsset.h"
#include "SaveGames/SaveGameCustomGameMode.h"
#include "SaveGames/SaveGamePlayerScore.h"
#include "Utilities/SaveLoadCommon.h"

/* --------------------------- */
/* ---- Custom Game Modes ---- */
/* --------------------------- */

TArray<FBSConfig> IBSGameModeInterface::LoadCustomGameModes()
{
	if (const USaveGameCustomGameMode* SaveGameCustomGameMode = SaveLoadCommon::LoadFromSlot<USaveGameCustomGameMode>(
		TEXT("CustomGameModesSlot"), 3))
	{
		return SaveGameCustomGameMode->GetCustomGameModes();
	}
	return TArray<FBSConfig>();
}

bool IBSGameModeInterface::FindCustomGameMode(const FString& CustomGameModeName, FBSConfig& OutConfig)
{
	if (const USaveGameCustomGameMode* SaveGameCustomGameMode = SaveLoadCommon::LoadFromSlot<USaveGameCustomGameMode>(
		TEXT("CustomGameModesSlot"), 3))
	{
		return SaveGameCustomGameMode->FindCustomGameMode(CustomGameModeName, OutConfig);
	}
	return false;
}

void IBSGameModeInterface::SaveCustomGameMode(const FBSConfig& ConfigToSave)
{
	if (USaveGameCustomGameMode* SaveGameCustomGameMode = SaveLoadCommon::LoadFromSlot<USaveGameCustomGameMode>(
		TEXT("CustomGameModesSlot"), 3))
	{
		SaveGameCustomGameMode->SaveCustomGameMode(ConfigToSave);
		SaveLoadCommon::SaveToSlot(SaveGameCustomGameMode, TEXT("CustomGameModesSlot"), 3);
	}
}

int32 IBSGameModeInterface::RemoveCustomGameMode(const FBSConfig& ConfigToRemove,
                                                 USaveGamePlayerScore* SaveGamePlayerScore)
{
	int32 NumScoresRemoved = 0;
	if (USaveGameCustomGameMode* SaveGameCustomGameMode = SaveLoadCommon::LoadFromSlot<USaveGameCustomGameMode>(
		TEXT("CustomGameModesSlot"), 3))
	{
		SaveGameCustomGameMode->RemoveCustomGameMode(ConfigToRemove);
		SaveLoadCommon::SaveToSlot(SaveGameCustomGameMode, TEXT("CustomGameModesSlot"), 3);
	}
	if (SaveGamePlayerScore)
	{
		SaveGamePlayerScore->RemoveCommonScoreInfoAndMatchingPlayerScores(ConfigToRemove.DefiningConfig);
	}
	return NumScoresRemoved;
}

FRemoveAllCustomGameModesResult IBSGameModeInterface::RemoveAllCustomGameModes(
	USaveGamePlayerScore* SaveGamePlayerScore)
{
	FRemoveAllCustomGameModesResult RemoveAllCustomGameModesResult;
	if (USaveGameCustomGameMode* SaveGameCustomGameMode = SaveLoadCommon::LoadFromSlot<USaveGameCustomGameMode>(
		TEXT("CustomGameModesSlot"), 3))
	{
		RemoveAllCustomGameModesResult.NumCustomGameModesRemoved = SaveGameCustomGameMode->RemoveAll();
		SaveLoadCommon::SaveToSlot(SaveGameCustomGameMode, TEXT("CustomGameModesSlot"), 3);
	}
	if (SaveGamePlayerScore)
	{
		RemoveAllCustomGameModesResult.NumScoresRemoved = SaveGamePlayerScore->
			RemoveAllCommonScoreInfoAndMatchingPlayerScores();
	}
	return RemoveAllCustomGameModesResult;
}

bool IBSGameModeInterface::IsCustomGameMode(const FString& GameModeName)
{
	if (const USaveGameCustomGameMode* SaveGameCustomGameMode = SaveLoadCommon::LoadFromSlot<USaveGameCustomGameMode>(
		TEXT("CustomGameModesSlot"), 3))
	{
		return SaveGameCustomGameMode->IsCustomGameMode(GameModeName);
	}
	return false;
}

bool IBSGameModeInterface::DoesCustomGameModeMatchConfig(const FString& CustomGameModeName, const FBSConfig& InConfig)
{
	if (!IsCustomGameMode(CustomGameModeName))
	{
		return false;
	}
	FBSConfig FoundCustom;
	if (!FindCustomGameMode(CustomGameModeName, FoundCustom))
	{
		return false;
	}
	if (FoundCustom.AIConfig == InConfig.AIConfig && FoundCustom.GridConfig == InConfig.GridConfig && FoundCustom.
	    TargetConfig == InConfig.TargetConfig && FoundCustom.DynamicTargetScaling == InConfig.DynamicTargetScaling &&
	    FoundCustom.DynamicSpawnAreaScaling == InConfig.DynamicSpawnAreaScaling)
	{
		return true;
	}
	return false;
}

ECustomGameModeImportResult IBSGameModeInterface::ImportCustomGameMode(const FString& InSerializedJsonString,
                                                                       FBSConfig& OutConfig,
                                                                       FText& OutDecodeFailureReason)
{
	if (!FBSConfig::DecodeFromString(InSerializedJsonString, OutConfig, &OutDecodeFailureReason))
	{
		return ECustomGameModeImportResult::InvalidImportString;
	}

	if (IsPresetGameMode(OutConfig.DefiningConfig.CustomGameModeName) || OutConfig.DefiningConfig.GameModeType ==
	    EGameModeType::Preset)
	{
		return ECustomGameModeImportResult::DefaultGameMode;
	}

	if (OutConfig.DefiningConfig.CustomGameModeName.IsEmpty())
	{
		return ECustomGameModeImportResult::EmptyCustomGameModeName;
	}

	if (IsCustomGameMode(OutConfig.DefiningConfig.CustomGameModeName))
	{
		return ECustomGameModeImportResult::Existing;
	}
	return ECustomGameModeImportResult::Success;
}

FString IBSGameModeInterface::ExportCustomGameMode(const FBSConfig& InConfig)
{
	return InConfig.EncodeToString();
}

/* --------------------------- */
/* ---- Preset Game Modes ---- */
/* --------------------------- */

bool IBSGameModeInterface::FindPresetGameMode(const FString& GameModeName,
                                              const EGameModeDifficulty& Difficulty,
                                              const UBSGameModeDataAsset* PresetGameModeDataAsset,
                                              FBSConfig& OutConfig)
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

	if (BaseGameMode == EBaseGameMode::None)
	{
		return false;
	}

	return FindPresetGameMode(BaseGameMode, Difficulty, PresetGameModeDataAsset, OutConfig);
}

bool IBSGameModeInterface::FindPresetGameMode(const EBaseGameMode& BaseGameMode,
                                              const EGameModeDifficulty& Difficulty,
                                              const UBSGameModeDataAsset* PresetGameModeDataAsset,
                                              FBSConfig& OutConfig)
{
	if (BaseGameMode != EBaseGameMode::None)
	{
		const FBS_DefiningConfig DefiningConfig = FBSConfig::GetConfigForPreset(BaseGameMode, Difficulty);
		const TMap<FBS_DefiningConfig, FBSConfig> Map = PresetGameModeDataAsset->GetGameModesMap();
		if (const FBSConfig* Found = Map.Find(DefiningConfig))
		{
			OutConfig = *Found;
			return true;
		}
	}

	return false;
}

bool IBSGameModeInterface::IsPresetGameMode(const FString& GameModeName)
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
