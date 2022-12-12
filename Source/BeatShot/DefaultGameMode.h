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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameModeActorInit, FGameModeActorStruct, GameModeActorStruct);

UCLASS()
class BEATSHOT_API ADefaultGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

public:
	/* does all of the AudioAnalyzer initialization, called during InitializeGameMode */
	UFUNCTION(BlueprintCallable, Category = "GameMode Initialization")
	void InitializeAudioManagers(FString SongFilePath);

	/* called from Countdown widget when user clicks to start game mode */
	UFUNCTION(BlueprintCallable, Category = "GameMode Initialization")
	void StartAAManagerPlayback();

	/* entry point into starting game, spawn GameModeActorBase, init AAManagers */
	UFUNCTION(BlueprintCallable, Category = "GameMode Initialization")
	void InitializeGameMode();

	/* play AAPlayer, used as callback function to set delay from AATracker */
	UFUNCTION(BlueprintCallable, Category = "GameMode Initialization")
	void PlayAAPlayer();

	/* opens file dialog for song selection */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "GameMode Initialization")
	void OpenSongFileDialog();

	/* get AASettings from GameInstance */
	UFUNCTION(BlueprintCallable, Category = "GameMode Initialization")
	void RefreshAASettings(FAASettingsStruct RefreshedAASettings);

	/* GameModeActorBase class to spawn, deals with GameModeActorStruct, scoring, etc. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameMode Initialization")
	TSubclassOf<AGameModeActorBase> GameModeActorBaseClass;

	/* TargetSpawner to spawn */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameMode Initialization")
	TSubclassOf<ATargetSpawner> TargetSpawnerClass;

	/* in game visualizer class to spawn */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameMode Initialization")
	TSubclassOf<AActor> VisualizerClass;

	/* Destroys all actors involved in a game mode and calls GameModeActorBase to save scores */
	UFUNCTION(BlueprintCallable, Category = "GameMode Ending")
	void EndGameMode(const bool ShouldSavePlayerScores, const bool ShowPostGameMenu);

	/* change volume of given AAManager, or if none provided change Player/Tracker volume */
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzer Settings")
	void SetAAManagerVolume(float GlobalVolume, float MusicVolume, UAudioAnalyzerManager* AAManager = nullptr);

	/* called from DefaultPlayerController when the game is paused */
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzer Settings")
	void PauseAAManager(bool ShouldPause, UAudioAnalyzerManager* AAManager = nullptr);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AudioAnalyzer Settings")
	UAudioAnalyzerManager* AATracker;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AudioAnalyzer Settings")
	UAudioAnalyzerManager* AAPlayer;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AudioAnalyzer Settings")
	FAASettingsStruct AASettings;

	/* Whether or not the first delay has been triggered */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AudioAnalyzer Settings")
	bool FirstDelayTriggered;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AudioAnalyzer Settings")
	bool LastTargetOnSet;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AudioAnalyzer Settings")
	float DeltaTime;

	/* The time length for the first delay */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AudioAnalyzer Settings")
	float FirstDelay;

	/* The time elapsed since last target spawn */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AudioAnalyzer Settings")
	float Elapsed;

	/* Delegate to pass AAManager to visualizer */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, BlueprintAssignable, Category = "AudioAnalyzer Settings")
	FOnAAPlayerLoaded OnAAPlayerLoaded;

	/* Delegate that is called when GameModeActorBase is initialized */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, BlueprintAssignable, Category = "GameMode Initialization")
	FOnGameModeActorInit OnGameModeActorInit;

	/* Reference Game Instance */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "References")
	UDefaultGameInstance* GI;

	/* reference to spawned GameModeActorBase */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "References")
	AGameModeActorBase* GameModeActorBase;

	/* reference to TargetSpawner */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameMode Initialization")
	ATargetSpawner* TargetSpawner;

	/* reference to in game visualizer */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "References")
	AActor* Visualizer;

	/* Whether or not to run tick functions */
	UPROPERTY(BlueprintReadOnly)
	bool bShouldTick;

	/* A timer used to set the difference in start times between AATracker and AAPlayer */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Countdown")
	FTimerHandle PlayerDelayTimer;

private:
	/* Displays an error message upon failed AudioAnalyzer initialization */
	void ShowSongPathErrorMessage() const;
	
	/* Function to tell TargetSpawner to spawn a new target */
	void UpdateTargetSpawn(bool bNewTargetState);
};
