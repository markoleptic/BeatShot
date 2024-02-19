// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once
#include "SaveLoadInterface.generated.h"

struct FCommonScoreInfo;
struct FBS_DefiningConfig;
struct FPlayerScore;
enum class EBaseGameMode : uint8;
enum class EGameModeDifficulty : uint8;
class USaveGamePlayerSettings;
class USaveGameCustomGameMode;
class USaveGamePlayerScore;
class UBSGameModeDataAsset;
struct FPlayerSettings;
struct FPlayerSettings_Game;
struct FPlayerSettings_User;
struct FPlayerSettings_VideoAndSound;
struct FPlayerSettings_AudioAnalyzer;
struct FPlayerSettings_CrossHair;
struct FBSConfig;

/** Broadcast when game specific settings are changed and saved */
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerSettingsChanged_Game, const FPlayerSettings_Game& GameSettings);

/** Broadcast when Audio Analyzer specific settings are changed and saved */
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerSettingsChanged_AudioAnalyzer,
	const FPlayerSettings_AudioAnalyzer& AudioAnalyzerSettings);

/** Broadcast when User specific settings are changed and saved */
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerSettingsChanged_User, const FPlayerSettings_User& UserSettings);

/** Broadcast when CrossHair specific settings are changed and saved */
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerSettingsChanged_CrossHair, const FPlayerSettings_CrossHair&
	CrossHairSettings);

/** Broadcast when VideoAndSound specific settings are changed and saved */
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerSettingsChanged_VideoAndSound,
	const FPlayerSettings_VideoAndSound& VideoAndSoundSettings);


/** Interface to allow all other classes in this game to query, load, and save all settings */
UINTERFACE()
class USaveLoadInterface : public UInterface
{
	GENERATED_BODY()
};

/** Interface to allow all other classes in this game to query, load, and save all settings */
class BEATSHOTGLOBAL_API ISaveLoadInterface
{
	GENERATED_BODY()

private:
	template <typename T>
	/** Performs the loading from SaveGame slot and returns the USaveGamePlayerScore object */
	static T* LoadFromSlot(const FString& InSlotName, const int32 InSlotIndex);

	template <typename T>
	/** Base template for LoadFromSlot with no parameters */
	static T* LoadFromSlot();

	template <typename T>
	/** Performs the saving to SaveGame slot and returns true if successful */
	static bool SaveToSlot(T* SaveGameClass);

public:
	/** Returns the GameModeDataAsset that contains all default/preset game modes. Expected to be overriden */
	virtual UBSGameModeDataAsset* GetGameModeDataAsset() const { return nullptr; }

	/** Loads all player settings from slot */
	static FPlayerSettings LoadPlayerSettings();

	/** Saves specific sub-setting struct, preserving all other settings */
	void SavePlayerSettings(const FPlayerSettings_User& InSettingsStruct);
	/** Saves specific sub-setting struct, preserving all other settings */
	void SavePlayerSettings(const FPlayerSettings_VideoAndSound& InSettingsStruct);
	/** Saves specific sub-setting struct, preserving all other settings */
	void SavePlayerSettings(const FPlayerSettings_CrossHair& InSettingsStruct);
	/** Saves specific sub-setting struct, preserving all other settings */
	void SavePlayerSettings(const FPlayerSettings_Game& InSettingsStruct);
	/** Saves specific sub-setting struct, preserving all other settings */
	void SavePlayerSettings(const FPlayerSettings_AudioAnalyzer& InSettingsStruct);

	/** Returns all Custom Game Modes */
	static TArray<FBSConfig> LoadCustomGameModes();

	/** Returns the Custom Game Mode corresponding to the input CustomGameModeName string */
	static FBSConfig FindCustomGameMode(const FString& CustomGameModeName);

	/** Saves a custom game mode to slot, checking if any matching exist and overriding if they do */
	static void SaveCustomGameMode(const FBSConfig& ConfigToSave);

	/** Removes a custom game mode from SaveGameCustomGameMode and saves to slot. Also removes the FCommonScoreInfo
	 *  from SaveGamePlayerScore. */
	static int32 RemoveCustomGameMode(const FBSConfig& ConfigToRemove);

	/** Removes all custom game modes and saves to slot. Also removes the FCommonScoreInfos for all custom game modes
	 *  from SaveGamePlayerScore. */
	static int32 RemoveAllCustomGameModes();

	/** Returns whether or not the GameModeName is already a custom game mode name */
	static bool IsCustomGameMode(const FString& GameModeName);

	/** Returns whether or not the CustomGameMode is Custom and identical to the config */
	static bool DoesCustomGameModeMatchConfig(const FString& CustomGameModeName, const FBSConfig& InConfig);

	/** Attempts to initialize a given config using  */
	static bool ImportCustomGameMode(const FString& InSerializedJsonString, FBSConfig& OutConfig,
		FText& OutFailureReason);

	/** Creates a serialized Json object export string from an FBSConfig */
	static FString ExportCustomGameMode(const FBSConfig& InConfig);

	/** Returns the FBSConfig corresponding to the input GameModeName string and difficulty. MUST OVERRIDE GetGameModeDataAsset() */
	FBSConfig FindPresetGameMode(const FString& GameModeName, const EGameModeDifficulty& Difficulty) const;

	/** Returns the FBSConfig corresponding to the input BaseGameMode and difficulty. MUST OVERRIDE GetGameModeDataAsset() */
	FBSConfig FindPresetGameMode(const EBaseGameMode& BaseGameMode, const EGameModeDifficulty& Difficulty) const;

	/** Returns whether or not the GameModeName is part of the game's default game modes */
	static bool IsPresetGameMode(const FString& GameModeName);

	/** Loads all player scores from slot */
	static TArray<FPlayerScore> LoadPlayerScores();

	/** Loads player scores not saved to database */
	static TArray<FPlayerScore> LoadPlayerScores_UnsavedToDatabase();

	/** Marks all player scores as saved to the database and saves to slot */
	static void SetAllPlayerScoresSavedToDatabase();

	/** Finds any PlayerScores that match the input PlayerScore based on DefaultMode, CustomGameModeName, Difficulty, and SongTitle */
	static TArray<FPlayerScore> GetMatchingPlayerScores(const FPlayerScore& PlayerScore);

	/** Saves an instance of an FPlayerScore to slot */
	static void SavePlayerScoreInstance(const FPlayerScore& PlayerScoreToSave);

	/** Returns the CommonScoreInfo that matches a given DefiningConfig, or creates a new one if none found */
	static FCommonScoreInfo FindOrAddCommonScoreInfo(const FBS_DefiningConfig& DefiningConfig);

	/** Finds or Adds a DefiningConfig CommonScoreInfo pair and saves to slot */
	static void SaveCommonScoreInfo(const FBS_DefiningConfig& DefiningConfig,
		const FCommonScoreInfo& CommonScoreInfoToSave);

	/** Removes a DefiningConfig CommonScoreInfo pair and saves to slot. Returns the number of successfully removed entries */
	static int32 RemoveCommonScoreInfo(const FBS_DefiningConfig& DefiningConfig);

	/** Resets the QTable for a DefiningConfig CommonScoreInfo pair and saves to slot. Returns the number of successful resets */
	static int32 ResetQTable(const FBS_DefiningConfig& DefiningConfig);

	// These functions should be overriden when a class wants to receive updates about settings changes
	// Example:
	// GameInstance->GetPublicGameSettingsChangedDelegate().AddUniqueDynamic(this, &ThisClass::OnPlayerSettingsChanged_Game);

	UFUNCTION()
	virtual void OnPlayerSettingsChanged_Game(const FPlayerSettings_Game& GameSettings)
	{
	}

	UFUNCTION()
	virtual void OnPlayerSettingsChanged_AudioAnalyzer(const FPlayerSettings_AudioAnalyzer& AudioAnalyzerSettings)
	{
	}

	UFUNCTION()
	virtual void OnPlayerSettingsChanged_User(const FPlayerSettings_User& UserSettings)
	{
	}

	UFUNCTION()
	virtual void OnPlayerSettingsChanged_CrossHair(const FPlayerSettings_CrossHair& CrossHairSettings)
	{
	}

	UFUNCTION()
	virtual void OnPlayerSettingsChanged_VideoAndSound(const FPlayerSettings_VideoAndSound& VideoAndSoundSettings)
	{
	}

protected:
	// These delegates are executed when their respective settings categories are changed, allowing other classes to receive updates
	// Example:
	// GameInstance->AddDelegateToOnPlayerSettingsChanged(OnPlayerSettingsChangedDelegate_Game);
	
	/** The delegate that is broadcast when this class saves Game settings */
	FOnPlayerSettingsChanged_Game OnPlayerSettingsChangedDelegate_Game;
	/** The delegate that is broadcast when this class saves AudioAnalyzer settings */
	FOnPlayerSettingsChanged_AudioAnalyzer OnPlayerSettingsChangedDelegate_AudioAnalyzer;
	/** The delegate that is broadcast when this class saves User settings */
	FOnPlayerSettingsChanged_User OnPlayerSettingsChangedDelegate_User;
	/** The delegate that is broadcast when this class saves CrossHair settings */
	FOnPlayerSettingsChanged_CrossHair OnPlayerSettingsChangedDelegate_CrossHair;
	/** The delegate that is broadcast when this class saves VideoAndSound settings */
	FOnPlayerSettingsChanged_VideoAndSound OnPlayerSettingsChangedDelegate_VideoAndSound;
};
	
/** Performs the loading from SaveGame slot and returns the USaveGamePlayerScore object */
template <>
USaveGamePlayerScore* ISaveLoadInterface::LoadFromSlot();

/** Performs the loading from SaveGame slot and returns the USaveGameCustomGameMode object */
template <>
USaveGameCustomGameMode* ISaveLoadInterface::LoadFromSlot();

/** Performs the loading from SaveGame slot and returns the USaveGamePlayerSettings object */
template <>
USaveGamePlayerSettings* ISaveLoadInterface::LoadFromSlot();

/** Performs the saving to SaveGame slot and returns true if successful */
template<>
bool ISaveLoadInterface::SaveToSlot(USaveGamePlayerScore* SaveGamePlayerScore);
	
/** Performs the saving to SaveGame slot and returns true if successful */
template<>
bool ISaveLoadInterface::SaveToSlot(USaveGameCustomGameMode* SaveGameCustomGameMode);

/** Performs the saving to SaveGame slot and returns true if successful */
template<>
bool ISaveLoadInterface::SaveToSlot(USaveGamePlayerSettings* SaveGamePlayerSettings);