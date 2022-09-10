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
#include "DefaultStatSaveGame.h"


void UDefaultGameInstance::Init()
{
	if (UGameplayStatics::DoesSaveGameExist(TEXT("SettingsSlot"), 0))
	{
		SaveSettingsGameInstance = Cast<UDefaultStatSaveGame>(UGameplayStatics::LoadGameFromSlot(TEXT("SettingsSlot"), 0));
	}
	else
	{
		SaveSettingsGameInstance = Cast<UDefaultStatSaveGame>(UGameplayStatics::CreateSaveGameObject(UDefaultStatSaveGame::StaticClass()));
	}

	LoadSettings();

	if (UGameplayStatics::DoesSaveGameExist(TEXT("ScoreSlot"), 1))
	{
		SaveScoreGameInstance = Cast<UDefaultStatSaveGame>(UGameplayStatics::LoadGameFromSlot(TEXT("ScoreSlot"), 1));
	}
	else
	{
		SaveScoreGameInstance = Cast<UDefaultStatSaveGame>(UGameplayStatics::CreateSaveGameObject(UDefaultStatSaveGame::StaticClass()));
	}

	LoadScores();
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

void UDefaultGameInstance::RegisterProjectile(AProjectile* Projectile)
{
	ProjectileRef = Projectile;
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

void UDefaultGameInstance::SaveSettings()
{
	if (UDefaultStatSaveGame* SaveGameInstanceToSave = Cast<UDefaultStatSaveGame>(UGameplayStatics::CreateSaveGameObject(UDefaultStatSaveGame::StaticClass())))
	{
		SaveGameInstanceToSave->SavedPlayerSettings = PlayerSettings;
		if (UGameplayStatics::SaveGameToSlot(SaveGameInstanceToSave, TEXT("SettingsSlot"), 0))
		{
			UE_LOG(LogTemp, Warning, TEXT("SaveSettings Succeeded"));
		}
	}
}

void UDefaultGameInstance::LoadSettings()
{
	if (SaveSettingsGameInstance)
	{
		PlayerSettings = SaveSettingsGameInstance->SavedPlayerSettings;
		UE_LOG(LogTemp, Warning, TEXT("Settings loaded to Game Instance"));
	}
}

void UDefaultGameInstance::SaveScores(FPlayerScore PlayerScoreStructToSave)
{
	if (UDefaultStatSaveGame* SaveGameInstanceToSave = Cast<UDefaultStatSaveGame>(UGameplayStatics::CreateSaveGameObject(UDefaultStatSaveGame::StaticClass())))
	{
		SaveGameInstanceToSave->InsertToPlayerScoreStructArray(PlayerScoreStructToSave);
		if (UGameplayStatics::SaveGameToSlot(SaveGameInstanceToSave, TEXT("ScoreSlot"), 1))
		{
			UE_LOG(LogTemp, Warning, TEXT("SaveScores Succeeded"));
		}
	}
}

void UDefaultGameInstance::LoadScores()
{
	if (SaveScoreGameInstance)
	{
		ArrayOfPlayerScoreStructs = SaveScoreGameInstance->GetArrayOfPlayerScoreStructs();
		UE_LOG(LogTemp, Warning, TEXT("PlayerScores loaded to Game Instance"));
	}
}


