// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "GlobalStructs.h"
#include "SaveLoadInterface.generated.h"

/** Broadcast when game specific settings are changed and saved */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerSettingsChanged_Game, const FPlayerSettings_Game&, GameSettings);

/** Broadcast when Audio Analyzer specific settings are changed and saved */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerSettingsChanged_AudioAnalyzer, const FPlayerSettings_AudioAnalyzer&, AudioAnalyzerSettings);

/** Broadcast when User specific settings are changed and saved */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerSettingsChanged_User, const FPlayerSettings_User&, UserSettings);

/** Broadcast when CrossHair specific settings are changed and saved */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerSettingsChanged_CrossHair, const FPlayerSettings_CrossHair&, CrossHairSettings);

/** Broadcast when VideoAndSound specific settings are changed and saved */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerSettingsChanged_VideoAndSound, const FPlayerSettings_VideoAndSound&, VideoAndSoundSettings);

/** Broadcast from GameModesWidget, SettingsMenuWidget, PauseMenuWidget, and PostGameMenuWidget any time the game should start, restart, or stop */
DECLARE_MULTICAST_DELEGATE_OneParam(FOnGameModeStateChanged, const FGameModeTransitionState& TransitionState);

/** Interface to allow all other classes in this game to query, load, and save all settings */
UINTERFACE()
class USaveLoadInterface : public UInterface
{
	GENERATED_BODY()
};

/** Interface to allow all other classes in this game to query, load, and save all settings */
class GLOBAL_API ISaveLoadInterface
{
	GENERATED_BODY()

public:
	
	/** Loads all player settings from slot */
	virtual FPlayerSettings LoadPlayerSettings() const;

	/** Saves Game specific settings, preserving all other settings */
	virtual void SavePlayerSettings(const FPlayerSettings_Game& InGameSettings);

	/** Saves Audio Analyzer specific settings, preserving all other settings */
	virtual void SavePlayerSettings(const FPlayerSettings_AudioAnalyzer& InAudioAnalyzerSettings);

	/** Saves User specific settings, preserving all other settings */
	virtual void SavePlayerSettings(const FPlayerSettings_User& InUserSettings);

	/** Saves CrossHair specific settings, preserving all other settings */
	virtual void SavePlayerSettings(const FPlayerSettings_CrossHair& InCrossHairSettings);

	/** Saves VideoAndSound settings, preserving all other settings */
	virtual void SavePlayerSettings(const FPlayerSettings_VideoAndSound& InVideoAndSoundSettings);
	
	/** Saves entire player settings, overwriting the previous player settings */
	virtual void SavePlayerSettings(const FPlayerSettings& InPlayerSettings);

	/** Loads all custom game modes from slot */
	TArray<FBSConfig> LoadCustomGameModes() const;

	/** Saves a custom game mode to slot, checking if any matching exist and overriding if they do */
	virtual void SaveCustomGameMode(const FBSConfig& ConfigToSave);

	/** Removes a custom game mode and saves to slot */
	virtual int32 RemoveCustomGameMode(const FBSConfig& ConfigToRemove);

	/** Removes all custom game modes and saves to slot */
	virtual void RemoveAllCustomGameModes();

	/** Returns the FBSConfig corresponding to the input DefaultMode string */
	FBSConfig FindDefaultGameMode(const FString& GameModeName) const;

	/** Returns the FBSConfig corresponding to the input CustomGameModeName string */
	FBSConfig FindCustomGameMode(const FString& CustomGameModeName) const;

	/** Returns whether or not the DefaultMode is part of the game's default game modes */
	bool IsDefaultGameMode(const FString& GameModeName) const;

	/** Returns whether or not the DefaultMode is already a custom game mode name */
	bool IsCustomGameMode(const FString& GameModeName) const;

	/** Loads all player scores from slot */
	TArray<FPlayerScore> LoadPlayerScores() const;
	
	/** Saves entire player score array, overwriting the previous player score array */
	virtual void SavePlayerScores(const TArray<FPlayerScore>& PlayerScoreArrayToSave);

	/** Finds any PlayerScores that match the input PlayerScore based on DefaultMode, CustomGameModeName, Difficulty, and SongTitle */
	TArray<FPlayerScore> GetMatchingPlayerScores(const FPlayerScore& PlayerScore) const;

	/** Loads the map containing common score info for all game modes */
	TMap<FBS_DefiningConfig, FCommonScoreInfo> LoadCommonScoreInfo() const;

	/** Returns the CommonScoreInfo that matches a given DefiningConfig */
	FCommonScoreInfo GetScoreInfoFromDefiningConfig(const FBS_DefiningConfig& DefiningConfig) const;
	
	/** Finds or Adds a DefiningConfig CommonScoreInfo pair and saves to slot */
	virtual void SaveCommonScoreInfo(const FBS_DefiningConfig& DefiningConfig, const FCommonScoreInfo& CommonScoreInfoToSave);
	
	UFUNCTION()
	virtual void OnPlayerSettingsChanged_Game(const FPlayerSettings_Game& GameSettings);
	UFUNCTION()
	virtual void OnPlayerSettingsChanged_AudioAnalyzer(const FPlayerSettings_AudioAnalyzer& AudioAnalyzerSettings);
	UFUNCTION()
	virtual void OnPlayerSettingsChanged_User(const FPlayerSettings_User& UserSettings);
	UFUNCTION()
	virtual void OnPlayerSettingsChanged_CrossHair(const FPlayerSettings_CrossHair& CrossHairSettings);
	UFUNCTION()
	virtual void OnPlayerSettingsChanged_VideoAndSound(const FPlayerSettings_VideoAndSound& VideoAndSoundSettings);

protected:
	
	FOnPlayerSettingsChanged_Game OnPlayerSettingsChangedDelegate_Game;
	FOnPlayerSettingsChanged_AudioAnalyzer OnPlayerSettingsChangedDelegate_AudioAnalyzer;
	FOnPlayerSettingsChanged_User OnPlayerSettingsChangedDelegate_User;
	FOnPlayerSettingsChanged_CrossHair OnPlayerSettingsChangedDelegate_CrossHair;
	FOnPlayerSettingsChanged_VideoAndSound OnPlayerSettingsChangedDelegate_VideoAndSound;
	
private:
	/** Performs the actual saving to slot of the CommonScoreInfo, do not call directly. Instead use SaveCommonScoreInfo */
	void UpdateCommonScoreInfo(const TMap<FBS_DefiningConfig, FCommonScoreInfo>& MapToSave) const;
};
