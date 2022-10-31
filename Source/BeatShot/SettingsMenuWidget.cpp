// Fill out your copyright notice in the Description page of Project Settings.


#include "SettingsMenuWidget.h"
#include "DefaultGameInstance.h"
#include "GameFramework/GameUserSettings.h"
#include <Blueprint/UserWidget.h>
#include <Kismet/GameplayStatics.h>

void USettingsMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
	GI = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(this));
	LoadPlayerSettings();
	PopulatePlayerSettings();
}

void USettingsMenuWidget::LoadPlayerSettings()
{
	PlayerSettings = GI->LoadPlayerSettings();
}

void USettingsMenuWidget::SavePlayerSettings()
{
	GI->SavePlayerSettings(PlayerSettings);
}

void USettingsMenuWidget::ResetPlayerSettings()
{
	PlayerSettings.ResetStruct();
}
