// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SaveGameAASettings.h"
#include "GameModeActorBase.h"
#include "GameFramework/GameModeBase.h"
#include "DefaultGameMode.generated.h"

class ATargetSpawner;
class ASphereTarget;
class AGameModeActorBase;
class UAudioAnalyzerManager;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAAPlayerLoaded, UAudioAnalyzerManager*, AAManager);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnAAManagerSecondPassed, const float, PlaybackTime);
DECLARE_DELEGATE_OneParam(FOnGameModeActorInit, const FGameModeActorStruct);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBeatTrackTargetSpawned, ASphereTarget*, TrackingTarget);
DECLARE_DELEGATE(FOnTargetSpawned);
DECLARE_DELEGATE_OneParam(FOnTargetDestroyed, const float);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStreakUpdate, const int32, NewStreak, const FVector, Position);
DECLARE_DELEGATE_OneParam(FUpdateScoresToHUD, FPlayerScore);

UCLASS()
class BEATSHOT_API ADefaultGameMode : public AGameModeBase
{
	GENERATED_BODY()

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
	void StartAAManagerPlayback();

	/** Called from Countdown widget to then call StartGameMode in GameModeActorBase. Also hides countdown widget,
	 *  shows PlayerHUD widget, and shows CrossHair widget*/
	void StartGameMode();

	/** Destroys all actors involved in a game mode and calls GameModeActorBase to optionally save scores */
	void EndGameMode(const bool ShouldSavePlayerScores, const bool ShowPostGameMenu);

	/** Called from DefaultPlayerController when the game is paused */
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzer Settings")
	void PauseAAManager(bool ShouldPause, UAudioAnalyzerManager* AAManager = nullptr);

#pragma region Delegates

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

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AGameModeActorBase> GameModeActorBaseClass;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ATargetSpawner> TargetSpawnerClass;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> VisualizerClass;
	UPROPERTY(BlueprintReadOnly)
	AGameModeActorBase* GameModeActorBase;
	UPROPERTY(BlueprintReadOnly)
	ATargetSpawner* TargetSpawner;
	UPROPERTY(BlueprintReadOnly)
	AActor* Visualizer;
	UPROPERTY(BlueprintReadOnly)
	UAudioAnalyzerManager* AATracker;
	UPROPERTY(BlueprintReadOnly)
	UAudioAnalyzerManager* AAPlayer;
	UPROPERTY(BlueprintReadOnly)
	FAASettingsStruct AASettings;

private:
	/** Does all of the AudioAnalyzer initialization, called during InitializeGameMode */
	void InitializeAudioManagers(const FString& SongFilePath);

	/** Displays an error message upon failed AudioAnalyzer initialization */
	void ShowSongPathErrorMessage() const;

	/** play AAPlayer, used as callback function to set delay from AATracker */
	UFUNCTION()
	void PlayAAPlayer();

	/** Get AASettings from GameInstance when AASettings are updated */
	UFUNCTION()
	void RefreshAASettings(const FAASettingsStruct& RefreshedAASettings);

	/** Get PlayerSettings from GameInstance when PlayerSettings are updated */
	UFUNCTION()
	void RefreshPlayerSettings(const FPlayerSettings& RefreshedPlayerSettings);

	/** Change volume of given AAManager, or if none provided change Player/Tracker volume */
	void SetAAManagerVolume(float GlobalVolume, float MusicVolume, UAudioAnalyzerManager* AAManager = nullptr);

	/** Function to tell TargetSpawner to spawn a new target */
	void UpdateTargetSpawn(bool bNewTargetState);

	UFUNCTION()
	void OnSecondPassed();

	/** A timer used to set the difference in start times between AATracker and AAPlayer */
	UPROPERTY()
	FTimerHandle PlayerDelayTimer;

	/** A timer used to track every passing second of the song */
	UPROPERTY()
	FTimerHandle OnSecondPassedTimer;

	/** Whether or not to run tick functions */
	bool bShouldTick;

	/** Whether or not the game was in fullscreen mode before showing OpenFileDialog */
	bool bWasInFullScreenMode = false;

	/** Honestly idk what this does, but it was used in the AudioAnalyzer example so I'm sticking with it >.> */
	bool LastTargetOnSet;

	/** The time elapsed since last target spawn */
	float Elapsed;
};
