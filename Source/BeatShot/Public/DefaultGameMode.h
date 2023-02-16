// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HttpRequestInterface.h"
#include "SaveGameCustomGameMode.h"
#include "SaveGamePlayerScore.h"
#include "SaveLoadInterface.h"
#include "GameFramework/GameModeBase.h"
#include "DefaultGameMode.generated.h"

class AVisualizerManager;
class AVisualizerBase;
class ABeamVisualizer;
class AStaticCubeVisualizer;
class AFloatingTextActor;
class ATargetSpawner;
class ASphereTarget;
class UAudioAnalyzerManager;

DECLARE_LOG_CATEGORY_EXTERN(LogAudioData, Log, All);

DECLARE_DELEGATE(FOnTargetSpawned);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnTargetDestroyed, const float, TimeAlive, const int32, NewStreak, const FVector, Position);
DECLARE_DELEGATE_OneParam(FUpdateScoresToHUD, FPlayerScore PlayerScore);
DECLARE_DELEGATE_OneParam(FOnGameModeInit, const bool bShouldTrace);
DECLARE_DELEGATE_OneParam(FOnAAManagerSecondPassed, const float PlaybackTime);
/* Passes a reference to the spawned BeatTrackTarget */
DECLARE_MULTICAST_DELEGATE_OneParam(FOnBeatTrackTargetSpawned, ASphereTarget* TrackingTarget);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStreakUpdate, const int32, NewStreak, const FVector, Position);

UCLASS()
class BEATSHOT_API ADefaultGameMode : public AGameModeBase, public ISaveLoadInterface, public IHttpRequestInterface
{
	GENERATED_BODY()

	ADefaultGameMode();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

#pragma region Classes

protected:
	/* The TargetSpawner class to spawn */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ATargetSpawner> TargetSpawnerClass;
	
	/** The FloatingTextActor class to spawn */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AFloatingTextActor> FloatingTextActorClass;

	/** The VisualizerManager class to spawn */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AVisualizerManager> VisualizerManagerClass;

	/* The spawned TargetSpawner */
	UPROPERTY(BlueprintReadOnly)
	ATargetSpawner* TargetSpawner;

	/** The Visualizer Manager */
	UPROPERTY(BlueprintReadOnly)
	AVisualizerManager* VisualizerManager;
	
	/* The spawned AATracker object */
	UPROPERTY(BlueprintReadOnly)
	UAudioAnalyzerManager* AATracker;

	/* The spawned AAPlayer object */
	UPROPERTY(BlueprintReadOnly)
	UAudioAnalyzerManager* AAPlayer;

	int32 UsingAAPlayer = 0;

#pragma endregion

#pragma region StartStopGameMode

public:
	/** Entry point into starting game. Spawn GameModeActorBase, TargetSpawner, and Visualizer and calls OpenSongFileDialog.
	 *  If a valid path is found, calls InitializeAudioManagers, otherwise calls ShowSongPathErrorMessage */
	void InitializeGameMode();

	/** Called from Countdown widget when the countdown has completed */
	UFUNCTION()
	void StartGameMode();

	void StartGameModeTimers();

	void BindGameModeDelegates();

	/** Destroys all actors involved in a game mode and optionally save scores */
	void EndGameMode(const bool ShouldSavePlayerScores, const bool ShowPostGameMenu);

private:
	/** Function to tell TargetSpawner to spawn a new target */
	void SpawnNewTarget(bool bNewTargetState);

	/** Timer that spans the length of the song */
	UPROPERTY(VisibleAnywhere)
	FTimerHandle GameModeLengthTimer;

	/** Reports to DefaultGameMode when the song has finished */
	UFUNCTION()
	void OnGameModeLengthTimerComplete();

#pragma endregion

#pragma region AudioAnalyzer

public:
	/** Called from Countdown widget when user clicks to start game mode. If player delay is > 0.05, the function
	 *  begins playback for AATracker and sets a timer of length player delay to then begin playback of AAPlayer.
	 *  If player delay isn't long enough to justify two separate players, only AATracker begins playback. */
	UFUNCTION()
	void StartAAManagerPlayback();

	/** Called from DefaultPlayerController when the game is paused */
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzer Settings")
	void PauseAAManager(bool ShouldPause);

private:
	/** Does all of the AudioAnalyzer initialization, called during InitializeGameMode */
	void InitializeAudioManagers();

	/** play AAPlayer, used as callback function to set delay from AATracker */
	UFUNCTION()
	void PlayAAPlayer();

	/** Change volume of given AAManager, or if none provided change Player/Tracker volume */
	void SetAAManagerVolume(float GlobalVolume, float MusicVolume, UAudioAnalyzerManager* AAManager = nullptr);

	/** Callback function for OnSecondPassedTimer, executes OnAAManagerSecondPassed */
	UFUNCTION()
	void OnSecondPassed();

	/* Locally stored AASettings since they must be accessed frequently in OnTick() */
	UPROPERTY()
	FAASettingsStruct AASettings;

	/** A timer used to set the difference in start times between AATracker and AAPlayer */
	UPROPERTY()
	FTimerHandle PlayerDelayTimer;

	/** A timer used to track every passing second of the song */
	UPROPERTY()
	FTimerHandle OnSecondPassedTimer;

	/** Honestly idk what this does, but it was used in the AudioAnalyzer example so I'm sticking with it >.> */
	bool LastTargetOnSet;

	/** The time elapsed since last target spawn */
	float Elapsed;

#pragma endregion

#pragma region PublicDelegates

public:
	/** Delegate that is executed when GameModeActorBase is initialized. This is so the character is informed that the
	 *  Gun needs to perform line tracing for BeatTrack game modes, but otherwise can skip line tracing.
	 *  DefaultCharacter binds to it, while DefaultGameMode (this) executes it */
	FOnGameModeInit OnGameModeInit;

	/** Delegate that is executed every second to update the progress into song on PlayerHUD.
	 *  PlayerHUD binds to it, while DefaultGameMode (this) executes it */
	FOnAAManagerSecondPassed OnAAManagerSecondPassed;

	/** Broadcasts when a BeatTrack target has been spawned.
	 *  DefaultCharacter binds to it, while TargetSpawner executes it */
	FOnBeatTrackTargetSpawned OnBeatTrackTargetSpawned;

	/** Delegate that is executed every time a target has been spawned.
	*   GameModeActorBase binds to it, while TargetSpawner executes it */
	FOnTargetSpawned OnTargetSpawned;

	/** Delegate that is executed when a player destroys a target. Passes the time the target was alive as payload data.
	 *  GameModeActorBase binds to it, while TargetSpawner executes it */
	UPROPERTY(BlueprintAssignable)
	FOnTargetDestroyed OnTargetDestroyed;

	/** Delegate that is executed when there is any score update that should be reflected in PlayerHUD stats.
	 *  GameModeActorBase binds to it, while TargetSpawner executes it */
	FUpdateScoresToHUD UpdateScoresToHUD;

#pragma endregion

#pragma region Scoring

private:
	/** Loads matching player scores into CurrentPlayerScore and calculates the MaxScorePerTarget */
	void LoadMatchingPlayerScores();

	/** Saves player scores to slot and calls SaveScoresToDatabase() if bShouldSavePlayerScores is true and
	 *  GetCompletedPlayerScores() returns a valid score object. Otherwise Broadcasts OnPostScoresResponse with "None" */
	void HandleScoreSaving(bool bShouldSavePlayerScores);

	/** Function bound to the response of an access token, which is broadcast from RequestAccessToken() */
	UFUNCTION()
	void OnAccessTokenResponseReceived(const FString AccessToken);

	/** Function bound to the response of posting player scores to database, which is broadcast from PostPlayerScores() */
	UFUNCTION()
	void OnPostScoresResponseReceived(const ELoginState& LoginState);

	/** Returns the current player scores, checking for NaNs and updating the time */
	FPlayerScore GetCompletedPlayerScores();

	/** Calls RequestAccessToken if player has logged in before */
	void SaveScoresToDatabase();
	
	/** Delegate that listens for the access token response after calling RequestAccessToken() inside HandleScoreSaving() */
	FOnAccessTokenResponse OnAccessTokenResponse;
	
public:
	/** Delegate that listens for post scores response after calling PostPlayerScores() inside SaveScoresToDatabase().
	 *  DefaultPlayerController also binds to this in order to display correct information about scoring. */
	FOnPostScoresResponse OnPostScoresResponse;

private:
	/** The "live" player score objects, which start fresh and import high score from SavedPlayerScores */
	UPROPERTY(VisibleAnywhere)
	FPlayerScore CurrentPlayerScore;

	/** Max score per target based on total amount of targets that could spawn */
	UPROPERTY(VisibleAnywhere)
	float MaxScorePerTarget;

#pragma endregion

#pragma region Settings

public:
	/** Get PlayerSettings when PlayerSettings are updated */
	UFUNCTION()
	void RefreshPlayerSettings(const FPlayerSettings& RefreshedPlayerSettings);

	/** Get AASettings when AASettings are updated */
	UFUNCTION()
	void RefreshAASettings(const FAASettingsStruct& RefreshedAASettings);

private:
	/** Whether or not to run tick functions */
	bool bShouldTick;

	/** Whether or not to show the Streak Combat Text */
	bool bShowStreakCombatText;

	/** The frequency at which to show Streak Combat Text */
	int32 CombatTextFrequency;

	/** The game mode defining properties */
	UPROPERTY(VisibleAnywhere)
	FGameModeActorStruct GameModeActorStruct;

	const FVector TargetSpawnerLocation = {3730, 0, 750};
	
	const FActorSpawnParameters SpawnParameters;

#pragma endregion

#pragma region HUDUpdate

	/** Function bound to DefaultGameMode's OnTargetDestroyed delegate, which is executed by TargetSpawner.
	 *  Passes the time that the target was alive for */
	UFUNCTION()
	void UpdatePlayerScores(const float TimeElapsed, const int32 NewStreak, const FVector Position);

	/** Function bound to the tracking target's health component's OnBeatTrackTick delegate,
	 *  which passes the current damage taken, and the total possible damage. Executed on tick
	 *  by SphereTarget */
	UFUNCTION()
	void UpdateTrackingScore(float DamageTaken, float TotalPossibleDamage);

	/** Function bound to DefaultGameMode's OnTargetSpawned delegate to keep track of number of targets spawned.
	 *  Executed by TargetSpawner */
	UFUNCTION()
	void UpdateTargetsSpawned();

	/** Function bound to Gun_AK47's FOnShotFired delegate to keep track of number of targets spawned.
	 *  Executed by Gun_AK47 */
	UFUNCTION()
	void UpdateShotsFired();
	
	UFUNCTION()
	void UpdateStreak(int32 Streak, FVector Location);

	/* Called by UpdatePlayerScores since everytime that function is called, a target has been hit */
	void UpdateTargetsHit();

	/** Called by UpdatePlayerScores or UpdatingTrackingScores to recalculate the high score if needed  */
	void UpdateHighScore();

	/** Function bound to OnBeatTrackTargetSpawned delegate.
	 *  Binds the tracking target's health component's OnBeatTrackTick delegate to UpdateTrackingScore */
	UFUNCTION()
	void OnBeatTrackTargetSpawnedCallback(ASphereTarget* TrackingTarget);

#pragma endregion

#pragma region Utility

	/** Checks if the value is NaN so we don't save a NaN value */
	float CheckFloatNaN(const float ValueToCheck, const float ValueToRound);

#pragma endregion
};
