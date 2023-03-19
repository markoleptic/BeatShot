// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HttpRequestInterface.h"
#include "SaveGameCustomGameMode.h"
#include "SaveGamePlayerScore.h"
#include "SaveLoadInterface.h"
#include "GameFramework/GameMode.h"
#include "BSGameMode.generated.h"

class AVisualizerManager;
class ABSCharacter;
class AVisualizerBase;
class ABeamVisualizer;
class AStaticCubeVisualizer;
class AFloatingTextActor;
class ATargetSpawner;
class ASphereTarget;
class ABSPlayerController;
class UAudioAnalyzerManager;

DECLARE_LOG_CATEGORY_EXTERN(LogAudioData, Log, All);

DECLARE_MULTICAST_DELEGATE_OneParam(FOnTargetSpawned, ASphereTarget* SpawnedTarget);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnTargetDestroyed, const float TimeAlive, const int32 NewStreak, const FVector Position);
DECLARE_MULTICAST_DELEGATE_OneParam(FUpdateScoresToHUD, FPlayerScore PlayerScore);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnAAManagerSecondPassed, const float PlaybackTime);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnStreakUpdate, const int32 NewStreak, const FVector Position);
DECLARE_MULTICAST_DELEGATE(OnGameModeStarted);

UCLASS()
class BEATSHOT_API ABSGameMode : public AGameMode, public ISaveLoadInterface, public IHttpRequestInterface
{
	GENERATED_BODY()

	ABSGameMode();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual void PostLoad() override;

	virtual void Logout(AController* Exiting) override;

	ABSCharacter* SpawnPlayer(ABSPlayerController* PlayerController);

	UPROPERTY()
	TArray<ABSPlayerController*> Controllers;

#pragma region Classes

protected:
	/* The TargetSpawner class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = "BeatShot|Spawnable Classes")
	TSubclassOf<ATargetSpawner> TargetSpawnerClass;

	/** The FloatingTextActor class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = "BeatShot|Spawnable Classes")
	TSubclassOf<AFloatingTextActor> FloatingTextActorClass;

	/** The VisualizerManager class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = "BeatShot|Spawnable Classes")
	TSubclassOf<AVisualizerManager> VisualizerManagerClass;

	/** The VisualizerManager class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = "BeatShot|Spawnable Classes")
	TSubclassOf<ACharacter> CharacterClass;

	/* The spawned TargetSpawner */
	UPROPERTY(EditDefaultsOnly, Category = "BeatShot|Spawned Actors")
	TObjectPtr<ATargetSpawner> TargetSpawner;

	/** The Visualizer Manager */
	UPROPERTY(EditDefaultsOnly, Category = "BeatShot|Spawned Actors")
	TObjectPtr<AVisualizerManager> VisualizerManager;

	/* The spawned AATracker object */
	UPROPERTY(EditDefaultsOnly, Category = "BeatShot|Spawned Objects")
	TObjectPtr<UAudioAnalyzerManager> AATracker;

	/* The spawned AAPlayer object */
	UPROPERTY(EditDefaultsOnly, Category = "BeatShot|Spawned Objects")
	TObjectPtr<UAudioAnalyzerManager> AAPlayer;

#pragma endregion

#pragma region StartStopGameMode

public:
	/** Entry point into starting game. Spawn TargetSpawner, Visualizers 
	 *  If a valid path is found, calls InitializeAudioManagers, otherwise calls ShowSongPathErrorMessage */
	void InitializeGameMode();

	/** Allows TargetSpawner to start spawning targets, starts timers, shows PlayerHUD, CrossHair, and hides the
	 *  countdown widget. Called from Countdown widget when the countdown has completed */
	UFUNCTION()
	void StartGameMode();

	/** Destroys all actors involved in a game mode and optionally save scores */
	void EndGameMode(const bool ShouldSavePlayerScores, const bool ShowPostGameMenu);

private:
	/** Starts all DefaultGameMode timers */
	void StartGameModeTimers();

	/** Binds all delegates associated with DefaultGameMode */
	void BindGameModeDelegates();

	/** Function to tell TargetSpawner to spawn a new target */
	void SpawnNewTarget(bool bNewTargetState);

	/** Timer that spans the length of the song */
	UPROPERTY(VisibleAnywhere, Category = "BeatShot|Timer")
	FTimerHandle GameModeLengthTimer;

	/** Calls EndGameMode after finding parameters for it */
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
	UFUNCTION(BlueprintCallable, Category = "BeatShot|AudioAnalyzer")
	void PauseAAManager(bool ShouldPause);

private:
	/** Does all of the AudioAnalyzer initialization, called during InitializeGameMode */
	void InitializeAudioManagers();

	/** Retrieves all AudioAnalyzer data on tick */
	void OnTick_AudioAnalyzers(const float DeltaSeconds);

	/** play AAPlayer, used as callback function to set delay from AATracker */
	UFUNCTION()
	void PlayAAPlayer();

	/** Change volume of given AAManager, or if none provided change Player/Tracker volume */
	void SetAAManagerVolume(float GlobalVolume, float MusicVolume, UAudioAnalyzerManager* AAManager = nullptr);

	void OnAAManagerError()
	{
		bShouldTick = false;
		UE_LOG(LogTemp, Warning, TEXT("Init Player Error"));
	}

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
	/** Delegate that is executed every second to update the progress into song on PlayerHUD.
	 *  PlayerHUD binds to it, while DefaultGameMode (this) executes it */
	FOnAAManagerSecondPassed OnSecondPassed;

	/** Delegate that is executed every time a target has been spawned.
	*   DefaultGameMode (this) binds to it, while TargetSpawner executes it */
	FOnTargetSpawned OnTargetSpawned;

	/** Delegate that is executed when a player destroys a target. Passes the time the target was alive as payload data.
	 *  DefaultGameMode (this) binds to it, while TargetSpawner executes it */
	FOnTargetDestroyed OnTargetDestroyed;

	/** Delegate that is executed when there is any score update that should be reflected in PlayerHUD stats.
	 *  DefaultGameMode (this) binds to it, while TargetSpawner executes it */
	FUpdateScoresToHUD UpdateScoresToHUD;

	/** Broadcasts when the countdown has completed and the actual game has began. */
	OnGameModeStarted OnGameModeStarted;
	
	/** Delegate that listens for post scores response after calling PostPlayerScores() inside SaveScoresToDatabase().
	 *  DefaultPlayerController also binds to this in order to display correct information about scoring. */
	FOnPostScoresResponse OnPostScoresResponse;

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
	
	/** The "live" player score objects, which start fresh and import high score from SavedPlayerScores */
	UPROPERTY(VisibleAnywhere, Category = "BeatShot|Score")
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
	UPROPERTY(VisibleAnywhere, Category = "BeatShot|General")
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
	void UpdateTargetsSpawned(ASphereTarget* SpawnedTarget);

	/** Function bound to Gun_AK47's FOnShotFired delegate to keep track of number of targets spawned.
	 *  Executed by Gun_AK47 */
	UFUNCTION()
	void UpdateShotsFired();

	/** Called by UpdatePlayerScores to update the streak */
	void UpdateStreak(int32 Streak, FVector Location);

	/* Called by UpdatePlayerScores since everytime that function is called, a target has been hit */
	void UpdateTargetsHit();

	/** Called by UpdatePlayerScores or UpdatingTrackingScores to recalculate the high score if needed  */
	void UpdateHighScore();

	/** Callback function for OnSecondPassedTimer, executes OnSecondPassed */
	UFUNCTION()
	void OnSecondPassedCallback();

#pragma endregion

#pragma region Utility

	static float FloatDivide(const float Num, const float Denom);

#pragma endregion
};