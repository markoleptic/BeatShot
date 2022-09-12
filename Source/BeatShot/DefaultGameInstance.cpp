// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultGameInstance.h"
#include "DefaultPlayerController.h"
#include "DefaultGameMode.h"
#include "GameModeActorBase.h"
#include "Projectile.h"
#include "DefaultCharacter.h"
#include "TargetSpawner.h"
#include "SphereTarget.h"
#include "Kismet/GameplayStatics.h"
#include "SaveGamePlayerSettings.h"


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

// Only used to be able to change target spawn CD from pause menu
void UDefaultGameInstance::SetTargetSpawnCD(float NewTargetSpawnCD)
{
	GameModeActorStruct.TargetSpawnCD = NewTargetSpawnCD;
	if (TargetSpawnerRef)
	{
		TargetSpawnerRef->SetTargetSpawnCD(NewTargetSpawnCD);
	}
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

void UDefaultGameInstance::SaveAudioAnalyzerSettings(FAASettingsStruct AASettingsStructToSave)
{
	AASettingsStruct = AASettingsStructToSave;
}

FAASettingsStruct UDefaultGameInstance::LoadAudioAnalyzerSettings()
{
	return AASettingsStruct;
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
}

void UDefaultGameInstance::LoadPlayerSettings()
{
	if (SaveGamePlayerSettings)
	{
		PlayerSettings = SaveGamePlayerSettings->PlayerSettings;
		UE_LOG(LogTemp, Warning, TEXT("Settings loaded to Game Instance"));
	}
}


