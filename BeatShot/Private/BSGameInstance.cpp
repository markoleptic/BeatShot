// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "BSGameInstance.h"
#include "BSGameMode.h"
#include "Player/BSPlayerController.h"
#include "System/SteamManager.h"
#include "Kismet/GameplayStatics.h"
#include "DLSSFunctions.h"
#include "GameFramework/GameUserSettings.h"

void UBSGameInstance::Init()
{
	Super::Init();
	InitializeSteamManager();
}

void UBSGameInstance::OnStart()
{
	Super::OnStart();
	InitVideoSettings();
	UGameplayStatics::SetBaseSoundMix(GetWorld(), GlobalSoundMix);
	UGameplayStatics::SetSoundMixClassOverride(GetWorld(), GlobalSoundMix, GlobalSound,
		LoadPlayerSettings().VideoAndSound.GlobalVolume / 100, 1, 0.0f, true);
	UGameplayStatics::SetSoundMixClassOverride(GetWorld(), GlobalSoundMix, MenuSound,
		LoadPlayerSettings().VideoAndSound.MenuVolume / 100, 1, 0.0f, true);
}

void UBSGameInstance::InitVideoSettings()
{
	UGameUserSettings* GameUserSettings = UGameUserSettings::GetGameUserSettings();

	// Run hardware benchmark if first time launching game
	if (!LoadPlayerSettings().User.bHasRanBenchmark)
	{
		GameUserSettings->RunHardwareBenchmark();
		GameUserSettings->ApplyHardwareBenchmarkResults();
		FPlayerSettings_User Settings = LoadPlayerSettings().User;
		Settings.bHasRanBenchmark = true;
		SavePlayerSettings(Settings);
	}

	FPlayerSettings_VideoAndSound VideoSettings = LoadPlayerSettings().VideoAndSound;
	InitDLSSSettings(VideoSettings);
	SavePlayerSettings(VideoSettings);
}

void UBSGameInstance::StartGameMode(const bool bIsRestart) const
{
	ABSPlayerController* PlayerController = Cast<ABSPlayerController>(
		UGameplayStatics::GetPlayerController(GetWorld(), 0));

	if (PlayerController->IsPaused())
	{
		PlayerController->HandlePause();
	}

	/** Hide all widgets and show the countdown after the screen fades to black */
	PlayerController->OnScreenFadeToBlackFinish.BindLambda([this, bIsRestart]
	{
		ABSPlayerController* Controller = Cast<ABSPlayerController>(
			UGameplayStatics::GetPlayerController(GetWorld(), 0));
		Controller->HideMainMenu();
		Controller->HidePostGameMenu();
		Controller->HidePauseMenu();
		if (GetWorld()->GetMapName().Contains("Range"))
		{
			Controller->ShowCountdown(bIsRestart);
			Cast<ABSGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->InitializeGameMode();
		}
		else
		{
			UGameplayStatics::OpenLevel(GetWorld(), FName("Range"));
		}
	});
	PlayerController->FadeScreenToBlack();
}

void UBSGameInstance::HandleGameModeTransition(const FGameModeTransitionState& NewGameModeTransitionState)
{
	switch (NewGameModeTransitionState.TransitionState)
	{
	case ETransitionState::StartFromMainMenu:
		{
			BSConfig = NewGameModeTransitionState.BSConfig;
			StartGameMode(false);
			break;
		}
	case ETransitionState::StartFromPostGameMenu:
		{
			BSConfig = NewGameModeTransitionState.BSConfig;
			StartGameMode(true);
			break;
		}
	case ETransitionState::Restart:
		{
			Cast<ABSGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->EndGameMode(
				NewGameModeTransitionState.bSaveCurrentScores, false);
			StartGameMode(true);
			break;
		}
	case ETransitionState::QuitToMainMenu:
		{
			Cast<ABSGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->EndGameMode(
				NewGameModeTransitionState.bSaveCurrentScores, false);
			ABSPlayerController* PlayerController = Cast<ABSPlayerController>(
				UGameplayStatics::GetPlayerController(GetWorld(), 0));

			/** Hide all widgets and open MainMenu after the screen fades to black */
			PlayerController->OnScreenFadeToBlackFinish.BindLambda([this]
			{
				ABSPlayerController* Controller = Cast<ABSPlayerController>(
					UGameplayStatics::GetPlayerController(GetWorld(), 0));
				Controller->HidePostGameMenu();
				Controller->HidePauseMenu();
				UGameplayStatics::OpenLevel(GetWorld(), "MainMenuLevel");
			});
			PlayerController->FadeScreenToBlack();
			break;
		}
	case ETransitionState::QuitToDesktop:
		{
			bQuitToDesktopAfterSave = NewGameModeTransitionState.bSaveCurrentScores;
			Cast<ABSGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->EndGameMode(
				NewGameModeTransitionState.bSaveCurrentScores, false);
			if (!bQuitToDesktopAfterSave)
			{
				UKismetSystemLibrary::QuitGame(GetWorld(), UGameplayStatics::GetPlayerController(GetWorld(), 0),
					EQuitPreference::Quit, false);
			}
			break;
		}
	case ETransitionState::PlayAgain:
		{
			BSConfig.AudioConfig = NewGameModeTransitionState.BSConfig.AudioConfig;
			BSConfig.OnCreate();
			StartGameMode(true);
			break;
		}
	}
}

void UBSGameInstance::InitializeSteamManager()
{
	SteamManager = NewObject<USteamManager>(this);
	SteamManager->AssignGameInstance(this);
	SteamManager->InitializeSteamManager();
}

void UBSGameInstance::SavePlayerScoresToDatabase(const EPostScoresResponse& CurrentResponse)
{
	// If game mode encountered a reason not to save to database
	if (CurrentResponse != EPostScoresResponse::HttpSuccess)
	{
		ABSPlayerController* PlayerController = Cast<ABSPlayerController>(
			UGameplayStatics::GetPlayerController(GetWorld(), 0));
		PlayerController->OnPostScoresResponseReceived(CurrentResponse);
		return;
	}
	if (LoadPlayerSettings().User.RefreshCookie.IsEmpty())
	{
		ABSPlayerController* PlayerController = Cast<ABSPlayerController>(
			UGameplayStatics::GetPlayerController(GetWorld(), 0));
		PlayerController->OnPostScoresResponseReceived(EPostScoresResponse::NoAccount);
		return;
	}
	
	TSharedPtr<FAccessTokenResponse> AccessTokenResponse = MakeShareable(new FAccessTokenResponse);
	AccessTokenResponse->OnHttpResponseReceived.BindLambda([this, AccessTokenResponse]
	{
		if (AccessTokenResponse->AccessToken.IsEmpty())
		{
			if (bQuitToDesktopAfterSave)
			{
				UKismetSystemLibrary::QuitGame(GetWorld(), UGameplayStatics::GetPlayerController(GetWorld(), 0),
					EQuitPreference::Quit, false);
				return;
			}
			ABSPlayerController* PlayerController = Cast<ABSPlayerController>(
				UGameplayStatics::GetPlayerController(GetWorld(), 0));
			PlayerController->OnPostScoresResponseReceived(EPostScoresResponse::HttpError);
			return;
		}
		TSharedPtr<FPostScoresResponse> PostScoresResponse = MakeShareable(new FPostScoresResponse);
		PostScoresResponse->OnHttpResponseReceived.BindLambda([this, PostScoresResponse]
		{
			if (PostScoresResponse->PostScoresDescription == EPostScoresResponse::HttpSuccess)
			{
				SetAllPlayerScoresSavedToDatabase();
				ABSPlayerController* PlayerController = Cast<ABSPlayerController>(
					UGameplayStatics::GetPlayerController(GetWorld(), 0));
				PlayerController->OnPostScoresResponseReceived(EPostScoresResponse::HttpSuccess);
			}
			if (bQuitToDesktopAfterSave)
			{
				UKismetSystemLibrary::QuitGame(GetWorld(), UGameplayStatics::GetPlayerController(GetWorld(), 0),
					EQuitPreference::Quit, false);
			}
		});
		PostPlayerScores(LoadPlayerScores_UnsavedToDatabase(), LoadPlayerSettings().User.UserID,
			AccessTokenResponse->AccessToken, PostScoresResponse);
	});
	RequestAccessToken(LoadPlayerSettings().User.RefreshCookie, AccessTokenResponse);
}

void UBSGameInstance::OnSteamOverlayIsOn()
{
	IsSteamOverlayActive = true;
	this->OnSteamOverlayIsActive(true);
}

void UBSGameInstance::OnSteamOverlayIsOff()
{
	IsSteamOverlayActive = false;
	this->OnSteamOverlayIsActive(false);
}

void UBSGameInstance::OnSteamOverlayIsActive(bool bIsOverlayActive) const
{
	if (bIsOverlayActive)
	{
		ABSPlayerController* PlayerController = Cast<ABSPlayerController>(
			UGameplayStatics::GetPlayerController(GetWorld(), 0));
		PlayerController->HandlePause();
	}
}

void UBSGameInstance::AddDelegateToOnPlayerSettingsChanged(FOnPlayerSettingsChanged_Game& Delegate)
{
	Delegate.AddUObject(this, &UBSGameInstance::OnPlayerSettingsChanged_Game);
}

void UBSGameInstance::AddDelegateToOnPlayerSettingsChanged(FOnPlayerSettingsChanged_AudioAnalyzer& Delegate)
{
	Delegate.AddUObject(this, &UBSGameInstance::OnPlayerSettingsChanged_AudioAnalyzer);
}

void UBSGameInstance::AddDelegateToOnPlayerSettingsChanged(FOnPlayerSettingsChanged_User& Delegate)
{
	Delegate.AddUObject(this, &UBSGameInstance::OnPlayerSettingsChanged_User);
}

void UBSGameInstance::AddDelegateToOnPlayerSettingsChanged(FOnPlayerSettingsChanged_CrossHair& Delegate)
{
	Delegate.AddUObject(this, &UBSGameInstance::OnPlayerSettingsChanged_CrossHair);
}

void UBSGameInstance::AddDelegateToOnPlayerSettingsChanged(FOnPlayerSettingsChanged_VideoAndSound& Delegate)
{
	Delegate.AddUObject(this, &UBSGameInstance::OnPlayerSettingsChanged_VideoAndSound);
}

void UBSGameInstance::OnPlayerSettingsChanged_Game(const FPlayerSettings_Game& GameSettings)
{
	OnPlayerSettingsChangedDelegate_Game.Broadcast(GameSettings);
}

void UBSGameInstance::OnPlayerSettingsChanged_AudioAnalyzer(const FPlayerSettings_AudioAnalyzer& AudioAnalyzerSettings)
{
	OnPlayerSettingsChangedDelegate_AudioAnalyzer.Broadcast(AudioAnalyzerSettings);
}

void UBSGameInstance::OnPlayerSettingsChanged_User(const FPlayerSettings_User& UserSettings)
{
	OnPlayerSettingsChangedDelegate_User.Broadcast(UserSettings);
}

void UBSGameInstance::OnPlayerSettingsChanged_CrossHair(const FPlayerSettings_CrossHair& CrossHairSettings)
{
	OnPlayerSettingsChangedDelegate_CrossHair.Broadcast(CrossHairSettings);
}

void UBSGameInstance::OnPlayerSettingsChanged_VideoAndSound(const FPlayerSettings_VideoAndSound& VideoAndSoundSettings)
{
	OnPlayerSettingsChangedDelegate_VideoAndSound.Broadcast(VideoAndSoundSettings);
}
