// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameModeActorStruct.h"
#include "GameFramework/GameModeBase.h"
#include "DefaultGameMode.generated.h"

class ABeatTrack;
class UDefaultGameInstance;
class AGameModeActorBase;
class UAudioAnalyzerManager;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAAPlayerLoaded, UAudioAnalyzerManager* , AAManager);

UCLASS()
class BEATSHOT_API ADefaultGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:

	virtual void BeginPlay() override;

public:

	// Reference Game Instance
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "References")
		UDefaultGameInstance* GI;

	// Blueprint event used to stop BPAAPlayer and BPAATracker during pause game
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "GameMode Initialization")
		void StopAAPlayerAndTracker();

	// does all of the AudioAnalyzer initialization, called during InitializeGameMode
	UFUNCTION(BlueprintCallable, Category = "GameMode Initialization")
		void InitializeAudioManagers();

	// called from DefaultPlayerController when the game is paused
	UFUNCTION(BlueprintCallable, Category = "AudioAnalyzer Settings")
		void PauseAAManager(bool ShouldPause, UAudioAnalyzerManager* AAManager = nullptr);

	UFUNCTION(BlueprintCallable, Category = "GameMode Initialization")
		void StartAAManagerPlayback();

	// entry point into actual game (called on BeginPlay for DefaultGameMode, or restarting game mode in PauseMenu/PostGameMenuWidget)
	UFUNCTION(BlueprintCallable, Category = "GameModeActor Spawning")
		void InitializeGameMode();

	UFUNCTION(BlueprintCallable, Category = "GameModeActor Spawning")
		void SetAAManagerVolume(float GlobalVolume, float MusicVolume, UAudioAnalyzerManager* AAManager = nullptr);

	UFUNCTION(BlueprintCallable, Category = "GameModeActor Spawning")
		void PlayAAPlayer();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameModeActor Spawning")
		TSubclassOf<AGameModeActorBase> GameModeActorBaseClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameModeActor Spawning")
		TSubclassOf<AActor> VisualizerClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "GameModeActor Spawning")
		AActor* Visualizer;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "GameModeActor Spawning")
		AGameModeActorBase* GameModeActorBase;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AudioAnalyzer Settings")
		FAASettingsStruct AASettings;

	UFUNCTION(BlueprintCallable, Category = "Bound functions")
		void RefreshAASettings();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AudioAnalyzer Settings")
		bool FirstDelayTriggered;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AudioAnalyzer Settings")
		bool LastTargetOnSet;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AudioAnalyzer Settings")
		UAudioAnalyzerManager* AATracker;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AudioAnalyzer Settings")
		UAudioAnalyzerManager* AAPlayer;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AudioAnalyzer Settings")
		float DeltaTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AudioAnalyzer Settings")
		float FirstDelay;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AudioAnalyzer Settings")
		FString SongTitle;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AudioAnalyzer Settings")
		float TotalSongLength;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AudioAnalyzer Settings")
		float Elapsed;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, BlueprintAssignable, Category = "AudioAnalyzer Settings")
		FOnAAPlayerLoaded OnAAPlayerLoaded;

	UFUNCTION(BlueprintCallable, Category = "GameMode Initialization")
		FString OpenSongFileDialog(EWindowMode::Type WindowMode);
};
