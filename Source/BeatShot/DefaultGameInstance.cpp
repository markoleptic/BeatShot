// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultGameInstance.h"
#include "DefaultPlayerController.h"
#include "DefaultGameMode.h"
#include "GameModeActorBase.h"
#include "DefaultCharacter.h"
#include "TargetSpawner.h"
#include "SphereTarget.h"
#include "Kismet/GameplayStatics.h"
#include "SaveGamePlayerSettings.h"
#include "SaveGameAASettings.h"
#include "SaveGamePlayerScore.h"


void UDefaultGameInstance::Init()
{
	if (UGameplayStatics::DoesSaveGameExist(TEXT("SettingsSlot"), 0))
	{
		SaveGamePlayerSettings = Cast<USaveGamePlayerSettings>(UGameplayStatics::LoadGameFromSlot(TEXT("SettingsSlot"), 0));
	}
	else
	{
		SaveGamePlayerSettings = Cast<USaveGamePlayerSettings>(UGameplayStatics::CreateSaveGameObject(USaveGamePlayerSettings::StaticClass()));
	}

	LoadPlayerSettings();
}

void UDefaultGameInstance::RegisterDefaultCharacter(ADefaultCharacter* DefaultCharacter)
{
	DefaultCharacterRef = DefaultCharacter;
	if (DefaultCharacterRef)
	{
		DefaultCharacterRef->SetSensitivity(GetSensitivity());
	}
}

void UDefaultGameInstance::RegisterTargetSpawner(ATargetSpawner* TargetSpawner)
{
	TargetSpawnerRef = TargetSpawner;
}

void UDefaultGameInstance::RegisterSphereTarget(ASphereTarget* SphereTarget)
{
	SphereTargetRef = SphereTarget;
	SphereTargetArray.Add(SphereTarget);
}

void UDefaultGameInstance::RegisterGameModeBase(AGameModeBase* GameModeBase)
{
	GameModeBaseRef = GameModeBase;
}

void UDefaultGameInstance::RegisterGameModeActorBase(AGameModeActorBase* GameModeActorBase)
{
	GameModeActorBaseRef = GameModeActorBase;
}

void UDefaultGameInstance::RegisterPlayerController(ADefaultPlayerController* DefaultPlayerController)
{
	DefaultPlayerControllerRef = DefaultPlayerController;
}

void UDefaultGameInstance::SetSensitivity(float InputSensitivity)
{
	PlayerSettings.Sensitivity = InputSensitivity;
	if (DefaultCharacterRef)
	{
		DefaultCharacterRef->SetSensitivity(InputSensitivity);
	}
}

float UDefaultGameInstance::GetSensitivity()
{
	return PlayerSettings.Sensitivity;
}

float UDefaultGameInstance::GetTargetSpawnCD()
{
	return GameModeActorStruct.TargetSpawnCD;
}

void UDefaultGameInstance::SetMasterVolume(float InputVolume)
{
	PlayerSettings.MasterVolume = InputVolume;
}

float UDefaultGameInstance::GetMasterVolume()
{
	return PlayerSettings.MasterVolume;
}

void UDefaultGameInstance::SetMenuVolume(float InputVolume)
{
	PlayerSettings.MenuVolume = InputVolume;
}

float UDefaultGameInstance::GetMenuVolume()
{
	return PlayerSettings.MenuVolume;
}

void UDefaultGameInstance::SetMusicVolume(float InputVolume)
{
	PlayerSettings.MusicVolume = InputVolume;
}

float UDefaultGameInstance::GetMusicVolume()
{
	return PlayerSettings.MusicVolume;
}

FAASettingsStruct UDefaultGameInstance::LoadAASettings()
{
	if (UGameplayStatics::DoesSaveGameExist(TEXT("AASettingsSlot"), 2))
	{
		SaveGameAASettings = Cast<USaveGameAASettings>(UGameplayStatics::LoadGameFromSlot(TEXT("AASettingsSlot"), 2));
	}
	else
	{
		SaveGameAASettings = Cast<USaveGameAASettings>(UGameplayStatics::CreateSaveGameObject(USaveGameAASettings::StaticClass()));
	}

	if (SaveGameAASettings)
	{
		UE_LOG(LogTemp, Warning, TEXT("AASettings loaded to Game Instance"));
		return SaveGameAASettings->AASettings;
	}
	return FAASettingsStruct();
}

void UDefaultGameInstance::SaveAASettings(FAASettingsStruct AASettingsToSave)
{
	if (USaveGameAASettings* SaveGameAASettingsObject = Cast<USaveGameAASettings>(UGameplayStatics::CreateSaveGameObject(USaveGameAASettings::StaticClass())))
	{
		SaveGameAASettingsObject->AASettings = AASettingsToSave;
		if (UGameplayStatics::SaveGameToSlot(SaveGameAASettingsObject, TEXT("AASettingsSlot"), 2))
		{
			UE_LOG(LogTemp, Warning, TEXT("SaveAASettings Succeeded"));
		}
	}
	OnAASettingsChange.Broadcast();
}

TMap<FGameModeActorStruct, FPlayerScoreArrayWrapper> UDefaultGameInstance::LoadPlayerScores()
{
	if (UGameplayStatics::DoesSaveGameExist(TEXT("ScoreSlot"), 1))
	{
		SaveGamePlayerScore = Cast<USaveGamePlayerScore>(UGameplayStatics::LoadGameFromSlot(TEXT("ScoreSlot"), 1));
		UE_LOG(LogTemp, Display, TEXT("PlayerScores Loaded"));
		return SaveGamePlayerScore->PlayerScoreMap;
	}
	SaveGamePlayerScore = Cast<USaveGamePlayerScore>(UGameplayStatics::CreateSaveGameObject(USaveGamePlayerScore::StaticClass()));
	return SaveGamePlayerScore->PlayerScoreMap;
}

void UDefaultGameInstance::SavePlayerScores(TMap<FGameModeActorStruct, FPlayerScoreArrayWrapper> PlayerScoreMapToSave)
{
	if (USaveGamePlayerScore* SaveGamePlayerScores = Cast<USaveGamePlayerScore>(UGameplayStatics::CreateSaveGameObject(USaveGamePlayerScore::StaticClass())))
	{
		SaveGamePlayerScores->PlayerScoreMap = PlayerScoreMapToSave;

		if (UGameplayStatics::SaveGameToSlot(SaveGamePlayerScores, TEXT("ScoreSlot"), 1))
		{
			UE_LOG(LogTemp, Warning, TEXT("SavePlayerScores Succeeded"));
		}
	}
	OnPlayerScoresChange.Broadcast();
}

void UDefaultGameInstance::SavePlayerSettings()
{
	if (USaveGamePlayerSettings* SaveGameInstanceToSave = Cast<USaveGamePlayerSettings>(UGameplayStatics::CreateSaveGameObject(USaveGamePlayerSettings::StaticClass())))
	{
		SaveGameInstanceToSave->PlayerSettings = PlayerSettings;
		if (UGameplayStatics::SaveGameToSlot(SaveGameInstanceToSave, TEXT("SettingsSlot"), 0))
		{
			UE_LOG(LogTemp, Warning, TEXT("SavePlayerSettings Succeeded"));
		}
	}
	OnPlayerSettingsChange.Broadcast();
}

void UDefaultGameInstance::LoadPlayerSettings()
{
	if (SaveGamePlayerSettings)
	{
		PlayerSettings = SaveGamePlayerSettings->PlayerSettings;
		UE_LOG(LogTemp, Warning, TEXT("Settings loaded to Game Instance"));
	}
}


