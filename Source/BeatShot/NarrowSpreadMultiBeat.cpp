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
}

void ANarrowSpreadMultiBeat::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ANarrowSpreadMultiBeat::HandleGameStart()
{
	Super::HandleGameStart();
	GI->TargetSpawnerRef->InitializeGameModeActor(GameModeActorStruct);
	GetWorldTimerManager().SetTimer(GameModeActorStruct.CountDownTimer, this, &ANarrowSpreadMultiBeat::StartGameMode, GameModeActorStruct.CountdownTimerLength, false);
}

void ANarrowSpreadMultiBeat::HandleGameRestart(bool ShouldSavePlayerScores)
{
	Super::HandleGameRestart(ShouldSavePlayerScores);
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
