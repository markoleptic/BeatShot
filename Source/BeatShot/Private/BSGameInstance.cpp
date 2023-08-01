// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "BSGameInstance.h"
#include "BSGameMode.h"
#include "Player/BSPlayerController.h"
#include "System/SteamManager.h"
#include "Kismet/GameplayStatics.h"
#include <steam/isteamuser.h>
#include <steam/steam_api.h>
#include "DLSSLibrary.h"
#include "NISLibrary.h"
#include "StreamlineLibraryReflex.h"
#include "GameFramework/GameUserSettings.h"

void UBSGameInstance::Init()
{
	Super::Init();
	
	OnPCFinishedUsingAuthTicket.BindUObject(this, &ThisClass::OnLoginToScoreBrowserAsyncTaskComplete);
	bSteamManagerInitialized = InitializeSteamManager();
	
	//IOnlineSubsystem* Ion = IOnlineSubsystem::Get(FName("Steam"));
}

void UBSGameInstance::OnStart()
{
	Super::OnStart();
	InitVideoSettings();
}

void UBSGameInstance::InitVideoSettings()
{
	UGameUserSettings* GameUserSettings = UGameUserSettings::GetGameUserSettings();
	FPlayerSettings Settings = LoadPlayerSettings();
	
	if (!Settings.User.bHasRanBenchmark)
	{
		GameUserSettings->RunHardwareBenchmark();
		GameUserSettings->ApplyHardwareBenchmarkResults();
		Settings.User.bHasRanBenchmark = true;
		SavePlayerSettings(Settings);
	}

	FIntPoint ScreenResolution = FIntPoint(0, 0);
	bool bIsSupported;
	float OptimalScreenPercentage;
	bool bIsFixedScreenPercentage;
	float MinScreenPercentage;
	float MaxScreenPercentage;
	float OptimalSharpness;
	
	if (Settings.VideoAndSound.DLSSMode == UDLSSMode::Auto)
	{
		ScreenResolution = UGameUserSettings::GetGameUserSettings()->GetScreenResolution();
	}
	
	UDLSSLibrary::GetDLSSModeInformation(Settings.VideoAndSound.DLSSMode, FVector2d(ScreenResolution.X, ScreenResolution.Y), bIsSupported, OptimalScreenPercentage,
	bIsFixedScreenPercentage, MinScreenPercentage, MaxScreenPercentage, OptimalSharpness);

	const bool bIsDLAA = Settings.VideoAndSound.DLSSMode == UDLSSMode::DLAA;
	const bool bShouldEnable = (Settings.VideoAndSound.DLSSMode != UDLSSMode::Off || bIsDLAA) && bIsSupported;
	const bool bValidScreenPercentage = OptimalScreenPercentage > 0.f && bIsSupported;

	// Enable/Disable DLSS
	UDLSSLibrary::EnableDLSS(bShouldEnable);

	// Set Screen Percentage
	float SelectedScreenPercentage;
	if (!bValidScreenPercentage || bIsDLAA)
	{
		// DLAA overrides DLSS mode if both are enabled
		SelectedScreenPercentage = 100.f;
	}
	else
	{
		SelectedScreenPercentage = OptimalScreenPercentage;
	}
	
	// Execute Screen Percentage Console Command
	if (bShouldEnable)
	{
		UDLSSLibrary::SetDLSSSharpness(Settings.VideoAndSound.DLSSSharpness);
		if (static IConsoleVariable* CVarScreenPercentage = IConsoleManager::Get().FindConsoleVariable(TEXT("r.ScreenPercentage")))
		{
			CVarScreenPercentage->Set(SelectedScreenPercentage);
		}
	}

	// Frame Generation
	if (UStreamlineLibraryDLSSG::IsDLSSGModeSupported(Settings.VideoAndSound.FrameGenerationEnabledMode))
	{
		UStreamlineLibraryDLSSG::SetDLSSGMode(Settings.VideoAndSound.FrameGenerationEnabledMode);
	}
	else
	{
		UStreamlineLibraryDLSSG::SetDLSSGMode(UStreamlineDLSSGMode::Off);
	}

	// NIS
	if (UNISLibrary::IsNISModeSupported(Settings.VideoAndSound.NISMode))
	{
		UNISLibrary::SetNISMode(Settings.VideoAndSound.NISMode);
		UNISLibrary::SetNISSharpness(Settings.VideoAndSound.NISSharpness);
	}
	else
	{
		UNISLibrary::SetNISMode(UNISMode::Off);
	}

	// Reflex
	if (UStreamlineLibraryReflex::IsReflexSupported())
	{
		UStreamlineLibraryReflex::SetReflexMode(Settings.VideoAndSound.StreamlineReflexMode);
	}
	else
	{
		UStreamlineLibraryReflex::SetReflexMode(UStreamlineReflexMode::Disabled);
	}
}

void UBSGameInstance::Shutdown()
{
	if (bSteamManagerInitialized && SteamManager)
	{
		SteamManager->ShutdownSteamManager();
	}
	SteamAPI_Shutdown();
	Super::Shutdown();
}

void UBSGameInstance::StartGameMode(const bool bIsRestart) const
{
	ABSPlayerController* PlayerController = Cast<ABSPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	
	if (PlayerController->IsPaused())
	{
		PlayerController->HandlePause();
	}

	/** Hide all widgets and show the countdown after the screen fades to black */
	PlayerController->OnScreenFadeToBlackFinish.BindLambda([this, bIsRestart]
	{
		ABSPlayerController* Controller = Cast<ABSPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
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
			UE_LOG(LogTemp, Display, TEXT("Restarting"));
			Cast<ABSGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->EndGameMode(NewGameModeTransitionState.bSaveCurrentScores, false);
			StartGameMode(true);
			break;
		}
	case ETransitionState::QuitToMainMenu:
		{
			Cast<ABSGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->EndGameMode(NewGameModeTransitionState.bSaveCurrentScores, false);
			ABSPlayerController* PlayerController = Cast<ABSPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

			/** Hide all widgets and open MainMenu after the screen fades to black */
			PlayerController->OnScreenFadeToBlackFinish.BindLambda([this]
			{
				ABSPlayerController* Controller = Cast<ABSPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
				Controller->HidePostGameMenu();
				Controller->HidePauseMenu();
				UGameplayStatics::OpenLevel(GetWorld(), "MainMenuLevel");
			});
			PlayerController->FadeScreenToBlack();
			break;
		}
	case ETransitionState::QuitToDesktop:
		{
			Cast<ABSGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->EndGameMode(NewGameModeTransitionState.bSaveCurrentScores, false);
			UKismetSystemLibrary::QuitGame(GetWorld(), UGameplayStatics::GetPlayerController(GetWorld(), 0), EQuitPreference::Quit, false);
			break;
		}
	default:
		{
			break;
		}
	}
}

bool UBSGameInstance::InitializeSteamManager()
{
	SteamManager = NewObject<USteamManager>(this);
	SteamManager->AssignGameInstance(this);
	
	if (!SteamAPI_Init())
	{
		UE_LOG(LogTemp, Warning, TEXT("SteamAPI_Init Failed"));
		return false;
	}
	if (SteamUser() != nullptr)
	{
		SteamManager->InitializeSteamManager();
		TicketWebApiResponse.BindUObject(this, &ThisClass::OnAuthTicketForWebApiResponse);
		UE_LOG(LogTemp, Display, TEXT("SteamAPI_Init Success"));
		return true;
	}
	return false;
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
		ABSPlayerController* PlayerController = Cast<ABSPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
		PlayerController->HandlePause();
	}
}

void UBSGameInstance::OnPlayerControllerReadyForSteamLogin(ABSPlayerController* PlayerController)
{
	// TODO: might need to handle case where SteamManager doesn't have Auth Ticket ready yet
	bHttpAuthTicketAsyncTaskComplete = false;
	bLoginToScoreBrowserAsyncTaskComplete = false;

	if (!bSteamManagerInitialized)
	{
		PlayerController->LoginToScoreBrowserWithSteam("", OnPCFinishedUsingAuthTicket);
		UE_LOG(LogTemp, Warning, TEXT("SteamManager not initialized"));
		return;
	}
	
	SteamManager->OnAuthTicketForWebApiReady.BindLambda([this, PlayerController] (const bool bSuccess)
	{
		if (bSuccess)
		{
			AsyncTask(ENamedThreads::GameThread, [this]()
			{
				AuthenticateSteamUser(SteamManager->GetWebApiTicket(), TicketWebApiResponse);
			});
			AsyncTask(ENamedThreads::GameThread, [this, PlayerController]()
			{
				// TEMP AUTO FAIL LOGIN
				//PlayerController->LoginToScoreBrowserWithSteam("", OnPCFinishedUsingAuthTicket);
				PlayerController->LoginToScoreBrowserWithSteam(SteamManager->GetWebApiTicket(), OnPCFinishedUsingAuthTicket);
			});
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Auth Ticket Creation Unsuccessful"));
			PlayerController->LoginToScoreBrowserWithSteam("", OnPCFinishedUsingAuthTicket);
		}
	});
	SteamManager->CreateAuthTicketForWebApi();
}

void UBSGameInstance::OnAuthTicketForWebApiResponse(const FSteamAuthTicketResponse& Response, const bool bSuccess)
{
	// TODO: handle unsuccessful
	if (bSuccess)
	{
		const uint64 LocalSteamID = SteamUser()->GetSteamID().ConvertToUint64();
		const uint64 ResponseSteamID = FCString::Atoi64(*Response.SteamID);
		
		if (LocalSteamID == ResponseSteamID)
		{
			FPlayerSettings_User PlayerSettings = LoadPlayerSettings().User;
			PlayerSettings.DisplayName = FString(SteamFriends()->GetPersonaName());
			PlayerSettings.UserID = Response.SteamID;
			PlayerSettings.RefreshCookie = Response.RefreshCookie;
			SavePlayerSettings(PlayerSettings);
		}
	}
	bHttpAuthTicketAsyncTaskComplete = true;
	CheckIfAsyncAuthTasksComplete();
}

void UBSGameInstance::OnLoginToScoreBrowserAsyncTaskComplete()
{
	bLoginToScoreBrowserAsyncTaskComplete = true;
	CheckIfAsyncAuthTasksComplete();
}

void UBSGameInstance::CheckIfAsyncAuthTasksComplete()
{
	if (bHttpAuthTicketAsyncTaskComplete && bLoginToScoreBrowserAsyncTaskComplete)
	{
		UE_LOG(LogTemp, Display, TEXT("Resetting Auth Ticket"));
		SteamManager->ResetWebApiTicket();
	}
}

void UBSGameInstance::AddDelegateToOnPlayerSettingsChanged(FOnPlayerSettingsChanged_Game& Delegate)
{
	Delegate.AddUniqueDynamic(this, &UBSGameInstance::OnPlayerSettingsChanged_Game);
}

void UBSGameInstance::AddDelegateToOnPlayerSettingsChanged(FOnPlayerSettingsChanged_AudioAnalyzer& Delegate)
{
	Delegate.AddUniqueDynamic(this, &UBSGameInstance::OnPlayerSettingsChanged_AudioAnalyzer);
}

void UBSGameInstance::AddDelegateToOnPlayerSettingsChanged(FOnPlayerSettingsChanged_User& Delegate)
{
	Delegate.AddUniqueDynamic(this, &UBSGameInstance::OnPlayerSettingsChanged_User);
}

void UBSGameInstance::AddDelegateToOnPlayerSettingsChanged(FOnPlayerSettingsChanged_CrossHair& Delegate)
{
	Delegate.AddUniqueDynamic(this, &UBSGameInstance::OnPlayerSettingsChanged_CrossHair);
}

void UBSGameInstance::AddDelegateToOnPlayerSettingsChanged(FOnPlayerSettingsChanged_VideoAndSound& Delegate)
{
	Delegate.AddUniqueDynamic(this, &UBSGameInstance::OnPlayerSettingsChanged_VideoAndSound);
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
