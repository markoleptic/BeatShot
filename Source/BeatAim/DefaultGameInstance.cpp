// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultGameInstance.h"
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

void UDefaultGameInstance::UpdateTargetsSpawned(bool doReset)
{
	if (doReset == false)
	{
		TargetsSpawned++;
	}
	else if (doReset == true)
	{
		TargetsSpawned = 0.f;
	}
}

void UDefaultGameInstance::UpdateShotsFired(bool doReset)
{
	if (doReset == false)
	{
		ShotsFired++;
	}
	else if (doReset == true)
	{
		ShotsFired = 0.f;
	}
}

void UDefaultGameInstance::UpdateTargetsHit(bool doReset)
{
	if (doReset == false)
	{
		TargetsHit++;
	}
	else if (doReset == true)
	{
		TargetsHit = 0.f;
	}
}

void UDefaultGameInstance::UpdateScore(float ScoreToAdd, bool doReset)
{
	if (doReset == false)
	{
		Score += ScoreToAdd;
	}
	else if (doReset == true)
	{
		Score = 0.f;
	}
}

void UDefaultGameInstance::UpdateHighScore(float HighScoreToUpdate)
{
	HighScore = HighScoreToUpdate;
}

float UDefaultGameInstance::GetTargetsHit()
{
	return TargetsHit;
}

float UDefaultGameInstance::GetShotsFired()
{
	return ShotsFired;
}

float UDefaultGameInstance::GetTargetsSpawned()
{
	return TargetsSpawned;
}

float UDefaultGameInstance::GetScore()
{
	return Score;
}

float UDefaultGameInstance::GetHighScore()
{
	return HighScore;
}

void UDefaultGameInstance::SetTargetsHit(float SavedTargetsHit)
{
	TargetsHit = SavedTargetsHit;
}

void UDefaultGameInstance::SetShotsFired(float SavedShotsFired)
{
	ShotsFired = SavedShotsFired;
}

void UDefaultGameInstance::SetTargetsSpawned(float SavedTargetsSpawned)
{
	TargetsSpawned = SavedTargetsSpawned;
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



