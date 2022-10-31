// Copyright Epic Games, Inc. All Rights Reserved.

#include "DefaultGameMode.h"
#include "AudioAnalyzerManager.h"
#include "DefaultGameInstance.h"
#include "GameModeActorStruct.h"
#include "GameFramework/GameUserSettings.h"
#include "GameModeActorBase.h"
#include "DefaultPlayerController.h"
#include "DesktopPlatformModule.h"
#include "IDesktopPlatform.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetStringLibrary.h"
#include "Engine/GameEngine.h"
#include "SlateCore.h"
#include "Framework/Application/SlateApplication.h"

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
	const FString SongPath = OpenSongFileDialog(UGameUserSettings::GetGameUserSettings()->GetFullscreenMode());

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
		UE_LOG(LogTemp, Display, TEXT("Creating AAPlayer"));
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
	UE_LOG(LogTemp, Display, TEXT("StartAAManagerPlayback Called"));
	// If delay is large enough, play AATracker and then AAPlayer after the delay
	if (GameModeActorBase->GameModeActorStruct.PlayerDelay > 0.05)
	{
		PauseAAManager(false, AATracker);
		AATracker->Play();
		UE_LOG(LogTemp, Display, TEXT("Tracker Playing"));
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
		UE_LOG(LogTemp, Display, TEXT("Tracker Only Playing with PlayerDelay < 0.05"));
		SetAAManagerVolume(Settings.GlobalVolume, Settings.MusicVolume, AATracker);
	}
}

void ADefaultGameMode::InitializeGameMode()
{
	// spawn GameModeActorBase
	GameModeActorBase = GetWorld()->SpawnActor<AGameModeActorBase>(GameModeActorBaseClass);

	// initialize AA settings
	RefreshAASettings();

	// spawn visualizer
	const FVector VisualizerLocation = { 100,0,60 };
	const FActorSpawnParameters SpawnParameters;
	Visualizer = GetWorld()->SpawnActor(VisualizerClass, &VisualizerLocation, &FRotator::ZeroRotator, SpawnParameters);

	// call blueprint function
	InitializeAudioManagers();

	// Call blueprint function
	PauseAAManager(true);

	// Show countdown widget
	Cast<ADefaultPlayerController>(GetWorld()->GetFirstPlayerController())->ShowCountdown();
}

void ADefaultGameMode::SetAAManagerVolume(float GlobalVolume, float MusicVolume, UAudioAnalyzerManager* AAManager)
{
	if (AAManager)
	{
		AAManager->SetPlaybackVolume(GlobalVolume / 100 * MusicVolume / 100);
		UE_LOG(LogTemp, Display, TEXT("AAManVolume: %f"), GlobalVolume / 100 * MusicVolume / 100);
	}
	else
	{
		if (AAPlayer)
		{
			AAPlayer->SetPlaybackVolume(GlobalVolume / 100 * MusicVolume / 100);
			UE_LOG(LogTemp, Display, TEXT("AAPlayer: %f"), GlobalVolume / 100 * MusicVolume / 100);
		}
		else if (AATracker)
		{
			AATracker->SetPlaybackVolume(GlobalVolume / 100 * MusicVolume / 100);
			UE_LOG(LogTemp, Display, TEXT("AATracker: %f"), GlobalVolume / 100 * MusicVolume / 100);
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
		UE_LOG(LogTemp, Display, TEXT("Player Playing"));
		SetAAManagerVolume(Settings.GlobalVolume, Settings.MusicVolume, AAPlayer);
	}
}

void ADefaultGameMode::RefreshAASettings()
{
	AASettings = GI->LoadAASettings();
}

FString ADefaultGameMode::OpenSongFileDialog(EWindowMode::Type WindowMode)
{
	FString SongPath;
	// scuffed fix for open file dialog while fullscreen
	if (WindowMode == EWindowMode::Fullscreen)
	{
		UGameUserSettings::GetGameUserSettings()->SetFullscreenMode(EWindowMode::WindowedFullscreen);
		UGameUserSettings::GetGameUserSettings()->ApplySettings(false);
	}

	// open file dialog
	TArray<FString> OutFileNames;
	if (IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get())
	{
		const void* ParentWindowPtr = FSlateApplication::Get().GetActiveTopLevelWindow()->GetNativeWindow()->GetOSWindowHandle();
		const uint32 SelectionFlag = 0;
		const FString Extensions = "Audio files|*.mp3;*.ogg";
		DesktopPlatform->OpenFileDialog(ParentWindowPtr,
			"Choose A Song", "/", "/",
			Extensions, SelectionFlag, OutFileNames);
	}
	if (OutFileNames.Num() == 0)
	{
		UE_LOG(LogTemp, Display, TEXT("No file selected"));
	}
	else
	{
		SongPath = OutFileNames[0];
		const FString WeirdPath = "../../../../../../";
		const FString CPath = "C:/";
		SongPath = UKismetStringLibrary::Replace(SongPath, WeirdPath, CPath);
	}

	if (WindowMode == EWindowMode::Fullscreen)
	{
		UGameUserSettings::GetGameUserSettings()->SetFullscreenMode(EWindowMode::Fullscreen);
		UGameUserSettings::GetGameUserSettings()->ApplySettings(false);
	}

	return SongPath;
}

//{
//	if (GEngine) {
//		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("GameMode Init Error"));
//	}
//}


