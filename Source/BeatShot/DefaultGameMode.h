// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SaveGameAASettings.h"
#include "GameModeActorBase.h"
#include "GameFramework/GameModeBase.h"
#include "DefaultGameMode.generated.h"

class ATargetSpawner;
class UDefaultGameInstance;
class AGameModeActorBase;
class UAudioAnalyzerManager;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAAPlayerLoaded, UAudioAnalyzerManager*, AAManager);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnAAManagerSecondPassed, const float, PlaybackTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameModeActorInit, FGameModeActorStruct, GameModeActorStruct);

UCLASS()
class BEATSHOT_API ADefaultGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:
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

	/** Delegate to pass AAManager to visualizer */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, BlueprintAssignable)
	FOnAAPlayerLoaded OnAAPlayerLoaded;

	/** Delegate that is called when GameModeActorBase is initialized. This is so the character is informed that the
	 *  Gun needs to perform line tracing for BeatTrack game modes, but otherwise can skip line tracing */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, BlueprintAssignable)
	FOnGameModeActorInit OnGameModeActorInit;
	
	/** Delegate that is called every second to update the progress into song on PlayerHUD */
	UPROPERTY()
	FOnAAManagerSecondPassed OnAAManagerSecondPassed;

protected:
	UPROPERTY(BlueprintReadOnly)
	TSubclassOf<AGameModeActorBase> GameModeActorBaseClass;
	UPROPERTY(BlueprintReadOnly)
	TSubclassOf<ATargetSpawner> TargetSpawnerClass;
	UPROPERTY(BlueprintReadOnly)
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
	
	bool LastTargetOnSet;

	/* The time elapsed since last target spawn */
	float Elapsed;

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

	/* A timer used to set the difference in start times between AATracker and AAPlayer */
	UPROPERTY()
	FTimerHandle PlayerDelayTimer;

	UPROPERTY()
	FTimerHandle OnSecondPassedTimer;

	/* Whether or not to run tick functions */
	bool bShouldTick;

	/** Whether or not the game was in fullscreen mode before showing OpenFileDialog */
	bool bWasInFullScreenMode = false;
};


