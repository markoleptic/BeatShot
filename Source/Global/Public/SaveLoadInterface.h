// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "GlobalStructs.h"
#include "SaveLoadInterface.generated.h"

/** Broadcast when Player Settings are changed and saved */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerSettingsChange, const FPlayerSettings&, RefreshedPlayerSettings);

/** Broadcast when AudioAnalyzer settings are changed and saved */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAASettingsChange, const FAASettingsStruct&, RefreshedAASettings);

/** Broadcast from GameModesWidget, SettingsMenuWidget, PauseMenuWidget, and PostGameMenuWidget any time the game should start, restart, or stop */
DECLARE_MULTICAST_DELEGATE_OneParam(FOnGameModeStateChanged, const FGameModeTransitionState& TransitionState);

UINTERFACE()
class USaveLoadInterface : public UInterface
{
	GENERATED_BODY()
};

class GLOBAL_API ISaveLoadInterface
{
	GENERATED_BODY()

public:
	virtual FAASettingsStruct LoadAASettings() const;

	virtual void SaveAASettings(const FAASettingsStruct AASettingsToSave);

	virtual FPlayerSettings LoadPlayerSettings() const;

	virtual void SavePlayerSettings(const FPlayerSettings PlayerSettingsToSave);

	TArray<FBSConfig> LoadCustomGameModes() const;

	void SaveCustomGameMode(const TArray<FBSConfig> GameModeArrayToSave);

	TArray<FPlayerScore> LoadPlayerScores() const;

	void SavePlayerScores(const TArray<FPlayerScore> PlayerScoreArrayToSave);
	
	TArray<FPlayerScore> GetMatchingPlayerScores(const FPlayerScore& PlayerScore) const;

	TMap<FBS_DefiningConfig, FCommonScoreInfo> LoadCommonScoreInfo() const;

	void SaveCommonScoreInfo(const TMap<FBS_DefiningConfig, FCommonScoreInfo>& MapToSave);

	FCommonScoreInfo GetScoreInfoFromDefiningConfig(const FBS_DefiningConfig& DefiningConfig);

	/** Finds or Adds a DefiningConfig CommonScoreInfo pair and saves to slot */
	void UpdateCommonScoreInfo(const FBS_DefiningConfig& DefiningConfig, const FCommonScoreInfo& CommonScoreInfoToSave);
};
