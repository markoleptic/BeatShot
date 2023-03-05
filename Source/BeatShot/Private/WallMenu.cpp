// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "WallMenu.h"
#include "DefaultCharacter.h"
#include "DefaultGameInstance.h"
#include "Kismet/GameplayStatics.h"

void AWallMenu::BeginPlay()
{
	Super::BeginPlay();
	Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()))->OnPlayerSettingsChange.AddUniqueDynamic(this, &AWallMenu::OnPlayerSettingsChanged);
	InitializeSettings(LoadPlayerSettings());
}

void AWallMenu::Destroyed()
{
	Super::Destroyed();
}

FPlayerSettings AWallMenu::LoadPlayerSettings() const
{
	return ISaveLoadInterface::LoadPlayerSettings();
}

void AWallMenu::SavePlayerSettings(const FPlayerSettings PlayerSettingsToSave)
{
	ISaveLoadInterface::SavePlayerSettings(PlayerSettingsToSave);
	Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()))->OnPlayerSettingsChange.Broadcast(PlayerSettingsToSave);
}

void AWallMenu::OnPlayerSettingsChanged(const FPlayerSettings& PlayerSettings)
{
	InitializeSettings(PlayerSettings);
}
