// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#include "BSGameMode.h"
#include "AudioAnalyzerManager.h"
#include "Visualizers/VisualizerManager.h"
#include "Character/BSCharacter.h"
#include "BSGameInstance.h"
#include "Player/BSPlayerController.h"
#include "Target/TargetManager.h"
#include "BeatShot/BSGameplayTags.h"
#include "GameFramework/PlayerStart.h"
#include "AbilitySystem/BSGameplayAbility_TrackGun.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogAudioData);

ABSGameMode::ABSGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
	bShouldTick = false;
}

void ABSGameMode::BeginPlay()
{
	Super::BeginPlay();

	OnPostScoresResponse.AddUObject(this, &ABSGameMode::OnPostScoresResponseReceived);
	OnAccessTokenResponse.BindUObject(this, &ABSGameMode::OnAccessTokenResponseReceived);
	UBSGameInstance* GI = Cast<UBSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	GI->AddDelegateToOnPlayerSettingsChanged(OnPlayerSettingsChangedDelegate_Game);
	GI->AddDelegateToOnPlayerSettingsChanged(OnPlayerSettingsChangedDelegate_User);
	GI->AddDelegateToOnPlayerSettingsChanged(OnPlayerSettingsChangedDelegate_AudioAnalyzer);
	GI->AddDelegateToOnPlayerSettingsChanged(OnPlayerSettingsChangedDelegate_VideoAndSound);
	GI->GetPublicGameSettingsChangedDelegate().AddUniqueDynamic(this, &ABSGameMode::OnPlayerSettingsChanged_Game);
	GI->GetPublicUserSettingsChangedDelegate().AddUniqueDynamic(this, &ABSGameMode::OnPlayerSettingsChanged_User);
	GI->GetPublicAudioAnalyzerSettingsChangedDelegate().AddUniqueDynamic(this, &ABSGameMode::OnPlayerSettingsChanged_AudioAnalyzer);
	GI->GetPublicVideoAndSoundSettingsChangedDelegate().AddUniqueDynamic(this, &ABSGameMode::OnPlayerSettingsChanged_VideoAndSound);

	InitializeGameMode();
}

void ABSGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (bShouldTick && GetWorldTimerManager().IsTimerActive(GameModeLengthTimer))
	{
		OnTick_AudioAnalyzers(DeltaSeconds);
	}
}

void ABSGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	ABSPlayerController* NewBSPlayer = Cast<ABSPlayerController>(NewPlayer);
	Controllers.Add(NewBSPlayer);

	if (SpawnPlayer(NewBSPlayer))
	{
		NewBSPlayer->ShowCountdown(false);
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

	/* Load settings */
	const FPlayerSettings PlayerSettings = LoadPlayerSettings();
	OnPlayerSettingsChanged_Game(PlayerSettings.Game);
	OnPlayerSettingsChanged_User(PlayerSettings.User);
	OnPlayerSettingsChanged_AudioAnalyzer(PlayerSettings.AudioAnalyzer);
	OnPlayerSettingsChanged_VideoAndSound(PlayerSettings.VideoAndSound);

	/* Get config from Game Instance */
	BSConfig = Cast<UBSGameInstance>(UGameplayStatics::GetGameInstance(this))->BSConfig;

	/* Spawn TargetManager and VisualizerManager */
	TargetManager = GetWorld()->SpawnActor<ATargetManager>(TargetManagerClass, FVector::Zero(), FRotator::ZeroRotator, SpawnParameters);
	TargetManager->InitTargetManager(BSConfig, PlayerSettings.Game);

	if (!VisualizerManager)
	{
		VisualizerManager = GetWorld()->SpawnActor<AVisualizerManager>(VisualizerManagerClass);
		VisualizerManager->InitializeVisualizers(PlayerSettings.Game, PlayerSettings.AudioAnalyzer);
	}

	/* Handle abilities for different game modes */
	for (const ABSPlayerController* Controller : Controllers)
	{
		const ABSCharacter* Character = Controller->GetBSCharacter();
		const TArray<FGameplayAbilitySpec*> AbilitySpecs = Character->GetBSAbilitySystemComponent()->GetAbilitySpecsFromGameplayTag(FBSGameplayTags::Get().Ability_Track);
		if (BSConfig.DefiningConfig.BaseGameMode == EBaseGameMode::BeatTrack)
		{
			if (AbilitySpecs.IsEmpty())
			{
				const FGameplayAbilitySpec AbilitySpec(TrackGunAbility->GetDefaultObject<UBSGameplayAbility>(), 1);
				Character->GetBSAbilitySystemComponent()->GiveAbility(AbilitySpec);
			}
		}
		else if (BSConfig.DefiningConfig.BaseGameMode != EBaseGameMode::BeatTrack && !AbilitySpecs.IsEmpty())
		{
			Character->GetBSAbilitySystemComponent()->CancelAbility(TrackGunAbility->GetDefaultObject<UBSGameplayAbility>());
		}
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
	TargetManager->SetShouldSpawn(true);
}

void ABSGameMode::StartGameModeTimers()
{
	if (BSConfig.AudioConfig.SongLength == 0.f)
	{
		GetWorldTimerManager().SetTimer(GameModeLengthTimer, 31536000, false);
	}
	else
	{
		GetWorldTimerManager().SetTimer(GameModeLengthTimer, this, &ABSGameMode::OnGameModeLengthTimerComplete, BSConfig.AudioConfig.SongLength, false);
	}
	GetWorldTimerManager().SetTimer(OnSecondPassedTimer, this, &ABSGameMode::OnSecondPassedCallback, 1.f, true);
}

void ABSGameMode::BindGameModeDelegates()
{
	if (!GetTargetManager()->OnTargetActivatedOrSpawned.IsBoundToObject(this))
	{
		GetTargetManager()->OnTargetActivatedOrSpawned.AddUObject(this, &ABSGameMode::UpdateTargetsSpawned);
	}
	if (!GetTargetManager()->OnTargetDeactivated.IsBoundToObject(this))
	{
		GetTargetManager()->OnTargetDeactivated.AddUObject(this, &ABSGameMode::UpdatePlayerScores);
	}
	if (!GetTargetManager()->OnBeatTrackTargetDamaged.IsBoundToObject(this))
	{
		GetTargetManager()->OnBeatTrackTargetDamaged.AddUObject(this, &ABSGameMode::UpdateTrackingScore);
	}
	for (const ABSPlayerController* Controller : Controllers)
	{
		if (ABSCharacter* Character = Controller->GetBSCharacter())
		{
			if (Character->HasMatchingGameplayTag(FBSGameplayTags::Get().Cheat_AimBot))
			{
				if (!GetTargetManager()->OnTargetActivated_AimBot.IsBoundToObject(Character))
				{
					GetTargetManager()->OnTargetActivated_AimBot.AddUObject(Character, &ABSCharacter::OnTargetSpawned_AimBot);
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

	if (TargetManager)
	{
		TargetManager->SetShouldSpawn(false);

		/* Saves CommonScoreInfo like total spawns, hits, and QTable */
		const FCommonScoreInfo ScoreInfo = TargetManager->GetCommonScoreInfo();
		SaveCommonScoreInfo(BSConfig.DefiningConfig, ScoreInfo);
		CurrentPlayerScore.LocationAccuracy = TargetManager->GetLocationAccuracy();

		if (GetTargetManager()->OnTargetActivatedOrSpawned.IsBoundToObject(this))
		{
			GetTargetManager()->OnTargetActivatedOrSpawned.RemoveAll(this);
		}
		if (GetTargetManager()->OnTargetDeactivated.IsBoundToObject(this))
		{
			GetTargetManager()->OnTargetDeactivated.RemoveAll(this);
		}
		if (GetTargetManager()->OnBeatTrackTargetDamaged.IsBoundToObject(this))
		{
			GetTargetManager()->OnBeatTrackTargetDamaged.RemoveAll(this);
		}
		TargetManager->Destroy();
		TargetManager = nullptr;
	}
	
	if (VisualizerManager)
	{
		VisualizerManager->DeactivateVisualizers();
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
			if (const ABSCharacter* Character = Controller->GetBSCharacter(); Character->HasMatchingGameplayTag(FBSGameplayTags::Get().State_Firing))
			{
				if (const TArray<FGameplayAbilitySpec*> AbilitySpecs = Character->GetBSAbilitySystemComponent()->GetAbilitySpecsFromGameplayTag(FBSGameplayTags::Get().Ability_Fire); !AbilitySpecs.
					IsEmpty())
				{
					Character->GetBSAbilitySystemComponent()->CancelAbilityHandle(AbilitySpecs[0]->Handle);
					Character->GetBSAbilitySystemComponent()->ClearAbilityInput();
				}
			}
		}
	}
	HandleScoreSaving(ShouldSavePlayerScores);
}

void ABSGameMode::RegisterWeapon(FOnShotFired& OnShotFiredDelegate)
{
	OnShotFiredDelegate.BindUObject(this, &ABSGameMode::UpdateShotsFired);
}

void ABSGameMode::SpawnNewTarget(const bool bNewTargetState)
{
	if (bNewTargetState && !LastTargetOnSet)
	{
		LastTargetOnSet = true;
		if (Elapsed > BSConfig.TargetConfig.TargetSpawnCD)
		{
			Elapsed = 0.f;
			TargetManager->OnAudioAnalyzerBeat();
		}
	}
	else if (!bNewTargetState && LastTargetOnSet)
	{
		LastTargetOnSet = false;
	}
}

void ABSGameMode::OnGameModeLengthTimerComplete()
{
	/* don't save scores if score is zero */
	if (CurrentPlayerScore.Score <= 0 || (CurrentPlayerScore.DefiningConfig.GameModeType == EGameModeType::Custom && CurrentPlayerScore.DefiningConfig.CustomGameModeName == ""))
	{
		EndGameMode(false, true);
		return;
	}
	EndGameMode(true, true);
}

void ABSGameMode::StartAAManagerPlayback()
{
	switch (BSConfig.AudioConfig.AudioFormat)
	{
	case EAudioFormat::File:
		AATracker->Play();
		if (AAPlayer)
		{
			/* Start playing back audio from AAPlayer after the delay */
			GetWorldTimerManager().SetTimer(PlayerDelayTimer, this, &ABSGameMode::PlayAAPlayer, BSConfig.AudioConfig.PlayerDelay, false);
		}
		else
		{
			/* If no AAPlayer, game has started and should use AATracker for audio playback */
			SetAAManagerVolume(LoadPlayerSettings().VideoAndSound.GlobalVolume, LoadPlayerSettings().VideoAndSound.MusicVolume, AATracker);
		}
		break;
	case EAudioFormat::Capture:
		{
			AATracker->StartCapture(BSConfig.AudioConfig.bPlaybackAudio, false);
			SetAAManagerVolume(LoadPlayerSettings().VideoAndSound.GlobalVolume, LoadPlayerSettings().VideoAndSound.MusicVolume, AATracker);
			break;
		}
	case EAudioFormat::Loopback:
		{
			AATracker->StartLoopback(false);
			SetAAManagerVolume(LoadPlayerSettings().VideoAndSound.GlobalVolume, LoadPlayerSettings().VideoAndSound.MusicVolume, AATracker);
			break;
		}
	default:
		break;
	}
	UE_LOG(LogTemp, Display, TEXT("Now Playing AATracker"));
}

void ABSGameMode::PauseAAManager(const bool ShouldPause)
{
	if (!AATracker)
	{
		return;
	}
	switch (BSConfig.AudioConfig.AudioFormat)
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
			AATracker->StartCapture(BSConfig.AudioConfig.bPlaybackAudio, false);
		}
		break;
	case EAudioFormat::Loopback:
		if (ShouldPause)
		{
			AATracker->StopLoopback();
		}
		else
		{
			AATracker->StartLoopback(false);
		}
		break;
	default:
		break;
	}
}

void ABSGameMode::InitializeAudioManagers()
{
	AATracker = NewObject<UAudioAnalyzerManager>(this);
	switch (BSConfig.AudioConfig.AudioFormat) 
	{
	case EAudioFormat::None:
		break;
	case EAudioFormat::File:
		if (!AATracker->InitPlayerAudio(BSConfig.AudioConfig.SongPath))
		{
			OnAAManagerError();
			return;
		}
		break;
	case EAudioFormat::Capture:
		AATracker->SetDefaultDevicesCapturerAudio(*BSConfig.AudioConfig.InAudioDevice, *BSConfig.AudioConfig.OutAudioDevice);
		 if (!AATracker->InitCapturerAudioEx(48000, EAA_AudioDepth::B_16, EAA_AudioFormat::Signed_Int, 1.f, BSConfig.AudioConfig.bPlaybackAudio))
		 {
		 	OnAAManagerError();
		 	return;
		 }
		 break;
	case EAudioFormat::Loopback:
		AATracker->SetDefaultDevicesCapturerAudio(*BSConfig.AudioConfig.InAudioDevice, *BSConfig.AudioConfig.OutAudioDevice);
		AATracker->SetDefaultDeviceLoopbackAudio(*BSConfig.AudioConfig.OutAudioDevice);
		if (!AATracker->InitLoopbackAudio())
		{
			OnAAManagerError();
			return;
		}
		break;
	default:
		break;
	}
	
	AATracker->InitBeatTrackingConfigWLimits(EAA_ChannelSelectionMode::All_in_one, 0, AASettings.BandLimits, AASettings.TimeWindow, 10 / AASettings.TimeWindow, false, 100, 2.1);
	AATracker->InitSpectrumConfigWLimits(EAA_ChannelSelectionMode::All_in_one, -1, AASettings.BandLimits, AASettings.TimeWindow, 10 / AASettings.TimeWindow, true, AASettings.NumBandChannels);

	//AATracker->InitPitchTrackingConfig(EAA_ChannelSelectionMode::All_in_one, -1, 0.02, 0.19);
	SetAAManagerVolume(0, 0, AATracker);
	
	/* AAPlayer will only be used if AudioFormat is File and PlayerDelay > 0.01f */
	if (BSConfig.AudioConfig.PlayerDelay < 0.01f)
	{
		AAPlayer = nullptr;
		return;
	}
	AAPlayer = NewObject<UAudioAnalyzerManager>(this);
	if (!AAPlayer->InitPlayerAudio(BSConfig.AudioConfig.SongPath))
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
		switch (BSConfig.AudioConfig.AudioFormat)
		{
		case EAudioFormat::File:
			AAManager->SetPlaybackVolume(GlobalVolume / 100 * MusicVolume / 100);
			break;
		case EAudioFormat::Capture:
			AAManager->SetCaptureVolume(1, GlobalVolume / 100 * MusicVolume / 100);
			break;
		case EAudioFormat::Loopback:
			break;
		case EAudioFormat::None:
			break;
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
		switch (BSConfig.AudioConfig.AudioFormat)
		{
		case EAudioFormat::File:
			AATracker->SetPlaybackVolume(GlobalVolume / 100 * MusicVolume / 100);
			break;
		case EAudioFormat::Capture:
			AATracker->SetCaptureVolume(1, GlobalVolume / 100 * MusicVolume / 100);
			break;
		case EAudioFormat::Loopback:
			break;
		case EAudioFormat::None:
			break;
		}
	}
}

void ABSGameMode::OnSecondPassedCallback() const
{
	OnSecondPassed.Broadcast(GetWorldTimerManager().GetTimerElapsed(GameModeLengthTimer));
}

void ABSGameMode::LoadMatchingPlayerScores()
{
	CurrentPlayerScore.ResetStruct();
	CurrentPlayerScore.DefiningConfig = BSConfig.DefiningConfig;
	CurrentPlayerScore.SongTitle = BSConfig.AudioConfig.SongTitle;
	CurrentPlayerScore.SongLength = BSConfig.AudioConfig.SongLength;
	CurrentPlayerScore.TotalPossibleDamage = 0.f;

	if (BSConfig.AudioConfig.SongLength == 0.f)
	{
		MaxScorePerTarget = 1000.f;
	}
	else
	{
		MaxScorePerTarget = 100000.f / ((BSConfig.AudioConfig.SongLength - 1.f) / BSConfig.TargetConfig.TargetSpawnCD);
	}

	const TArray<FPlayerScore> PlayerScores = LoadPlayerScores().FilterByPredicate([&](const FPlayerScore& PlayerScore)
	{
		if (PlayerScore == CurrentPlayerScore)
		{
			return true;
		}
		return false;
	});

	for (const FPlayerScore& ScoreObject : PlayerScores)
	{
		if (ScoreObject.Score > CurrentPlayerScore.HighScore)
		{
			CurrentPlayerScore.HighScore = ScoreObject.Score;
		}
	}
}

void ABSGameMode::HandleScoreSaving(const bool bShouldSavePlayerScores)
{
	/** don't save scores if score is zero */
	if (!bShouldSavePlayerScores || CurrentPlayerScore.Score <= 0 || (CurrentPlayerScore.DefiningConfig.GameModeType == EGameModeType::Custom && CurrentPlayerScore.DefiningConfig.CustomGameModeName ==
		""))
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
	if (BSConfig.DefiningConfig.BaseGameMode == EBaseGameMode::BeatTrack)
	{
		CurrentPlayerScore.Accuracy = FloatDivide(CurrentPlayerScore.Score, CurrentPlayerScore.TotalPossibleDamage);
		CurrentPlayerScore.Completion = FloatDivide(CurrentPlayerScore.Score, CurrentPlayerScore.TotalPossibleDamage);
		return CurrentPlayerScore;
	}
	CurrentPlayerScore.AvgTimeOffset = FloatDivide(CurrentPlayerScore.TotalTimeOffset, CurrentPlayerScore.TargetsHit);
	CurrentPlayerScore.Accuracy = FloatDivide(CurrentPlayerScore.TargetsHit, CurrentPlayerScore.ShotsFired);
	CurrentPlayerScore.Completion = FloatDivide(CurrentPlayerScore.TargetsHit, CurrentPlayerScore.TargetsSpawned);
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

void ABSGameMode::OnPlayerSettingsChanged_Game(const FPlayerSettings_Game& GameSettings)
{
	bShowStreakCombatText = GameSettings.bShowStreakCombatText;
	CombatTextFrequency = GameSettings.CombatTextFrequency;
	if (VisualizerManager)
	{
		VisualizerManager->UpdateVisualizerSettings(GameSettings);
	}
	if (TargetManager)
	{
		TargetManager->UpdatePlayerSettings(GameSettings);
	}
}

void ABSGameMode::OnPlayerSettingsChanged_AudioAnalyzer(const FPlayerSettings_AudioAnalyzer& AudioAnalyzerSettings)
{
	AASettings = AudioAnalyzerSettings;
	if (VisualizerManager)
	{
		VisualizerManager->UpdateAASettings(AudioAnalyzerSettings);
	}
}

void ABSGameMode::OnPlayerSettingsChanged_User(const FPlayerSettings_User& UserSettings)
{
	bNightModeUnlocked = UserSettings.bNightModeUnlocked;
}

void ABSGameMode::OnPlayerSettingsChanged_VideoAndSound(const FPlayerSettings_VideoAndSound& VideoAndSoundSettings)
{
	SetAAManagerVolume(VideoAndSoundSettings.GlobalVolume, VideoAndSoundSettings.MusicVolume);
}

void ABSGameMode::OnAccessTokenResponseReceived(const FString& AccessToken)
{
	if (AccessToken.IsEmpty())
	{
		OnPostScoresResponse.Broadcast(ELoginState::InvalidHttp);
		return;
	}
	TArray<FPlayerScore> ScoresNotSavedToDB;
	for (const FPlayerScore& PlayerScoreObj : LoadPlayerScores())
	{
		if (!PlayerScoreObj.bSavedToDatabase)
		{
			ScoresNotSavedToDB.Add(PlayerScoreObj);
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
	if (BSConfig.DefiningConfig.BaseGameMode == EBaseGameMode::BeatTrack || TimeElapsed == -1)
	{
		return;
	}

	if (TimeElapsed <= BSConfig.AudioConfig.PlayerDelay - 0.05f)
	{
		CurrentPlayerScore.Score += FMath::Lerp(MaxScorePerTarget / 2, MaxScorePerTarget, TimeElapsed / BSConfig.AudioConfig.PlayerDelay);
	}
	else if (TimeElapsed <= BSConfig.AudioConfig.PlayerDelay + 0.05f)
	{
		CurrentPlayerScore.Score += MaxScorePerTarget;
	}
	else if (TimeElapsed <= BSConfig.TargetConfig.TargetMaxLifeSpan)
	{
		CurrentPlayerScore.Score += FMath::Lerp(MaxScorePerTarget, MaxScorePerTarget / 2,
		                                        (TimeElapsed - BSConfig.AudioConfig.PlayerDelay + 0.05f) / (BSConfig.TargetConfig.TargetMaxLifeSpan - (BSConfig.AudioConfig.PlayerDelay + 0.05f)));
		// UE_LOG(LogTemp, Display, TEXT("Last: %f"), FMath::Lerp(MaxScorePerTarget, MaxScorePerTarget / 2,
		// (TimeElapsed - BSConfig.PlayerDelay + 0.05f) /
		// (BSConfig.TargetMaxLifeSpan - (BSConfig.PlayerDelay + 0.05f))))
	}
	UpdateTargetsHit();
	UpdateStreak(NewStreak, Position);
	UpdateHighScore();
	CurrentPlayerScore.TotalTimeOffset += FMath::Abs(TimeElapsed - BSConfig.AudioConfig.PlayerDelay);
	UpdateScoresToHUD.Broadcast(CurrentPlayerScore);
}

void ABSGameMode::UpdateTrackingScore(const float DamageDelta, const float TotalPossibleDamage)
{
	CurrentPlayerScore.TotalPossibleDamage = TotalPossibleDamage;
	CurrentPlayerScore.Score += DamageDelta;
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
			for (ABSPlayerController* Controller : Controllers)
			{
				Controller->ShowCombatText(Streak, Location);
			}
		}
	}

	if (Streak > StreakThreshold && !bNightModeUnlocked)
	{
		if (!OnStreakThresholdPassed.ExecuteIfBound())
		{
			UE_LOG(LogTemp, Display, TEXT("OnStreakThresholdPassed not bound."));
		}
		FPlayerSettings_User Settings = LoadPlayerSettings().User;
		Settings.bNightModeUnlocked = true;
		bNightModeUnlocked = true;
		SavePlayerSettings(Settings);
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
