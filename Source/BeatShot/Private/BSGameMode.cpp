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
#include "AbilitySystem/Abilities/BSGA_TrackGun.h"
#include "AbilitySystem/Globals/BSAttributeSetBase.h"
#include "Kismet/GameplayStatics.h"
#include "System/SteamManager.h"

DEFINE_LOG_CATEGORY(LogAudioData);

ABSGameMode::ABSGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
	bShouldTick = false;
	Elapsed = 0.f;
	LastTargetOnSet = false;
	TimePlayedGameMode = 0.f;
	AATracker = nullptr;
	AAPlayer = nullptr;
	TrackGunAbilitySet = nullptr;
	MaxScorePerTarget = 0;
	bShowStreakCombatText = false;
	CombatTextFrequency = 0;
	bNightModeUnlocked = false;
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
	TimePlayedGameMode = 0.f;
	bShouldTick = false;

	/* Load settings */
	const FPlayerSettings PlayerSettings = LoadPlayerSettings();
	OnPlayerSettingsChanged_Game(PlayerSettings.Game);
	OnPlayerSettingsChanged_User(PlayerSettings.User);
	OnPlayerSettingsChanged_AudioAnalyzer(PlayerSettings.AudioAnalyzer);
	OnPlayerSettingsChanged_VideoAndSound(PlayerSettings.VideoAndSound);

	/* Get config from Game Instance */
	BSConfig = Cast<UBSGameInstance>(UGameplayStatics::GetGameInstance(this))->GetBSConfig();

	/* Spawn TargetManager and VisualizerManager */
	TargetManager = GetWorld()->SpawnActor<ATargetManager>(TargetManagerClass, FVector::Zero(), FRotator::ZeroRotator, SpawnParameters);
	TargetManager->Init(BSConfig, PlayerSettings.Game);

	if (!VisualizerManager)
	{
		VisualizerManager = GetWorld()->SpawnActor<AVisualizerManager>(VisualizerManagerClass);
		VisualizerManager->InitializeVisualizers(PlayerSettings.Game, PlayerSettings.AudioAnalyzer);
	}
	
	for (const ABSPlayerController* Controller : Controllers)
	{
		if (ABSCharacter* Character = Controller->GetBSCharacter())
		{
			UBSAbilitySystemComponent* ASC = Character->GetBSAbilitySystemComponent();
			const UBSAttributeSetBase* Set = ASC->GetSet<UBSAttributeSetBase>();
			if (ensure(Set))
			{
				ASC->SetNumericAttributeBase(Set->GetHitDamageAttribute(), BSConfig.TargetConfig.BasePlayerHitDamage);
				ASC->SetNumericAttributeBase(Set->GetTrackingDamageAttribute(), BSConfig.TargetConfig.BasePlayerTrackingDamage);
			}
			
			if (Character->HasMatchingGameplayTag(FBSGameplayTags::Get().Cheat_AimBot))
			{
				if (!GetTargetManager()->OnTargetActivated_AimBot.IsBoundToObject(Character))
				{
					GetTargetManager()->OnTargetActivated_AimBot.AddUObject(Character, &ABSCharacter::OnTargetSpawned_AimBot);
				}
			}

			if (BSConfig.TargetConfig.TargetDamageType == ETargetDamageType::Tracking)
			{
				if (TrackGunAbilityGrantedHandles.IsEmpty())
				{
					TrackGunAbilitySet->GiveToAbilitySystem(ASC, &TrackGunAbilityGrantedHandles);
				}
				for (FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
				{
					if (UGameplayAbility* Ability = Spec.GetPrimaryInstance())
					{
						if (UBSGA_TrackGun* TrackAbility = Cast<UBSGA_TrackGun>(Ability))
						{
							TrackAbility->OnPlayerStopTrackingTarget.AddUniqueDynamic(TargetManager.Get(), &ATargetManager::OnPlayerStopTrackingTarget);
							ASC->MarkAbilitySpecDirty(Spec);
							break;
						}
					}
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
	}

	const bool bAudioManagerInitSuccess = InitializeAudioManagers();
	BindGameModeDelegates();
	bShouldTick = bAudioManagerInitSuccess;
}

void ABSGameMode::StartGameMode()
{
	if (OnGameModeStarted.IsBound())
	{
		OnGameModeStarted.Broadcast();
	}
	LoadMatchingPlayerScores();
	UpdateScoresToHUD.Broadcast(CurrentPlayerScore, -1.f, -1.f);
	StartGameModeTimers();
	TargetManager->SetShouldSpawn(true);
	
	/*for (float i = 0.f; i < BSConfig.TargetConfig.TargetMaxLifeSpan; i+=0.01f)
	{
		UE_LOG(LogTemp, Display, TEXT("Score: %f Err: %f: AbsError: %f NormError: %f"), GetScoreFromTimeAlive(i), GetHitTimingError(i), GetAbsHitTimingError(i), GetNormalizedHitTimingError(i));
	}*/
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
	if (!GetTargetManager()->OnTargetActivated.IsBoundToObject(this))
	{
		GetTargetManager()->OnTargetActivated.AddUObject(this, &ABSGameMode::UpdateTargetsSpawned);
	}
	if (!GetTargetManager()->OnTargetDeactivated.IsBoundToObject(this))
	{
		GetTargetManager()->OnTargetDeactivated.AddUObject(this, &ABSGameMode::UpdatePlayerScores);
	}
	if (!GetTargetManager()->OnBeatTrackTargetDamaged.IsBoundToObject(this))
	{
		GetTargetManager()->OnBeatTrackTargetDamaged.AddUObject(this, &ABSGameMode::UpdateTrackingScore);
	}
}

void ABSGameMode::EndGameMode(const bool bSaveScores, const bool ShowPostGameMenu)
{
	TimePlayedGameMode = GetWorldTimerManager().GetTimerElapsed(GameModeLengthTimer);
	GetWorldTimerManager().ClearTimer(GameModeLengthTimer);
	GetWorldTimerManager().ClearTimer(PlayerDelayTimer);
	GetWorldTimerManager().ClearTimer(OnSecondPassedTimer);
	
	FCommonScoreInfo ScoreInfo;
	
	if (TargetManager)
	{
		TargetManager->SetShouldSpawn(false);

		const FAccuracyData AccuracyData = TargetManager->GetLocationAccuracy();
		CurrentPlayerScore.LocationAccuracy = AccuracyData.AccuracyRows;

		ScoreInfo = FindCommonScoreInfo(BSConfig.DefiningConfig);
		ScoreInfo.UpdateAccuracy(AccuracyData);
		if (BSConfig.AIConfig.bEnableReinforcementLearning)
		{
			GetTargetManager()->SaveQTable(ScoreInfo);
		}
		
		if (GetTargetManager()->OnTargetActivated.IsBoundToObject(this))
		{
			GetTargetManager()->OnTargetActivated.RemoveAll(this);
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
				if (const TArray<FGameplayAbilitySpec*> AbilitySpecs = Character->GetBSAbilitySystemComponent()->GetAbilitySpecsFromGameplayTag(FBSGameplayTags::Get().Ability_Fire); !AbilitySpecs.IsEmpty())
				{
					Character->GetBSAbilitySystemComponent()->CancelAbilityHandle(AbilitySpecs[0]->Handle);
					Character->GetBSAbilitySystemComponent()->ClearAbilityInput();
				}
			}
		}
	}
	
	HandleScoreSaving(bSaveScores, ScoreInfo);
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

bool ABSGameMode::InitializeAudioManagers()
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
			return false;
		}
		break;
	case EAudioFormat::Capture:
		 AATracker->SetDefaultDevicesCapturerAudio(*BSConfig.AudioConfig.InAudioDevice, *BSConfig.AudioConfig.OutAudioDevice);
		 if (!AATracker->InitCapturerAudioEx(48000, EAA_AudioDepth::B_16, EAA_AudioFormat::Signed_Int, 1.f, BSConfig.AudioConfig.bPlaybackAudio))
		 {
		 	OnAAManagerError();
		 	return false;
		 }
		 break;
	case EAudioFormat::Loopback:
		AATracker->SetDefaultDevicesCapturerAudio(*BSConfig.AudioConfig.InAudioDevice, *BSConfig.AudioConfig.OutAudioDevice);
		AATracker->SetDefaultDeviceLoopbackAudio(*BSConfig.AudioConfig.OutAudioDevice);
		if (!AATracker->InitLoopbackAudio())
		{
			OnAAManagerError();
			return false;
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
		return true;
	}
	AAPlayer = NewObject<UAudioAnalyzerManager>(this);
	if (!AAPlayer->InitPlayerAudio(BSConfig.AudioConfig.SongPath))
	{
		OnAAManagerError();
		return false;
	}
	AAPlayer->InitBeatTrackingConfigWLimits(EAA_ChannelSelectionMode::All_in_one, 0, AASettings.BandLimits, AASettings.TimeWindow, AASettings.HistorySize, false, 100, 2.1);
	AAPlayer->InitSpectrumConfigWLimits(EAA_ChannelSelectionMode::All_in_one, -1, AASettings.BandLimits, AASettings.TimeWindow, 10 / AASettings.TimeWindow, true, AASettings.NumBandChannels);
	SetAAManagerVolume(0, 0, AAPlayer);
	return true;
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

void ABSGameMode::HandleScoreSaving(const bool bExternalSaveScores, const FCommonScoreInfo& InCommonScoreInfo)
{
	if (!bExternalSaveScores)
	{
		CurrentPlayerScore.ResetStruct();
		return;
	}
	if (CurrentPlayerScore.Score <= 0)
	{
		CurrentPlayerScore.ResetStruct();
		OnPostScoresResponse.Broadcast(EPostScoresResponse::ZeroScore);
		return;
	}
	if (CurrentPlayerScore.DefiningConfig.GameModeType == EGameModeType::Custom && CurrentPlayerScore.DefiningConfig.CustomGameModeName == "")
	{
		CurrentPlayerScore.ResetStruct();
		OnPostScoresResponse.Broadcast(EPostScoresResponse::UnsavedGameMode);
		return;
	}
	
	UBSGameInstance* GI = Cast<UBSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	// Update Steam Stat for Game Mode
	if (TimePlayedGameMode > MinStatRequirement_Duration_NumGamesPlayed)
	{
		if (CurrentPlayerScore.DefiningConfig.GameModeType == EGameModeType::Custom)
		{
			GI->GetSteamManager()->UpdateStat_NumGamesPlayed(EBaseGameMode::None, 1);
		}
		else
		{
			GI->GetSteamManager()->UpdateStat_NumGamesPlayed(CurrentPlayerScore.DefiningConfig.BaseGameMode, 1);
		}
	}
			
	SaveCommonScoreInfo(BSConfig.DefiningConfig, InCommonScoreInfo);
	SavePlayerScoreInstance(GetCompletedPlayerScores());
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
	if (LoadPlayerSettings().User.RefreshCookie.IsEmpty())
	{
		OnPostScoresResponse.Broadcast(EPostScoresResponse::NoAccount);
		return;
	}
	RequestAccessToken(LoadPlayerSettings().User.RefreshCookie, OnAccessTokenResponse);
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
		OnPostScoresResponse.Broadcast(EPostScoresResponse::HttpError);
		return;
	}
	const TArray<FPlayerScore> UnsavedScores = LoadPlayerScores_UnsavedToDatabase();
	UE_LOG(LogTemp, Display, TEXT("UnsavedScores: %d"), UnsavedScores.Num());
	PostPlayerScores(UnsavedScores, LoadPlayerSettings().User.UserID, AccessToken, OnPostScoresResponse);
}

void ABSGameMode::OnPostScoresResponseReceived(const EPostScoresResponse& LoginState)
{
	if (LoginState == EPostScoresResponse::HttpSuccess)
	{
		SetAllPlayerScoresSavedToDatabase();
	}
	CurrentPlayerScore.ResetStruct();
}

void ABSGameMode::UpdatePlayerScores(const float TimeAlive, const int32 NewStreak, const FTransform& Transform)
{
	if (BSConfig.TargetConfig.TargetDamageType == ETargetDamageType::Tracking || TimeAlive < 0.f)
	{
		return;
	}

	CurrentPlayerScore.Score += GetScoreFromTimeAlive(TimeAlive);
	CurrentPlayerScore.TotalTimeOffset += GetAbsHitTimingError(TimeAlive);
	
	UpdateTargetsHit();
	UpdateStreak(NewStreak, Transform);
	UpdateHighScore();
	//UpdateTimeOffset(TimeOffset, Transform);
	
	UpdateScoresToHUD.Broadcast(CurrentPlayerScore, GetNormalizedHitTimingError(TimeAlive), GetHitTimingError(TimeAlive));
}

void ABSGameMode::UpdateTrackingScore(const float DamageDelta, const float TotalPossibleDamage)
{
	CurrentPlayerScore.TotalPossibleDamage = TotalPossibleDamage;
	CurrentPlayerScore.Score += DamageDelta;
	UpdateHighScore();
	UpdateScoresToHUD.Broadcast(CurrentPlayerScore, -1.f, -1.f);
}

void ABSGameMode::UpdateTargetsSpawned()
{
	CurrentPlayerScore.TargetsSpawned++;
	UpdateScoresToHUD.Broadcast(CurrentPlayerScore, -1.f, -1.f);
}

void ABSGameMode::UpdateShotsFired()
{
	CurrentPlayerScore.ShotsFired++;
	UpdateScoresToHUD.Broadcast(CurrentPlayerScore, -1.f, -1.f);
}

void ABSGameMode::UpdateStreak(const int32 Streak, const FTransform& Transform)
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
				Controller->ShowCombatText(Streak, Transform);
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

void ABSGameMode::UpdateTimeOffset(const float TimeOffset, const FTransform& Transform)
{
	for (ABSPlayerController* Controller : Controllers)
	{
		Controller->ShowAccuracyText(TimeOffset, Transform);
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

float ABSGameMode::GetScoreFromTimeAlive(const float InTimeAlive) const
{
	// Perfect shot
	if (GetAbsHitTimingError(InTimeAlive) < PerfectScoreTimeThreshold / 2.f)
	{
		return MaxScorePerTarget;
	}

	const float MinScorePerTarget = MaxScorePerTarget / 2.f;
	// Early shot
	if (InTimeAlive < BSConfig.TargetConfig.SpawnBeatDelay)
	{
		const float MinEarlyShot = 0.f;
		const float MaxEarlyShot = BSConfig.TargetConfig.SpawnBeatDelay - PerfectScoreTimeThreshold / 2.f;
		const FVector2d InputRange = FVector2d(MinEarlyShot, MaxEarlyShot);
		const float LerpValue = FMath::GetMappedRangeValueClamped(InputRange, FVector2D(0.f, 1.f), InTimeAlive);
		
		// interp between half perfect score at MinEarlyShot to perfect score at MaxEarlyShot
		return MinScorePerTarget + LerpValue * (MaxScorePerTarget - MinScorePerTarget);
		//return FMath::Lerp<float>(MaxScorePerTarget / 2.f, MaxScorePerTarget, LerpValue);
	}
	
	// Late shot
	const float MinLateShot = BSConfig.TargetConfig.SpawnBeatDelay + PerfectScoreTimeThreshold / 2.f;
	const float MaxLateShot = BSConfig.TargetConfig.TargetMaxLifeSpan;
	const FVector2d InputRange = FVector2d(MinLateShot, MaxLateShot);
	const float LerpValue = FMath::GetMappedRangeValueClamped(InputRange, FVector2D(0.f, 1.f), InTimeAlive);
	
	// interp between perfect score at MinLateShot to half perfect score at MaxLateShot
	return MaxScorePerTarget + LerpValue * (MinScorePerTarget - MaxScorePerTarget);
	//return FMath::Lerp<float>(MaxScorePerTarget, MaxScorePerTarget / 2.f, LerpValue);
}

float ABSGameMode::GetHitTimingError(const float InTimeAlive) const
{
	return InTimeAlive - BSConfig.TargetConfig.SpawnBeatDelay;
}

float ABSGameMode::GetAbsHitTimingError(const float InTimeAlive) const
{
	return FMath::Abs<float>(InTimeAlive - BSConfig.TargetConfig.SpawnBeatDelay);
}

float ABSGameMode::GetNormalizedHitTimingError(const float InTimeAlive) const
{

	if (InTimeAlive == BSConfig.TargetConfig.SpawnBeatDelay)
	{
		return 0.5f;
	}
	
	// Early shot
	if (InTimeAlive < BSConfig.TargetConfig.SpawnBeatDelay)
	{
		const float MinEarlyShot = 0.f;
		const float MaxEarlyShot = BSConfig.TargetConfig.SpawnBeatDelay;
		const FVector2d InputRange = FVector2d(MinEarlyShot, MaxEarlyShot);
		return FMath::GetMappedRangeValueClamped(InputRange, FVector2D(0.f, 0.5f), InTimeAlive);
	}
	
	// Late shot
	const float MinLateShot = BSConfig.TargetConfig.SpawnBeatDelay;
	const float MaxLateShot = BSConfig.TargetConfig.TargetMaxLifeSpan;
	const FVector2d InputRange = FVector2d(MinLateShot, MaxLateShot);
	return FMath::GetMappedRangeValueClamped(InputRange, FVector2D(0.5f, 1.f), InTimeAlive);
}
