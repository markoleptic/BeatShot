// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultGameInstance.h"
#include "DefaultGameMode.h"
#include "DefaultPlayerController.h"
#include "Kismet/GameplayStatics.h"

void UDefaultGameInstance::StartGameMode(const bool bShowOpenFileDialog) const
{
	ADefaultPlayerController* PlayerController = Cast<ADefaultPlayerController>(
		UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PlayerController->IsPaused())
	{
		PlayerController->HandlePause();
	}
	PlayerController->OnScreenFadeToBlackFinish.BindLambda([this, bShowOpenFileDialog]
	{
		ADefaultPlayerController* Controller = Cast<ADefaultPlayerController>(
			UGameplayStatics::GetPlayerController(GetWorld(), 0));
		Controller->HideMainMenu();
		Controller->HidePostGameMenu();
		Controller->HidePauseMenu();
		if (GetWorld()->GetMapName().Contains("Range"))
		{
			Cast<ADefaultGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->InitializeGameMode(bShowOpenFileDialog);
		}
		else
		{
			UGameplayStatics::OpenLevel(GetWorld(), FName("Range"));
		}
	});
	PlayerController->FadeScreenToBlack();
}

void UDefaultGameInstance::HandleGameModeTransition(const FGameModeTransitionState& GameModeTransitionState)
{
	bLastSavedShowOpenFileDialog = GameModeTransitionState.bShowOpenFileDialog;
	switch (GameModeTransitionState.TransitionState)
	{
	case ETransitionState::StartFromMainMenu:
		{
			GameModeActorStruct = GameModeTransitionState.GameModeActorStruct;
			StartGameMode(GameModeTransitionState.bShowOpenFileDialog);
			break;
		}
	case ETransitionState::StartFromPostGameMenu:
		{
			GameModeActorStruct = GameModeTransitionState.GameModeActorStruct;
			StartGameMode(GameModeTransitionState.bShowOpenFileDialog);
			break;
		}
	case ETransitionState::Restart:
		{
			Cast<ADefaultGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->EndGameMode(GameModeTransitionState.bSaveCurrentScores, false);
			StartGameMode(GameModeTransitionState.bShowOpenFileDialog);
			break;
		}
	case ETransitionState::QuitToMainMenu:
		{
			Cast<ADefaultGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->EndGameMode(GameModeTransitionState.bSaveCurrentScores, false);
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
			Cast<ADefaultGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->EndGameMode(GameModeTransitionState.bSaveCurrentScores, false);
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
