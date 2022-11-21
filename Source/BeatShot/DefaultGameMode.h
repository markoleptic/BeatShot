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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAAPlayerLoaded, UAudioAnalyzerManager* , AAManager);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameModeActorInit, FGameModeActorStruct, GameModeActorStruct);

UCLASS()
class BEATSHOT_API ADefaultGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:

	virtual void BeginPlay() override;

public:

	// does all of the AudioAnalyzer initialization, called during InitializeGameMode
	UFUNCTION(BlueprintCallable, Category = "GameMode Initialization")
		void InitializeAudioManagers(FString SongFilePath);

	// called from Countdown widget when user clicks to start game mode
	UFUNCTION(BlueprintCallable, Category = "GameMode Initialization")
		void StartAAManagerPlayback();

	// entry point into starting game, spawn GameModeActorBase, init AAManagers
	UFUNCTION(BlueprintCallable, Category = "GameMode Initialization")
		void InitializeGameMode();

	// play AAPlayer, used as callback function to set delay from AATracker
	UFUNCTION(BlueprintCallable, Category = "GameMode Initialization")
		void PlayAAPlayer();

	// opens file dialog for song selection
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "GameMode Initialization")
		void OpenSongFileDialog();

	// get AASettings from GameInstance
	UFUNCTION(BlueprintCallable, Category = "GameMode Initialization")
		void RefreshAASettings(FAASettingsStruct RefreshedAASettings);

	// GameModeActorBase class to spawn, deals with GameModeActorStruct, scoring, etc.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameMode Initialization")
		TSubclassOf<AGameModeActorBase> GameModeActorBaseClass;

	// TargetSpawner to spawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameMode Initialization")
		TSubclassOf<ATargetSpawner> TargetSpawnerClass;

	// in game visualizer class to spawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameMode Initialization")
		TSubclassOf<AActor> VisualizerClass;

	UFUNCTION(BlueprintCallable, Category = "GameMode Ending")
		void EndGameMode(bool ShouldSavePlayerScores);

	// change volume of given AAManager, or if none provided change Player/Tracker volume
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzer Settings")
		void SetAAManagerVolume(float GlobalVolume, float MusicVolume, UAudioAnalyzerManager* AAManager = nullptr);

	// called from DefaultPlayerController when the game is paused
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzer Settings")
		void PauseAAManager(bool ShouldPause, UAudioAnalyzerManager* AAManager = nullptr);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AudioAnalyzer Settings")
		UAudioAnalyzerManager* AATracker;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AudioAnalyzer Settings")
		UAudioAnalyzerManager* AAPlayer;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AudioAnalyzer Settings")
		FAASettingsStruct AASettings;

	// used in blueprint
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AudioAnalyzer Settings")
		bool FirstDelayTriggered;

	// used in blueprint
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AudioAnalyzer Settings")
		bool LastTargetOnSet;

	// used in blueprint
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AudioAnalyzer Settings")
		float DeltaTime;

	// used in blueprint
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AudioAnalyzer Settings")
		float FirstDelay;

	// used in blueprint
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AudioAnalyzer Settings")
		float Elapsed;

	// delegate to pass AAManager to visualizer
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, BlueprintAssignable, Category = "AudioAnalyzer Settings")
		FOnAAPlayerLoaded OnAAPlayerLoaded;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, BlueprintAssignable, Category = "GameMode Initialization")
		FOnGameModeActorInit OnGameModeActorInit;

	// Reference Game Instance
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "References")
		UDefaultGameInstance* GI;

	// reference to spawned GameModeActorBase
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "References")
		AGameModeActorBase* GameModeActorBase;

	// reference to TargetSpawner
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameMode Initialization")
		ATargetSpawner* TargetSpawner;

	// reference to in game visualizer
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "References")
		AActor* Visualizer;
};
