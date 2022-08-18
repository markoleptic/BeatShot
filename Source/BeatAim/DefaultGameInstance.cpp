// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultGameInstance.h"
#include "DefaultPlayerController.h"
#include "BeatAimGameModeBase.h"
#include "GameModeActorBase.h"
#include "Projectile.h"
#include "DefaultCharacter.h"
#include "TargetSpawner.h"
#include "SphereTarget.h"

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
	if (TargetSpawnerRef)
	{
		TargetSpawnerRef->SetTargetSpawnCD(GetTargetSpawnCD());
	}
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
	Sensitivity = InputSensitivity;
	if (DefaultCharacterRef)
	{
		DefaultCharacterRef->SetSensitivity(InputSensitivity);
	}
}

float UDefaultGameInstance::GetSensitivity()
{
	return Sensitivity;
}

void UDefaultGameInstance::SetTargetSpawnCD(float NewTargetSpawnCD)
{
	TargetSpawnCD = NewTargetSpawnCD;
	if (TargetSpawnerRef)
	{
		TargetSpawnerRef->SetTargetSpawnCD(NewTargetSpawnCD);
	}
}

float UDefaultGameInstance::GetTargetSpawnCD()
{
	return TargetSpawnCD;
}



