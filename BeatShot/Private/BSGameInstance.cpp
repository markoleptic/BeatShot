// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "BSGameInstance.h"
#include "BSGameMode.h"
#include "Player/BSPlayerController.h"
#include "System/SteamManager.h"
#include "Kismet/GameplayStatics.h"
#include "DLSSFunctions.h"
#include "MoviePlayer.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/GameUserSettings.h"
#include "OverlayWidgets/LoadingScreenWidgets/SLoadingScreenWidget.h"

void UBSGameInstance::Init()
{
	Super::Init();
	GetMoviePlayer()->OnPrepareLoadingScreen().AddUObject(this, &ThisClass::PrepareLoadingScreen);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &ThisClass::OnPostLoadMapWithWorld);
	InitializeSteamManager();
}

FGameInstancePIEResult UBSGameInstance::StartPlayInEditorGameInstance(ULocalPlayer* LocalPlayer,
	const FGameInstancePIEParameters& Params)
{
	const FBSConfig LocalConfig = FBSConfig();
	SetBSConfig(LocalConfig);
	if (GetWorld()->GetMapName().Contains("Range"))
	{
		ABSGameMode* GM = Cast<ABSGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
		if (GM)
		{
			GM->InitializeGameMode(GetBSConfig());
		}
	}
	return Super::StartPlayInEditorGameInstance(LocalPlayer, Params);
}

void UBSGameInstance::OnPostLoadMapWithWorld(UWorld* World)
{
	if (World->GetMapName().Contains("Range"))
	{
		ABSGameMode* GM = Cast<ABSGameMode>(UGameplayStatics::GetGameMode(World));
		if (GM)
		{
			GM->InitializeGameMode(GetBSConfig());
		}
	}
	// Fade out the loading screen when map is ready
	if (LoadingScreenWidget)
	{
		LoadingScreenWidget->SetLoadingScreenState(ELoadingScreenState::FadingOut);
	}
}

void UBSGameInstance::OnLoadingScreenFadeOutComplete()
{
	// Widget has completed fade out, so can be removed
	GetMoviePlayer()->StopMovie();
	if (LoadingScreenWidget.IsValid())
	{
		LoadingScreenWidget.Reset();
	}

	// No longer the initial loading screen
	bIsInitialLoadingScreen = false;

	// Fade screen from black
	ABSPlayerController* PC = Cast<ABSPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PC)
	{
		PC->FadeScreenFromBlack();
	}
}

void UBSGameInstance::PrepareLoadingScreen()
{
	FLoadingScreenAttributes Attributes;
	Attributes.bAutoCompleteWhenLoadingCompletes = false;
	Attributes.bAllowEngineTick = true;
	Attributes.bMoviesAreSkippable = false;
	Attributes.bWaitForManualStop = true;
	Attributes.MinimumLoadingScreenDisplayTime = 2.f;
	if (SlateWidgetStyleAsset && SlateWidgetStyleAsset->CustomStyle)
	{
		if (const FLoadingScreenStyle* Style = static_cast<const struct FLoadingScreenStyle*>(SlateWidgetStyleAsset->CustomStyle->GetStyle()))
		{
			SAssignNew(LoadingScreenWidget, SLoadingScreenWidget)
			.LoadingScreenStyle(Style)
			.OnFadeOutComplete(BIND_UOBJECT_DELEGATE(FOnFadeOutComplete, OnLoadingScreenFadeOutComplete))
			.bIsInitialLoadingScreen(bIsInitialLoadingScreen);
		}
	}
	Attributes.WidgetLoadingScreen = LoadingScreenWidget;
	GetMoviePlayer()->SetupLoadingScreen(Attributes);
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

void UBSGameInstance::SetBSConfig(const FBSConfig& InConfig)
{
	if (!BSConfig.IsValid())
	{
		BSConfig = MakeShareable(new FBSConfig(InConfig));
	}
	else
	{
		*BSConfig = InConfig;
	}
}

void UBSGameInstance::StartGameMode(const bool bIsRestart) const
{
	ABSPlayerController* PC = Cast<ABSPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	if (!PC) return;

	if (PC->IsPaused())
	{
		PC->HandlePause();
	}

	/** Hide all widgets and show the countdown after the screen fades to black */
	PC->OnScreenFadeToBlackFinish.BindLambda([this, bIsRestart]
	{
		ABSPlayerController* Controller = Cast<ABSPlayerController>(
			UGameplayStatics::GetPlayerController(GetWorld(), 0));
		if (Controller)
		{
			Controller->HideMainMenu();
			Controller->HidePostGameMenu();
			Controller->HidePauseMenu();

			if (GetWorld()->GetMapName().Contains("Range"))
			{
				Controller->ShowCountdown(bIsRestart);
				if (ABSGameMode* GM = Cast<ABSGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
				{
					GM->InitializeGameMode(GetBSConfig());
				}
			}
			else
			{
				UGameplayStatics::OpenLevel(GetWorld(), FName("Range"));
			}
		}
	});
	PC->FadeScreenToBlack();
}

void UBSGameInstance::HandleGameModeTransition(const FGameModeTransitionState& NewGameModeTransitionState)
{
	switch (NewGameModeTransitionState.TransitionState)
	{
	case ETransitionState::StartFromMainMenu:
		{
			SetBSConfig(NewGameModeTransitionState.BSConfig);
			StartGameMode(false);
			break;
		}
	case ETransitionState::StartFromPostGameMenu:
		{
			SetBSConfig(NewGameModeTransitionState.BSConfig);
			StartGameMode(true);
			break;
		}
	case ETransitionState::Restart:
		{
			if (ABSGameMode* GM = Cast<ABSGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
			{
				GM->EndGameMode(NewGameModeTransitionState.bSaveCurrentScores, false);
			}
			StartGameMode(true);
			break;
		}
	case ETransitionState::QuitToMainMenu:
		{
			if (ABSGameMode* GM = Cast<ABSGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
			{
				GM->EndGameMode(NewGameModeTransitionState.bSaveCurrentScores, false);
			}
			ABSPlayerController* PC = Cast<ABSPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
			if (PC)
			{
				/** Hide all widgets and open MainMenu after the screen fades to black */
				PC->OnScreenFadeToBlackFinish.BindLambda([this]
				{
					ABSPlayerController* Controller = Cast<ABSPlayerController>(
						UGameplayStatics::GetPlayerController(GetWorld(), 0));
					if (Controller)
					{
						Controller->HidePostGameMenu();
						Controller->HidePauseMenu();
					}
					UGameplayStatics::OpenLevel(GetWorld(), "MainMenuLevel");
				});
				PC->FadeScreenToBlack();
			}
			break;
		}
	case ETransitionState::QuitToDesktop:
		{
			bQuitToDesktopAfterSave = NewGameModeTransitionState.bSaveCurrentScores;
			if (ABSGameMode* GM = Cast<ABSGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
			{
				GM->EndGameMode(NewGameModeTransitionState.bSaveCurrentScores, false);
			}
			if (!bQuitToDesktopAfterSave)
			{
				UKismetSystemLibrary::QuitGame(GetWorld(), UGameplayStatics::GetPlayerController(GetWorld(), 0),
					EQuitPreference::Quit, false);
			}
			break;
		}
	case ETransitionState::PlayAgain:
		{
			BSConfig->AudioConfig = NewGameModeTransitionState.BSConfig.AudioConfig;
			BSConfig->OnCreate();
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

void UBSGameInstance::SavePlayerScoresToDatabase(const bool bWasValidToSave)
{
	// If game mode encountered a reason not to save to database
	if (!bWasValidToSave)
	{
		if (ABSPlayerController* PC = Cast<ABSPlayerController>(GetFirstLocalPlayerController(GetWorld())))
		{
			PC->OnPostScoresResponseReceived("SBW_DidNotSaveScores");
		}
		return;
	}

	// No account
	if (LoadPlayerSettings().User.RefreshCookie.IsEmpty())
	{
		if (ABSPlayerController* PC = Cast<ABSPlayerController>(GetFirstLocalPlayerController(GetWorld())))
		{
			PC->OnPostScoresResponseReceived("SBW_NoAccount");
		}
		return;
	}
	
	// Acquire access token
	TSharedPtr<FAccessTokenResponse> AccessTokenResponse = MakeShareable(new FAccessTokenResponse());
	AccessTokenResponse->OnHttpResponseReceived.BindLambda([this, AccessTokenResponse]
	{
		if (AccessTokenResponse->OK) // Successful access token retrieval
		{
			TSharedPtr<FBSHttpResponse> PostScoresResponse = MakeShareable(new FBSHttpResponse());
			PostScoresResponse->OnHttpResponseReceived.BindLambda([this, PostScoresResponse]
			{
				ABSPlayerController* PC = Cast<ABSPlayerController>(GetFirstLocalPlayerController(GetWorld()));
				if (PostScoresResponse->OK) // Successful scores post
				{
					SetAllPlayerScoresSavedToDatabase();
					if (PC) PC->OnPostScoresResponseReceived();
				}
				else // Unsuccessful scores post
				{
					if (PC) PC->OnPostScoresResponseReceived("SBW_SavedScoresLocallyOnly");
				}
				
				if (bQuitToDesktopAfterSave)
				{
					UKismetSystemLibrary::QuitGame(GetWorld(), UGameplayStatics::GetPlayerController(GetWorld(), 0),
						EQuitPreference::Quit, false);
				}
			});
			PostPlayerScores(LoadPlayerScores_UnsavedToDatabase(), LoadPlayerSettings().User.UserID,
				AccessTokenResponse->AccessToken, PostScoresResponse);
		}
		else // Unsuccessful access token retrieval
		{
			if (ABSPlayerController* PC = Cast<ABSPlayerController>(GetFirstLocalPlayerController(GetWorld())))
			{
				PC->OnPostScoresResponseReceived("SBW_SavedScoresLocallyOnly");
			}
			
			if (bQuitToDesktopAfterSave)
			{
				UKismetSystemLibrary::QuitGame(GetWorld(), UGameplayStatics::GetPlayerController(GetWorld(), 0),
					EQuitPreference::Quit, false);
			}
		}
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
		if (ABSPlayerController* PC = Cast<ABSPlayerController>(GetFirstLocalPlayerController(GetWorld())))
		{
			PC->HandlePause();
		}
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
