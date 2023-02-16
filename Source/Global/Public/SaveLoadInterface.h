// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "SaveGameCustomGameMode.h"
#include "SaveGamePlayerScore.h"
#include "SaveGamePlayerSettings.h"
#include "SaveLoadInterface.generated.h"

/* The transition state describing the start state and end state of a transition */
UENUM()
enum class ETransitionState : uint8
{
	StartFromMainMenu UMETA(DisplayName, "StartFromMainMenu"),
	StartFromPostGameMenu UMETA(DisplayName, "StartFromPostGameMenu"),
	Restart UMETA(DisplayName, "Restart"),
	QuitToMainMenu UMETA(DisplayName, "QuitToMainMenu"),
	QuitToDesktop UMETA(DisplayName, "QuitToDesktop")
};
ENUM_RANGE_BY_FIRST_AND_LAST(ETransitionState, ETransitionState::StartFromMainMenu, ETransitionState::QuitToDesktop);

/* Information about the transition state of the game */
USTRUCT()
struct FGameModeTransitionState
{
	GENERATED_BODY()

	/* The game mode transition to perform */
	ETransitionState TransitionState;
	
	/* Whether or not to save current scores if the transition is Restart or Quit */
	bool bSaveCurrentScores;
	
	/* The game mode properties, only used if Start or Restart */
	FGameModeActorStruct GameModeActorStruct;
};

/** Broadcast when Player Settings are changed and saved */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerSettingsChange, const FPlayerSettings&, RefreshedPlayerSettings);

/** Broadcast when AudioAnalyzer settings are changed and saved */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAASettingsChange, const FAASettingsStruct&, RefreshedAASettings);

/** Broadcast from GameModesWidget, SettingsMenuWidget, PauseMenuWidget, and PostGameMenuWidget any time the game should
 *  start, restart, or stop */
DECLARE_MULTICAST_DELEGATE_OneParam(FOnGameModeStateChanged, FGameModeTransitionState& TransitionState);

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
	
	TArray<FGameModeActorStruct> LoadCustomGameModes() const;

	void SaveCustomGameMode(const TArray<FGameModeActorStruct> GameModeArrayToSave);
	
	TArray<FPlayerScore> LoadPlayerScores() const;
	
	void SavePlayerScores(const TArray<FPlayerScore> PlayerScoreArrayToSave);
};