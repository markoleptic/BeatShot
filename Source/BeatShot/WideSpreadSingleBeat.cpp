// Fill out your copyright notice in the Description page of Project Settings.


#include "WideSpreadSingleBeat.h"
#include "DefaultGameInstance.h"
#include "TargetSpawner.h"

AWideSpreadSingleBeat::AWideSpreadSingleBeat()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AWideSpreadSingleBeat::BeginPlay()
{
	Super::BeginPlay();
}

void AWideSpreadSingleBeat::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWideSpreadSingleBeat::HandleGameStart()
{
	Super::HandleGameStart();
	
	GI->TargetSpawnerRef->InitializeGameModeActor(GameModeActorStruct);
	GetWorldTimerManager().SetTimer(GameModeActorStruct.CountDownTimer, this, &AWideSpreadSingleBeat::StartGameMode, GameModeActorStruct.CountdownTimerLength, false);
}

void AWideSpreadSingleBeat::HandleGameRestart(bool ShouldSavePlayerScores)
{
	Super::HandleGameRestart(ShouldSavePlayerScores);
}

void AWideSpreadSingleBeat::StartGameMode()
{
	Super::StartGameMode();
	GetWorldTimerManager().SetTimer(GameModeActorStruct.GameModeLengthTimer, this, &AWideSpreadSingleBeat::EndGameMode, GameModeActorStruct.GameModeLength, false);
}

void AWideSpreadSingleBeat::EndGameMode()
{
	Super::EndGameMode();
}

