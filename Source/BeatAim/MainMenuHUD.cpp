// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuHUD.h"
#include "MainMenuWidget.h"
#include "SettingsMenuWidget.h"
#include <GameFramework/PlayerController.h>
#include <Blueprint/UserWidget.h>
#include <Kismet/GameplayStatics.h>

void AMainMenuHUD::ShowMainMenu()
{
	// Make widget owned by our PlayerController
	APlayerController* PC = Cast<APlayerController>(GetOwner());
	MainMenu = CreateWidget<UMainMenuWidget>(PC, MainMenuClass);
	MainMenu->AddToViewport();
}

void AMainMenuHUD::HideMainMenu()
{
	if (MainMenu)
	{
		MainMenu->RemoveFromViewport();
		MainMenu = nullptr;
	}
}

void AMainMenuHUD::ShowSettingsMenu()
{
	// Make widget owned by our PlayerController
	APlayerController* PC = Cast<APlayerController>(GetOwner());
	SettingsMenu = CreateWidget<USettingsMenuWidget>(PC, SettingsMenuClass);
	SettingsMenu->AddToViewport();
}

void AMainMenuHUD::HideSettingsMenu()
{
	if (SettingsMenu)
	{
		SettingsMenu->RemoveFromViewport();
		SettingsMenu = nullptr;
	}
}

