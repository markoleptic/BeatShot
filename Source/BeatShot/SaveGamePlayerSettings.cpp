// Fill out your copyright notice in the Description page of Project Settings.

#include "SaveGamePlayerSettings.h"
#include "GameModeActorStruct.h"

USaveGamePlayerSettings::USaveGamePlayerSettings()
{
}

void USaveGamePlayerSettings::SaveSettings(FPlayerSettings PlayerSettingsToSave)
{
	PlayerSettings = PlayerSettingsToSave;
	UE_LOG(LogTemp, Warning, TEXT("Settings saved to USaveGamePlayerSettings object."));
}

FPlayerSettings USaveGamePlayerSettings::LoadSettings()
{
	UE_LOG(LogTemp, Warning, TEXT("Settings loaded from USaveGamePlayerSettings object."));
	return PlayerSettings;
}



