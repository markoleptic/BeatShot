// Fill out your copyright notice in the Description page of Project Settings.


#include "NarrowSpreadMultiBeat.h"
#include "DefaultGameInstance.h"
#include "TargetSpawner.h"

ANarrowSpreadMultiBeat::ANarrowSpreadMultiBeat()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ANarrowSpreadMultiBeat::BeginPlay()
{
	Super::BeginPlay();
	GI->RegisterGameModeActorBase(this);
}

void ANarrowSpreadMultiBeat::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ANarrowSpreadMultiBeat::HandleGameStart()
{
	Super::HandleGameStart();
	GameModeActorStruct.BoxBounds.X = 0;
	GameModeActorStruct.BoxBounds.Y = 300.f;
	GameModeActorStruct.BoxBounds.Z = 600.f;
	GameModeActorStruct.TargetMaxLifeSpan = 1.5f;
	GameModeActorStruct.MinDistanceBetweenTargets = 100.f;
	CountdownTimerLength = 3.f;
	GameModeActorStruct.TargetSpawnCD = 0.30f;
	GameModeActorStruct.MinTargetScale = 1.2f;
	GameModeActorStruct.MaxTargetScale = 1.8f;
	GI->TargetSpawnerRef->InitializeGameModeActor(GameModeActorStruct);
	GetWorldTimerManager().SetTimer(GameModeActorStruct.CountDownTimer, this, &ANarrowSpreadMultiBeat::StartGameMode, GameModeActorStruct.CountdownTimerLength, false);
}

void ANarrowSpreadMultiBeat::HandleGameRestart()
{
	Super::HandleGameRestart();
}

void ANarrowSpreadMultiBeat::StartGameMode()
{
	Super::StartGameMode();
	GetWorldTimerManager().SetTimer(GameModeActorStruct.GameModeLengthTimer, this, &ANarrowSpreadMultiBeat::EndGameMode, GameModeActorStruct.GameModeLength, false);
}

void ANarrowSpreadMultiBeat::EndGameMode()
{
	Super::EndGameMode();
}
