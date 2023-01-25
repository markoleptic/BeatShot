// Copyright Epic Games, Inc. All Rights Reserved.

#include "DefaultGameMode.h"
#include <vector>
#include "AudioAnalyzerManager.h"
#include "BeamVisualizer.h"
#include "DefaultCharacter.h"
#include "DefaultGameInstance.h"
#include "DefaultHealthComponent.h"
#include "DefaultPlayerController.h"
#include "FloatingTextActor.h"
#include "TargetSpawner.h"
#include "StaticCubeVisualizer.h"
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
			AATracker->GetBeatTrackingAverageAndVariance(SpectrumVariance, AvgSpectrumValues);
			AATracker->GetSpectrumPeaks(SpectrumPeaks, SpectrumPeakEnergy);
			break;
		}
	default:
		{
			AAPlayer->GetBeatTrackingWLimitsWThreshold(Beats, SpectrumValues, BPMCurrent, BPMTotal,
			                                           AASettings.BandLimitsThreshold);
			AAPlayer->GetBeatTrackingAverageAndVariance(SpectrumVariance, AvgSpectrumValues);
			AAPlayer->GetSpectrumPeaks(SpectrumPeaks, SpectrumPeakEnergy);
			break;
		}
	}
	UpdateVisualizers(SpectrumValues);
}

void ADefaultGameMode::InitializeVisualizers()
{
	AvgSpectrumValues.SetNum(AASettings.NumBandChannels);
	CurrentSpectrumValues.SetNum(AASettings.NumBandChannels);
	MaxSpectrumValues.SetNum(AASettings.NumBandChannels);
	MaxSpectrumValues.Init(1, AASettings.NumBandChannels);
	CurrentCubeSpectrumValues.SetNum(AASettings.NumBandChannels);

	for (AVisualizerBase* Visualizer : Visualizers)
	{
		Visualizer->InitializeVisualizer();
	}
}

float ADefaultGameMode::GetNormalizedSpectrumValue(const int32 Index, const bool bIsBeam)
{
	if (bIsBeam)
	{
		return UKismetMathLibrary::MapRangeClamped(CurrentSpectrumValues[Index], 0, MaxSpectrumValues[Index], 0, 1);
	}
	return UKismetMathLibrary::MapRangeClamped(CurrentCubeSpectrumValues[Index] - AvgSpectrumValues[Index], 0, MaxSpectrumValues[Index], 0, 1);
}

void ADefaultGameMode::UpdateVisualizers(const TArray<float> SpectrumValues)
{
	for (int i = 0; i < SpectrumValues.Num(); i++)
	{
		if (SpectrumValues[i] > MaxSpectrumValues[i])
		{
			MaxSpectrumValues[i] = SpectrumValues[i];
		}
		if (SpectrumValues[i] > CurrentCubeSpectrumValues[i])
		{
			CurrentCubeSpectrumValues[i] = SpectrumValues[i];
		}

		if (SpectrumValues[i] > CurrentSpectrumValues[i] && CurrentSpectrumValues[i] <= 0)
		{
			CurrentSpectrumValues[i] = SpectrumValues[i];
			if (Visualizers[2])
			{
				Visualizers[2]->UpdateVisualizer(i, GetNormalizedSpectrumValue(i, true));
			}
		}

		if (Visualizers[0])
		{
			Visualizers[0]->UpdateVisualizer(i, GetNormalizedSpectrumValue(i, false));
		}
		if (Visualizers[1])
		{
			Visualizers[1]->UpdateVisualizer(i, GetNormalizedSpectrumValue(i, false));
		}

		if (CurrentSpectrumValues[i] >= 0)
		{
			CurrentSpectrumValues[i] -= 0.0005;
		}
		if (CurrentCubeSpectrumValues[i] >= 0)
		{
			CurrentCubeSpectrumValues[i] -= 0.005;
		}
		if (MaxSpectrumValues[i] >= 0)
		{
			MaxSpectrumValues[i] -= 0.0001;
		}
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
	Visualizers.Empty();
	Visualizers.EmplaceAt(0, Cast<AStaticCubeVisualizer>(GetWorld()->SpawnActor(StaticCubeVisualizerClass,
		                      &VisualizerLocation,
		                      &VisualizerRotation,
		                      SpawnParameters)));
	Visualizers.EmplaceAt(1, Cast<AStaticCubeVisualizer>(GetWorld()->SpawnActor(StaticCubeVisualizerClass,
		                      &Visualizer2Location,
		                      &VisualizerRotation,
		                      SpawnParameters)));
	Visualizers.EmplaceAt(2, Cast<ABeamVisualizer>(
		                      GetWorld()->SpawnActor(BeamVisualizerClass, &BeamVisualizerLocation, &BeamRotation,
		                                             SpawnParameters)));
	InitializeVisualizers();
	InitializeAudioManagers(GameModeActorStruct.bPlaybackAudio, GameModeActorStruct.SongPath,
	                        GameModeActorStruct.InAudioDevice, GameModeActorStruct.OutAudioDevice);
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
	if (GameModeActorStruct.GameModeLength == 0.f)
	{
		GetWorldTimerManager().SetTimer(GameModeLengthTimer, 31536000, false);
	}
	else
	{
		GetWorldTimerManager().SetTimer(GameModeLengthTimer, this, &ADefaultGameMode::OnGameModeLengthTimerComplete,
		                                GameModeActorStruct.GameModeLength, false);
	}
	if (!UpdateScoresToHUD.ExecuteIfBound(CurrentPlayerScore))
	{
		UE_LOG(LogTemp, Display, TEXT("Initial UpdateScoresToHUD failed."));
	}
	GetWorldTimerManager().SetTimer(OnSecondPassedTimer, this, &ADefaultGameMode::OnSecondPassed, 1.f, true);
	TargetSpawner->SetShouldSpawn(true);
}

void ADefaultGameMode::EndGameMode(const bool ShouldSavePlayerScores, const bool ShowPostGameMenu)
{
	ADefaultPlayerController* Controller = Cast<ADefaultPlayerController>(
		UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (ShowPostGameMenu)
	{
		Controller->ShowPostGameMenu();
	}
	GetWorldTimerManager().ClearTimer(GameModeLengthTimer);
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

	HandleScoreSaving(ShouldSavePlayerScores);

	/** Unbinding delegates */
	if (const ADefaultPlayerController* PlayerController = Cast<ADefaultPlayerController>(
			UGameplayStatics::GetPlayerController(GetWorld(), 0)); Cast<ADefaultCharacter>(PlayerController->GetPawn())
		->Gun)
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

	for (AVisualizerBase* Visualizer : Visualizers)
	{
		Visualizer->Destroy();
	}
	Visualizers.Empty();

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
			AATracker->StartCapture(GameModeActorStruct.bPlaybackAudio, false);
			SetAAManagerVolume(LoadPlayerSettings().GlobalVolume, LoadPlayerSettings().MusicVolume, AATracker);
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

void ADefaultGameMode::InitializeAudioManagers(const bool bPlaybackAudio, const FString& SongFilePath,
                                               const FString& InAudioDevice, const FString& OutAudioDevice)
{
	AASettings = LoadAASettings();
	const bool bUseCaptureAudio = SongFilePath.IsEmpty();
	AATracker = NewObject<UAudioAnalyzerManager>(this);

	/* Initialize AATracker Manager */
	if (bUseCaptureAudio)
	{
		AATracker->SetDefaultDevicesCapturerAudio(*InAudioDevice, *OutAudioDevice);
		if (GameModeActorStruct.PlayerDelay < 0.01f)
		{
			if (!AATracker->InitCapturerAudioEx(48000, EAA_AudioDepth::B_16, EAA_AudioFormat::Signed_Int, 1.f,
			                                    bPlaybackAudio))
			{
				bShouldTick = false;
				UE_LOG(LogTemp, Display, TEXT("Init Tracker Error"));
				return;
			}
		}
		else
		{
			if (!AATracker->InitCapturerAudioEx(48000, EAA_AudioDepth::B_16, EAA_AudioFormat::Signed_Int, 1.f,
			                                    false))
			{
				bShouldTick = false;
				UE_LOG(LogTemp, Display, TEXT("Init Tracker Error"));
				return;
			}
			AATracker->SetCaptureVolume(1, 0);
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
		GameModeActorStruct.GameModeLength = AATracker->GetTotalDuration();
	}

	AATracker->InitBeatTrackingConfigWLimits(
		EAA_ChannelSelectionMode::All_in_one, 0,
		AASettings.BandLimits, AASettings.TimeWindow, 10 / AASettings.TimeWindow,
		false, 100, 2.1);
	AATracker->InitSpectrumConfigWLimits(EAA_ChannelSelectionMode::All_in_one, -1, AASettings.BandLimits,
	                                     AASettings.TimeWindow, 10 / AASettings.TimeWindow, true,
	                                     AASettings.NumBandChannels);
	SetAAManagerVolume(0, 0, AATracker);
	//AATracker->InitPitchTrackingConfig(EAA_ChannelSelectionMode::All_in_one, -1, 0.02, 0.19);

	/* Only initialize AAPlayer if the player delay is large enough */
	if (GameModeActorStruct.PlayerDelay < 0.01f)
	{
		UsingAAPlayer = 0;
		AAPlayer = nullptr;
		return;
	}
	AAPlayer = NewObject<UAudioAnalyzerManager>(this);
	if (bUseCaptureAudio)
	{
		AAPlayer->SetDefaultDevicesCapturerAudio(*InAudioDevice, *OutAudioDevice);
		if (!AAPlayer->InitCapturerAudioEx(48000, EAA_AudioDepth::B_16, EAA_AudioFormat::Signed_Int, 1.f,
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

	if (!GameModeActorStruct.SongPath.IsEmpty())
	{
		AAPlayer->Play();
	}
	else
	{
		AAPlayer->StartCapture(GameModeActorStruct.bPlaybackAudio, false);
	}
	UE_LOG(LogTemp, Display, TEXT("Now Playing AAPlayer"));
	SetAAManagerVolume(LoadPlayerSettings().GlobalVolume, LoadPlayerSettings().MusicVolume, AAPlayer);
}

void ADefaultGameMode::SetAAManagerVolume(const float GlobalVolume, const float MusicVolume,
                                          UAudioAnalyzerManager* AAManager)
{
	if (AAManager)
	{
		if (GameModeActorStruct.SongPath.IsEmpty())
		{
			AAManager->SetCaptureVolume(1, GlobalVolume / 100 * MusicVolume / 100);
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
			AAPlayer->SetCaptureVolume(1, GlobalVolume / 100 * MusicVolume / 100);
		}
		else
		{
			AAPlayer->SetPlaybackVolume(GlobalVolume / 100 * MusicVolume / 100);
		}
		/* Exit early if AAPlayer exists so we don't set volume for two playback sources at once */
		return;
	}
	if (AATracker)
	{
		if (GameModeActorStruct.SongPath.IsEmpty())
		{
			AATracker->SetCaptureVolume(1, GlobalVolume / 100 * MusicVolume / 100);
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
				OnAAManagerSecondPassed.Execute(GetWorldTimerManager().GetTimerElapsed(GameModeLengthTimer));
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
				OnAAManagerSecondPassed.Execute(GetWorldTimerManager().GetTimerElapsed(GameModeLengthTimer));
			}
			else
			{
				OnAAManagerSecondPassed.Execute(AATracker->GetPlaybackTime());
			}
		}
	}
}

void ADefaultGameMode::RefreshAASettings(const FAASettingsStruct& RefreshedAASettings)
{
	AASettings = RefreshedAASettings;
	for (AVisualizerBase* Visualizer : Visualizers)
	{
		Visualizer->UpdateAASettings(RefreshedAASettings);
	}
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
