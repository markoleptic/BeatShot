// Fill out your copyright notice in the Description page of Project Settings.

#include "DefaultStatSaveGame.h"

UDefaultStatSaveGame::UDefaultStatSaveGame()
{
}

void UDefaultStatSaveGame::InsertToPlayerScoreStructArray(FPlayerScore PlayerScoreStructToAdd)
{
	ArrayOfPlayerScoreStructs.Add(PlayerScoreStructToAdd);
}

TArray<FPlayerScore> UDefaultStatSaveGame::GetArrayOfPlayerScoreStructs()
{

	return ArrayOfPlayerScoreStructs;
}

void UDefaultStatSaveGame::SaveSettings(FPlayerSettings PlayerSettingsToSave)
{
	SavedPlayerSettings = PlayerSettingsToSave;
	UE_LOG(LogTemp, Warning, TEXT("Settings saved to UDefaultStatSaveGame object."));
}

FPlayerSettings UDefaultStatSaveGame::LoadSettings()
{
	UE_LOG(LogTemp, Warning, TEXT("Settings loaded from UDefaultStatSaveGame object."));
	return SavedPlayerSettings;
}



