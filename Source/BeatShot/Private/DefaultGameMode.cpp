// Copyright Epic Games, Inc. All Rights Reserved.

#include "DefaultGameMode.h"
#include "AudioAnalyzerManager.h"
#include "VisualizerManager.h"
#include "DefaultCharacter.h"
#include "DefaultGameInstance.h"
#include "DefaultHealthComponent.h"
#include "DefaultPlayerController.h"
#include "FloatingTextActor.h"
#include "TargetSpawner.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetTextLibrary.h"

DEFINE_LOG_CATEGORY(LogAudioData);

ADefaultGameMode::ADefaultGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ADefaultGameMode::BeginPlay()
{
	Super::BeginPlay();
	InitializeGameMode();
	OnPostScoresResponse.AddUFunction(this, "OnPostScoresResponseReceived");
	OnAccessTokenResponse.BindUFunction(this, "OnAccessTokenResponseReceived");
	OnPostScoresResponse.AddUFunction(
		Cast<ADefaultPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)),
		"OnPostScoresResponseReceived");
	Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()))->OnPlayerSettingsChange.AddUniqueDynamic(
		this, &ADefaultGameMode::RefreshPlayerSettings);
}

void ADefaultGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (!bShouldTick || !GetWorldTimerManager().IsTimerActive(GameModeLengthTimer))
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

	switch (UsingAAPlayer)
	{
	case 0:
		{
			AATracker->GetBeatTrackingAverageAndVariance(VisualizerManager->SpectrumVariance,
			                                             VisualizerManager->AvgSpectrumValues);
			AATracker->GetSpectrumPeaks(VisualizerManager->SpectrumPeaks, VisualizerManager->SpectrumPeakEnergy);
			break;
		}
	default:
		{
			AAPlayer->GetBeatTrackingWLimitsWThreshold(Beats, SpectrumValues, BPMCurrent, BPMTotal,
			                                           AASettings.BandLimitsThreshold);
			AAPlayer->GetBeatTrackingAverageAndVariance(VisualizerManager->SpectrumVariance,
			                                            VisualizerManager->AvgSpectrumValues);
			AAPlayer->GetSpectrumPeaks(VisualizerManager->SpectrumPeaks, VisualizerManager->SpectrumPeakEnergy);
			break;
		}
	}
	VisualizerManager->UpdateVisualizers(SpectrumValues);
}

void ADefaultGameMode::InitializeGameMode()
{
	Elapsed = 0.f;
	LastTargetOnSet = false;
	RefreshPlayerSettings(LoadPlayerSettings());
	GameModeActorStruct = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(this))->GameModeActorStruct;
	TargetSpawner = Cast<ATargetSpawner>(GetWorld()->SpawnActor(TargetSpawnerClass,
	                                                            &TargetSpawnerLocation,
	                                                            &FRotator::ZeroRotator,
	                                                            SpawnParameters));
	TargetSpawner->InitializeGameModeActor(GameModeActorStruct);
	VisualizerManager = GetWorld()->SpawnActor<AVisualizerManager>(VisualizerManagerClass);
	VisualizerManager->InitializeVisualizers(LoadPlayerSettings());
	InitializeAudioManagers();
	Cast<ADefaultPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0))->ShowCountdown();
	bShouldTick = true;
}

void ADefaultGameMode::StartGameMode()
{
	if (!OnGameModeInit.ExecuteIfBound(GameModeActorStruct.IsBeatTrackMode))
	{
		UE_LOG(LogTemp, Display, TEXT("OnGameModeInit not bound."));
	}

	ADefaultPlayerController* DefaultPlayerController = Cast<ADefaultPlayerController>(
		UGameplayStatics::GetPlayerController(GetWorld(), 0));
	DefaultPlayerController->ShowCrossHair();
	DefaultPlayerController->ShowPlayerHUD();
	DefaultPlayerController->HideCountdown();

	LoadMatchingPlayerScores();
	BindGameModeDelegates();

	if (!UpdateScoresToHUD.ExecuteIfBound(CurrentPlayerScore))
	{
		UE_LOG(LogTemp, Display, TEXT("Initial UpdateScoresToHUD failed."));
	}
	StartGameModeTimers();
	TargetSpawner->SetShouldSpawn(true);
}

void ADefaultGameMode::StartGameModeTimers()
{
	if (GameModeActorStruct.GameModeLength == 0.f)
	{
		GetWorldTimerManager().SetTimer(GameModeLengthTimer, 31536000, false);
	}
	else
	{
		GetWorldTimerManager().SetTimer(GameModeLengthTimer, this, &ADefaultGameMode::OnGameModeLengthTimerComplete,
		                                GameModeActorStruct.GameModeLength, false);
	}
	GetWorldTimerManager().SetTimer(OnSecondPassedTimer, this, &ADefaultGameMode::OnSecondPassed, 1.f, true);
}

void ADefaultGameMode::BindGameModeDelegates()
{
	Cast<ADefaultCharacter>(
			Cast<ADefaultPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0))->GetPawn())->Gun->
		OnShotFired.BindUFunction(this, FName("UpdateShotsFired"));
	OnTargetSpawned.BindUFunction(this, FName("UpdateTargetsSpawned"));
	OnTargetDestroyed.AddUniqueDynamic(this, &ADefaultGameMode::UpdatePlayerScores);
	if (GameModeActorStruct.IsBeatTrackMode)
	{
		OnBeatTrackTargetSpawned.AddUFunction(this, FName("OnBeatTrackTargetSpawnedCallback"));
	}
}

void ADefaultGameMode::EndGameMode(const bool ShouldSavePlayerScores, const bool ShowPostGameMenu)
{
	GetWorldTimerManager().ClearTimer(GameModeLengthTimer);
	GetWorldTimerManager().ClearTimer(PlayerDelayTimer);
	GetWorldTimerManager().ClearTimer(OnSecondPassedTimer);

	if (TargetSpawner)
	{
		TargetSpawner->SetShouldSpawn(false);
		TargetSpawner->Destroy();
		TargetSpawner = nullptr;
	}

	HandleScoreSaving(ShouldSavePlayerScores);

	/** Unbinding delegates */
	if (const ADefaultPlayerController* PlayerController = Cast<ADefaultPlayerController>(
			UGameplayStatics::GetPlayerController(GetWorld(), 0)); Cast<ADefaultCharacter>(PlayerController->GetPawn())
		->Gun)
	{
		Cast<ADefaultCharacter>(PlayerController->GetPawn())->Gun->OnShotFired.Unbind();
	}

	if (VisualizerManager)
	{
		VisualizerManager->DestroyVisualizers();
		VisualizerManager->Destroy();
	}
	
	if (AATracker)
	{
		AATracker->UnloadCapturerAudio();
		AATracker->UnloadPlayerAudio();
		AATracker = nullptr;
	}
	if (AAPlayer)
	{
		AAPlayer->UnloadCapturerAudio();
		AAPlayer->UnloadPlayerAudio();
		AAPlayer = nullptr;
	}

	ADefaultPlayerController* Controller = Cast<ADefaultPlayerController>(
	UGameplayStatics::GetPlayerController(GetWorld(), 0));
	//Hide HUD and countdown
	Controller->HidePlayerHUD();
	Controller->HideCountdown();
	Controller->HideCrossHair();
	
	if (ShowPostGameMenu)
	{
		Controller->ShowPostGameMenu();
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
		EndGameMode(false, true);
		return;
	}
	EndGameMode(true, true);
}

void ADefaultGameMode::StartAAManagerPlayback()
{
	if (!AATracker)
	{
		return;
	}

	/* If delay is large enough, play AATracker and then AAPlayer after the delay */
	if (GameModeActorStruct.PlayerDelay >= 0.01f)
	{
		GetWorldTimerManager().SetTimer(PlayerDelayTimer, this, &ADefaultGameMode::PlayAAPlayer,
		                                GameModeActorStruct.PlayerDelay, false);
	}
	if (!GameModeActorStruct.SongPath.IsEmpty())
	{
		AATracker->Play();
	}
	else
	{
		if (GameModeActorStruct.PlayerDelay < 0.01f)
		{
			/* Using capture audio */
			if (GameModeActorStruct.SongPath.IsEmpty())
			{
				AATracker->StartCapture(GameModeActorStruct.bPlaybackAudio, false);
				SetAAManagerVolume(LoadPlayerSettings().GlobalVolume, LoadPlayerSettings().MusicVolume, AATracker);
			}
			/* BeatTrack or game mode with no delay */
			else
			{
				AATracker->Play();
				SetAAManagerVolume(LoadPlayerSettings().GlobalVolume, LoadPlayerSettings().MusicVolume, AATracker);
			}
		}
		else
		{
			AATracker->StartCapture(false, false);
		}
	}
	UE_LOG(LogTemp, Display, TEXT("Now Playing AATracker"));
}

void ADefaultGameMode::PauseAAManager(const bool ShouldPause)
{
	if (AATracker != nullptr)
	{
		if (!GameModeActorStruct.SongPath.IsEmpty())
		{
			AATracker->SetPaused(ShouldPause);
		}
		else
		{
			if (ShouldPause)
			{
				AATracker->StopCapture();
			}
			else
			{
				if (GameModeActorStruct.PlayerDelay < 0.01f)
				{
					AATracker->StartCapture(GameModeActorStruct.bPlaybackAudio, false);
				}
				else
				{
					AATracker->StartCapture(false, false);
				}
			}
		}
	}
	if (AAPlayer != nullptr)
	{
		if (!GameModeActorStruct.SongPath.IsEmpty())
		{
			AAPlayer->SetPaused(ShouldPause);
		}
		else
		{
			if (ShouldPause)
			{
				AAPlayer->StopCapture();
			}
			else
			{
				AAPlayer->StartCapture(GameModeActorStruct.bPlaybackAudio, false);
			}
		}
	}
}

void ADefaultGameMode::InitializeAudioManagers()
{
	AASettings = LoadAASettings();
	UsingAAPlayer = 0;
	AATracker = NewObject<UAudioAnalyzerManager>(this);

	switch (GameModeActorStruct.AudioFormat)
	{
	case EAudioFormat::Capture:
		AATracker->SetDefaultDevicesCapturerAudio(*GameModeActorStruct.InAudioDevice,
		                                          *GameModeActorStruct.OutAudioDevice);
		if (!AATracker->InitCapturerAudioEx(48000, EAA_AudioDepth::B_16, EAA_AudioFormat::Signed_Int, 1.f,
		                                    GameModeActorStruct.bPlaybackAudio))
		{
			bShouldTick = false;
			UE_LOG(LogTemp, Display, TEXT("Init Tracker Error"));
			return;
		}
		break;
	case EAudioFormat::File:
		if (!AATracker->InitPlayerAudio(GameModeActorStruct.SongPath))
		{
			bShouldTick = false;
			UE_LOG(LogTemp, Display, TEXT("Init Tracker Error"));
			return;
		}
		if (GameModeActorStruct.SongTitle.IsEmpty())
		{
			/* set Song length and song title in GameModeActorStruct if using song file */
			FString Filename, Extension, MetaType, Title, Artist, Album, Year, Genre;
			AATracker->GetMetadata(Filename, Extension, MetaType, Title,
			                       Artist, Album, Year, Genre);
			if (Title.IsEmpty())
			{
				GameModeActorStruct.SongTitle = Filename;
			}
			else
			{
				GameModeActorStruct.SongTitle = Title;
			}
		}
		GameModeActorStruct.GameModeLength = AATracker->GetTotalDuration();
		break;
	case EAudioFormat::None: break;
	}

	AATracker->InitBeatTrackingConfigWLimits(
		EAA_ChannelSelectionMode::All_in_one, 0,
		AASettings.BandLimits, AASettings.TimeWindow, 10 / AASettings.TimeWindow,
		false, 100, 2.1);
	AATracker->InitSpectrumConfigWLimits(EAA_ChannelSelectionMode::All_in_one, -1, AASettings.BandLimits,
	                                     AASettings.TimeWindow, 10 / AASettings.TimeWindow, true,
	                                     AASettings.NumBandChannels);
	//AATracker->InitPitchTrackingConfig(EAA_ChannelSelectionMode::All_in_one, -1, 0.02, 0.19);
	SetAAManagerVolume(0, 0, AATracker);
	
	if (GameModeActorStruct.PlayerDelay < 0.01f)
	{
		UsingAAPlayer = 0;
		AAPlayer = nullptr;
		return;
	}

	AAPlayer = NewObject<UAudioAnalyzerManager>(this);
	switch (GameModeActorStruct.AudioFormat)
	{
	case EAudioFormat::Capture:
		AAPlayer->SetDefaultDevicesCapturerAudio(*GameModeActorStruct.InAudioDevice,
		                                         *GameModeActorStruct.OutAudioDevice);
		if (!AAPlayer->InitCapturerAudioEx(48000, EAA_AudioDepth::B_16, EAA_AudioFormat::Signed_Int, 1.f,
		                                   GameModeActorStruct.bPlaybackAudio))
		{
			bShouldTick = false;
			UE_LOG(LogTemp, Display, TEXT("Init Player Error"));
			return;
		}
		break;
	case EAudioFormat::File:
		if (!AAPlayer->InitPlayerAudio(GameModeActorStruct.SongPath))
		{
			bShouldTick = false;
			UE_LOG(LogTemp, Display, TEXT("Init Player Error"));
			return;
		}
		break;
	case EAudioFormat::None: break;
	}
	AAPlayer->InitBeatTrackingConfigWLimits(
		EAA_ChannelSelectionMode::All_in_one, 0,
		AASettings.BandLimits, AASettings.TimeWindow, AASettings.HistorySize,
		false, 100, 2.1);
	AAPlayer->InitSpectrumConfigWLimits(EAA_ChannelSelectionMode::All_in_one, -1, AASettings.BandLimits,
	                                    AASettings.TimeWindow, 10 / AASettings.TimeWindow, true,
	                                    AASettings.NumBandChannels);
	UsingAAPlayer = 1;
	SetAAManagerVolume(0, 0, AAPlayer);
}

void ADefaultGameMode::PlayAAPlayer()
{
	if (!AAPlayer)
	{
		return;
	}

	switch (GameModeActorStruct.AudioFormat)
	{
	case EAudioFormat::File:
		AAPlayer->Play();
		break;
	case EAudioFormat::Capture:
		AAPlayer->StartCapture(GameModeActorStruct.bPlaybackAudio, false);
		break;
	case EAudioFormat::None: break;
	}
	UE_LOG(LogTemp, Display, TEXT("Now Playing AAPlayer"));
	SetAAManagerVolume(LoadPlayerSettings().GlobalVolume, LoadPlayerSettings().MusicVolume, AAPlayer);
}

void ADefaultGameMode::SetAAManagerVolume(const float GlobalVolume, const float MusicVolume,
                                          UAudioAnalyzerManager* AAManager)
{
	if (AAManager)
	{
		switch (GameModeActorStruct.AudioFormat)
		{
		case EAudioFormat::File:
			AAManager->SetPlaybackVolume(GlobalVolume / 100 * MusicVolume / 100);
			break;
		case EAudioFormat::Capture:
			AAManager->SetCaptureVolume(1, GlobalVolume / 100 * MusicVolume / 100);
			break;
		case EAudioFormat::None: break;
		}
		return;
	}
	if (AAPlayer)
	{
		switch (GameModeActorStruct.AudioFormat)
		{
		case EAudioFormat::File:
			AAPlayer->SetPlaybackVolume(GlobalVolume / 100 * MusicVolume / 100);
			break;
		case EAudioFormat::Capture:
			AAPlayer->SetCaptureVolume(1, GlobalVolume / 100 * MusicVolume / 100);
			break;
		case EAudioFormat::None: break;
		}
		/* Exit early if AAPlayer exists so we don't set volume for two playback sources at once */
		return;
	}
	if (AATracker)
	{
		switch (GameModeActorStruct.AudioFormat)
		{
		case EAudioFormat::File:
			AATracker->SetPlaybackVolume(GlobalVolume / 100 * MusicVolume / 100);
			break;
		case EAudioFormat::Capture:
			AATracker->SetCaptureVolume(1, GlobalVolume / 100 * MusicVolume / 100);
			break;
		case EAudioFormat::None: break;
		}
	}
}

void ADefaultGameMode::OnSecondPassed()
{
	if (OnAAManagerSecondPassed.IsBound())
	{
		OnAAManagerSecondPassed.Execute(GetWorldTimerManager().GetTimerElapsed(GameModeLengthTimer));
	}
}

void ADefaultGameMode::RefreshAASettings(const FAASettingsStruct& RefreshedAASettings)
{
	AASettings = RefreshedAASettings;
	VisualizerManager->UpdateAASettings(RefreshedAASettings);
}

void ADefaultGameMode::RefreshPlayerSettings(const FPlayerSettings& RefreshedPlayerSettings)
{
	SetAAManagerVolume(RefreshedPlayerSettings.GlobalVolume, RefreshedPlayerSettings.MusicVolume);
	bShowStreakCombatText = RefreshedPlayerSettings.bShowStreakCombatText;
	CombatTextFrequency = RefreshedPlayerSettings.CombatTextFrequency;
	if (VisualizerManager)
	{
		VisualizerManager->UpdateVisualizerStates(RefreshedPlayerSettings);
	}
}

void ADefaultGameMode::LoadMatchingPlayerScores()
{
	CurrentPlayerScore.ResetStruct();
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
	if (GameModeActorStruct.GameModeLength == 0.f)
	{
		MaxScorePerTarget = 1000.f;
	}
	else
	{
		MaxScorePerTarget = 100000.f / ((GameModeActorStruct.GameModeLength - 1.f) / GameModeActorStruct.TargetSpawnCD);
	}
}

void ADefaultGameMode::HandleScoreSaving(const bool bShouldSavePlayerScores)
{
	const FPlayerScore Scores = GetCompletedPlayerScores();
	if (!bShouldSavePlayerScores || Scores.Score <= 0.f)
	{
		OnPostScoresResponse.Broadcast(ELoginState::None);
		return;
	}
	TArray<FPlayerScore> AllSavedScores = LoadPlayerScores();
	AllSavedScores.Emplace(Scores);
	SavePlayerScores(AllSavedScores);
	SaveScoresToDatabase();
}

FPlayerScore ADefaultGameMode::GetCompletedPlayerScores()
{
	/** don't save scores if score is zero */
	if (CurrentPlayerScore.Score <= 0 ||
		(CurrentPlayerScore.GameModeActorName == EGameModeActorName::Custom &&
			CurrentPlayerScore.CustomGameModeName == ""))
	{
		return FPlayerScore();
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

	return CurrentPlayerScore;
}

void ADefaultGameMode::SaveScoresToDatabase()
{
	if (!LoadPlayerSettings().HasLoggedInHttp)
	{
		OnPostScoresResponse.Broadcast(ELoginState::NewUser);
		return;
	}
	RequestAccessToken(LoadPlayerSettings().LoginCookie, OnAccessTokenResponse);
}

void ADefaultGameMode::OnAccessTokenResponseReceived(const FString AccessToken)
{
	if (AccessToken.IsEmpty())
	{
		OnPostScoresResponse.Broadcast(ELoginState::InvalidHttp);
		return;
	}
	TArray<FPlayerScore> ScoresNotSavedToDB;
	TArray<FPlayerScore> AllPlayerScores = LoadPlayerScores();
	for (FPlayerScore PlayerScoreObj : LoadPlayerScores())
	{
		if (!PlayerScoreObj.bSavedToDatabase)
		{
			ScoresNotSavedToDB.Emplace(PlayerScoreObj);
		}
	}
	PostPlayerScores(ScoresNotSavedToDB, LoadPlayerSettings().Username, AccessToken, OnPostScoresResponse);
}

void ADefaultGameMode::OnPostScoresResponseReceived(const ELoginState& LoginState)
{
	if (LoginState == ELoginState::LoggedInHttp)
	{
		TArray<FPlayerScore> ScoresToUpdate = LoadPlayerScores();
		for (FPlayerScore& Score : ScoresToUpdate)
		{
			Score.bSavedToDatabase = true;
		}
		SavePlayerScores(ScoresToUpdate);
	}
}

void ADefaultGameMode::UpdatePlayerScores(const float TimeElapsed, const int32 NewStreak, const FVector Position)
{
	FString Hit;
	if (TimeElapsed == -1)
	{
		Hit = "Miss";
	}
	else
	{
		Hit = "Hit";
	}
	UE_LOG(LogTemp, Display, TEXT("%s, Position: %s"), *Hit, *Position.ToString());
	if (GameModeActorStruct.IsBeatTrackMode == true || TimeElapsed == -1)
	{
		return;
	}

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
	UpdateTargetsHit();
	UpdateStreak(NewStreak, Position);
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

void ADefaultGameMode::UpdateStreak(const int32 Streak, const FVector Location)
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

float ADefaultGameMode::CheckFloatNaN(const float ValueToCheck, const float ValueToRound)
{
	if (!isnan(roundf(ValueToRound * ValueToCheck) / ValueToRound))
	{
		return roundf(ValueToRound * ValueToCheck) / ValueToRound;
	}
	return 0;
}


//AATracker->OnCapturedData.RemoveDynamic(this, &ADefaultGameMode::FeedStreamCaptureTracker);
//AAPlayer->OnCapturedData.RemoveDynamic(this, &ADefaultGameMode::FeedStreamCapturePlayer);

// if (!AATracker->InitLoopbackAudio())
// {
// 	ShowSongPathErrorMessage();
// 	bShouldTick = false;
// 	UE_LOG(LogTemp, Display, TEXT("Init Tracker Error"));
// 	return;
// }
// AATracker->SetDefaultDeviceLoopbackAudio(*AudioDeviceList[6]);
// if (!AATracker->InitLoopbackAudio())
// {
// 	ShowSongPathErrorMessage();
// 	bShouldTick = false;
// 	UE_LOG(LogTemp, Display, TEXT("Init Tracker Error"));
// 	return;
// }


// AATracker->InitSpectrumConfigWLimits(
// EAA_ChannelSelectionMode::All_in_one, 0,
// AASettings.BandLimits, AASettings.TimeWindow, AASettings.HistorySize,
// false, 1);

// AATracker->InitSpectrumConfigWLimits(
// 	EAA_ChannelSelectionMode::All_in_one, 0,
// 	AASettings.BandLimits, AASettings.TimeWindow, AASettings.HistorySize,
// 	true, 1);
// OnAAPlayerLoaded.Broadcast(AATracker);

//AATracker->InitCapturerAudioEx(48000, EAA_AudioDepth::B_16, EAA_AudioFormat::Signed_Int, AudioBufferSeconds);
//AATracker->SetDefaultDeviceStreamAudio(*OutAudioDevice);
//AATracker->InitStreamAudio(1, 48000, EAA_AudioDepth::B_16, EAA_AudioFormat::Signed_Int,1.f, bPlaybackAudio);
//AATracker->OnCapturedData.AddUniqueDynamic(this, &ADefaultGameMode::FeedStreamCaptureTracker);

//AAPlayer->SetDefaultDeviceStreamAudio(*OutAudioDevice);
//AAPlayer->InitStreamAudio(1, 48000, EAA_AudioDepth::B_16, EAA_AudioFormat::Signed_Int,1.f, bPlaybackAudio);
//AAPlayer->OnCapturedData.AddUniqueDynamic(this, &ADefaultGameMode::FeedStreamCapturePlayer);

/*Visualizer2 = Cast<AStaticCubeVisualizer>(GetWorld()->SpawnActor(VisualizerClass,
												  &Visualizer2Location,
												  &FRotator::ZeroRotator,
												  SpawnParameters));*/

/*void ADefaultGameMode::FeedStreamCaptureTracker(const TArray<uint8>& StreamData)
{
	AATracker->FeedStreamCapture(StreamData);
}

void ADefaultGameMode::FeedStreamCapturePlayer(const TArray<uint8>& StreamData)
{
	AAPlayer->FeedStreamCapture(StreamData);
}*/
