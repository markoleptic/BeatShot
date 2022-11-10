// Copyright Epic Games, Inc. All Rights Reserved.

#include "DefaultGameMode.h"
#include "AudioAnalyzerManager.h"
#include "DefaultGameInstance.h"
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

void ADefaultGameMode::InitializeAudioManagers(FString SongFilePath)
{
	AATracker = NewObject<UAudioAnalyzerManager>(this);
	if (const bool InitPlayerSuccess = AATracker->InitPlayerAudio(SongFilePath); !InitPlayerSuccess)
	{
		UE_LOG(LogTemp, Display, TEXT("Init Tracker Error"));
	}
	AATracker->InitBeatTrackingConfigWLimits(
		EAA_ChannelSelectionMode::All_in_one, 0,
		AASettings.BandLimits, AASettings.TimeWindow, AASettings.HistorySize,
		false, 100, 50);

	// create AAPlayer if delay large enough, init audio, init spectrum config, broadcast to Visualizer
	if (GameModeActorBase->GameModeActorStruct.PlayerDelay > 0.05f)
	{
		AAPlayer = NewObject<UAudioAnalyzerManager>(this);
		if (const bool InitPlayerSuccess = AAPlayer->InitPlayerAudio(SongFilePath); !InitPlayerSuccess)
		{
			UE_LOG(LogTemp, Display, TEXT("Init Player Error"));
		}
		AAPlayer->InitSpectrumConfigWLimits(
			EAA_ChannelSelectionMode::All_in_one, 0,
			AASettings.BandLimits, AASettings.TimeWindow, AASettings.HistorySize,
			true, 1);
		OnAAPlayerLoaded.Broadcast(AAPlayer);
	}
	// otherwise use AATracker
	else
	{
		AAPlayer = nullptr;
		AATracker->InitSpectrumConfigWLimits(
			EAA_ChannelSelectionMode::All_in_one, 0,
			AASettings.BandLimits, AASettings.TimeWindow, AASettings.HistorySize,
			true, 1);
		OnAAPlayerLoaded.Broadcast(AATracker);
	}

	// mute all by default
	if (IsValid(AAPlayer))
	{
		SetAAManagerVolume(0, 0, AAPlayer);
	}
	SetAAManagerVolume(0, 0, AATracker);

	// set Song length and song title in GameModeActorStruct
	FString Filename, Extension, MetaType, Title, Artist, Album, Year, Genre;
	AATracker->GetMetadata(Filename, Extension, MetaType, Title,
		Artist, Album, Year, Genre);
	if (Title.Equals("")) {
		GameModeActorBase->GameModeActorStruct.SongTitle = Filename;
	}
	else {
		GameModeActorBase->GameModeActorStruct.SongTitle = Title;
	}
	GameModeActorBase->GameModeActorStruct.GameModeLength = AATracker->GetTotalDuration();
}

void ADefaultGameMode::PauseAAManager(bool ShouldPause, UAudioAnalyzerManager* AAManager)
{
	if (IsValid(AAManager))
	{
		AAManager->SetPaused(ShouldPause);
	}
	else 
	{
		if (IsValid(AATracker))
		{
			AATracker->SetPaused(ShouldPause);
		}
		if (IsValid(AAPlayer))
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
		if (AATracker)
		{
			AATracker->Play();
			FLatentActionInfo LatentInfo;
			LatentInfo.CallbackTarget = this;
			LatentInfo.Linkage = 0;
			LatentInfo.UUID = 0;
			LatentInfo.ExecutionFunction = FName("PlayAAPlayer");
			UKismetSystemLibrary::Delay(World, GameModeActorBase->GameModeActorStruct.PlayerDelay, LatentInfo);
		}
	}
	else
	{
		PauseAAManager(false, AATracker);
		if (AATracker)
		{
			AATracker->Play();
			SetAAManagerVolume(Settings.GlobalVolume, Settings.MusicVolume, AATracker);
		}
	}
}

void ADefaultGameMode::InitializeGameMode()
{
	FirstDelayTriggered = false;
	DeltaTime = 0.f;
	FirstDelay = 0.f;
	Elapsed = 0.f;

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

	RefreshAASettings();
	// call the blueprint function to display open file dialog window, which then
	// calls InitializeAudioManagers with the path to the song file,
	// and then displays the Countdown widget
	OpenSongFileDialog();
}

void ADefaultGameMode::SetAAManagerVolume(float GlobalVolume, float MusicVolume, UAudioAnalyzerManager* AAManager)
{
	if (IsValid(AAManager))
	{
		AAManager->SetPlaybackVolume(GlobalVolume / 100 * MusicVolume / 100);
	}
	else
	{
		if (IsValid(AAPlayer))
		{
			AAPlayer->SetPlaybackVolume(GlobalVolume / 100 * MusicVolume / 100);
		}
		else if (IsValid(AATracker))
		{
			AATracker->SetPlaybackVolume(GlobalVolume / 100 * MusicVolume / 100);
		}
	}
}

void ADefaultGameMode::PlayAAPlayer()
{
	if (IsValid(AAPlayer))
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
	if (IsValid(GameModeActorBase))
	{
		GameModeActorBase->EndGameMode(ShouldSavePlayerScores);
	}
	if (IsValid(TargetSpawner))
	{
		TargetSpawner->Destroy();
	}
	if (IsValid(Visualizer))
	{
		Visualizer->Destroy();
	}
	if (IsValid(AATracker))
	{
		AATracker = nullptr;
	}
	if (IsValid(AAPlayer))
	{
		AAPlayer = nullptr;
	}

	//Hide HUD and countdown
	GI->DefaultPlayerControllerRef->HidePlayerHUD();
	GI->DefaultPlayerControllerRef->HideCountdown();
	GI->DefaultPlayerControllerRef->HideCrosshair();
}



