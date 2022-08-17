// Fill out your copyright notice in the Description page of Project Settings.


#include "SettingsMenuWidget.h"
#include <GameFramework/PlayerController.h>
#include <Blueprint/UserWidget.h>
#include <Kismet/GameplayStatics.h>

void USettingsMenuWidget::ShowSettingsMenu()
{
	// Make widget owned by our PlayerController
	APlayerController* PC = Cast<APlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	SettingsMenu = CreateWidget<USettingsMenuWidget>(PC, SettingsMenuClass);
	SettingsMenu->AddToViewport();
}

void USettingsMenuWidget::HideSettingsMenu()
{
	if (SettingsMenu)
	{
		SettingsMenu->RemoveFromViewport();
		SettingsMenu = nullptr;
	}
}
