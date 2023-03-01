// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

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
	OnTick_AudioAnalyzers(DeltaSeconds);
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
	GetWorldTimerManager().SetTimer(OnSecondPassedTimer, this, &ADefaultGameMode::OnSecondPassedCallback, 1.f, true);
}

void ADefaultGameMode::BindGameModeDelegates()
{
	if (ADefaultCharacter* Character = Cast<ADefaultCharacter>(
		Cast<ADefaultPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0))->GetPawn()))
	{
		if (!Character->Gun->OnShotFired.IsBoundToObject(this))
		{
			Character->Gun->OnShotFired.BindUFunction(this, FName("UpdateShotsFired"));
		}
		if (GameModeActorStruct.IsBeatTrackMode)
		{
			Character->Gun->SetShouldTrace(true);
		}
		else
		{
			Character->Gun->SetShouldTrace(false);
		}
		Character->SetTimelinePlaybackRate_AimBot(GameModeActorStruct.TargetSpawnCD);
	}
	if (!OnTargetSpawned.IsBoundToObject(this))
	{
		OnTargetSpawned.AddUFunction(this, FName("UpdateTargetsSpawned"));
	}
	if (!OnTargetDestroyed.IsBoundToObject(this))
	{
		OnTargetDestroyed.AddUFunction(this, FName("UpdatePlayerScores"));
	}
}

void ADefaultGameMode::EndGameMode(const bool ShouldSavePlayerScores, const bool ShowPostGameMenu)
{
	GetWorldTimerManager().ClearTimer(GameModeLengthTimer);
	GetWorldTimerManager().ClearTimer(PlayerDelayTimer);
	GetWorldTimerManager().ClearTimer(OnSecondPassedTimer);

	if (TargetSpawner)
	{
		CurrentPlayerScore.LocationAccuracy = TargetSpawner->GetLocationAccuracy();
		TargetSpawner->SetShouldSpawn(false);
		TargetSpawner->Destroy();
		TargetSpawner = nullptr;
	}

	/** Unbinding delegates */
	if (const ADefaultCharacter* Character = Cast<ADefaultCharacter>(
		Cast<ADefaultPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0))->GetPawn()))
	{
		if (Character->Gun->OnShotFired.IsBoundToObject(this))
		{
			Character->Gun->OnShotFired.Unbind();
		}
	}
	if (OnTargetSpawned.IsBoundToObject(this))
	{
		OnTargetSpawned.RemoveAll(this);
	}
	if (OnTargetDestroyed.IsBoundToObject(this))
	{
		OnTargetDestroyed.RemoveAll(this);
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

	Controller->HidePlayerHUD();
	Controller->HideCountdown();
	Controller->HideCrossHair();

	if (ShowPostGameMenu)
	{
		Controller->ShowPostGameMenu();
		OnPostScoresResponse.AddUFunction(
			Cast<ADefaultPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)),
			"OnPostScoresResponseReceived");
	}
	HandleScoreSaving(ShouldSavePlayerScores);
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
	switch (GameModeActorStruct.AudioFormat)
	{
	case EAudioFormat::File:
		AATracker->Play();
		if (AAPlayer)
		{
			/* Start playing back audio from AAPlayer after the delay */
			GetWorldTimerManager().SetTimer(PlayerDelayTimer, this, &ADefaultGameMode::PlayAAPlayer,
			                                GameModeActorStruct.PlayerDelay, false);
		}
		else
		{
			/* If no AAPlayer, game has started and should use AATracker for audio playback */
			SetAAManagerVolume(LoadPlayerSettings().VideoAndSound.GlobalVolume,
			                   LoadPlayerSettings().VideoAndSound.MusicVolume, AATracker);
		}
		break;
	case EAudioFormat::Capture:
		{
			AATracker->StartCapture(GameModeActorStruct.bPlaybackAudio, false);
			SetAAManagerVolume(LoadPlayerSettings().VideoAndSound.GlobalVolume,
			                   LoadPlayerSettings().VideoAndSound.MusicVolume, AATracker);
			break;
		}
	default: break;
	}
	UE_LOG(LogTemp, Display, TEXT("Now Playing AATracker"));
}

void ADefaultGameMode::PauseAAManager(const bool ShouldPause)
{
	if (!AATracker)
	{
		return;
	}
	switch (GameModeActorStruct.AudioFormat)
	{
	case EAudioFormat::File:
		AATracker->SetPaused(ShouldPause);
		if (AAPlayer)
		{
			AAPlayer->SetPaused(ShouldPause);
		}
		break;
	case EAudioFormat::Capture:
		if (ShouldPause)
		{
			AATracker->StopCapture();
		}
		else
		{
			AATracker->StartCapture(GameModeActorStruct.bPlaybackAudio, false);
		}
		break;
	default: break;
	}
}

void ADefaultGameMode::InitializeAudioManagers()
{
	AASettings = LoadAASettings();
	AATracker = NewObject<UAudioAnalyzerManager>(this);

	switch (GameModeActorStruct.AudioFormat)
	{
	case EAudioFormat::Capture:
		AATracker->SetDefaultDevicesCapturerAudio(*GameModeActorStruct.InAudioDevice,
		                                          *GameModeActorStruct.OutAudioDevice);
		if (!AATracker->InitCapturerAudioEx(48000, EAA_AudioDepth::B_16, EAA_AudioFormat::Signed_Int, 1.f,
		                                    GameModeActorStruct.bPlaybackAudio))
		{
			OnAAManagerError();
			return;
		}
		break;
	case EAudioFormat::File:
		if (!AATracker->InitPlayerAudio(GameModeActorStruct.SongPath))
		{
			OnAAManagerError();
			return;
		}
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

	/* AAPlayer will only be used if AudioFormat is File and PlayerDelay > 0.01f */
	if (GameModeActorStruct.PlayerDelay < 0.01f)
	{
		AAPlayer = nullptr;
		return;
	}
	AAPlayer = NewObject<UAudioAnalyzerManager>(this);
	if (!AAPlayer->InitPlayerAudio(GameModeActorStruct.SongPath))
	{
		OnAAManagerError();
		return;
	}
	AAPlayer->InitBeatTrackingConfigWLimits(
		EAA_ChannelSelectionMode::All_in_one, 0,
		AASettings.BandLimits, AASettings.TimeWindow, AASettings.HistorySize,
		false, 100, 2.1);
	AAPlayer->InitSpectrumConfigWLimits(EAA_ChannelSelectionMode::All_in_one, -1, AASettings.BandLimits,
	                                    AASettings.TimeWindow, 10 / AASettings.TimeWindow, true,
	                                    AASettings.NumBandChannels);
	SetAAManagerVolume(0, 0, AAPlayer);
}

void ADefaultGameMode::OnTick_AudioAnalyzers(const float DeltaSeconds)
{
	Elapsed += DeltaSeconds;

	TArray<bool> Beats;
	TArray<float> SpectrumValues;
	TArray<float> SpectrumVariance;
	TArray<int32> BpmCurrent;
	TArray<int32> BpmTotal;

	AATracker->GetBeatTrackingWLimitsWThreshold(Beats, SpectrumValues, BpmCurrent, BpmTotal,
	                                            AASettings.BandLimitsThreshold);
	for (const bool Beat : Beats)
	{
		SpawnNewTarget(Beat);
	}

	if (AAPlayer)
	{
		AAPlayer->GetBeatTrackingWLimitsWThreshold(Beats, SpectrumValues, BpmCurrent, BpmTotal,
		                                           AASettings.BandLimitsThreshold);
		AAPlayer->GetBeatTrackingAverageAndVariance(SpectrumVariance,
		                                            VisualizerManager->AvgSpectrumValues);
	}
	else
	{
		AATracker->GetBeatTrackingAverageAndVariance(SpectrumVariance,
		                                             VisualizerManager->AvgSpectrumValues);
	}
	VisualizerManager->UpdateVisualizers(SpectrumValues);
}

void ADefaultGameMode::PlayAAPlayer()
{
	if (!AAPlayer)
	{
		return;
	}
	AAPlayer->Play();
	SetAAManagerVolume(LoadPlayerSettings().VideoAndSound.GlobalVolume, LoadPlayerSettings().VideoAndSound.MusicVolume,
	                   AAPlayer);
	UE_LOG(LogTemp, Display, TEXT("Now Playing AAPlayer %f"), AAPlayer->GetPlaybackVolume());
	
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
		AAPlayer->SetPlaybackVolume(GlobalVolume / 100 * MusicVolume / 100);
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

void ADefaultGameMode::OnSecondPassedCallback()
{
	if (!OnSecondPassed.ExecuteIfBound(GetWorldTimerManager().GetTimerElapsed(GameModeLengthTimer)))
	{
		UE_LOG(LogTemp, Display, TEXT("OnSecondPassed not bound."));;
	}
}

void ADefaultGameMode::RefreshAASettings(const FAASettingsStruct& RefreshedAASettings)
{
	AASettings = RefreshedAASettings;
	VisualizerManager->UpdateAASettings(RefreshedAASettings);
}

void ADefaultGameMode::RefreshPlayerSettings(const FPlayerSettings& RefreshedPlayerSettings)
{
	SetAAManagerVolume(RefreshedPlayerSettings.VideoAndSound.GlobalVolume,
	                   RefreshedPlayerSettings.VideoAndSound.MusicVolume);
	bShowStreakCombatText = RefreshedPlayerSettings.Game.bShowStreakCombatText;
	CombatTextFrequency = RefreshedPlayerSettings.Game.CombatTextFrequency;
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
	/** don't save scores if score is zero */
	if (!bShouldSavePlayerScores || CurrentPlayerScore.Score <= 0 || (CurrentPlayerScore.GameModeActorName == EGameModeActorName::Custom &&
			CurrentPlayerScore.CustomGameModeName == ""))
	{
		OnPostScoresResponse.Broadcast(ELoginState::None);
		return;
	}
	const FPlayerScore Scores = GetCompletedPlayerScores();
	TArray<FPlayerScore> AllSavedScores = LoadPlayerScores();
	AllSavedScores.Emplace(Scores);
	SavePlayerScores(AllSavedScores);
	SaveScoresToDatabase();
}

FPlayerScore ADefaultGameMode::GetCompletedPlayerScores()
{
	/** save current time */
	CurrentPlayerScore.Time = FDateTime::UtcNow().ToIso8601();

	/** for BeatTrack modes */
	if (GameModeActorStruct.IsBeatTrackMode)
	{
		CurrentPlayerScore.Accuracy = FloatDivide(CurrentPlayerScore.Score, CurrentPlayerScore.TotalPossibleDamage);
		CurrentPlayerScore.Completion = FloatDivide(CurrentPlayerScore.Score, CurrentPlayerScore.TotalPossibleDamage);
	}
	else
	{
		CurrentPlayerScore.AvgTimeOffset = FloatDivide(CurrentPlayerScore.TotalTimeOffset, CurrentPlayerScore.TargetsHit);
		CurrentPlayerScore.Accuracy = FloatDivide(CurrentPlayerScore.TargetsHit, CurrentPlayerScore.ShotsFired);
		CurrentPlayerScore.Completion = FloatDivide(CurrentPlayerScore.TargetsHit, CurrentPlayerScore.TargetsSpawned);
	}
	return CurrentPlayerScore;
}

void ADefaultGameMode::SaveScoresToDatabase()
{
	if (!LoadPlayerSettings().User.HasLoggedInHttp)
	{
		OnPostScoresResponse.Broadcast(ELoginState::NewUser);
		return;
	}
	RequestAccessToken(LoadPlayerSettings().User.LoginCookie, OnAccessTokenResponse);
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
	PostPlayerScores(ScoresNotSavedToDB, LoadPlayerSettings().User.Username, AccessToken, OnPostScoresResponse);
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

void ADefaultGameMode::UpdateTargetsSpawned(ASphereTarget* SpawnedTarget)
{
	CurrentPlayerScore.TargetsSpawned++;
	if (!UpdateScoresToHUD.ExecuteIfBound(CurrentPlayerScore))
	{
		UE_LOG(LogTemp, Display, TEXT("UpdateScoresToHUD not bound."));
	}

	/** Update tracking score if a Tracking target has spawned */
	if (GameModeActorStruct.IsBeatTrackMode && !SpawnedTarget->HealthComp->OnBeatTrackTick.IsBoundToObject(this))
	{
		SpawnedTarget->HealthComp->OnBeatTrackTick.BindUFunction(this, FName("UpdateTrackingScore"));
		if (ADefaultCharacter* Character = Cast<ADefaultCharacter>(
	Cast<ADefaultPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0))->GetPawn()))
		{
			Character->PassTrackingTargetToGun(SpawnedTarget);
		}
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
	if (Streak > CurrentPlayerScore.Streak)
	{
		CurrentPlayerScore.Streak = Streak;
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
	UE_LOG(LogTemp, Display, TEXT("TargetsHit: %d"), CurrentPlayerScore.TargetsHit);
}

void ADefaultGameMode::UpdateHighScore()
{
	if (CurrentPlayerScore.Score > CurrentPlayerScore.HighScore)
	{
		CurrentPlayerScore.HighScore = CurrentPlayerScore.Score;
	}
}

float ADefaultGameMode::FloatDivide(const float Num, const float Denom)
{
	if (Denom == 0)
	{
		return 0;
	}
	return Num / Denom;
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
