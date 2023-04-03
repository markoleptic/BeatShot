// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#include "BSGameMode.h"
#include "AudioAnalyzerManager.h"
#include "Visualizers/VisualizerManager.h"
#include "BSCharacter.h"
#include "BSGameInstance.h"
#include "BSPlayerController.h"
#include "FloatingTextActor.h"
#include "TargetSpawner.h"
#include "BeatShot/BSGameplayTags.h"
#include "GameFramework/PlayerStart.h"
#include "GameplayAbility/BSGameplayAbility_TrackGun.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetTextLibrary.h"

DEFINE_LOG_CATEGORY(LogAudioData);

ABSGameMode::ABSGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABSGameMode::BeginPlay()
{
	Super::BeginPlay();
	InitializeGameMode();
	OnPostScoresResponse.AddUObject(this, &ABSGameMode::OnPostScoresResponseReceived);
	OnAccessTokenResponse.BindUObject(this, &ABSGameMode::OnAccessTokenResponseReceived);
	Cast<UBSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()))->OnPlayerSettingsChange.AddUniqueDynamic(this, &ABSGameMode::RefreshPlayerSettings);
}

void ABSGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (!bShouldTick || !GetWorldTimerManager().IsTimerActive(GameModeLengthTimer))
	{
		return;
	}
	OnTick_AudioAnalyzers(DeltaSeconds);
}

void ABSGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	ABSPlayerController* NewBSPlayer = Cast<ABSPlayerController>(NewPlayer);
	Controllers.Add(NewBSPlayer);

	if (const ABSCharacter* Character = SpawnPlayer(NewBSPlayer))
	{
		NewBSPlayer->ShowCountdown();

		if (!Character->GetGun()->OnShotFired.IsBoundToObject(this))
		{
			Character->GetGun()->OnShotFired.BindUObject(this, &ABSGameMode::UpdateShotsFired);
		}
		if (BSConfig.IsBeatTrackMode)
		{
			Character->GetGun()->SetShouldTrace(true);
		}
		else
		{
			Character->GetGun()->SetShouldTrace(false);
		}
	}
}

void ABSGameMode::PostLoad()
{
	Super::PostLoad();
}

void ABSGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	Controllers.Remove(Cast<ABSPlayerController>(Exiting));
}

ABSCharacter* ABSGameMode::SpawnPlayer(ABSPlayerController* PlayerController)
{
	if (ABSCharacter* Character = PlayerController->GetBSCharacter())
	{
		Character->Destroy();
	}
	const APlayerStart* ChosenPlayerStart = Cast<APlayerStart>(ChoosePlayerStart(PlayerController));
	ABSCharacter* SpawnedCharacter = GetWorld()->SpawnActor<ABSCharacter>(CharacterClass, ChosenPlayerStart->GetTransform());
	PlayerController->Possess(SpawnedCharacter);
	return SpawnedCharacter;
}

void ABSGameMode::InitializeGameMode()
{
	Elapsed = 0.f;
	LastTargetOnSet = false;
	const FPlayerSettings PlayerSettings = LoadPlayerSettings();
	RefreshPlayerSettings(PlayerSettings);
	BSConfig = Cast<UBSGameInstance>(UGameplayStatics::GetGameInstance(this))->BSConfig;

	TargetSpawner = GetWorld()->SpawnActor<ATargetSpawner>(TargetSpawnerClass, TargetSpawnerLocation, FRotator::ZeroRotator, SpawnParameters);
	TargetSpawner->InitTargetSpawner(BSConfig, PlayerSettings);

	VisualizerManager = GetWorld()->SpawnActor<AVisualizerManager>(VisualizerManagerClass);
	VisualizerManager->InitializeVisualizers(PlayerSettings);

	const ABSCharacter* Character = Cast<ABSCharacter>(Cast<ABSPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0))->GetPawn());
	FGameplayTagContainer Container;
	TArray<FGameplayAbilitySpec*> Activatable;
	Container.AddTag(FBSGameplayTags::Get().Ability_Track);
	UBSGameplayAbility* TrackGunAbilityCDO = TrackGunAbility->GetDefaultObject<UBSGameplayAbility>();
	const FGameplayAbilitySpec AbilitySpec(TrackGunAbilityCDO, 1);
	Character->GetBSAbilitySystemComponent()->GetActivatableGameplayAbilitySpecsByAllMatchingTags(Container, Activatable);

	if (BSConfig.IsBeatTrackMode)
	{
		Character->GetBSAbilitySystemComponent()->TryActivateAbilityByClass(TrackGunAbility);
	}
	else if (!BSConfig.IsBeatTrackMode && Activatable[0]->IsActive())
	{
		Character->GetBSAbilitySystemComponent()->CancelAbility(TrackGunAbilityCDO);
	}

	InitializeAudioManagers();
	BindGameModeDelegates();
	bShouldTick = true;
}

void ABSGameMode::StartGameMode()
{
	if (OnGameModeStarted.IsBound())
	{
		OnGameModeStarted.Broadcast();
	}
	LoadMatchingPlayerScores();
	UpdateScoresToHUD.Broadcast(CurrentPlayerScore);
	StartGameModeTimers();
	TargetSpawner->SetShouldSpawn(true);
}

void ABSGameMode::StartGameModeTimers()
{
	if (BSConfig.GameModeLength == 0.f)
	{
		GetWorldTimerManager().SetTimer(GameModeLengthTimer, 31536000, false);
	}
	else
	{
		GetWorldTimerManager().SetTimer(GameModeLengthTimer, this, &ABSGameMode::OnGameModeLengthTimerComplete, BSConfig.GameModeLength, false);
	}
	GetWorldTimerManager().SetTimer(OnSecondPassedTimer, this, &ABSGameMode::OnSecondPassedCallback, 1.f, true);
}

void ABSGameMode::BindGameModeDelegates()
{
	if (!GetTargetSpawner()->OnTargetSpawned.IsBoundToObject(this))
	{
		GetTargetSpawner()->OnTargetSpawned.AddUObject(this, &ABSGameMode::UpdateTargetsSpawned);
	}
	if (!GetTargetSpawner()->OnTargetDestroyed.IsBoundToObject(this))
	{
		GetTargetSpawner()->OnTargetDestroyed.AddUObject(this, &ABSGameMode::UpdatePlayerScores);
	}
	if (!GetTargetSpawner()->OnBeatTrackTargetDamaged.IsBoundToObject(this))
	{
		GetTargetSpawner()->OnBeatTrackTargetDamaged.AddUObject(this, &ABSGameMode::UpdateTrackingScore);
	}
	for (const ABSPlayerController* Controller : Controllers)
	{
		if (ABSCharacter* Character = Controller->GetBSCharacter())
		{
			if (Character->HasMatchingGameplayTag(FBSGameplayTags::Get().Cheat_AimBot))
			{
				if (!GetTargetSpawner()->OnTargetSpawned_AimBot.IsBoundToObject(Character))
				{
					GetTargetSpawner()->OnTargetSpawned_AimBot.AddUObject(Character, &ABSCharacter::OnTargetSpawned_AimBot);
				}
			}
		}
	}
}

void ABSGameMode::EndGameMode(const bool ShouldSavePlayerScores, const bool ShowPostGameMenu)
{
	GetWorldTimerManager().ClearTimer(GameModeLengthTimer);
	GetWorldTimerManager().ClearTimer(PlayerDelayTimer);
	GetWorldTimerManager().ClearTimer(OnSecondPassedTimer);

	if (TargetSpawner)
	{
		CurrentPlayerScore.LocationAccuracy = TargetSpawner->GetLocationAccuracy();
		TargetSpawner->SetShouldSpawn(false);
		/* Save the QTable */
		if (TargetSpawner->RLBase)
		{
			TargetSpawner->RLBase->PrintRewards();
			BSConfig.AIConfig.QTable = TargetSpawner->RLBase->GetSaveReadyQTable();
			TArray<FBSConfig> CustomGameModesArray = LoadCustomGameModes();
			CustomGameModesArray.RemoveAll([&](const FBSConfig& MatchingConfig)
			{
				return MatchingConfig.CustomGameModeName == BSConfig.CustomGameModeName;
			});
			CustomGameModesArray.Shrink();
			CustomGameModesArray.Add(BSConfig);
			SaveCustomGameMode(CustomGameModesArray);
		}
		if (GetTargetSpawner()->OnTargetSpawned.IsBoundToObject(this))
		{
			GetTargetSpawner()->OnTargetSpawned.RemoveAll(this);
		}
		if (GetTargetSpawner()->OnTargetDestroyed.IsBoundToObject(this))
		{
			GetTargetSpawner()->OnTargetDestroyed.RemoveAll(this);
		}
		if (GetTargetSpawner()->OnBeatTrackTargetDamaged.IsBoundToObject(this))
		{
			GetTargetSpawner()->OnBeatTrackTargetDamaged.RemoveAll(this);
		}
		TargetSpawner->Destroy();
		TargetSpawner = nullptr;
	}

	/** Unbinding delegates */
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

	for (ABSPlayerController* Controller : Controllers)
	{
		Controller->HidePlayerHUD();
		Controller->HideCountdown();
		Controller->HideCrossHair();

		if (Controller->IsLocalController())
		{
			OnPostScoresResponse.AddUObject(Controller, &ABSPlayerController::OnPostScoresResponseReceived);
		}

		if (ShowPostGameMenu)
		{
			Controller->ShowPostGameMenu();
		}
	}
	HandleScoreSaving(ShouldSavePlayerScores);
}

void ABSGameMode::SpawnNewTarget(const bool bNewTargetState)
{
	if (bNewTargetState && !LastTargetOnSet)
	{
		LastTargetOnSet = true;
		if (Elapsed > BSConfig.TargetSpawnCD)
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

void ABSGameMode::OnGameModeLengthTimerComplete()
{
	/** don't save scores if score is zero */
	if (CurrentPlayerScore.Score <= 0 || (CurrentPlayerScore.DefaultMode == EDefaultMode::Custom && CurrentPlayerScore.CustomGameModeName == ""))
	{
		EndGameMode(false, true);
		return;
	}
	EndGameMode(true, true);
}

void ABSGameMode::StartAAManagerPlayback()
{
	switch (BSConfig.AudioFormat)
	{
	case EAudioFormat::File: AATracker->Play();
		if (AAPlayer)
		{
			/* Start playing back audio from AAPlayer after the delay */
			GetWorldTimerManager().SetTimer(PlayerDelayTimer, this, &ABSGameMode::PlayAAPlayer, BSConfig.PlayerDelay, false);
		}
		else
		{
			/* If no AAPlayer, game has started and should use AATracker for audio playback */
			SetAAManagerVolume(LoadPlayerSettings().VideoAndSound.GlobalVolume, LoadPlayerSettings().VideoAndSound.MusicVolume, AATracker);
		}
		break;
	case EAudioFormat::Capture:
		{
			AATracker->StartCapture(BSConfig.bPlaybackAudio, false);
			SetAAManagerVolume(LoadPlayerSettings().VideoAndSound.GlobalVolume, LoadPlayerSettings().VideoAndSound.MusicVolume, AATracker);
			break;
		}
	default: break;
	}
	UE_LOG(LogTemp, Display, TEXT("Now Playing AATracker"));
}

void ABSGameMode::PauseAAManager(const bool ShouldPause)
{
	if (!AATracker)
	{
		return;
	}
	switch (BSConfig.AudioFormat)
	{
	case EAudioFormat::File: AATracker->SetPaused(ShouldPause);
		if (AAPlayer)
		{
			AAPlayer->SetPaused(ShouldPause);
		}
		break;
	case EAudioFormat::Capture: if (ShouldPause)
		{
			AATracker->StopCapture();
		}
		else
		{
			AATracker->StartCapture(BSConfig.bPlaybackAudio, false);
		}
		break;
	default: break;
	}
}

void ABSGameMode::InitializeAudioManagers()
{
	AASettings = LoadAASettings();
	AATracker = NewObject<UAudioAnalyzerManager>(this);

	switch (BSConfig.AudioFormat)
	{
	case EAudioFormat::Capture: AATracker->SetDefaultDevicesCapturerAudio(*BSConfig.InAudioDevice, *BSConfig.OutAudioDevice);
		if (!AATracker->InitCapturerAudioEx(48000, EAA_AudioDepth::B_16, EAA_AudioFormat::Signed_Int, 1.f, BSConfig.bPlaybackAudio))
		{
			OnAAManagerError();
			return;
		}
		break;
	case EAudioFormat::File: if (!AATracker->InitPlayerAudio(BSConfig.SongPath))
		{
			OnAAManagerError();
			return;
		}
		break;
	case EAudioFormat::None: break;
	}

	AATracker->InitBeatTrackingConfigWLimits(EAA_ChannelSelectionMode::All_in_one, 0, AASettings.BandLimits, AASettings.TimeWindow, 10 / AASettings.TimeWindow, false, 100, 2.1);
	AATracker->InitSpectrumConfigWLimits(EAA_ChannelSelectionMode::All_in_one, -1, AASettings.BandLimits, AASettings.TimeWindow, 10 / AASettings.TimeWindow, true, AASettings.NumBandChannels);
	//AATracker->InitPitchTrackingConfig(EAA_ChannelSelectionMode::All_in_one, -1, 0.02, 0.19);
	SetAAManagerVolume(0, 0, AATracker);

	/* AAPlayer will only be used if AudioFormat is File and PlayerDelay > 0.01f */
	if (BSConfig.PlayerDelay < 0.01f)
	{
		AAPlayer = nullptr;
		return;
	}
	AAPlayer = NewObject<UAudioAnalyzerManager>(this);
	if (!AAPlayer->InitPlayerAudio(BSConfig.SongPath))
	{
		OnAAManagerError();
		return;
	}
	AAPlayer->InitBeatTrackingConfigWLimits(EAA_ChannelSelectionMode::All_in_one, 0, AASettings.BandLimits, AASettings.TimeWindow, AASettings.HistorySize, false, 100, 2.1);
	AAPlayer->InitSpectrumConfigWLimits(EAA_ChannelSelectionMode::All_in_one, -1, AASettings.BandLimits, AASettings.TimeWindow, 10 / AASettings.TimeWindow, true, AASettings.NumBandChannels);
	SetAAManagerVolume(0, 0, AAPlayer);
}

void ABSGameMode::OnTick_AudioAnalyzers(const float DeltaSeconds)
{
	Elapsed += DeltaSeconds;

	AATracker->GetBeatTrackingWLimitsWThreshold(Beats, SpectrumValues, BpmCurrent, BpmTotal, AASettings.BandLimitsThreshold);
	for (const bool Beat : Beats)
	{
		SpawnNewTarget(Beat);
	}

	if (AAPlayer)
	{
		AAPlayer->GetBeatTrackingWLimitsWThreshold(Beats, SpectrumValues, BpmCurrent, BpmTotal, AASettings.BandLimitsThreshold);
		AAPlayer->GetBeatTrackingAverageAndVariance(SpectrumVariance, VisualizerManager->AvgSpectrumValues);
	}
	else
	{
		AATracker->GetBeatTrackingAverageAndVariance(SpectrumVariance, VisualizerManager->AvgSpectrumValues);
	}
	VisualizerManager->UpdateVisualizers(SpectrumValues);
}

void ABSGameMode::PlayAAPlayer() const
{
	if (!AAPlayer)
	{
		return;
	}
	AAPlayer->Play();
	SetAAManagerVolume(LoadPlayerSettings().VideoAndSound.GlobalVolume, LoadPlayerSettings().VideoAndSound.MusicVolume, AAPlayer);
	UE_LOG(LogTemp, Display, TEXT("Now Playing AAPlayer %f"), AAPlayer->GetPlaybackVolume());
}

void ABSGameMode::SetAAManagerVolume(const float GlobalVolume, const float MusicVolume, UAudioAnalyzerManager* AAManager) const
{
	if (AAManager)
	{
		switch (BSConfig.AudioFormat)
		{
		case EAudioFormat::File: AAManager->SetPlaybackVolume(GlobalVolume / 100 * MusicVolume / 100);
			break;
		case EAudioFormat::Capture: AAManager->SetCaptureVolume(1, GlobalVolume / 100 * MusicVolume / 100);
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
		switch (BSConfig.AudioFormat)
		{
		case EAudioFormat::File: AATracker->SetPlaybackVolume(GlobalVolume / 100 * MusicVolume / 100);
			break;
		case EAudioFormat::Capture: AATracker->SetCaptureVolume(1, GlobalVolume / 100 * MusicVolume / 100);
			break;
		case EAudioFormat::None: break;
		}
	}
}

void ABSGameMode::OnSecondPassedCallback() const
{
	OnSecondPassed.Broadcast(GetWorldTimerManager().GetTimerElapsed(GameModeLengthTimer));
}

void ABSGameMode::RefreshAASettings(const FAASettingsStruct& RefreshedAASettings)
{
	AASettings = RefreshedAASettings;
	VisualizerManager->UpdateAASettings(RefreshedAASettings);
}

void ABSGameMode::RefreshPlayerSettings(const FPlayerSettings& RefreshedPlayerSettings)
{
	SetAAManagerVolume(RefreshedPlayerSettings.VideoAndSound.GlobalVolume, RefreshedPlayerSettings.VideoAndSound.MusicVolume);
	bShowStreakCombatText = RefreshedPlayerSettings.Game.bShowStreakCombatText;
	CombatTextFrequency = RefreshedPlayerSettings.Game.CombatTextFrequency;
	bNightModeUnlocked = RefreshedPlayerSettings.User.bNightModeUnlocked;

	if (VisualizerManager)
	{
		VisualizerManager->UpdateVisualizerStates(RefreshedPlayerSettings);
	}
	if (TargetSpawner)
	{
		TargetSpawner->UpdatePlayerSettings(RefreshedPlayerSettings);
	}
}

void ABSGameMode::LoadMatchingPlayerScores()
{
	CurrentPlayerScore.ResetStruct();
	const TArray<FPlayerScore> PlayerScores = LoadPlayerScores();
	if (BSConfig.DefaultMode == EDefaultMode::Custom)
	{
		for (FPlayerScore ScoreObject : PlayerScores)
		{
			if (ScoreObject.CustomGameModeName.Equals(BSConfig.CustomGameModeName) && ScoreObject.SongTitle.Equals(BSConfig.SongTitle))
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
			if (ScoreObject.DefaultMode == BSConfig.DefaultMode && ScoreObject.SongTitle.Equals(BSConfig.SongTitle) && ScoreObject.Difficulty == BSConfig.GameModeDifficulty)
			{
				if (ScoreObject.Score > CurrentPlayerScore.HighScore)
				{
					CurrentPlayerScore.HighScore = ScoreObject.Score;
				}
			}
		}
	}
	CurrentPlayerScore.DefaultMode = BSConfig.DefaultMode;
	CurrentPlayerScore.SongTitle = BSConfig.SongTitle;
	CurrentPlayerScore.SongLength = BSConfig.GameModeLength;
	CurrentPlayerScore.CustomGameModeName = BSConfig.CustomGameModeName;
	CurrentPlayerScore.Difficulty = BSConfig.GameModeDifficulty;
	CurrentPlayerScore.TotalPossibleDamage = 0.f;
	if (BSConfig.GameModeLength == 0.f)
	{
		MaxScorePerTarget = 1000.f;
	}
	else
	{
		MaxScorePerTarget = 100000.f / ((BSConfig.GameModeLength - 1.f) / BSConfig.TargetSpawnCD);
	}
}

void ABSGameMode::HandleScoreSaving(const bool bShouldSavePlayerScores)
{
	/** don't save scores if score is zero */
	if (!bShouldSavePlayerScores || CurrentPlayerScore.Score <= 0 || (CurrentPlayerScore.DefaultMode == EDefaultMode::Custom && CurrentPlayerScore.CustomGameModeName == ""))
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

FPlayerScore ABSGameMode::GetCompletedPlayerScores()
{
	/** save current time */
	CurrentPlayerScore.Time = FDateTime::UtcNow().ToIso8601();

	/** for BeatTrack modes */
	if (BSConfig.IsBeatTrackMode)
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

void ABSGameMode::SaveScoresToDatabase()
{
	if (!LoadPlayerSettings().User.HasLoggedInHttp)
	{
		OnPostScoresResponse.Broadcast(ELoginState::NewUser);
		return;
	}
	RequestAccessToken(LoadPlayerSettings().User.LoginCookie, OnAccessTokenResponse);
}

void ABSGameMode::OnAccessTokenResponseReceived(const FString& AccessToken)
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

void ABSGameMode::OnPostScoresResponseReceived(const ELoginState& LoginState)
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

void ABSGameMode::UpdatePlayerScores(const float TimeElapsed, const int32 NewStreak, const FVector& Position)
{
	if (BSConfig.IsBeatTrackMode == true || TimeElapsed == -1)
	{
		return;
	}

	if (TimeElapsed <= BSConfig.PlayerDelay - 0.05f)
	{
		CurrentPlayerScore.Score += FMath::Lerp(MaxScorePerTarget / 2, MaxScorePerTarget, TimeElapsed / BSConfig.PlayerDelay);
	}
	else if (TimeElapsed <= BSConfig.PlayerDelay + 0.05f)
	{
		CurrentPlayerScore.Score += MaxScorePerTarget;
	}
	else if (TimeElapsed <= BSConfig.TargetMaxLifeSpan)
	{
		CurrentPlayerScore.Score += FMath::Lerp(MaxScorePerTarget, MaxScorePerTarget / 2, (TimeElapsed - BSConfig.PlayerDelay + 0.05f) / (BSConfig.TargetMaxLifeSpan - (BSConfig.PlayerDelay + 0.05f)));
		// UE_LOG(LogTemp, Display, TEXT("Last: %f"), FMath::Lerp(MaxScorePerTarget, MaxScorePerTarget / 2,
		// (TimeElapsed - BSConfig.PlayerDelay + 0.05f) /
		// (BSConfig.TargetMaxLifeSpan - (BSConfig.PlayerDelay + 0.05f))))
	}
	UpdateTargetsHit();
	UpdateStreak(NewStreak, Position);
	UpdateHighScore();
	CurrentPlayerScore.TotalTimeOffset += FMath::Abs(TimeElapsed - BSConfig.PlayerDelay);
	UpdateScoresToHUD.Broadcast(CurrentPlayerScore);
}

void ABSGameMode::UpdateTrackingScore(const float OldValue, const float NewValue, const float TotalPossibleDamage)
{
	UE_LOG(LogTemp, Display, TEXT("OldValue %f NewValue %f TotalPossibleDamage %f"), OldValue, NewValue, TotalPossibleDamage);
	CurrentPlayerScore.TotalPossibleDamage = TotalPossibleDamage;
	CurrentPlayerScore.Score += abs(OldValue - NewValue);
	UpdateHighScore();
	UpdateScoresToHUD.Broadcast(CurrentPlayerScore);
}

void ABSGameMode::UpdateTargetsSpawned()
{
	CurrentPlayerScore.TargetsSpawned++;
	UpdateScoresToHUD.Broadcast(CurrentPlayerScore);
}

void ABSGameMode::UpdateShotsFired()
{
	CurrentPlayerScore.ShotsFired++;
	UpdateScoresToHUD.Broadcast(CurrentPlayerScore);
}

void ABSGameMode::UpdateStreak(const int32 Streak, const FVector& Location)
{
	if (Streak > CurrentPlayerScore.Streak)
	{
		CurrentPlayerScore.Streak = Streak;
	}
	if (bShowStreakCombatText)
	{
		if (CombatTextFrequency != 0 && Streak % CombatTextFrequency == 0)
		{
			if (AFloatingTextActor* FloatingTextActor = GetWorld()->SpawnActor<AFloatingTextActor>(FloatingTextActorClass, Location, FRotator()))
			{
				FloatingTextActor->Initialize(UKismetTextLibrary::Conv_IntToText(Streak));
			}
		}
	}

	if (Streak > StreakThreshold && !bNightModeUnlocked)
	{
		if (!OnStreakThresholdPassed.ExecuteIfBound())
		{
			UE_LOG(LogTemp, Display, TEXT("OnStreakThresholdPassed not bound."));
		}
		FPlayerSettings Settings = LoadPlayerSettings();
		bNightModeUnlocked = true;
		Settings.User.bNightModeUnlocked = true;
		SavePlayerSettings(Settings);
		Cast<UBSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()))->OnPlayerSettingsChange.Broadcast(Settings);
	}
}

void ABSGameMode::UpdateTargetsHit()
{
	CurrentPlayerScore.TargetsHit++;
}

void ABSGameMode::UpdateHighScore()
{
	if (CurrentPlayerScore.Score > CurrentPlayerScore.HighScore)
	{
		CurrentPlayerScore.HighScore = CurrentPlayerScore.Score;
	}
}

float ABSGameMode::FloatDivide(const float Num, const float Denom)
{
	if (Denom == 0)
	{
		return 0;
	}
	return Num / Denom;
}
