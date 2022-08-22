// Fill out your copyright notice in the Description page of Project Settings.


#include "WideSpreadMultiBeat.h"
#include "DefaultGameInstance.h"
#include "DefaultPlayerController.h"
#include "TargetSpawner.h"

AWideSpreadMultiBeat::AWideSpreadMultiBeat()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AWideSpreadMultiBeat::BeginPlay()
{
	Super::BeginPlay();
	GI->RegisterGameModeActorBase(this);
}

void AWideSpreadMultiBeat::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWideSpreadMultiBeat::HandleGameStart()
{
	Super::HandleGameStart();
	GameModeActorStruct.BoxBounds.X = 0;
	GameModeActorStruct.BoxBounds.Y = 350.f;
	GameModeActorStruct.BoxBounds.Z = 900.f;
	GameModeActorStruct.TargetMaxLifeSpan = 1.5f;
	GameModeActorStruct.MinDistanceBetweenTargets = 100.f;
	GameModeActorStruct.CountdownTimerLength = 3.f;
	GameModeActorStruct.TargetSpawnCD = 0.35f;
	GameModeActorStruct.MinTargetScale = 1.2f;
	GameModeActorStruct.MaxTargetScale = 1.8f;
	GI->TargetSpawnerRef->InitializeGameModeActor(GameModeActorStruct);
	GetWorldTimerManager().SetTimer(GameModeActorStruct.CountDownTimer, this, &AWideSpreadMultiBeat::StartGameMode, GameModeActorStruct.CountdownTimerLength, false);
}

void AWideSpreadMultiBeat::HandleGameRestart()
{
	Super::HandleGameRestart();
}

void AWideSpreadMultiBeat::StartGameMode()
{
	Super::StartGameMode();
	GetWorldTimerManager().SetTimer(GameModeActorStruct.GameModeLengthTimer, this, &AWideSpreadMultiBeat::EndGameMode, GameModeActorStruct.GameModeLength, false);
}

void AWideSpreadMultiBeat::EndGameMode()
{
	Super::EndGameMode();
}
