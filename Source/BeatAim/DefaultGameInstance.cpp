// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultGameInstance.h"
#include "DefaultCharacter.h"
#include "TargetSpawner.h"
#include "SphereTarget.h"

void UDefaultGameInstance::RegisterDefaultCharacter(ADefaultCharacter* DefaultCharacter)
{
	DefaultCharacterRef = DefaultCharacter;
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

void UDefaultGameInstance::IncrementTargetsSpawned()
{
	TargetsSpawned++;
}

void UDefaultGameInstance::IncrementShotsFired()
{
	ShotsFired++;
}

void UDefaultGameInstance::IncrementTargetsHit()
{
	TargetsHit++;
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
