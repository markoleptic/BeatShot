// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultGameInstance.h"
#include "DefaultGameMode.h"
#include "DefaultPlayerController.h"
#include "Kismet/GameplayStatics.h"

void UDefaultGameInstance::QuitToMainMenu(const bool bShouldSaveScores) const
{
	Cast<ADefaultGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->EndGameMode(bShouldSaveScores, false);
	ADefaultPlayerController* PlayerController = Cast<ADefaultPlayerController>(
		UGameplayStatics::GetPlayerController(GetWorld(), 0));
	PlayerController->FadeScreenToBlack();
	PlayerController->OnScreenFadeToBlackFinish.BindLambda([&]
	{
		ADefaultPlayerController* Controller = Cast<ADefaultPlayerController>(
			UGameplayStatics::GetPlayerController(GetWorld(), 0));
		Controller->HidePostGameMenu();
		Controller->HidePauseMenu();
		UGameplayStatics::OpenLevel(GetWorld(), "MainMenuLevel");
	});
}

void UDefaultGameInstance::StartGameMode(const bool bIsRestarting, const bool bShouldSaveScores) const
{
	ADefaultPlayerController* PlayerController = Cast<ADefaultPlayerController>(
		UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PlayerController->IsPaused())
	{
		PlayerController->HandlePause();
	}
	if (bIsRestarting)
	{
		Cast<ADefaultGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->EndGameMode(bShouldSaveScores, false);
	}
	PlayerController->HidePostGameMenu();
	PlayerController->HidePauseMenu();
	PlayerController->OnScreenFadeToBlackFinish.BindLambda([&]
	{
		ADefaultPlayerController* Controller = Cast<ADefaultPlayerController>(
			UGameplayStatics::GetPlayerController(GetWorld(), 0));
		Controller->HideMainMenu();
		/* Can be called from MainMenuWidget and PostGameMenuWidget */
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

void UDefaultGameInstance::OnQuit(const bool bShouldSaveScores, const bool bShouldRestart,
                                  const bool bGoToMainMenu)
{
	if (bGoToMainMenu)
	{
		QuitToMainMenu(bShouldSaveScores);
		return;
	}
	if (bShouldRestart)
	{
		StartGameMode(true, bShouldSaveScores);
		return;
	}
	QuitToDesktop(bShouldSaveScores);
}

void UDefaultGameInstance::InitializeGameModeActor(const FGameModeActorStruct& GameModeActor)
{
	GameModeActorStruct = GameModeActor;
}

void UDefaultGameInstance::QuitToDesktop(const bool bShouldSaveScores) const
{
	Cast<ADefaultGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->EndGameMode(bShouldSaveScores, false);
}
