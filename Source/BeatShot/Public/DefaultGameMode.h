// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HttpRequestInterface.h"
#include "SaveGameCustomGameMode.h"
#include "SaveGamePlayerScore.h"
#include "SaveLoadInterface.h"
#include "GameFramework/GameModeBase.h"
#include "DefaultGameMode.generated.h"

class AVisualizer;
class AFloatingTextActor;
class ATargetSpawner;
class ASphereTarget;
class UPopupMessageWidget;
class UAudioAnalyzerManager;

DECLARE_DELEGATE(FOnTargetSpawned);
DECLARE_DELEGATE_OneParam(FOnTargetDestroyed, const float TimeAlive);
DECLARE_DELEGATE_OneParam(FUpdateScoresToHUD, FPlayerScore PlayerScore);
DECLARE_DELEGATE_OneParam(FOnGameModeInit, const FGameModeActorStruct GameModeActorStruct);
DECLARE_DELEGATE_OneParam(FOnVisualizerSpawned, AVisualizer* Visualizer);
DECLARE_DELEGATE_OneParam(FOnAAManagerSecondPassed, const float PlaybackTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAAPlayerLoaded, UAudioAnalyzerManager*, AAManager);

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

	/* The Visualizer class to spawn */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AVisualizer> VisualizerClass;

	/** The FloatingTextActor class to spawn */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AFloatingTextActor> FloatingTextActorClass;

	/* The spawned TargetSpawner */
	UPROPERTY(BlueprintReadOnly)
	ATargetSpawner* TargetSpawner;

	/* The spawned Visualizer */
	UPROPERTY(BlueprintReadOnly)
	AVisualizer* Visualizer;

	/* The spawned AATracker object */
	UPROPERTY(BlueprintReadOnly)
	UAudioAnalyzerManager* AATracker;

	/* The spawned AAPlayer object */
	UPROPERTY(BlueprintReadOnly)
	UAudioAnalyzerManager* AAPlayer;

#pragma endregion

#pragma region StartStopGameMode

public:
	/** Entry point into starting game. Spawn GameModeActorBase, TargetSpawner, and Visualizer and calls OpenSongFileDialog.
	 *  If a valid path is found, calls InitializeAudioManagers, otherwise calls ShowSongPathErrorMessage */
	void InitializeGameMode(const bool bShowOpenFileDialog);

	/** Called from Countdown widget to begin the game mode. Also hides countdown widget, shows PlayerHUD widget, and shows CrossHair widget*/
	UFUNCTION()
	void StartGameMode();

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
	void PauseAAManager(bool ShouldPause, UAudioAnalyzerManager* AAManager = nullptr);

private:
	/** Does all of the AudioAnalyzer initialization, called during InitializeGameMode */
	void InitializeAudioManagers(const bool bUseSongFromFile, const FString& SongFilePath);

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
	/** Delegate to pass AAManager to visualizer */
	UPROPERTY(BlueprintAssignable, BlueprintCallable, BlueprintReadWrite)
	FOnAAPlayerLoaded OnAAPlayerLoaded;

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

	/** Delegate that is executed when a new streak has been achieved by player, to update streak in PlayerHUD.
	*   GameModeActorBase binds to it, while TargetSpawner executes it. Range level also listens to this to look
	*   for streak values going greater than a certain amount */
	UPROPERTY(BlueprintAssignable)
	FOnStreakUpdate OnStreakUpdate;

	/** Delegate that is executed when a player destroys a target. Passes the time the target was alive as payload data.
	 *  GameModeActorBase binds to it, while TargetSpawner executes it */
	FOnTargetDestroyed OnTargetDestroyed;

	/** Delegate that is executed when there is any score update that should be reflected in PlayerHUD stats.
	 *  GameModeActorBase binds to it, while TargetSpawner executes it */
	FUpdateScoresToHUD UpdateScoresToHUD;

#pragma endregion

#pragma region Scoring

private:
	/** Loads matching player scores into CurrentPlayerScore and calculates the MaxScorePerTarget */
	void LoadMatchingPlayerScores();

	/** Loads matching player scores into CurrentPlayerScore and calculates the MaxScorePerTarget */
	TArray<FPlayerScore> GetCompletedPlayerScores();

	/** Calls RequestAccessToken and then calls PostPlayerScores if access token is not empty string */
	void SaveScoresToDatabase(const bool ShowPostGameMenu, const FPlayerSettings PlayerSettings,
							  const TArray<FPlayerScore> Scores);
	
	/** Delegate that listens for the access token response after calling RequestAccessToken() inside EndGameMode */
	FOnAccessTokenResponse OnAccessTokenResponseDelegate;

	/** Delegate that listens for post scores response after calling PostPlayerScores() inside EndGameMode. Passes
	 *  the information received about the login state to DefaultPlayerController, which then passes to ScoreBrowserWidget */
	FOnPostScoresResponse OnPostScoresResponse;

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

	/** Whether or not the game was in fullscreen mode before showing OpenFileDialog */
	bool bWasInFullScreenMode = false;

	/** Whether or not to show the Streak Combat Text */
	bool bShowStreakCombatText;

	/** The frequency at which to show Streak Combat Text */
	int32 CombatTextFrequency;

	/** The game mode defining properties */
	UPROPERTY(VisibleAnywhere)
	FGameModeActorStruct GameModeActorStruct;

	const FVector TargetSpawnerLocation = {3590, 0, 750};

	const FVector VisualizerLocation = {-3900, 0, 60};

	const FActorSpawnParameters SpawnParameters;

#pragma endregion

#pragma region HUDUpdate

	/** Function bound to DefaultGameMode's OnTargetDestroyed delegate, which is executed by TargetSpawner.
	 *  Passes the time that the target was alive for */
	UFUNCTION()
	void UpdatePlayerScores(const float TimeElapsed);

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

	/** Function bound to DefaultGameMode's OnStreakUpdateCallback delegate to keep track of streak.
	 *  Executed by TargetSpawner */
	UFUNCTION()
	void OnStreakUpdateCallback(int32 Streak, FVector Location);

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

public:
	/** Opens file dialog for song selection. The Implementation version only checks the fullscreen mode,
	 *  and changes it to Windowed Fullscreen if necessary */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OpenSongFileDialog(TArray<FString>& OutFileNames);

private:
	/** Displays an error message upon failed AudioAnalyzer initialization */
	void ShowSongPathErrorMessage() const;

	/** Checks if the value is NaN so we don't save a NaN value */
	float CheckFloatNaN(const float ValueToCheck, const float ValueToRound);

#pragma endregion
};
