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
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetTextLibrary.h"

ADefaultGameMode::ADefaultGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ADefaultGameMode::BeginPlay()
{
	Super::BeginPlay();
	InitializeGameMode();
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

void ADefaultGameMode::InitializeGameMode()
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

	InitializeAudioManagers(GameModeActorStruct.bPlaybackAudio, GameModeActorStruct.SongPath, GameModeActorStruct.InAudioDevice, GameModeActorStruct.OutAudioDevice);
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

	if (OnGameModeInit.ExecuteIfBound(GameModeActorStruct.IsBeatTrackMode))
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
	//AATracker->OnCapturedData.RemoveDynamic(this, &ADefaultGameMode::FeedStreamCaptureTracker);
	//AAPlayer->OnCapturedData.RemoveDynamic(this, &ADefaultGameMode::FeedStreamCapturePlayer);

	if (const ADefaultPlayerController* PlayerController = Cast<ADefaultPlayerController>(
		UGameplayStatics::GetPlayerController(GetWorld(), 0)); Cast<ADefaultCharacter>(PlayerController->GetPawn())->Gun)
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
	if (!AATracker)
	{
		return;
	}

	const FPlayerSettings Settings = LoadPlayerSettings();
	// If delay is large enough, play AATracker and then AAPlayer after the delay
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
			AATracker->StartCapture(GameModeActorStruct.bPlaybackAudio, false);
		}
		else
		{
			AATracker->StartCapture(false, false);
		}
	}
	UE_LOG(LogTemp, Display, TEXT("Now Playing AATracker"));
	if (GameModeActorStruct.PlayerDelay < 0.01f)
	{
		SetAAManagerVolume(Settings.GlobalVolume, Settings.MusicVolume, AATracker);
	}
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

void ADefaultGameMode::InitializeAudioManagers(const bool bPlaybackAudio, const FString& SongFilePath,
                                               const FString& InAudioDevice, const FString& OutAudioDevice)
{
	AASettings = LoadAASettings();
	const bool bUseCaptureAudio = SongFilePath.IsEmpty();
	AATracker = NewObject<UAudioAnalyzerManager>(this);

	/* set Song length and song title in GameModeActorStruct if using song file */
	if (!bUseCaptureAudio)
	{
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
		GameModeActorStruct.GameModeLength = AATracker->GetTotalDuration();
	}

	/* Initialize AATracker Manager */
	if (bUseCaptureAudio)
	{
		AATracker->SetDefaultDevicesCapturerAudio(*InAudioDevice, *OutAudioDevice);
		if (GameModeActorStruct.PlayerDelay < 0.01f)
		{
			if (!AATracker->InitCapturerAudioEx(44100, EAA_AudioDepth::B_16, EAA_AudioFormat::Signed_Int, 1.f,
			                                    bPlaybackAudio))
			{
				bShouldTick = false;
				UE_LOG(LogTemp, Display, TEXT("Init Tracker Error"));
				return;
			}
		}
		else
		{
			if (!AATracker->InitCapturerAudioEx(44100, EAA_AudioDepth::B_16, EAA_AudioFormat::Signed_Int, 1.f,
			                                    false))
			{
				bShouldTick = false;
				UE_LOG(LogTemp, Display, TEXT("Init Tracker Error"));
				return;
			}
		}
	}
	else
	{
		if (!AATracker->InitPlayerAudio(SongFilePath))
		{
			bShouldTick = false;
			UE_LOG(LogTemp, Display, TEXT("Init Tracker Error"));
			return;
		}
	}
	AATracker->InitBeatTrackingConfigWLimits(
		EAA_ChannelSelectionMode::All_in_one, 0,
		AASettings.BandLimits, AASettings.TimeWindow, AASettings.HistorySize,
		false, 100, 2.1);
	SetAAManagerVolume(0, 0, AATracker);

	/* Only initialize AAPlayer if the player delay is large enough */
	if (GameModeActorStruct.PlayerDelay < 0.01f)
	{
		AAPlayer = nullptr;
		OnAAPlayerLoaded.Broadcast(AATracker);
		return;
	}
	AAPlayer = NewObject<UAudioAnalyzerManager>(this);
	if (bUseCaptureAudio)
	{
		AAPlayer->SetDefaultDevicesCapturerAudio(*InAudioDevice, *OutAudioDevice);
		if (!AAPlayer->InitCapturerAudioEx(44100, EAA_AudioDepth::B_16, EAA_AudioFormat::Signed_Int, 1.f,
		                                   bPlaybackAudio))
		{
			bShouldTick = false;
			UE_LOG(LogTemp, Display, TEXT("Init Player Error"));
			return;
		}
	}
	else
	{
		if (!AAPlayer->InitPlayerAudio(SongFilePath))
		{
			bShouldTick = false;
			UE_LOG(LogTemp, Display, TEXT("Init Player Error"));
			return;
		}
	}
	AAPlayer->InitBeatTrackingConfigWLimits(
		EAA_ChannelSelectionMode::All_in_one, 0,
		AASettings.BandLimits, AASettings.TimeWindow, AASettings.HistorySize,
		false, 100, 2.1);
	SetAAManagerVolume(0, 0, AAPlayer);
	OnAAPlayerLoaded.Broadcast(AAPlayer);
}

void ADefaultGameMode::PlayAAPlayer()
{
	if (!AAPlayer)
	{
		return;
	}

	const FPlayerSettings Settings = LoadPlayerSettings();
	if (!GameModeActorStruct.SongPath.IsEmpty())
	{
		AAPlayer->Play();
	}
	else
	{
		AAPlayer->StartCapture(false, GameModeActorStruct.bPlaybackAudio);
	}
	UE_LOG(LogTemp, Display, TEXT("Now Playing AAPlayer"));
	SetAAManagerVolume(Settings.GlobalVolume, Settings.MusicVolume, AAPlayer);
}

void ADefaultGameMode::SetAAManagerVolume(const float GlobalVolume, const float MusicVolume,
                                          UAudioAnalyzerManager* AAManager)
{
	if (AAManager)
	{
		if (GameModeActorStruct.SongPath.IsEmpty())
		{
			AAManager->SetCaptureVolume(GlobalVolume / 100 * MusicVolume / 100, GlobalVolume / 100 * MusicVolume / 100);
		}
		else
		{
			AAManager->SetPlaybackVolume(GlobalVolume / 100 * MusicVolume / 100);
		}
		return;
	}
	if (AAPlayer)
	{
		if (GameModeActorStruct.SongPath.IsEmpty())
		{
			AAPlayer->SetCaptureVolume(GlobalVolume / 100 * MusicVolume / 100, GlobalVolume / 100 * MusicVolume / 100);
		}
		else
		{
			AAPlayer->SetPlaybackVolume(GlobalVolume / 100 * MusicVolume / 100);
		}
		return;
	}
	if (AATracker)
	{
		if (GameModeActorStruct.SongPath.IsEmpty())
		{
			AATracker->SetCaptureVolume(GlobalVolume / 100 * MusicVolume / 100, GlobalVolume / 100 * MusicVolume / 100);
		}
		else
		{
			AATracker->SetPlaybackVolume(GlobalVolume / 100 * MusicVolume / 100);
		}
	}

}

void ADefaultGameMode::OnSecondPassed()
{
	if (OnAAManagerSecondPassed.IsBound())
	{
		if (AAPlayer != nullptr)
		{
			if (GameModeActorStruct.SongPath.IsEmpty())
			{
				OnAAManagerSecondPassed.Execute(GetWorldTimerManager().GetTimerElapsed(OnSecondPassedTimer));
			}
			else
			{
				OnAAManagerSecondPassed.Execute(AAPlayer->GetPlaybackTime());
			}
		}
		else if (AATracker != nullptr)
		{
			if (GameModeActorStruct.SongPath.IsEmpty())
			{
				OnAAManagerSecondPassed.Execute(GetWorldTimerManager().GetTimerElapsed(OnSecondPassedTimer));
			}
			else
			{
				OnAAManagerSecondPassed.Execute(AATracker->GetPlaybackTime());
			}
		}
	}
}

void ADefaultGameMode::FeedStreamCaptureTracker(const TArray<uint8>& StreamData)
{
	AATracker->FeedStreamCapture(StreamData);
}

void ADefaultGameMode::FeedStreamCapturePlayer(const TArray<uint8>& StreamData)
{
	AAPlayer->FeedStreamCapture(StreamData);
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

float ADefaultGameMode::CheckFloatNaN(const float ValueToCheck, const float ValueToRound)
{
	if (!isnan(roundf(ValueToRound * ValueToCheck) / ValueToRound))
	{
		return roundf(ValueToRound * ValueToCheck) / ValueToRound;
	}
	return 0;
}


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

//AATracker->InitCapturerAudioEx(44100, EAA_AudioDepth::B_16, EAA_AudioFormat::Signed_Int, AudioBufferSeconds);
//AATracker->SetDefaultDeviceStreamAudio(*OutAudioDevice);
//AATracker->InitStreamAudio(1, 44100, EAA_AudioDepth::B_16, EAA_AudioFormat::Signed_Int,1.f, bPlaybackAudio);
//AATracker->OnCapturedData.AddUniqueDynamic(this, &ADefaultGameMode::FeedStreamCaptureTracker);

//AAPlayer->SetDefaultDeviceStreamAudio(*OutAudioDevice);
//AAPlayer->InitStreamAudio(1, 44100, EAA_AudioDepth::B_16, EAA_AudioFormat::Signed_Int,1.f, bPlaybackAudio);
//AAPlayer->OnCapturedData.AddUniqueDynamic(this, &ADefaultGameMode::FeedStreamCapturePlayer);
