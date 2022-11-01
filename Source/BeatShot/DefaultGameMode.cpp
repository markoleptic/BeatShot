// Copyright Epic Games, Inc. All Rights Reserved.

#include "DefaultGameMode.h"
#include "AudioAnalyzerManager.h"
#include "DefaultGameInstance.h"
#include "GameModeActorStruct.h"
#include "GameFramework/GameUserSettings.h"
#include "GameModeActorBase.h"
#include "DefaultPlayerController.h"
#include "TargetSpawner.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void ADefaultGameMode::BeginPlay()
{
	Super::BeginPlay();

	GI = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (GI)
	{
		GI->RegisterGameModeBase(this);
		// listen to changes that are made to Audio Analyzer settings in case user changes during a game
		GI->OnAASettingsChange.AddDynamic(this, &ADefaultGameMode::RefreshAASettings);
	}
	InitializeGameMode();
}

void ADefaultGameMode::InitializeAudioManagers()
{
	// resetting variables and unloading audio if restarting a game mode or choosing a new one
	if (AATracker) AATracker->UnloadPlayerAudio();
	if (AAPlayer) AAPlayer->UnloadPlayerAudio();

	// Show open file dialog and get the song path
	const FString SongPath = OpenSongFileDialog();
	if (SongPath.IsEmpty())
	{
		return;
	}

	AATracker = NewObject<UAudioAnalyzerManager>(this, TEXT("AATracker"));
	if (const bool InitPlayerSuccess = AATracker->InitPlayerAudio(SongPath); !InitPlayerSuccess)
	{
		UE_LOG(LogTemp, Display, TEXT("Init Tracker Error"));
	}
	AATracker->InitBeatTrackingConfigWLimits(
		EChannelSelectionMode::All_in_one, 0,
		AASettings.BandLimits, AASettings.TimeWindow, AASettings.HistorySize,
		false, 100, 50);

	// create AAPlayer if delay large enough, init audio, init spectrum config, broadcast to Visualizer
	if (GameModeActorBase->GameModeActorStruct.PlayerDelay > 0.05f)
	{
		AAPlayer = NewObject<UAudioAnalyzerManager>(this, TEXT("AAPlayer"));
		if (const bool InitPlayerSuccess = AAPlayer->InitPlayerAudio(SongPath); !InitPlayerSuccess)
		{
			UE_LOG(LogTemp, Display, TEXT("Init Player Error"));
		}
		AAPlayer->InitSpectrumConfigWLimits(
			EChannelSelectionMode::All_in_one, 0,
			AASettings.BandLimits, AASettings.TimeWindow, AASettings.HistorySize,
			true, 1);
		OnAAPlayerLoaded.Broadcast(AAPlayer);
	}
	// otherwise use AATracker
	else
	{
		AAPlayer = nullptr;
		AATracker->InitSpectrumConfigWLimits(
			EChannelSelectionMode::All_in_one, 0,
			AASettings.BandLimits, AASettings.TimeWindow, AASettings.HistorySize,
			true, 1);
		OnAAPlayerLoaded.Broadcast(AATracker);
	}

	// mute all by default
	if (AAPlayer)
	{
		SetAAManagerVolume(0, 0, AAPlayer);
	}
	SetAAManagerVolume(0, 0, AATracker);

	// set Song length and song title in GameModeActorStruct
	FString Filename, Extension, MetaType, Title, Artist, Album, Year, Genre;
	AATracker->GetMetadata(Filename, Extension, MetaType, Title,
		Artist, Album, Year, Genre);
	if (Title.Equals("")) GameModeActorBase->GameModeActorStruct.SongTitle = Filename; 
	else GameModeActorBase->GameModeActorStruct.SongTitle = Title;
	GameModeActorBase->GameModeActorStruct.GameModeLength = AATracker->GetTotalDuration();
}

void ADefaultGameMode::PauseAAManager(bool ShouldPause, UAudioAnalyzerManager* AAManager)
{
	if (AAManager)
	{
		AAManager->SetPaused(ShouldPause);
	}
	else 
	{
		if (AATracker)
		{
			AATracker->SetPaused(ShouldPause);
		}
		if (AAPlayer)
		{
			AAPlayer->SetPaused(ShouldPause);
		}
	}
}

void ADefaultGameMode::StartAAManagerPlayback()
{
	const UWorld* World = GetWorld();
	const FPlayerSettings Settings = GI->LoadPlayerSettings();

	// If delay is large enough, play AATracker and then AAPlayer after the delay
	if (GameModeActorBase->GameModeActorStruct.PlayerDelay > 0.05)
	{
		PauseAAManager(false, AATracker);
		AATracker->Play();
		FLatentActionInfo LatentInfo;
		LatentInfo.CallbackTarget = this;
		LatentInfo.Linkage = 0;
		LatentInfo.UUID = 0;
		LatentInfo.ExecutionFunction = FName("PlayAAPlayer");
		UKismetSystemLibrary::Delay(World, GameModeActorBase->GameModeActorStruct.PlayerDelay, LatentInfo);
	}
	else
	{
		PauseAAManager(false, AATracker);
		AATracker->Play();
		SetAAManagerVolume(Settings.GlobalVolume, Settings.MusicVolume, AATracker);
	}
}

void ADefaultGameMode::InitializeGameMode()
{
	// spawn GameModeActorBase
	GameModeActorBase = GetWorld()->SpawnActor<AGameModeActorBase>(GameModeActorBaseClass);

	// spawn TargetSpawner
	const FVector TargetSpawnerLocation = { 3590, 0, 750 };
	const FActorSpawnParameters TargetSpawnerSpawnParameters;
	TargetSpawner = Cast<ATargetSpawner>(GetWorld()->SpawnActor(TargetSpawnerClass, 
		&TargetSpawnerLocation, 
		&FRotator::ZeroRotator, 
		TargetSpawnerSpawnParameters));

	// get GameModeActorStruct and pass to GameModeActorBase
	GameModeActorBase->GameModeActorStruct = GI->GameModeActorStruct;

	// initialize GameModeActorStruct with TargetSpawner
	TargetSpawner->InitializeGameModeActor(GI->GameModeActorStruct);

	// spawn visualizer
	const FVector VisualizerLocation = { 100,0,60 };
	const FActorSpawnParameters SpawnParameters;
	Visualizer = GetWorld()->SpawnActor(VisualizerClass, 
		&VisualizerLocation, 
		&FRotator::ZeroRotator, 
		SpawnParameters);

	// Show countdown widget
	Cast<ADefaultPlayerController>(GetWorld()->GetFirstPlayerController())->ShowCountdown();

	RefreshAASettings();
	InitializeAudioManagers();
	PauseAAManager(true);
}

void ADefaultGameMode::SetAAManagerVolume(float GlobalVolume, float MusicVolume, UAudioAnalyzerManager* AAManager)
{
	if (AAManager)
	{
		AAManager->SetPlaybackVolume(GlobalVolume / 100 * MusicVolume / 100);
	}
	else
	{
		if (AAPlayer)
		{
			AAPlayer->SetPlaybackVolume(GlobalVolume / 100 * MusicVolume / 100);
		}
		else if (AATracker)
		{
			AATracker->SetPlaybackVolume(GlobalVolume / 100 * MusicVolume / 100);
		}
	}
}

void ADefaultGameMode::PlayAAPlayer()
{
	if (AAPlayer)
	{
		const FPlayerSettings Settings = GI->LoadPlayerSettings();
		PauseAAManager(false, AAPlayer);
		AAPlayer->Play();
		SetAAManagerVolume(Settings.GlobalVolume, Settings.MusicVolume, AAPlayer);
	}
}

void ADefaultGameMode::RefreshAASettings()
{
	AASettings = GI->LoadAASettings();
}

void ADefaultGameMode::EndGameMode(bool ShouldSavePlayerScores)
{
	if (GameModeActorBase)
	{
		GameModeActorBase->EndGameMode(ShouldSavePlayerScores);
		GameModeActorBase->Destroy();
	}
	if (TargetSpawner)
	{
		TargetSpawner->Destroy();
	}
	if (Visualizer)
	{
		Visualizer->Destroy();
	}
	if (AATracker)
	{
		AATracker = nullptr;
	}
	if (AAPlayer)
	{
		AAPlayer = nullptr;
	}
}

//{
//	if (GEngine) {
//		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("GameMode Init Error"));
//	}
//}


