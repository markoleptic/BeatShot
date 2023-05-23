// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "BSGameInstance.h"
#include "BSGameMode.h"
#include "Player/BSPlayerController.h"
#include "System/SteamManager.h"
#include "Kismet/GameplayStatics.h"

void UBSGameInstance::Init()
{
	Super::Init();
	InitializeCPPElements();
}

bool UBSGameInstance::InitializeCPPElements()
{
	if (!SteamAPI_Init())
	{
		UE_LOG(LogTemp, Display, TEXT("SteamAPI_Init Failed"));
		return false;
	}

	if (EnableUSteamManagerFeatures && SteamUser() != nullptr)
	{
		SteamManager = NewObject<USteamManager>(this);
		SteamManager->InitializeSteamManager();
		SteamManager->AssignGameInstance(this);
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
