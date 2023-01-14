// Copyright Epic Games, Inc. All Rights Reserved.

#include "DefaultGameMode.h"
#include "AudioAnalyzerManager.h"
#include "DefaultCharacter.h"
#include "DefaultGameInstance.h"
#include "DefaultHealthComponent.h"
#include "DefaultPlayerController.h"
#include "FloatingTextActor.h"
#include "TargetSpawner.h"
#include "Visualizer.h"
#include "GameFramework/GameUserSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetTextLibrary.h"

ADefaultGameMode::ADefaultGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ADefaultGameMode::BeginPlay()
{
	Super::BeginPlay();
	InitializeGameMode(Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(this))->bLastSavedShowOpenFileDialog);
}

void ADefaultGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (!bShouldTick || !GetWorldTimerManager().IsTimerActive(OnSecondPassedTimer))
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
		SpawnNewTarget(Beat);
	}
}

void ADefaultGameMode::InitializeGameMode(const bool bShowOpenFileDialog)
{
	Elapsed = 0.f;
	LastTargetOnSet = false;
	RefreshPlayerSettings(LoadPlayerSettings());
	GameModeActorStruct = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(this))->GameModeActorStruct;
	/* Broadcasting to DefaultCharacter */
	TargetSpawner = Cast<ATargetSpawner>(GetWorld()->SpawnActor(TargetSpawnerClass,
	                                                            &TargetSpawnerLocation,
	                                                            &FRotator::ZeroRotator,
	                                                            SpawnParameters));
	TargetSpawner->InitializeGameModeActor(GameModeActorStruct);
	Visualizer = Cast<AVisualizer>(GetWorld()->SpawnActor(VisualizerClass,
	                                                      &VisualizerLocation,
	                                                      &FRotator::ZeroRotator,
	                                                      SpawnParameters));

	if (bShowOpenFileDialog)
	{
		TArray<FString> FileNames = {""};
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
		InitializeAudioManagers(true, FileNames[0]);
	}
	else
	{
		InitializeAudioManagers(false, "");
	}
	Cast<ADefaultPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0))->ShowCountdown();
	bShouldTick = true;
}

void ADefaultGameMode::StartGameMode()
{
	ADefaultPlayerController* DefaultPlayerController = Cast<ADefaultPlayerController>(
		UGameplayStatics::GetPlayerController(GetWorld(), 0));
	DefaultPlayerController->ShowCrossHair();
	DefaultPlayerController->ShowPlayerHUD();
	DefaultPlayerController->HideCountdown();

	if (OnGameModeInit.ExecuteIfBound(GameModeActorStruct))
	{
		UE_LOG(LogTemp, Display, TEXT("OnGameModeInit not bound."));
	}

	LoadMatchingPlayerScores();

	/** Binding delegates */
	Cast<ADefaultCharacter>(
			Cast<ADefaultPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0))->GetPawn())->Gun->
		OnShotFired.BindUFunction(this, FName("UpdateShotsFired"));
	OnTargetSpawned.BindUFunction(this, FName("UpdateTargetsSpawned"));
	OnTargetDestroyed.BindUFunction(this, FName("UpdatePlayerScores"));
	OnStreakUpdate.AddUniqueDynamic(this, &ADefaultGameMode::OnStreakUpdateCallback);
	if (GameModeActorStruct.IsBeatTrackMode)
	{
		OnBeatTrackTargetSpawned.AddUFunction(this, FName("OnBeatTrackTargetSpawnedCallback"));
	}
	GetWorldTimerManager().SetTimer(GameModeLengthTimer, this, &ADefaultGameMode::OnGameModeLengthTimerComplete,
									GameModeActorStruct.GameModeLength, false);
	if (!UpdateScoresToHUD.ExecuteIfBound(CurrentPlayerScore))
	{
		UE_LOG(LogTemp, Display, TEXT("Initial UpdateScoresToHUD failed."));
	}

	TargetSpawner->SetShouldSpawn(true);
	GetWorldTimerManager().SetTimer(OnSecondPassedTimer, this, &ADefaultGameMode::OnSecondPassed, 1.f, true);
}

void ADefaultGameMode::EndGameMode(const bool ShouldSavePlayerScores, const bool ShowPostGameMenu)
{
	ADefaultPlayerController* Controller = Cast<ADefaultPlayerController>(
		UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (ShowPostGameMenu)
	{
		Controller->ShowPostGameMenu();
	}
	GetWorldTimerManager().ClearTimer(PlayerDelayTimer);
	GetWorldTimerManager().ClearTimer(OnSecondPassedTimer);

	//Hide HUD and countdown
	Controller->HidePlayerHUD();
	Controller->HideCountdown();
	Controller->HideCrossHair();

	if (TargetSpawner)
	{
		TargetSpawner->SetShouldSpawn(false);
		TargetSpawner->Destroy();
		TargetSpawner = nullptr;
	}

	if (const TArray<FPlayerScore> Scores = GetCompletedPlayerScores(); ShouldSavePlayerScores && !Scores.
		IsEmpty())
	{
		SavePlayerScores(Scores);
		if (const FPlayerSettings PlayerSettings = LoadPlayerSettings(); PlayerSettings.HasLoggedInHttp)
		{
			SaveScoresToDatabase(ShowPostGameMenu, PlayerSettings, Scores);
		}
	}
	/** Unbinding delegates */
	const ADefaultPlayerController* PlayerController = Cast<ADefaultPlayerController>(
		UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (Cast<ADefaultCharacter>(PlayerController->GetPawn())->Gun)
	{
		Cast<ADefaultCharacter>(PlayerController->GetPawn())->Gun->OnShotFired.Unbind();
	}
	if (OnStreakUpdate.IsBound())
	{
		OnStreakUpdate.RemoveDynamic(this, &ADefaultGameMode::OnStreakUpdateCallback);
	}
	if (OnTargetSpawned.IsBoundToObject(this))
	{
		OnTargetSpawned.Unbind();
	}
	if (OnTargetDestroyed.IsBoundToObject(this))
	{
		OnTargetDestroyed.Unbind();
	}

	if (Visualizer)
	{
		Visualizer->Destroy();
		Visualizer = nullptr;
	}
	if (AATracker)
	{
		AATracker->UnloadPlayerAudio();
		AATracker = nullptr;
	}
	if (AAPlayer)
	{
		AAPlayer->UnloadPlayerAudio();
		AAPlayer = nullptr;
	}
}

void ADefaultGameMode::SpawnNewTarget(const bool bNewTargetState)
{
	if (bNewTargetState && !LastTargetOnSet)
	{
		LastTargetOnSet = true;
		if (Elapsed > GameModeActorStruct.TargetSpawnCD)
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

void ADefaultGameMode::OnGameModeLengthTimerComplete()
{
	/** don't save scores if score is zero */
	if (CurrentPlayerScore.Score <= 0 ||
		(CurrentPlayerScore.GameModeActorName == EGameModeActorName::Custom &&
			CurrentPlayerScore.CustomGameModeName == ""))
	{
		UE_LOG(LogTemp, Display, TEXT("Shouldn't save player scores"));
		EndGameMode(false, true);
		return;
	}
	EndGameMode(true, true);
}

void ADefaultGameMode::StartAAManagerPlayback()
{
	const FPlayerSettings Settings = LoadPlayerSettings();

	// If delay is large enough, play AATracker and then AAPlayer after the delay
	if (GameModeActorStruct.PlayerDelay >= 0.01f)
	{
		if (AATracker)
		{
			PauseAAManager(false, AATracker);

			AATracker->Play();
			AATracker->StartLoopback(false);

			UE_LOG(LogTemp, Display, TEXT("Now Playing AATracker"));
			GetWorldTimerManager().SetTimer(PlayerDelayTimer, this, &ADefaultGameMode::PlayAAPlayer,
											GameModeActorStruct.PlayerDelay, false);
		}
	}
	else
	{
		if (AATracker)
		{
			PauseAAManager(false, AATracker);

			AATracker->Play();
			AATracker->StartLoopback(false);

			UE_LOG(LogTemp, Display, TEXT("Now Playing AATracker"));
			SetAAManagerVolume(Settings.GlobalVolume, Settings.MusicVolume, AATracker);
		}
	}
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

void ADefaultGameMode::InitializeAudioManagers(const bool bUseSongFromFile, const FString& SongFilePath)
{
	AASettings = LoadAASettings();
	/*
	AASettings.BandLimits = {
		FVector2d(0,100),
		FVector2d(101,150),
		FVector2d(151,200),
		FVector2d(201,400),
		FVector2d(401,999),
		FVector2d(1000,2000),
		FVector2d(2001,3000),
		FVector2d(3001,4000)
	};
	AASettings.BandLimitsThreshold = {2.1,2.1,2.1,2.1,2.1,2.1,2.1,2.1};
	AASettings.NumBandChannels = 8;
	Visualizer->UpdateAASettings(AASettings);
	*/
	AATracker = NewObject<UAudioAnalyzerManager>(this);
	TArray<FString> AudioDeviceList;
	AATracker->GetOutputAudioDevices(AudioDeviceList);
	for (FString String : AudioDeviceList)
	{
		UE_LOG(LogTemp, Display, TEXT("%s"), *String);
	}
	if (!bUseSongFromFile)
	{
		AATracker->SetDefaultDeviceLoopbackAudio(AudioDeviceList[3]);
		if (!AATracker->InitLoopbackAudio())
		{
			ShowSongPathErrorMessage();
			bShouldTick = false;
			UE_LOG(LogTemp, Display, TEXT("Init Tracker Error"));
			return;
		}
	}
	else
	{
		if (!AATracker->InitPlayerAudio(SongFilePath))
		{
			ShowSongPathErrorMessage();
			bShouldTick = false;
			UE_LOG(LogTemp, Display, TEXT("Init Tracker Error"));
			return;
		}
	}
	AATracker->InitBeatTrackingConfigWLimits(
		EAA_ChannelSelectionMode::All_in_one, 0,
		AASettings.BandLimits, AASettings.TimeWindow, AASettings.HistorySize,
		false, 100, 2.1);

	// create AAPlayer if delay large enough, init audio, init spectrum config, broadcast to Visualizer
	if (GameModeActorStruct.PlayerDelay >= 0.01f)
	{
		AAPlayer = NewObject<UAudioAnalyzerManager>(this);
		if (!bUseSongFromFile)
		{
			AAPlayer->SetDefaultDeviceLoopbackAudio(AudioDeviceList[3]);
			if (!AAPlayer->InitLoopbackAudio())
			{
				ShowSongPathErrorMessage();
				bShouldTick = false;
				UE_LOG(LogTemp, Display, TEXT("Init Tracker Error"));
				return;
			}
		}
		else
		{
			if (!AAPlayer->InitPlayerAudio(SongFilePath))
			{
				ShowSongPathErrorMessage();
				bShouldTick = false;
				UE_LOG(LogTemp, Display, TEXT("Init Player Error"));
				return;
			}
		}
		AAPlayer->InitBeatTrackingConfigWLimits(
			EAA_ChannelSelectionMode::All_in_one, 0,
			AASettings.BandLimits, AASettings.TimeWindow, AASettings.HistorySize,
			false, 100, 2.1);
		// AATracker->InitSpectrumConfigWLimits(
		// EAA_ChannelSelectionMode::All_in_one, 0,
		// AASettings.BandLimits, AASettings.TimeWindow, AASettings.HistorySize,
		// false, 1);
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
	if (bUseSongFromFile)
	{
		FString Filename, Extension, MetaType, Title, Artist, Album, Year, Genre;
		AATracker->GetMetadata(Filename, Extension, MetaType, Title,
							   Artist, Album, Year, Genre);
		if (Title.Equals(""))
		{
			GameModeActorStruct.SongTitle = Filename;
		}
		else
		{
			GameModeActorStruct.SongTitle = Title;
		}
		GameModeActorStruct.GameModeLength = AATracker->GetTotalDuration();
	}
}

void ADefaultGameMode::PlayAAPlayer()
{
	if (IsValid(AAPlayer))
	{
		const FPlayerSettings Settings = LoadPlayerSettings();
		PauseAAManager(false, AAPlayer);

		AAPlayer->Play();
		AAPlayer->StartLoopback(false);

		UE_LOG(LogTemp, Display, TEXT("Now Playing AAPlayer"));
		SetAAManagerVolume(Settings.GlobalVolume, Settings.MusicVolume, AAPlayer);
	}
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

void ADefaultGameMode::RefreshAASettings(const FAASettingsStruct& RefreshedAASettings)
{
	AASettings = RefreshedAASettings;
	Visualizer->UpdateAASettings(RefreshedAASettings);
}

void ADefaultGameMode::RefreshPlayerSettings(const FPlayerSettings& RefreshedPlayerSettings)
{
	SetAAManagerVolume(RefreshedPlayerSettings.GlobalVolume, RefreshedPlayerSettings.MusicVolume);
	bShowStreakCombatText = RefreshedPlayerSettings.bShowStreakCombatText;
	CombatTextFrequency = RefreshedPlayerSettings.CombatTextFrequency;
}

void ADefaultGameMode::LoadMatchingPlayerScores()
{
	const TArray<FPlayerScore> PlayerScores = LoadPlayerScores();
	if (GameModeActorStruct.GameModeActorName == EGameModeActorName::Custom)
	{
		for (FPlayerScore ScoreObject : PlayerScores)
		{
			if (ScoreObject.CustomGameModeName.Equals(GameModeActorStruct.CustomGameModeName) &&
				ScoreObject.SongTitle.Equals(GameModeActorStruct.SongTitle))
			{
				if (ScoreObject.Score > CurrentPlayerScore.HighScore)
				{
					CurrentPlayerScore.HighScore = ScoreObject.Score;
				}
			}
		}
	}
	else
	{
		for (FPlayerScore ScoreObject : PlayerScores)
		{
			if (ScoreObject.GameModeActorName == GameModeActorStruct.GameModeActorName &&
				ScoreObject.SongTitle.Equals(GameModeActorStruct.SongTitle) &&
				ScoreObject.Difficulty == GameModeActorStruct.GameModeDifficulty)
			{
				if (ScoreObject.Score > CurrentPlayerScore.HighScore)
				{
					CurrentPlayerScore.HighScore = ScoreObject.Score;
				}
			}
		}
	}
	CurrentPlayerScore.GameModeActorName = GameModeActorStruct.GameModeActorName;
	CurrentPlayerScore.SongTitle = GameModeActorStruct.SongTitle;
	CurrentPlayerScore.SongLength = GameModeActorStruct.GameModeLength;
	CurrentPlayerScore.CustomGameModeName = GameModeActorStruct.CustomGameModeName;
	CurrentPlayerScore.Difficulty = GameModeActorStruct.GameModeDifficulty;
	CurrentPlayerScore.TotalPossibleDamage = 0.f;
	MaxScorePerTarget = 100000.f / ((GameModeActorStruct.GameModeLength - 1.f) / GameModeActorStruct.TargetSpawnCD);
}

TArray<FPlayerScore> ADefaultGameMode::GetCompletedPlayerScores()
{
	/** don't save scores if score is zero */
	if (CurrentPlayerScore.Score <= 0 ||
		(CurrentPlayerScore.GameModeActorName == EGameModeActorName::Custom &&
			CurrentPlayerScore.CustomGameModeName == ""))
	{
		return TArray<FPlayerScore>();
	}

	/** save current time */
	CurrentPlayerScore.Time = FDateTime::UtcNow().ToIso8601();

	/** for BeatTrack modes */
	if (CurrentPlayerScore.TotalPossibleDamage > 0.01f)
	{
		CurrentPlayerScore.Accuracy = CheckFloatNaN(CurrentPlayerScore.Score / CurrentPlayerScore.TotalPossibleDamage,
													100);
		CurrentPlayerScore.Completion = CheckFloatNaN(CurrentPlayerScore.Score / CurrentPlayerScore.TotalPossibleDamage,
													  100);
	}
	else
	{
		CurrentPlayerScore.AvgTimeOffset = CheckFloatNaN(
			CurrentPlayerScore.TotalTimeOffset / CurrentPlayerScore.TargetsHit, 1000);
		CurrentPlayerScore.Accuracy = CheckFloatNaN(CurrentPlayerScore.TargetsHit / CurrentPlayerScore.ShotsFired, 100);
		CurrentPlayerScore.Completion = CheckFloatNaN(CurrentPlayerScore.TargetsHit / CurrentPlayerScore.TargetsSpawned,
													  100);
	}
	CurrentPlayerScore.HighScore = CheckFloatNaN(CurrentPlayerScore.HighScore, 100);
	CurrentPlayerScore.Score = CheckFloatNaN(CurrentPlayerScore.Score, 100);
	CurrentPlayerScore.SongLength = CheckFloatNaN(CurrentPlayerScore.SongLength, 100);

	TArray<FPlayerScore> Scores = LoadPlayerScores();
	Scores.Emplace(CurrentPlayerScore);
	UE_LOG(LogTemp, Display,
		   TEXT("Accuracy: %f \n Completion: %f \n AvgTimeOffset: %f \n HighScore: %f \n Score: %f \n SongLength: %f"),
		   CurrentPlayerScore.Accuracy, CurrentPlayerScore.Completion, CurrentPlayerScore.AvgTimeOffset,
		   CurrentPlayerScore.HighScore,
		   CurrentPlayerScore.Score, CurrentPlayerScore.SongLength);
	return Scores;
}

void ADefaultGameMode::SaveScoresToDatabase(const bool ShowPostGameMenu, const FPlayerSettings PlayerSettings,
											const TArray<FPlayerScore> Scores)
{
	ADefaultPlayerController* Controller = Cast<ADefaultPlayerController>(
		UGameplayStatics::GetPlayerController(GetWorld(), 0));
	/* Request access token by binding to the response of the delegate, and passing the delegate to RequestAccessToken */
	OnAccessTokenResponseDelegate.BindLambda(
		[this, Scores, PlayerSettings, Controller, ShowPostGameMenu](const FString AccessToken)
		{
			/* Post player scores by binding to the response of the delegate, and passing the delegate to PostPlayerScores */
			OnPostScoresResponse.BindLambda([this, Scores, Controller, ShowPostGameMenu](const ELoginState& LoginState)
			{
				if (LoginState == ELoginState::LoggedInHttp)
				{
					TArray<FPlayerScore> ScoresToUpdate = Scores;
					for (FPlayerScore& Score : ScoresToUpdate)
					{
						Score.bSavedToDatabase = true;
					}
					SavePlayerScores(ScoresToUpdate);
				}
				if (ShowPostGameMenu)
				{
					Controller->OnPostPlayerScoresResponse(ShowPostGameMenu, LoginState);
				}
			});
			if (!AccessToken.IsEmpty())
			{
				PostPlayerScores(Scores, PlayerSettings.Username, AccessToken, OnPostScoresResponse);
			}
		});
	RequestAccessToken(PlayerSettings.LoginCookie, OnAccessTokenResponseDelegate);
}

void ADefaultGameMode::UpdatePlayerScores(const float TimeElapsed)
{
	if (GameModeActorStruct.IsBeatTrackMode == true)
	{
		UE_LOG(LogTemp, Warning, TEXT("Error: trying to call UpdateScore() with BeatTrackMode"));
		return;
	}
	UpdateTargetsHit();
	if (TimeElapsed <= GameModeActorStruct.PlayerDelay - 0.05f)
	{
		CurrentPlayerScore.Score += FMath::Lerp(MaxScorePerTarget / 2, MaxScorePerTarget,
		                                        TimeElapsed / GameModeActorStruct.PlayerDelay);
	}
	else if (TimeElapsed <= GameModeActorStruct.PlayerDelay + 0.05f)
	{
		CurrentPlayerScore.Score += MaxScorePerTarget;
	}
	else if (TimeElapsed <= GameModeActorStruct.TargetMaxLifeSpan)
	{
		CurrentPlayerScore.Score += FMath::Lerp(MaxScorePerTarget, MaxScorePerTarget / 2,
		                                        (TimeElapsed - GameModeActorStruct.PlayerDelay + 0.05f) / (
			                                        GameModeActorStruct.TargetMaxLifeSpan - (GameModeActorStruct.
				                                        PlayerDelay + 0.05f)));
		// UE_LOG(LogTemp, Display, TEXT("Last: %f"), FMath::Lerp(MaxScorePerTarget, MaxScorePerTarget / 2,
		// (TimeElapsed - GameModeActorStruct.PlayerDelay + 0.05f) /
		// (GameModeActorStruct.TargetMaxLifeSpan - (GameModeActorStruct.PlayerDelay + 0.05f))))
	}
	UpdateHighScore();
	CurrentPlayerScore.TotalTimeOffset += FMath::Abs(TimeElapsed - GameModeActorStruct.PlayerDelay);
	if (!UpdateScoresToHUD.ExecuteIfBound(CurrentPlayerScore))
	{
		UE_LOG(LogTemp, Display, TEXT("UpdateScoresToHUD not bound."));
	}
}

void ADefaultGameMode::UpdateTrackingScore(const float DamageTaken, const float TotalPossibleDamage)
{
	CurrentPlayerScore.TotalPossibleDamage = TotalPossibleDamage;
	CurrentPlayerScore.Score += DamageTaken;
	UpdateHighScore();
	if (!UpdateScoresToHUD.ExecuteIfBound(CurrentPlayerScore))
	{
		UE_LOG(LogTemp, Display, TEXT("UpdateScoresToHUD not bound."));
	}
}

void ADefaultGameMode::UpdateTargetsSpawned()
{
	CurrentPlayerScore.TargetsSpawned++;
	if (!UpdateScoresToHUD.ExecuteIfBound(CurrentPlayerScore))
	{
		UE_LOG(LogTemp, Display, TEXT("UpdateScoresToHUD not bound."));
	}
}

void ADefaultGameMode::UpdateShotsFired()
{
	CurrentPlayerScore.ShotsFired++;
	if (!UpdateScoresToHUD.ExecuteIfBound(CurrentPlayerScore))
	{
		UE_LOG(LogTemp, Display, TEXT("UpdateScoresToHUD not bound."));
	}
}

void ADefaultGameMode::OnStreakUpdateCallback(const int32 Streak, const FVector Location)
{
	/** Only update best streak in PlayerScores and HUD */
	if (Streak > CurrentPlayerScore.Streak)
	{
		CurrentPlayerScore.Streak = Streak;
		if (!UpdateScoresToHUD.ExecuteIfBound(CurrentPlayerScore))
		{
			UE_LOG(LogTemp, Display, TEXT("UpdateScoresToHUD not bound."));
		}
	}
	if (bShowStreakCombatText)
	{
		if (CombatTextFrequency != 0 && Streak % CombatTextFrequency == 0)
		{
			if (AFloatingTextActor* FloatingTextActor = GetWorld()->SpawnActor<AFloatingTextActor>(
				FloatingTextActorClass, Location, FRotator()))
			{
				FloatingTextActor->Initialize(UKismetTextLibrary::Conv_IntToText(Streak));
			}
		}
	}
}

void ADefaultGameMode::UpdateTargetsHit()
{
	CurrentPlayerScore.TargetsHit++;
}

void ADefaultGameMode::UpdateHighScore()
{
	if (CurrentPlayerScore.Score > CurrentPlayerScore.HighScore)
	{
		CurrentPlayerScore.HighScore = CurrentPlayerScore.Score;
	}
}

void ADefaultGameMode::OnBeatTrackTargetSpawnedCallback(ASphereTarget* TrackingTarget)
{
	/** Update tracking score if a Tracking target has spawned */
	TrackingTarget->HealthComp->OnBeatTrackTick.BindUFunction(this, FName("UpdateTrackingScore"));
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

void ADefaultGameMode::ShowSongPathErrorMessage() const
{
	ADefaultPlayerController* PlayerController = Cast<ADefaultPlayerController>(
		UGameplayStatics::GetPlayerController(
			GetWorld(), 0));
	PlayerController->FadeScreenFromBlack();
	PlayerController->ShowSongPathErrorMessage();
}

float ADefaultGameMode::CheckFloatNaN(const float ValueToCheck, const float ValueToRound)
{
	if (!isnan(roundf(ValueToRound * ValueToCheck) / ValueToRound))
	{
		return roundf(ValueToRound * ValueToCheck) / ValueToRound;
	}
	return 0;
}