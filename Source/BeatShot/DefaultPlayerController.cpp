// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultPlayerController.h"
#include "MainMenuWidget.h"
#include "Crosshair.h"
#include "PlayerHUD.h"
#include "PauseMenu.h"
#include "Countdown.h"
#include "PostGameMenuWidget.h"
#include "DefaultGameInstance.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"

void ADefaultPlayerController::BeginPlay()
{
	Super::BeginPlay();
	GI = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(this));
	GI->RegisterPlayerController(this);
	PlayerHUDActive = false;
	PostGameMenuActive = false;
}

void ADefaultPlayerController::setPlayerEnabledState(bool bPlayerEnabled)
{
	if (bPlayerEnabled) 
	{
		GetPawn()->EnableInput(this);
	}
	else
	{
		GetPawn()->DisableInput(this);
	}
}

void ADefaultPlayerController::ShowMainMenu()
{
	MainMenu = CreateWidget<UMainMenuWidget>(this, MainMenuClass);
	MainMenu->AddToViewport();
}

void ADefaultPlayerController::HideMainMenu()
{
	if (MainMenu)
	{
		MainMenu->RemoveFromViewport();
		MainMenu = nullptr;
	}
}

void ADefaultPlayerController::ShowPauseMenu()
{
	PauseMenu = CreateWidget<UPauseMenu>(this, PauseMenuClass);
	PauseMenu->AddToViewport();
}

void ADefaultPlayerController::HidePauseMenu()
{
	if (PauseMenu)
	{
		PauseMenu->RemoveFromViewport();
		PauseMenu = nullptr;
	}
}

void ADefaultPlayerController::ShowCrosshair()
{
	Crosshair = CreateWidget<UCrosshair>(this, CrosshairClass);
	Crosshair->AddToViewport();
}

void ADefaultPlayerController::HideCrosshair()
{
	if (Crosshair)
	{
		Crosshair->RemoveFromViewport();
		Crosshair = nullptr;
	}
}

void ADefaultPlayerController::ShowPlayerHUD()
{
	PlayerHUD = CreateWidget<UPlayerHUD>(this, PlayerHUDClass);
	PlayerHUD->AddToViewport();
	PlayerHUDActive = true;
}

void ADefaultPlayerController::HidePlayerHUD()
{
	if (PlayerHUD)
	{
		PlayerHUD->RemoveFromViewport();
		PlayerHUD = nullptr;
		PlayerHUDActive = false;
	}
}

void ADefaultPlayerController::ShowCountdown()
{
	Countdown = CreateWidget<UCountdown>(this, CountdownClass);
	Countdown->AddToViewport();
	CountdownActive = true;
}

void ADefaultPlayerController::ShowPostGameMenu()
{
	PostGameMenuWidget = CreateWidget<UPostGameMenuWidget>(this, PostGameMenuWidgetClass);
	PostGameMenuWidget->AddToViewport();
	PostGameMenuActive = true;
}

void ADefaultPlayerController::HidePostGameMenu()
{
	if (PostGameMenuWidget)
	{
		PostGameMenuWidget->RemoveFromViewport();
		PostGameMenuWidget = nullptr;
		PostGameMenuActive = false;
	}
}

void ADefaultPlayerController::HideCountdown()
{
	if (Countdown)
	{
		Countdown->RemoveFromViewport();
		Countdown = nullptr;
		CountdownActive = false;
	}
}

bool ADefaultPlayerController::IsPlayerHUDActive()
{
	return PlayerHUDActive;
}

bool ADefaultPlayerController::IsPostGameMenuActive()
{
	return PostGameMenuActive;
}
