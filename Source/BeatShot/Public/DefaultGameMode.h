// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameModeActorBase.h"
#include "HttpRequestInterface.h"
#include "SaveGameCustomGameMode.h"
#include "SaveGamePlayerScore.h"
#include "SaveLoadInterface.h"
#include "GameFramework/GameModeBase.h"
#include "DefaultGameMode.generated.h"

class AVisualizer;
class ATargetSpawner;
class ASphereTarget;
class AGameModeActorBase;
class UPopupMessageWidget;
class UAudioAnalyzerManager;

DECLARE_DELEGATE(FOnTargetSpawned);
DECLARE_DELEGATE_OneParam(FOnTargetDestroyed, const float TimeAlive);
DECLARE_DELEGATE_OneParam(FUpdateScoresToHUD, FPlayerScore PlayerScore);
DECLARE_DELEGATE_OneParam(FOnGameModeActorInit, const FGameModeActorStruct GameModeActorStruct);
DECLARE_DELEGATE_OneParam(FOnVisualizerSpawned, AVisualizer* Visualizer);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnAAManagerSecondPassed, const float, PlaybackTime);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAAPlayerLoaded, UAudioAnalyzerManager*, AAManager);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBeatTrackTargetSpawned, ASphereTarget*, TrackingTarget);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStreakUpdate, const int32, NewStreak, const FVector, Position);

UCLASS()
class BEATSHOT_API ADefaultGameMode : public AGameModeBase, public ISaveLoadInterface, public IHttpRequestInterface
{
	GENERATED_BODY()

	ADefaultGameMode();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

public:
	/** Entry point into starting game. Spawn GameModeActorBase, TargetSpawner, and Visualizer and calls OpenSongFileDialog.
	 *  If a valid path is found, calls InitializeAudioManagers, otherwise calls ShowSongPathErrorMessage */
	void InitializeGameMode();

	/** Opens file dialog for song selection. The Implementation version only checks the fullscreen mode,
	 *  and changes it to Windowed Fullscreen if necessary */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GameMode Initialization")
	void OpenSongFileDialog(TArray<FString>& OutFileNames);

	/** Called from Countdown widget when user clicks to start game mode. If player delay is > 0.05, the function
	 *  begins playback for AATracker and sets a timer of length player delay to then begin playback of AAPlayer.
	 *  If player delay isn't long enough to justify two separate players, only AATracker begins playback. */
	UFUNCTION()
	void StartAAManagerPlayback();

	/** Called from Countdown widget to then call StartGameModeMainMenu in GameModeActorBase. Also hides countdown widget,
	 *  shows PlayerHUD widget, and shows CrossHair widget*/
	UFUNCTION()
	void StartGameMode();

	/** Destroys all actors involved in a game mode and calls GameModeActorBase to optionally save scores */
	void EndGameMode(const bool ShouldSavePlayerScores, const bool ShowPostGameMenu);

	/** Called from DefaultPlayerController when the game is paused */
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzer Settings")
	void PauseAAManager(bool ShouldPause, UAudioAnalyzerManager* AAManager = nullptr);

	/** Get AASettings from GameInstance when AASettings are updated */
	UFUNCTION()
	void RefreshAASettings(const FAASettingsStruct& RefreshedAASettings);

	/** Get PlayerSettings from GameInstance when PlayerSettings are updated */
	UFUNCTION()
	void RefreshPlayerSettings(const FPlayerSettings& RefreshedPlayerSettings);

#pragma region PublicDelegates

	/** Delegate to pass AAManager to visualizer */
	UPROPERTY(BlueprintAssignable, BlueprintCallable, BlueprintReadWrite)
	FOnAAPlayerLoaded OnAAPlayerLoaded;

	/** Delegate that is executed when GameModeActorBase is initialized. This is so the character is informed that the
	 *  Gun needs to perform line tracing for BeatTrack game modes, but otherwise can skip line tracing.
	 *  DefaultCharacter binds to it, while DefaultGameMode (this) executes it */
	FOnGameModeActorInit OnGameModeActorInit;

	/** Delegate that is executed every second to update the progress into song on PlayerHUD.
	 *  PlayerHUD binds to it, while DefaultGameMode (this) executes it */
	FOnAAManagerSecondPassed OnAAManagerSecondPassed;

	/** Broadcasts when a BeatTrack target has been spawned.
	 *  DefaultCharacter and GameModeActorBase bind to it, while TargetSpawner executes it */
	FOnBeatTrackTargetSpawned OnBeatTrackTargetSpawned;

	/** Delegate that is executed every time a target has been spawned.
	*   GameModeActorBase binds to it, while TargetSpawner executes it */
	FOnTargetSpawned OnTargetSpawned;

	/** Delegate that is executed when a new streak has been achieved by player, to update streak in PlayerHUD.
	*   GameModeActorBase binds to it, while TargetSpawner executes it */
	UPROPERTY(BlueprintAssignable)
	FOnStreakUpdate OnStreakUpdate;

	/** Delegate that is executed when a player destroys a target. Passes the time the target was alive as payload data.
	 *  GameModeActorBase binds to it, while TargetSpawner executes it */
	FOnTargetDestroyed OnTargetDestroyed;

	/** Delegate that is executed when there is any score update that should be reflected in PlayerHUD stats.
	 *  GameModeActorBase binds to it, while TargetSpawner executes it */
	FUpdateScoresToHUD UpdateScoresToHUD;

#pragma endregion
	
	UPROPERTY(BlueprintReadOnly)
	AGameModeActorBase* GameModeActorBase;

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AGameModeActorBase> GameModeActorBaseClass;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ATargetSpawner> TargetSpawnerClass;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AVisualizer> VisualizerClass;
	UPROPERTY(BlueprintReadOnly)
	ATargetSpawner* TargetSpawner;
	UPROPERTY(BlueprintReadOnly)
	AVisualizer* Visualizer;
	UPROPERTY(BlueprintReadOnly)
	UAudioAnalyzerManager* AATracker;
	UPROPERTY(BlueprintReadOnly)
	UAudioAnalyzerManager* AAPlayer;
	UPROPERTY()
	FAASettingsStruct AASettings;

private:
	/** Does all of the AudioAnalyzer initialization, called during InitializeGameMode */
	void InitializeAudioManagers(const FString& SongFilePath);

	/** Displays an error message upon failed AudioAnalyzer initialization */
	void ShowSongPathErrorMessage() const;

	/** play AAPlayer, used as callback function to set delay from AATracker */
	UFUNCTION()
	void PlayAAPlayer();

	/** Change volume of given AAManager, or if none provided change Player/Tracker volume */
	void SetAAManagerVolume(float GlobalVolume, float MusicVolume, UAudioAnalyzerManager* AAManager = nullptr);

	/** Function to tell TargetSpawner to spawn a new target */
	void UpdateTargetSpawn(bool bNewTargetState);

	/** Callback function for OnSecondPassedTimer, executes OnAAManagerSecondPassed */
	UFUNCTION()
	void OnSecondPassed();

	/** Calls RequestAccessToken and then calls PostPlayerScores if access token is not empty string */
	void SaveScoresToDatabase(const bool ShowPostGameMenu, const FPlayerSettings PlayerSettings,
	                          const TArray<FPlayerScore> Scores);

	/** A timer used to set the difference in start times between AATracker and AAPlayer */
	UPROPERTY()
	FTimerHandle PlayerDelayTimer;

	/** A timer used to track every passing second of the song */
	UPROPERTY()
	FTimerHandle OnSecondPassedTimer;

	/** Delegate that listens for the access token response after calling RequestAccessToken() inside EndGameMode */
	FOnAccessTokenResponse OnAccessTokenResponseDelegate;

	/** Delegate that listens for post scores response after calling PostPlayerScores() inside EndGameMode. Passes
	 *  the information received about the login state to DefaultPlayerController, which then passes to ScoreBrowserWidget */
	FOnPostScoresResponse OnPostScoresResponse;

	/** Whether or not to run tick functions */
	bool bShouldTick;

	/** Whether or not the game was in fullscreen mode before showing OpenFileDialog */
	bool bWasInFullScreenMode = false;

	/** Honestly idk what this does, but it was used in the AudioAnalyzer example so I'm sticking with it >.> */
	bool LastTargetOnSet;

	/** The time elapsed since last target spawn */
	float Elapsed;
};
