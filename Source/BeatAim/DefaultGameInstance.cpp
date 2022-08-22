// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultGameInstance.h"
#include "DefaultPlayerController.h"
#include "BeatAimGameModeBase.h"
#include "GameModeActorBase.h"
#include "Projectile.h"
#include "DefaultCharacter.h"
#include "TargetSpawner.h"
#include "SphereTarget.h"
#include "Kismet/GameplayStatics.h"
#include "DefaultStatSaveGame.h"


void UDefaultGameInstance::Init()
{
	PlayerSettings = FPlayerSettings();
	GameModeActorStruct = FGameModeActorStruct();
	LoadSettings();
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

void UDefaultGameInstance::RegisterGameModeBase(ABeatAimGameModeBase* GameModeBase)
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
	SaveSettings();
}

float UDefaultGameInstance::GetSensitivity()
{
	return PlayerSettings.Sensitivity;
}

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
	SaveSettings();
}

float UDefaultGameInstance::GetMasterVolume()
{
	return PlayerSettings.MasterVolume;
}

void UDefaultGameInstance::SetMenuVolume(float InputVolume)
{
	PlayerSettings.MenuVolume = InputVolume;
	SaveSettings();
}

float UDefaultGameInstance::GetMenuVolume()
{
	return PlayerSettings.MenuVolume;
}

void UDefaultGameInstance::SetMusicVolume(float InputVolume)
{
	PlayerSettings.MusicVolume = InputVolume;
	SaveSettings();
}

float UDefaultGameInstance::GetMusicVolume()
{
	return PlayerSettings.MusicVolume;
}

void UDefaultGameInstance::SaveSettings()
{
	if (UDefaultStatSaveGame* SaveGameInstance = Cast<UDefaultStatSaveGame>(UGameplayStatics::CreateSaveGameObject(UDefaultStatSaveGame::StaticClass())))
	{
		SaveGameInstance->SaveSettings(PlayerSettings);
		if (UGameplayStatics::SaveGameToSlot(SaveGameInstance, TEXT("MySlot"), 0))
		{
			UE_LOG(LogTemp, Display, TEXT("Save Game Succeeded."));
		}
	}
}

void UDefaultGameInstance::LoadSettings()
{
	if (UDefaultStatSaveGame* SaveGameInstance = Cast<UDefaultStatSaveGame>(UGameplayStatics::LoadGameFromSlot("MySlot", 0)))
	{
		PlayerSettings = SaveGameInstance->LoadSettings();
	}
}


