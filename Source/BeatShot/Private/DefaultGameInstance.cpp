// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultGameInstance.h"
#include "DefaultGameMode.h"
#include "DefaultPlayerController.h"
#include "SteamManager.h"
#include "Kismet/GameplayStatics.h"

void UDefaultGameInstance::Init()
{
	Super::Init();
	InitializeCPPElements();
}

bool UDefaultGameInstance::InitializeCPPElements()
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

void UDefaultGameInstance::OnSteamOverlayIsOn()
{
	IsSteamOverlayActive = true;
	this->OnSteamOverlayIsActive(true);
}

void UDefaultGameInstance::OnSteamOverlayIsOff()
{
	IsSteamOverlayActive = false;
	this->OnSteamOverlayIsActive(false);
}

void UDefaultGameInstance::OnSteamOverlayIsActive(bool bIsOverlayActive) const
{
	if (bIsOverlayActive)
	{
		ADefaultPlayerController* PlayerController = Cast<ADefaultPlayerController>(
	UGameplayStatics::GetPlayerController(GetWorld(), 0));
		PlayerController->HandlePause();
	}
}

void UDefaultGameInstance::StartGameMode() const
{
	ADefaultPlayerController* PlayerController = Cast<ADefaultPlayerController>(
		UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PlayerController->IsPaused())
	{
		PlayerController->HandlePause();
	}
	PlayerController->OnScreenFadeToBlackFinish.BindLambda([this]
	{
		ADefaultPlayerController* Controller = Cast<ADefaultPlayerController>(
			UGameplayStatics::GetPlayerController(GetWorld(), 0));
		Controller->HideMainMenu();
		Controller->HidePostGameMenu();
		Controller->HidePauseMenu();
		if (GetWorld()->GetMapName().Contains("Range"))
		{
			Cast<ADefaultGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->InitializeGameMode();
		}
		else
		{
			UGameplayStatics::OpenLevel(GetWorld(), FName("Range"));
		}
	});
	PlayerController->FadeScreenToBlack();
}

void UDefaultGameInstance::HandleGameModeTransition(FGameModeTransitionState& NewGameModeTransitionState)
{
	switch (NewGameModeTransitionState.TransitionState)
	{
	case ETransitionState::StartFromMainMenu:
		{
			GameModeActorStruct = NewGameModeTransitionState.GameModeActorStruct;
			StartGameMode();
			break;
		}
	case ETransitionState::StartFromPostGameMenu:
		{
			GameModeActorStruct = NewGameModeTransitionState.GameModeActorStruct;
			StartGameMode();
			break;
		}
	case ETransitionState::Restart:
		{
			Cast<ADefaultGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->EndGameMode(
				NewGameModeTransitionState.bSaveCurrentScores, false);
			StartGameMode();
			break;
		}
	case ETransitionState::QuitToMainMenu:
		{
			Cast<ADefaultGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->EndGameMode(
				NewGameModeTransitionState.bSaveCurrentScores, false);
			ADefaultPlayerController* PlayerController = Cast<ADefaultPlayerController>(
				UGameplayStatics::GetPlayerController(GetWorld(), 0));
			PlayerController->FadeScreenToBlack();
			PlayerController->OnScreenFadeToBlackFinish.BindLambda([this]
			{
				ADefaultPlayerController* Controller = Cast<ADefaultPlayerController>(
					UGameplayStatics::GetPlayerController(GetWorld(), 0));
				Controller->HidePostGameMenu();
				Controller->HidePauseMenu();
				UGameplayStatics::OpenLevel(GetWorld(), "MainMenuLevel");
			});
			break;
		}
	case ETransitionState::QuitToDesktop:
		{
			Cast<ADefaultGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->EndGameMode(
				NewGameModeTransitionState.bSaveCurrentScores, false);
			UKismetSystemLibrary::QuitGame(GetWorld(), UGameplayStatics::GetPlayerController(GetWorld(), 0),
			                               EQuitPreference::Quit, false);
			break;
		}
	default:
		{
			break;
		}
	}
}
