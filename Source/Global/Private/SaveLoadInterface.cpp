﻿#include "SaveLoadInterface.h"
#include "SaveGamePlayerScore.h"
#include "Kismet/GameplayStatics.h"

FAASettingsStruct ISaveLoadInterface::LoadAASettings() const
{
	USaveGamePlayerSettings* SaveGamePlayerSettings;
	if (UGameplayStatics::DoesSaveGameExist(TEXT("SettingsSlot"), 0))
	{
		SaveGamePlayerSettings = Cast<USaveGamePlayerSettings>(UGameplayStatics::LoadGameFromSlot(TEXT("SettingsSlot"), 0));
	}
	else
	{
		SaveGamePlayerSettings = Cast<USaveGamePlayerSettings>(UGameplayStatics::CreateSaveGameObject(USaveGamePlayerSettings::StaticClass()));
	}
	return SaveGamePlayerSettings->AASettings;
}

void ISaveLoadInterface::SaveAASettings(const FAASettingsStruct AASettingsToSave)
{
	if (USaveGamePlayerSettings* SaveGameObject = Cast<USaveGamePlayerSettings>(UGameplayStatics::CreateSaveGameObject(USaveGamePlayerSettings::StaticClass())))
	{
		SaveGameObject->AASettings = AASettingsToSave;
		UGameplayStatics::SaveGameToSlot(SaveGameObject, TEXT("SettingsSlot"), 0);
	}
}

FPlayerSettings ISaveLoadInterface::LoadPlayerSettings() const
{
	USaveGamePlayerSettings* SaveGamePlayerSettings;
	if (UGameplayStatics::DoesSaveGameExist(TEXT("SettingsSlot"), 0))
	{
		SaveGamePlayerSettings = Cast<USaveGamePlayerSettings>(UGameplayStatics::LoadGameFromSlot(TEXT("SettingsSlot"), 0));
	}
	else
	{
		SaveGamePlayerSettings = Cast<USaveGamePlayerSettings>(UGameplayStatics::CreateSaveGameObject(USaveGamePlayerSettings::StaticClass()));
	}
	return SaveGamePlayerSettings->PlayerSettings;
}

void ISaveLoadInterface::SavePlayerSettings(const FPlayerSettings PlayerSettingsToSave)
{
	if (USaveGamePlayerSettings* SaveGameObject = Cast<USaveGamePlayerSettings>(UGameplayStatics::CreateSaveGameObject(USaveGamePlayerSettings::StaticClass())))
	{
		SaveGameObject->PlayerSettings = PlayerSettingsToSave;
		UGameplayStatics::SaveGameToSlot(SaveGameObject, TEXT("SettingsSlot"), 0);
	}
}

void ISaveLoadInterface::SaveCustomGameMode(const TArray<FGameModeActorStruct> GameModeArrayToSave)
{
	if (USaveGameCustomGameMode* SaveCustomGameModeObject = Cast<USaveGameCustomGameMode>(
	UGameplayStatics::CreateSaveGameObject(USaveGameCustomGameMode::StaticClass())))
	{
		SaveCustomGameModeObject->CustomGameModes = GameModeArrayToSave;
		UGameplayStatics::SaveGameToSlot(SaveCustomGameModeObject, TEXT("CustomGameModesSlot"), 3);
	}
}

TArray<FGameModeActorStruct> ISaveLoadInterface::LoadCustomGameModes() const
{
	USaveGameCustomGameMode* SaveGameCustomGameMode;
	if (UGameplayStatics::DoesSaveGameExist(TEXT("CustomGameModesSlot"), 3))
	{
		SaveGameCustomGameMode = Cast<USaveGameCustomGameMode>(
			UGameplayStatics::LoadGameFromSlot(TEXT("CustomGameModesSlot"), 3));
	}
	else
	{
		SaveGameCustomGameMode = Cast<USaveGameCustomGameMode>(
			UGameplayStatics::CreateSaveGameObject(USaveGameCustomGameMode::StaticClass()));
	}
	if (SaveGameCustomGameMode)
	{
		return SaveGameCustomGameMode->CustomGameModes;
	}
	return TArray<FGameModeActorStruct>();
}

TArray<FPlayerScore> ISaveLoadInterface::LoadPlayerScores() const
{
	USaveGamePlayerScore* SaveGamePlayerScore;
	if (UGameplayStatics::DoesSaveGameExist(TEXT("ScoreSlot"), 1))
	{
		SaveGamePlayerScore = Cast<USaveGamePlayerScore>(UGameplayStatics::LoadGameFromSlot(TEXT("ScoreSlot"), 1));
		return SaveGamePlayerScore->PlayerScoreArray;
	}
	SaveGamePlayerScore = Cast<USaveGamePlayerScore>(UGameplayStatics::CreateSaveGameObject(USaveGamePlayerScore::StaticClass()));
	return SaveGamePlayerScore->PlayerScoreArray;
}

void ISaveLoadInterface::SavePlayerScores(const TArray<FPlayerScore> PlayerScoreArrayToSave)
{
	if (USaveGamePlayerScore* SaveGamePlayerScores = Cast<USaveGamePlayerScore>(UGameplayStatics::CreateSaveGameObject(USaveGamePlayerScore::StaticClass())))
	{
		SaveGamePlayerScores->PlayerScoreArray = PlayerScoreArrayToSave;
		if (UGameplayStatics::SaveGameToSlot(SaveGamePlayerScores, TEXT("ScoreSlot"), 1))
		{
			UE_LOG(LogTemp, Warning, TEXT("SavePlayerScores Succeeded"));
		}
	}
}