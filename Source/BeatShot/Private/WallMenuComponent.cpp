// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "WallMenuComponent.h"

#include "DefaultGameInstance.h"
#include "Kismet/GameplayStatics.h"

UWallMenuComponent::UWallMenuComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UWallMenuComponent::BeginPlay()
{
	Super::BeginPlay();
	Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()))->OnPlayerSettingsChange.AddUniqueDynamic(this, &UWallMenuComponent::OnPlayerSettingsChanged);
	InitializeSettings(LoadPlayerSettings());
}

FPlayerSettings UWallMenuComponent::LoadPlayerSettings() const
{
	return ISaveLoadInterface::LoadPlayerSettings();
}

void UWallMenuComponent::SavePlayerSettings(const FPlayerSettings PlayerSettingsToSave)
{
	ISaveLoadInterface::SavePlayerSettings(PlayerSettingsToSave);
	Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()))->OnPlayerSettingsChange.Broadcast(PlayerSettingsToSave);
}

void UWallMenuComponent::OnPlayerSettingsChanged(const FPlayerSettings& PlayerSettings)
{
	InitializeSettings(PlayerSettings);
}
