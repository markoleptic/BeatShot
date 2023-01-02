// Copyright Epic Games, Inc. All Rights Reserved.

#include "DefaultGameMode.h"
#include "AudioAnalyzerManager.h"
#include "DefaultGameInstance.h"
#include "GameModeActorBase.h"
#include "DefaultPlayerController.h"
#include "TargetSpawner.h"
#include "Components/Button.h"
#include "GameFramework/GameUserSettings.h"
#include "Kismet/GameplayStatics.h"

void ADefaultGameMode::BeginPlay()
{
	Super::BeginPlay();
	PrimaryActorTick.bCanEverTick = true;
	if (UDefaultGameInstance* GI = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		/** listen to changes that are made to Audio Analyzer settings in case user changes during a game */
		GI->OnAASettingsChange.AddDynamic(this, &ADefaultGameMode::RefreshAASettings);
		/** listen to changes that are made to Player settings in case music volume changes during a game */
		GI->OnPlayerSettingsChange.AddDynamic(this, &ADefaultGameMode::RefreshPlayerSettings);
	}
	InitializeGameMode();
}

void ADefaultGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (!bShouldTick || !GameModeActorBase || !GetWorldTimerManager().IsTimerActive(
		GameModeActorBase->GameModeLengthTimer))
	{
		return;
	}
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

void ADefaultGameMode::InitializeAudioManagers(const FString& SongFilePath)
{
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

void ADefaultGameMode::PauseAAManager(const bool ShouldPause, UAudioAnalyzerManager* AAManager)
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
	const FPlayerSettings Settings = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(this))->
		LoadPlayerSettings();

	// If delay is large enough, play AATracker and then AAPlayer after the delay
	if (GameModeActorBase->GameModeActorStruct.PlayerDelay > 0.05)
	{
		if (AATracker)
		{
			PauseAAManager(false, AATracker);
			AATracker->Play();
			UE_LOG(LogTemp, Display, TEXT("Now Playing AATracker"));
			GetWorldTimerManager().SetTimer(PlayerDelayTimer, this, &ADefaultGameMode::PlayAAPlayer,
			                                GameModeActorBase->GameModeActorStruct.PlayerDelay, false);
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

void ADefaultGameMode::StartGameMode()
{
	ADefaultPlayerController* DefaultPlayerController = Cast<ADefaultPlayerController>(
		UGameplayStatics::GetPlayerController(GetWorld(), 0));
	DefaultPlayerController->ShowCrosshair();
	DefaultPlayerController->ShowPlayerHUD();
	DefaultPlayerController->HideCountdown();
	GameModeActorBase->StartGameMode();
	GetWorldTimerManager().SetTimer(OnSecondPassedTimer, this, &ADefaultGameMode::OnSecondPassed, 1.f, true);
}

void ADefaultGameMode::InitializeGameMode()
{
	const UDefaultGameInstance* GI = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(this));
	LastTargetOnSet = false;
	Elapsed = 0.f;

	// spawn GameModeActorBase
	GameModeActorBase = GetWorld()->SpawnActor<AGameModeActorBase>(GameModeActorBaseClass);
	OnGameModeActorInit.ExecuteIfBound(GI->GameModeActorStruct);
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
	TArray<FString> FileNames;
	OpenSongFileDialog(FileNames);
	if (bWasInFullScreenMode)
	{
		UGameUserSettings* GameUserSettings = UGameUserSettings::GetGameUserSettings();
		GameUserSettings->SetFullscreenMode(EWindowMode::Fullscreen);
		GameUserSettings->ApplySettings(false);
		bWasInFullScreenMode = false;
	}
	if (FileNames.Num() == 0 || FileNames[0].IsEmpty())
	{
		ShowSongPathErrorMessage();
		bShouldTick = false;
		EndGameMode(false, false);
		return;
	}
	Cast<ADefaultPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0))->ShowCountdown();
	InitializeAudioManagers(FileNames[0]);
	bShouldTick = true;
}

void ADefaultGameMode::SetAAManagerVolume(const float GlobalVolume, const float MusicVolume,
                                          UAudioAnalyzerManager* AAManager)
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
		const FPlayerSettings Settings = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(this))->LoadPlayerSettings();
		PauseAAManager(false, AAPlayer);
		AAPlayer->Play();
		UE_LOG(LogTemp, Display, TEXT("Now Playing AAPlayer"));
		SetAAManagerVolume(Settings.GlobalVolume, Settings.MusicVolume, AAPlayer);
	}
}

void ADefaultGameMode::OpenSongFileDialog_Implementation(TArray<FString>& OutFileNames)
{
	/** Cheap fix to make sure open file dialog is always on top of the game */
	if (UGameUserSettings* GameUserSettings = UGameUserSettings::GetGameUserSettings(); GameUserSettings->
		GetFullscreenMode() == EWindowMode::Fullscreen)
	{
		bWasInFullScreenMode = true;
		GameUserSettings->SetFullscreenMode(EWindowMode::WindowedFullscreen);
		GameUserSettings->ApplySettings(false);
	}
}

void ADefaultGameMode::RefreshAASettings(const FAASettingsStruct& RefreshedAASettings)
{
	AASettings = RefreshedAASettings;
}

void ADefaultGameMode::RefreshPlayerSettings(const FPlayerSettings& RefreshedPlayerSettings)
{
	SetAAManagerVolume(RefreshedPlayerSettings.GlobalVolume, RefreshedPlayerSettings.MusicVolume);
}

void ADefaultGameMode::EndGameMode(const bool ShouldSavePlayerScores, const bool ShowPostGameMenu)
{
	ADefaultPlayerController* Controller = Cast<ADefaultPlayerController>(
		UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (ShowPostGameMenu)
	{
		Controller->ShowPostGameMenu(ShouldSavePlayerScores);
	}
	GetWorldTimerManager().ClearTimer(PlayerDelayTimer);
	GetWorldTimerManager().ClearTimer(OnSecondPassedTimer);

	//Hide HUD and countdown
	Controller->HidePlayerHUD();
	Controller->HideCountdown();
	Controller->HideCrosshair();

	if (IsValid(GameModeActorBase))
	{
		if (ShouldSavePlayerScores)
		{
			GameModeActorBase->SavePlayerScores();
		}
		GameModeActorBase->Destroy();
	}
	if (IsValid(TargetSpawner))
	{
		TargetSpawner->SetShouldSpawn(false);
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
	PlayerController->FadeScreenFromBlack();
	if (PlayerController->IsPostGameMenuActive())
	{
		PlayerController->HidePostGameMenu();
	}
	const UPopupMessageWidget* PopupMessageWidget = PlayerController->CreatePopupMessageWidget(true, 1);
	PopupMessageWidget->InitPopup("Error",
	                              "There was a problem loading the song. Make sure the song is in mp3 or ogg format. If this problem persists, please contact support.",
	                              "Okay");
	PopupMessageWidget->Button1->OnClicked.AddDynamic(PlayerController, &ADefaultPlayerController::HidePopupMessage);
}

void ADefaultGameMode::UpdateTargetSpawn(const bool bNewTargetState)
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

void ADefaultGameMode::OnSecondPassed()
{
	if (OnAAManagerSecondPassed.IsBound())
	{
		if (IsValid(AAPlayer))
		{
			OnAAManagerSecondPassed.Execute(AAPlayer->GetPlaybackTime());
		}
		else if (IsValid(AATracker))
		{
			OnAAManagerSecondPassed.Execute(AATracker->GetPlaybackTime());
		}
	}
}
