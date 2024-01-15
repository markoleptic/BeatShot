// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#include "BSGameMode.h"
#include "AudioAnalyzerManager.h"
#include "Visualizers/VisualizerManager.h"
#include "Character/BSCharacter.h"
#include "BSGameInstance.h"
#include "AbilitySystem/Abilities/BSGA_AimBot.h"
#include "Player/BSPlayerController.h"
#include "Target/TargetManager.h"
#include "GameFramework/PlayerStart.h"
#include "AbilitySystem/Abilities/BSGA_TrackGun.h"
#include "AbilitySystem/Globals/BSAttributeSetBase.h"
#include "Equipment/BSGun.h"
#include "Kismet/GameplayStatics.h"
#include "System/SteamManager.h"

DEFINE_LOG_CATEGORY(LogAudioData);

ABSGameMode::ABSGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
	bShouldTick = false;
	Elapsed = 0.f;
	bLastTargetOnSet = false;
	TimePlayedGameMode = 0.f;
	AATracker = nullptr;
	AAPlayer = nullptr;
	TrackGunAbilitySet = nullptr;
	MaxScorePerTarget = 0;
}

void ABSGameMode::BeginPlay()
{
	Super::BeginPlay();

	UBSGameInstance* GI = Cast<UBSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	GI->AddDelegateToOnPlayerSettingsChanged(OnPlayerSettingsChangedDelegate_Game);
	GI->AddDelegateToOnPlayerSettingsChanged(OnPlayerSettingsChangedDelegate_User);
	GI->AddDelegateToOnPlayerSettingsChanged(OnPlayerSettingsChangedDelegate_AudioAnalyzer);
	GI->AddDelegateToOnPlayerSettingsChanged(OnPlayerSettingsChangedDelegate_VideoAndSound);
	GI->GetPublicGameSettingsChangedDelegate().AddUObject(this, &ABSGameMode::OnPlayerSettingsChanged_Game);
	GI->GetPublicUserSettingsChangedDelegate().AddUObject(this, &ABSGameMode::OnPlayerSettingsChanged_User);
	GI->GetPublicAudioAnalyzerSettingsChangedDelegate().AddUObject(this,
		&ABSGameMode::OnPlayerSettingsChanged_AudioAnalyzer);
	GI->GetPublicVideoAndSoundSettingsChangedDelegate().AddUObject(this,
		&ABSGameMode::OnPlayerSettingsChanged_VideoAndSound);

	InitializeGameMode(GI->GetBSConfig());
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
	CurrentPlayerScores.Add(NewBSPlayer, FPlayerScore());
	SpawnPlayer(NewBSPlayer);
}

void ABSGameMode::PostLoad()
{
	Super::PostLoad();
}

void ABSGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	Controllers.Remove(Cast<ABSPlayerController>(Exiting));
	CurrentPlayerScores.Remove(Cast<ABSPlayerController>(Exiting));
}

ABSCharacter* ABSGameMode::SpawnPlayer(ABSPlayerController* PlayerController)
{
	if (ABSCharacter* Character = PlayerController->GetBSCharacter())
	{
		Character->Destroy();
	}
	const APlayerStart* ChosenPlayerStart = Cast<APlayerStart>(ChoosePlayerStart(PlayerController));
	ABSCharacter* SpawnedCharacter = GetWorld()->SpawnActor<ABSCharacter>(CharacterClass,
		ChosenPlayerStart->GetTransform());
	PlayerController->Possess(SpawnedCharacter);
	return SpawnedCharacter;
}

void ABSGameMode::InitializeGameMode(const TSharedPtr<FBSConfig> InConfig)
{
	Elapsed = 0.f;
	bLastTargetOnSet = false;
	TimePlayedGameMode = 0.f;
	bShouldTick = false;

	for (ABSPlayerController* Controller : Controllers)
	{
		check(Controller);
		Controller->FadeScreenFromBlack();
		Controller->ShowCountdown();
	}

	/* Load settings */
	const FPlayerSettings PlayerSettings = LoadPlayerSettings();
	OnPlayerSettingsChanged_Game(PlayerSettings.Game);
	OnPlayerSettingsChanged_User(PlayerSettings.User);
	OnPlayerSettingsChanged_AudioAnalyzer(PlayerSettings.AudioAnalyzer);
	OnPlayerSettingsChanged_VideoAndSound(PlayerSettings.VideoAndSound);

	/* Get config from Game Instance */
	check(InConfig);
	BSConfig = InConfig;

	/* Spawn TargetManager and VisualizerManager */
	TargetManager = GetWorld()->SpawnActor<ATargetManager>(TargetManagerClass, FVector::Zero(), FRotator::ZeroRotator,
		SpawnParameters);
	TargetManager->Init(BSConfig, PlayerSettings.Game);

	if (!VisualizerManager)
	{
		VisualizerManager = GetWorld()->SpawnActor<AVisualizerManager>(VisualizerManagerClass);
		VisualizerManager->InitializeVisualizers(PlayerSettings.Game, PlayerSettings.AudioAnalyzer);
	}

	for (const ABSPlayerController* Controller : Controllers)
	{
		const ABSCharacter* Character = Controller->GetBSCharacter();
		check(Character);

		UBSAbilitySystemComponent* ASC = Character->GetBSAbilitySystemComponent();
		check(ASC);

		const UBSAttributeSetBase* Set = ASC->GetSet<UBSAttributeSetBase>();
		check(Set);

		ASC->SetNumericAttributeBase(Set->GetHitDamageAttribute(), BSConfig->TargetConfig.BasePlayerHitDamage);
		ASC->SetNumericAttributeBase(Set->GetTrackingDamageAttribute(),
			BSConfig->TargetConfig.BasePlayerTrackingDamage);

		if (BSConfig->TargetConfig.TargetDamageType == ETargetDamageType::Tracking)
		{
			if (TrackGunAbilityGrantedHandles.IsEmpty())
			{
				TrackGunAbilitySet->GiveToAbilitySystem(ASC, &TrackGunAbilityGrantedHandles);
			}
			FGameplayAbilitySpec* TrackGunSpec = TrackGunAbilityGrantedHandles.FindFirstAbilitySpecFromHandle(ASC);
			if (UBSGA_TrackGun* TrackAbility = Cast<UBSGA_TrackGun>(TrackGunSpec->GetPrimaryInstance()))
			{
				TrackAbility->OnPlayerStopTrackingTarget.AddUniqueDynamic(TargetManager.Get(),
					&ATargetManager::OnPlayerStopTrackingTarget);
				ASC->MarkAbilitySpecDirty(*TrackGunSpec);
			}
		}
		else
		{
			if (!TrackGunAbilityGrantedHandles.IsEmpty())
			{
				TrackGunAbilityGrantedHandles.TakeFromAbilitySystem(Character->GetBSAbilitySystemComponent());
			}
		}
	}

	const bool bAudioManagerInitSuccess = InitializeAudioManagers();
	BindGameModeDelegates();
	bShouldTick = bAudioManagerInitSuccess;
}

void ABSGameMode::StartGameMode()
{
	LoadMatchingPlayerScores();

	for (auto& CurrentPlayerScore : CurrentPlayerScores)
	{
		CurrentPlayerScore.Key->ShowCrossHair();
		CurrentPlayerScore.Key->ShowPlayerHUD();
		CurrentPlayerScore.Key->HideCountdown();
		CurrentPlayerScore.Key->UpdatePlayerHUD(CurrentPlayerScore.Value, -1.f, -1.f);
	}

	StartGameModeTimers();
	TargetManager->SetShouldSpawn(true);
}

void ABSGameMode::StartGameModeTimers()
{
	if (BSConfig->AudioConfig.SongLength == 0.f)
	{
		GetWorldTimerManager().SetTimer(GameModeLengthTimer, 31536000, false);
	}
	else
	{
		GameModeLengthTimerDelegate.BindUObject(this, &ABSGameMode::EndGameMode, true, ETransitionState::None);
		GetWorldTimerManager().SetTimer(GameModeLengthTimer, GameModeLengthTimerDelegate,
			BSConfig->AudioConfig.SongLength, false);
	}
	GetWorldTimerManager().SetTimer(OnSecondPassedTimer, this, &ABSGameMode::OnSecondPassedCallback, 1.f, true);
}

void ABSGameMode::BindGameModeDelegates()
{
	if (!GetTargetManager()->OnTargetActivated.IsBoundToObject(this))
	{
		GetTargetManager()->OnTargetActivated.AddUObject(this, &ABSGameMode::UpdateTargetsSpawned);
	}
	if (!GetTargetManager()->PostTargetDamageEvent.IsBoundToObject(this))
	{
		GetTargetManager()->PostTargetDamageEvent.AddUObject(this, &ABSGameMode::OnPostTargetDamageEvent);
	}
}

void ABSGameMode::EndGameMode(const bool bSaveScores, const ETransitionState TransitionState)
{
	TimePlayedGameMode = GetWorldTimerManager().GetTimerElapsed(GameModeLengthTimer);
	GetWorldTimerManager().ClearTimer(GameModeLengthTimer);
	GetWorldTimerManager().ClearTimer(PlayerDelayTimer);
	GetWorldTimerManager().ClearTimer(OnSecondPassedTimer);
	GameModeLengthTimerDelegate.Unbind();

	TMap<ABSPlayerController*, FCommonScoreInfo> ScoreInfo;

	if (TargetManager)
	{
		TargetManager->SetShouldSpawn(false);

		const FAccuracyData AccuracyData = TargetManager->GetLocationAccuracy();

		for (auto& CurrentPlayerScore : CurrentPlayerScores)
		{
			CurrentPlayerScore.Value.LocationAccuracy = AccuracyData.AccuracyRows;
			FCommonScoreInfo ScoreInfoInst = CurrentPlayerScore.Key->FindCommonScoreInfo(BSConfig->DefiningConfig);
			ScoreInfoInst.UpdateAccuracy(AccuracyData);
			ScoreInfo.Add(CurrentPlayerScore.Key, ScoreInfoInst);
			if (BSConfig->AIConfig.ReinforcementLearningMode != EReinforcementLearningMode::None)
			{
				GetTargetManager()->SaveQTable(ScoreInfoInst);
			}
		}

		if (GetTargetManager()->OnTargetActivated.IsBoundToObject(this))
		{
			GetTargetManager()->OnTargetActivated.RemoveAll(this);
		}
		if (GetTargetManager()->PostTargetDamageEvent.IsBoundToObject(this))
		{
			GetTargetManager()->PostTargetDamageEvent.RemoveAll(this);
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
		if (Controller->IsPaused())
		{
			Controller->HandlePause();
		}

		// Always try to hide the countdown
		Controller->HideCountdown();

		switch (TransitionState)
		{
		case ETransitionState::StartFromPostGameMenu:
		case ETransitionState::Restart:
		case ETransitionState::PlayAgain:
			{
				// Fade screen to black and reinitialize
				Controller->OnScreenFadeToBlackFinish.BindLambda([this, Controller]
				{
					Controller->HidePostGameMenu();
					Controller->HidePlayerHUD();
					Controller->HideCrossHair();
					InitializeGameMode(BSConfig);
				});
				Controller->FadeScreenToBlack();
			}
			break;
		case ETransitionState::QuitToMainMenu:
			{
				Controller->FadeScreenToBlack();
			}
			break;
		case ETransitionState::None:
			{
				Controller->HidePlayerHUD();
				Controller->HideCrossHair();
				Controller->ShowPostGameMenu();
			}
		case ETransitionState::QuitToDesktop:
		case ETransitionState::StartFromMainMenu:
			break;
		}
	}

	HandleScoreSaving(bSaveScores, ScoreInfo);

	if (TransitionState == ETransitionState::QuitToMainMenu && !Controllers.IsEmpty())
	{
		GetWorldTimerManager().SetTimer(GoToMainMenuTimer, this, &ABSGameMode::GoToMainMenu,
			Controllers[0]->ScreenFadeWidgetAnimationDuration, false);
	}
}

void ABSGameMode::SpawnNewTarget(const bool bNewTargetState)
{
	if (bNewTargetState && !bLastTargetOnSet)
	{
		bLastTargetOnSet = true;
		if (Elapsed > BSConfig->TargetConfig.TargetSpawnCD)
		{
			Elapsed = 0.f;
			TargetManager->OnAudioAnalyzerBeat();
		}
	}
	else if (!bNewTargetState && bLastTargetOnSet)
	{
		bLastTargetOnSet = false;
	}
}

void ABSGameMode::StartAAManagerPlayback()
{
	switch (BSConfig->AudioConfig.AudioFormat)
	{
	case EAudioFormat::File:
		AATracker->Play();
		if (AAPlayer)
		{
			/* Start playing back audio from AAPlayer after the delay */
			GetWorldTimerManager().SetTimer(PlayerDelayTimer, this, &ABSGameMode::PlayAAPlayer,
				BSConfig->AudioConfig.PlayerDelay, false);
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
			AATracker->StartCapture(BSConfig->AudioConfig.bPlaybackAudio, false);
			SetAAManagerVolume(LoadPlayerSettings().VideoAndSound.GlobalVolume,
				LoadPlayerSettings().VideoAndSound.MusicVolume, AATracker);
			break;
		}
	case EAudioFormat::Loopback:
		{
			AATracker->StartLoopback(false);
			SetAAManagerVolume(LoadPlayerSettings().VideoAndSound.GlobalVolume,
				LoadPlayerSettings().VideoAndSound.MusicVolume, AATracker);
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
	switch (BSConfig->AudioConfig.AudioFormat)
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
			AATracker->StartCapture(BSConfig->AudioConfig.bPlaybackAudio, false);
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

void ABSGameMode::RegisterGun(ABSGun* InGun)
{
	if (InGun)
	{
		InGun->OnShotFired.BindUObject(this, &ABSGameMode::UpdateShotsFired);
	}
}

bool ABSGameMode::InitializeAudioManagers()
{
	AATracker = NewObject<UAudioAnalyzerManager>(this);
	switch (BSConfig->AudioConfig.AudioFormat)
	{
	case EAudioFormat::None:
		break;
	case EAudioFormat::File:
		if (!AATracker->InitPlayerAudio(BSConfig->AudioConfig.SongPath))
		{
			OnAAManagerError();
			return false;
		}
		break;
	case EAudioFormat::Capture:
		AATracker->SetDefaultDevicesCapturerAudio(*BSConfig->AudioConfig.InAudioDevice,
			*BSConfig->AudioConfig.OutAudioDevice);
		if (!AATracker->InitCapturerAudioEx(48000, EAA_AudioDepth::B_16, EAA_AudioFormat::Signed_Int, 1.f,
			BSConfig->AudioConfig.bPlaybackAudio))
		{
			OnAAManagerError();
			return false;
		}
		break;
	case EAudioFormat::Loopback:
		AATracker->SetDefaultDevicesCapturerAudio(*BSConfig->AudioConfig.InAudioDevice,
			*BSConfig->AudioConfig.OutAudioDevice);
		AATracker->SetDefaultDeviceLoopbackAudio(*BSConfig->AudioConfig.OutAudioDevice);
		if (!AATracker->InitLoopbackAudio())
		{
			OnAAManagerError();
			return false;
		}
		break;
	default:
		break;
	}

	AATracker->InitBeatTrackingConfigWLimits(EAA_ChannelSelectionMode::All_in_one, 0, AASettings.BandLimits,
		AASettings.TimeWindow, 10 / AASettings.TimeWindow, false, 100, 2.1);
	AATracker->InitSpectrumConfigWLimits(EAA_ChannelSelectionMode::All_in_one, -1, AASettings.BandLimits,
		AASettings.TimeWindow, 10 / AASettings.TimeWindow, true, AASettings.NumBandChannels);

	//AATracker->InitPitchTrackingConfig(EAA_ChannelSelectionMode::All_in_one, -1, 0.02, 0.19);
	SetAAManagerVolume(0, 0, AATracker);

	/* AAPlayer will only be used if AudioFormat is File and PlayerDelay > 0.01f */
	if (BSConfig->AudioConfig.PlayerDelay < 0.01f)
	{
		AAPlayer = nullptr;
		return true;
	}
	AAPlayer = NewObject<UAudioAnalyzerManager>(this);
	if (!AAPlayer->InitPlayerAudio(BSConfig->AudioConfig.SongPath))
	{
		OnAAManagerError();
		return false;
	}
	AAPlayer->InitBeatTrackingConfigWLimits(EAA_ChannelSelectionMode::All_in_one, 0, AASettings.BandLimits,
		AASettings.TimeWindow, AASettings.HistorySize, false, 100, 2.1);
	AAPlayer->InitSpectrumConfigWLimits(EAA_ChannelSelectionMode::All_in_one, -1, AASettings.BandLimits,
		AASettings.TimeWindow, 10 / AASettings.TimeWindow, true, AASettings.NumBandChannels);
	SetAAManagerVolume(0, 0, AAPlayer);
	return true;
}

void ABSGameMode::OnTick_AudioAnalyzers(const float DeltaSeconds)
{
	Elapsed += DeltaSeconds;

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
	SetAAManagerVolume(LoadPlayerSettings().VideoAndSound.GlobalVolume, LoadPlayerSettings().VideoAndSound.MusicVolume,
		AAPlayer);
	UE_LOG(LogTemp, Display, TEXT("Now Playing AAPlayer %f"), AAPlayer->GetPlaybackVolume());
}

void ABSGameMode::SetAAManagerVolume(const float GlobalVolume, const float MusicVolume,
	UAudioAnalyzerManager* AAManager) const
{
	if (AAManager)
	{
		switch (BSConfig->AudioConfig.AudioFormat)
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
		switch (BSConfig->AudioConfig.AudioFormat)
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

void ABSGameMode::GoToMainMenu()
{
	for (ABSPlayerController* Controller : Controllers)
	{
		Controller->HidePostGameMenu();
		Controller->HidePlayerHUD();
		Controller->HideCrossHair();
	}
	UGameplayStatics::OpenLevel(GetWorld(), "MainMenuLevel");
}

void ABSGameMode::LoadMatchingPlayerScores()
{
	if (BSConfig->AudioConfig.SongLength == 0.f)
	{
		MaxScorePerTarget = 1000.f;
	}
	else
	{
		MaxScorePerTarget = 100000.f / ((BSConfig->AudioConfig.SongLength - 1.f) / BSConfig->TargetConfig.
			TargetSpawnCD);
	}

	for (auto& CurrentPlayerScore : CurrentPlayerScores)
	{
		CurrentPlayerScore.Value = FPlayerScore();
		CurrentPlayerScore.Value.DefiningConfig = BSConfig->DefiningConfig;
		CurrentPlayerScore.Value.SongTitle = BSConfig->AudioConfig.SongTitle;
		CurrentPlayerScore.Value.SongLength = BSConfig->AudioConfig.SongLength;
		CurrentPlayerScore.Value.TotalPossibleDamage = 0.f;

		const TArray<FPlayerScore> PlayerScores = CurrentPlayerScore.Key->LoadPlayerScores().FilterByPredicate(
			[&](const FPlayerScore& PlayerScore)
			{
				if (PlayerScore == CurrentPlayerScore.Value)
				{
					return true;
				}
				return false;
			});

		for (const FPlayerScore& ScoreObject : PlayerScores)
		{
			if (ScoreObject.Score > CurrentPlayerScore.Value.HighScore)
			{
				CurrentPlayerScore.Value.HighScore = ScoreObject.Score;
			}
		}
	}
}

void ABSGameMode::HandleScoreSaving(const bool bExternalSaveScores,
	const TMap<ABSPlayerController*, FCommonScoreInfo>& InCommonScoreInfo)
{
	if (!bExternalSaveScores)
	{
		for (auto& CurrentPlayerScore : CurrentPlayerScores)
		{
			CurrentPlayerScore.Value = FPlayerScore();
		}
		return;
	}

	UBSGameInstance* GI = Cast<UBSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	for (auto& CurrentPlayerScore : CurrentPlayerScores)
	{
		const bool bValidToSave = CurrentPlayerScore.Value.IsValidToSave();
		if (bValidToSave)
		{
			// Update Steam Stat for Game Mode
			#if UE_BUILD_SHIPPING
			if (TimePlayedGameMode > MinStatRequirement_Duration_NumGamesPlayed)
			{
				if (CurrentPlayerScore.Value.DefiningConfig.GameModeType == EGameModeType::Custom)
				{
					GI->GetSteamManager()->UpdateStat_NumGamesPlayed(EBaseGameMode::None, 1);
				}
				else
				{
					GI->GetSteamManager()->UpdateStat_NumGamesPlayed(CurrentPlayerScore.Value.DefiningConfig.BaseGameMode, 1);
				}
			}
			#else
			if (CurrentPlayerScore.Value.DefiningConfig.GameModeType == EGameModeType::Custom)
			{
				GI->GetSteamManager()->UpdateStat_NumGamesPlayed(EBaseGameMode::None, 1);
			}
			else
			{
				GI->GetSteamManager()->UpdateStat_NumGamesPlayed(CurrentPlayerScore.Value.DefiningConfig.BaseGameMode,
					1);
			}
			#endif
			// Save common score info and completed scores locally
			const FCommonScoreInfo* CommonScoreInfo = InCommonScoreInfo.Find(CurrentPlayerScore.Key);
			if (CommonScoreInfo)
			{
				CurrentPlayerScore.Key->SaveCommonScoreInfo(BSConfig->DefiningConfig, *CommonScoreInfo);
			}
			GetCompletedPlayerScores(CurrentPlayerScore.Value);
			CurrentPlayerScore.Key->SavePlayerScoreInstance(CurrentPlayerScore.Value);
		}

		CurrentPlayerScore.Value = FPlayerScore();

		// Let game instance handle posting scores to db
		GI->SavePlayerScoresToDatabase(CurrentPlayerScore.Key, bValidToSave);
	}
}

void ABSGameMode::GetCompletedPlayerScores(FPlayerScore& InScore)
{
	/** save current time */
	InScore.Time = FDateTime::UtcNow().ToIso8601();

	/** for BeatTrack modes */
	if (BSConfig->TargetConfig.TargetDamageType == ETargetDamageType::Tracking)
	{
		InScore.Accuracy = FloatDivide(InScore.Score, InScore.TotalPossibleDamage);
		InScore.Completion = FloatDivide(InScore.Score, InScore.TotalPossibleDamage);
		return;
	}
	InScore.AvgTimeOffset = FloatDivide(InScore.TotalTimeOffset, InScore.TargetsHit);
	InScore.Accuracy = FloatDivide(InScore.TargetsHit, InScore.ShotsFired);
	InScore.Completion = FloatDivide(InScore.TargetsHit, InScore.TargetsSpawned);
}

void ABSGameMode::OnPlayerSettingsChanged_Game(const FPlayerSettings_Game& GameSettings)
{
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

void ABSGameMode::OnPlayerSettingsChanged_VideoAndSound(const FPlayerSettings_VideoAndSound& VideoAndSoundSettings)
{
	SetAAManagerVolume(VideoAndSoundSettings.GlobalVolume, VideoAndSoundSettings.MusicVolume);
}

void ABSGameMode::OnPostTargetDamageEvent(const FTargetDamageEvent& Event)
{
	float NormalizedError = -1.f;
	float Error = -1.f;

	if (Event.DamageCauser)
	{
		if (ABSCharacter* Character = Cast<ABSCharacter>(Event.DamageCauser))
		{
			if (ABSPlayerController* Controller = Character->GetBSPlayerController())
			{
				FPlayerScore* Score = CurrentPlayerScores.Find(Controller);
				if (Score)
				{
					switch (Event.DamageType)
					{
					case ETargetDamageType::Tracking:
						{
							Score->TotalPossibleDamage = Event.TotalPossibleTrackingDamage;
							Score->Score += Event.DamageDelta;
						}
						break;
					case ETargetDamageType::Hit:
						{
							Score->Score += GetScoreFromTimeAlive(Event.TimeAlive);
							Score->TotalTimeOffset += GetAbsHitTimingError(Event.TimeAlive);
							Score->TargetsHit++;
							UpdateStreak(Controller, *Score, Event.Streak, Event.Transform);
							// UpdateTimeOffset(TimeOffset, Transform);
							NormalizedError = GetNormalizedHitTimingError(Event.TimeAlive);
							Error = GetHitTimingError(Event.TimeAlive);
						}
						break;
					case ETargetDamageType::Combined:
					case ETargetDamageType::None:
						{
							UE_LOG(LogTemp, Warning,
								TEXT("TargetDamageType of Combined/None received in OnPostTargetDamageEvent."));
						}
						return;
					case ETargetDamageType::Self:
						return;
					}

					// Update high score
					if (Score->Score > Score->HighScore)
					{
						Score->HighScore = Score->Score;
					}

					// Update PlayerHUD
					Controller->UpdatePlayerHUD(*Score, NormalizedError, Error);
				}
			}
		}
	}
}

void ABSGameMode::UpdateTargetsSpawned(const ETargetDamageType& DamageType)
{
	if (DamageType == ETargetDamageType::Hit)
	{
		for (auto& CurrentPlayerScore : CurrentPlayerScores)
		{
			CurrentPlayerScore.Value.TargetsSpawned++;
			CurrentPlayerScore.Key->UpdatePlayerHUD(CurrentPlayerScore.Value, -1.f, -1.f);
		}
	}
}

void ABSGameMode::UpdateShotsFired(ABSPlayerController* Controller)
{
	if (Controller)
	{
		FPlayerScore* Score = CurrentPlayerScores.Find(Controller);
		if (Score)
		{
			Score->ShotsFired++;
			Controller->UpdatePlayerHUD(*Score, -1.f, -1.f);
		}
	}
}

void ABSGameMode::UpdateStreak(ABSPlayerController* Controller, FPlayerScore& InScore, const int32 Streak,
	const FTransform& Transform)
{
	if (Streak > InScore.Streak)
	{
		InScore.Streak = Streak;
	}
	Controller->ShowCombatText(Streak, Transform);

	if (Streak > StreakThreshold && !Controller->LoadPlayerSettings().User.bNightModeUnlocked)
	{
		if (OnStreakThresholdPassed.IsBound())
		{
			OnStreakThresholdPassed.Execute();
		}
		FPlayerSettings_User Settings = Controller->LoadPlayerSettings().User;
		Settings.bNightModeUnlocked = true;
		Controller->SavePlayerSettings(Settings);
	}
}

void ABSGameMode::UpdateTimeOffset(const float TimeOffset, const FTransform& Transform)
{
	for (ABSPlayerController* Controller : Controllers)
	{
		Controller->ShowAccuracyText(TimeOffset, Transform);
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

float ABSGameMode::GetScoreFromTimeAlive(const float InTimeAlive) const
{
	// Perfect shot
	if (GetAbsHitTimingError(InTimeAlive) < PerfectScoreTimeThreshold / 2.f)
	{
		return MaxScorePerTarget;
	}

	const float MinScorePerTarget = MaxScorePerTarget / 2.f;
	// Early shot
	if (InTimeAlive < BSConfig->TargetConfig.SpawnBeatDelay)
	{
		const float MinEarlyShot = 0.f;
		const float MaxEarlyShot = BSConfig->TargetConfig.SpawnBeatDelay - PerfectScoreTimeThreshold / 2.f;
		const FVector2d InputRange = FVector2d(MinEarlyShot, MaxEarlyShot);
		const float LerpValue = FMath::GetMappedRangeValueClamped(InputRange, FVector2D(0.f, 1.f), InTimeAlive);

		// interp between half perfect score at MinEarlyShot to perfect score at MaxEarlyShot
		return MinScorePerTarget + LerpValue * (MaxScorePerTarget - MinScorePerTarget);
		//return FMath::Lerp<float>(MaxScorePerTarget / 2.f, MaxScorePerTarget, LerpValue);
	}

	// Late shot
	const float MinLateShot = BSConfig->TargetConfig.SpawnBeatDelay + PerfectScoreTimeThreshold / 2.f;
	const float MaxLateShot = BSConfig->TargetConfig.TargetMaxLifeSpan;
	const FVector2d InputRange = FVector2d(MinLateShot, MaxLateShot);
	const float LerpValue = FMath::GetMappedRangeValueClamped(InputRange, FVector2D(0.f, 1.f), InTimeAlive);

	// interp between perfect score at MinLateShot to half perfect score at MaxLateShot
	return MaxScorePerTarget + LerpValue * (MinScorePerTarget - MaxScorePerTarget);
	//return FMath::Lerp<float>(MaxScorePerTarget, MaxScorePerTarget / 2.f, LerpValue);
}

float ABSGameMode::GetHitTimingError(const float InTimeAlive) const
{
	return InTimeAlive - BSConfig->TargetConfig.SpawnBeatDelay;
}

float ABSGameMode::GetAbsHitTimingError(const float InTimeAlive) const
{
	return FMath::Abs<float>(InTimeAlive - BSConfig->TargetConfig.SpawnBeatDelay);
}

float ABSGameMode::GetNormalizedHitTimingError(const float InTimeAlive) const
{
	if (InTimeAlive == BSConfig->TargetConfig.SpawnBeatDelay)
	{
		return 0.5f;
	}

	// Early shot
	if (InTimeAlive < BSConfig->TargetConfig.SpawnBeatDelay)
	{
		const float MinEarlyShot = 0.f;
		const float MaxEarlyShot = BSConfig->TargetConfig.SpawnBeatDelay;
		const FVector2d InputRange = FVector2d(MinEarlyShot, MaxEarlyShot);
		return FMath::GetMappedRangeValueClamped(InputRange, FVector2D(0.f, 0.5f), InTimeAlive);
	}

	// Late shot
	const float MinLateShot = BSConfig->TargetConfig.SpawnBeatDelay;
	const float MaxLateShot = BSConfig->TargetConfig.TargetMaxLifeSpan;
	const FVector2d InputRange = FVector2d(MinLateShot, MaxLateShot);
	return FMath::GetMappedRangeValueClamped(InputRange, FVector2D(0.5f, 1.f), InTimeAlive);
}
