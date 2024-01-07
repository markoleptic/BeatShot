// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "BSGameModeDataAsset.h"
#include "SaveGamePlayerScore.h"
#include "SaveGamePlayerSettings.h"
#include "Kismet/GameplayStatics.h"
#include "SaveLoadInterface.generated.h"

class USaveGamePlayerSettings;
class USaveGameCustomGameMode;
class USaveGamePlayerScore;
class UBSGameModeDataAsset;

/** The transition state describing the start state and end state of a transition */
UENUM(BlueprintType)
enum class ETransitionState : uint8
{
	StartFromMainMenu UMETA(DisplayName="StartFromMainMenu"),
	StartFromPostGameMenu UMETA(DisplayName="StartFromPostGameMenu"),
	Restart UMETA(DisplayName="Restart"),
	QuitToMainMenu UMETA(DisplayName="QuitToMainMenu"),
	QuitToDesktop UMETA(DisplayName="QuitToDesktop"),
	PlayAgain UMETA(DisplayName="PlayAgain")};

ENUM_RANGE_BY_FIRST_AND_LAST(ETransitionState, ETransitionState::StartFromMainMenu, ETransitionState::PlayAgain);

/** Information about the transition state of the game */
USTRUCT()
struct FGameModeTransitionState
{
	GENERATED_BODY()

	/** The game mode transition to perform */
	ETransitionState TransitionState;

	/** Whether or not to save current scores if the transition is Restart or Quit */
	bool bSaveCurrentScores;

	/** The game mode properties, only used if Start or Restart */
	FBSConfig BSConfig;
};

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

/** Broadcast from GameModesWidget, SettingsMenuWidget, PauseMenuWidget, and PostGameMenuWidget any time the game should start, restart, or stop */
DECLARE_MULTICAST_DELEGATE_OneParam(FOnGameModeStateChanged, const FGameModeTransitionState& TransitionState);

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

	/** Performs the loading from SaveGame slot and returns the USaveGamePlayerScore object */
	static USaveGamePlayerScore* LoadFromSlot_SaveGamePlayerScore();

	/** Performs the loading from SaveGame slot and returns the USaveGameCustomGameMode object */
	static USaveGameCustomGameMode* LoadFromSlot_SaveGameCustomGameMode();

	/** Performs the loading from SaveGame slot and returns the USaveGamePlayerSettings object */
	static USaveGamePlayerSettings* LoadFromSlot_SaveGamePlayerSettings();

	/** Performs the saving to SaveGame slot and returns true if successful */
	static bool SaveToSlot(USaveGamePlayerScore* SaveGamePlayerScore);

	/** Performs the saving to SaveGame slot and returns true if successful */
	static bool SaveToSlot(USaveGameCustomGameMode* SaveGameCustomGameMode);

	/** Performs the saving to SaveGame slot and returns true if successful */
	static bool SaveToSlot(USaveGamePlayerSettings* SaveGamePlayerSettings);

public:
	/** Returns the GameModeDataAsset that contains all default/preset game modes. Expected to be overriden */
	virtual UBSGameModeDataAsset* GetGameModeDataAsset() const { return nullptr; }

	/** Loads all player settings from slot */
	static FPlayerSettings LoadPlayerSettings();

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

	/** Loads the map containing common score info for all game modes */
	static TMap<FBS_DefiningConfig, FCommonScoreInfo> LoadCommonScoreInfoMap();

	/** Returns the CommonScoreInfo that matches a given DefiningConfig, or a blank one if none found */
	static FCommonScoreInfo FindCommonScoreInfo(const FBS_DefiningConfig& DefiningConfig);

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

template <typename T>
T* ISaveLoadInterface::LoadFromSlot(const FString& InSlotName, const int32 InSlotIndex)
{
	if (UGameplayStatics::DoesSaveGameExist(InSlotName, InSlotIndex))
	{
		if (T* SaveGameObject = Cast<T>(UGameplayStatics::LoadGameFromSlot(InSlotName, InSlotIndex)))
		{
			return SaveGameObject;
		}
	}
	else
	{
		if (T* SaveGameObject = Cast<T>(UGameplayStatics::CreateSaveGameObject(T::StaticClass())))
		{
			return SaveGameObject;
		}
	}
	return nullptr;
}
