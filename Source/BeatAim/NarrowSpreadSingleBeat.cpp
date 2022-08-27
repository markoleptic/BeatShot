// Fill out your copyright notice in the Description page of Project Settings.

#include "NarrowSpreadSingleBeat.h"
#include "DefaultGameInstance.h"
#include "TargetSpawner.h"

ANarrowSpreadSingleBeat::ANarrowSpreadSingleBeat()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ANarrowSpreadSingleBeat::BeginPlay()
{
	Super::BeginPlay();
}

void ANarrowSpreadSingleBeat::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ANarrowSpreadSingleBeat::HandleGameStart()
{
	Super::HandleGameStart();
	GI->TargetSpawnerRef->InitializeGameModeActor(GameModeActorStruct);
	GetWorldTimerManager().SetTimer(GameModeActorStruct.CountDownTimer, this, &ANarrowSpreadSingleBeat::StartGameMode, GameModeActorStruct.CountdownTimerLength, false);
}

void ANarrowSpreadSingleBeat::HandleGameRestart()
{
	Super::HandleGameRestart();
}

void ANarrowSpreadSingleBeat::StartGameMode()
{
	Super::StartGameMode();
	GetWorldTimerManager().SetTimer(GameModeActorStruct.GameModeLengthTimer, this, &ANarrowSpreadSingleBeat::EndGameMode, GameModeActorStruct.GameModeLength, false);

}

void ANarrowSpreadSingleBeat::EndGameMode()
{
	Super::EndGameMode();
}
