// Copyright Epic Games, Inc. All Rights Reserved.

#include "DefaultGameMode.h"
#include "AudioAnalyzerManager.h"
#include "DefaultGameInstance.h"
#include "GameModeActorBase.h"
#include "DefaultPlayerController.h"
#include "TargetSpawner.h"
#include "Kismet/GameplayStatics.h"

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
	bShouldTick = true;
	InitializeGameMode();
}

void ADefaultGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (!bShouldTick || !GameModeActorBase || !GetWorldTimerManager().IsTimerActive(GameModeActorBase->GameModeLengthTimer))
	{
		return;
	}

	//DeltaTime = DeltaSeconds;
	Elapsed += DeltaSeconds;

	TArray<bool> Beats;
	TArray<float> SpectrumValues;
	TArray<int32> BPMCurrent;
	TArray<int32> BPMTotal;
	AATracker->GetBeatTrackingWLimitsWThreshold(Beats, SpectrumValues, BPMCurrent, BPMTotal,
	                                            AASettings.BandLimitsThreshold);

	for (const bool Beat : Beats)
	{
		UpdateTargetSpawn(Beat);
	}
}

void ADefaultGameMode::InitializeAudioManagers(FString SongFilePath)
{
	if (SongFilePath.IsEmpty())
	{
		ShowSongPathErrorMessage();
		bShouldTick = false;
		return;
	}

	AATracker = NewObject<UAudioAnalyzerManager>(this);
	if (!AATracker->InitPlayerAudio(SongFilePath))
	{
		ShowSongPathErrorMessage();
		bShouldTick = false;
		UE_LOG(LogTemp, Display, TEXT("Init Tracker Error"));
		return;
	}

	AATracker->InitBeatTrackingConfigWLimits(
		EAA_ChannelSelectionMode::All_in_one, 0,
		AASettings.BandLimits, AASettings.TimeWindow, AASettings.HistorySize,
		false, 100, 50);

	// create AAPlayer if delay large enough, init audio, init spectrum config, broadcast to Visualizer
	if (GameModeActorBase->GameModeActorStruct.PlayerDelay > 0.05f)
	{
		AAPlayer = NewObject<UAudioAnalyzerManager>(this);
		if (!AAPlayer->InitPlayerAudio(SongFilePath))
		{
			ShowSongPathErrorMessage();
			bShouldTick = false;
			UE_LOG(LogTemp, Display, TEXT("Init Player Error"));
			return;
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
	if (Title.Equals(""))
	{
		GameModeActorBase->GameModeActorStruct.SongTitle = Filename;
	}
	else
	{
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
	const FPlayerSettings Settings = GI->LoadPlayerSettings();

	// If delay is large enough, play AATracker and then AAPlayer after the delay
	if (GameModeActorBase->GameModeActorStruct.PlayerDelay > 0.05)
	{
		if (AATracker)
		{
			PauseAAManager(false, AATracker);
			AATracker->Play();
			UE_LOG(LogTemp, Display, TEXT("Now Playing AATracker"));
			GetWorldTimerManager().SetTimer(PlayerDelayTimer,this, &ADefaultGameMode::PlayAAPlayer, GameModeActorBase->GameModeActorStruct.PlayerDelay, false);
		}
	}
	else
	{
		if (AATracker)
		{
			PauseAAManager(false, AATracker);
			AATracker->Play();
			UE_LOG(LogTemp, Display, TEXT("Now Playing AATracker"));
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
	OnGameModeActorInit.Broadcast(GI->GameModeActorStruct);

	// spawn TargetSpawner
	const FVector TargetSpawnerLocation = {3590, 0, 750};
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
	const FVector VisualizerLocation = {100, 0, 60};
	const FActorSpawnParameters SpawnParameters;
	Visualizer = GetWorld()->SpawnActor(VisualizerClass,
	                                    &VisualizerLocation,
	                                    &FRotator::ZeroRotator,
	                                    SpawnParameters);

	AASettings = GI->LoadAASettings();
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
		UE_LOG(LogTemp, Display, TEXT("Now Playing AAPlayer"));
		SetAAManagerVolume(Settings.GlobalVolume, Settings.MusicVolume, AAPlayer);
	}
}

void ADefaultGameMode::RefreshAASettings(FAASettingsStruct RefreshedAASettings)
{
	AASettings = RefreshedAASettings;
}

void ADefaultGameMode::EndGameMode(bool ShouldSavePlayerScores)
{
	//Hide HUD and countdown
	GI->DefaultPlayerControllerRef->HidePlayerHUD();
	GI->DefaultPlayerControllerRef->HideCountdown();
	GI->DefaultPlayerControllerRef->HideCrosshair();
	
	GetWorldTimerManager().ClearTimer(PlayerDelayTimer);
	
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
}

void ADefaultGameMode::ShowSongPathErrorMessage() const
{
	ADefaultPlayerController* PlayerController = Cast<ADefaultPlayerController>(
		UGameplayStatics::GetPlayerController(
			GetWorld(), 0));
	UPopupMessageWidget* PopupMessageWidget = PlayerController->CreatePopupMessageWidget(true, 1);
	PopupMessageWidget->InitPopup("Error",
	                              "There was a problem loading the song. Make sure the song is in mp3 or ogg format. If this problem persists, please contact support.",
	                              "Okay");
	PlayerController->ShowPopupMessage();
}

void ADefaultGameMode::UpdateTargetSpawn(bool bNewTargetState)
{
	if (bNewTargetState && !LastTargetOnSet)
	{
		LastTargetOnSet = true;
		if (Elapsed > GameModeActorBase->GameModeActorStruct.TargetSpawnCD)
		{
			Elapsed = 0.f;
			TargetSpawner->CallSpawnFunction();
		}
	}
	else if (!bNewTargetState && LastTargetOnSet)
	{
		LastTargetOnSet = false;
	}
}
