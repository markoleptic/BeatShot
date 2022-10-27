// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomBeat.h"
#include "DefaultGameInstance.h"
#include "TargetSpawner.h"

ACustomBeat::ACustomBeat()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ACustomBeat::BeginPlay()
{
	Super::BeginPlay();
}

void ACustomBeat::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACustomBeat::HandleGameStart()
{
	Super::HandleGameStart();
	GI->TargetSpawnerRef->InitializeGameModeActor(GameModeActorStruct);
	GetWorldTimerManager().SetTimer(GameModeActorStruct.CountDownTimer, this, &::ACustomBeat::StartGameMode, GameModeActorStruct.CountdownTimerLength, false);
}

void ACustomBeat::HandleGameRestart(bool ShouldSavePlayerScores)
{
	Super::HandleGameRestart(ShouldSavePlayerScores);
}

void ACustomBeat::StartGameMode()
{
	Super::StartGameMode();
	GetWorldTimerManager().SetTimer(GameModeActorStruct.GameModeLengthTimer, this, &ACustomBeat::EndGameMode, GameModeActorStruct.GameModeLength, false);
}

void ACustomBeat::EndGameMode()
{
	Super::EndGameMode();
}
