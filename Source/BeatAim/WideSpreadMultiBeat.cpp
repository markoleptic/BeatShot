// Fill out your copyright notice in the Description page of Project Settings.


#include "WideSpreadMultiBeat.h"
#include "DefaultGameInstance.h"
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

//void AWideSpreadMultiBeat::SetCurrentGameModeClass(AGameModeActorBase* GameModeActor)
//{
//
//}

void AWideSpreadMultiBeat::HandleGameStart()
{
	Super::HandleGameStart();
	SetGameModeSelected(true);
	GetWorldTimerManager().SetTimer(CountDownTimer, this, &AWideSpreadMultiBeat::StartGameMode, CountdownTimerLength, false);
}

void AWideSpreadMultiBeat::HandleGameRestart()
{
	Super::HandleGameRestart();
}

void AWideSpreadMultiBeat::StartGameMode()
{
	Super::StartGameMode();
	if (IsGameModeSelected() == true)
	{
		GI->TargetSpawnerRef->SetShouldSpawn(true);
		GetWorldTimerManager().SetTimer(GameModeLengthTimer, this, &AWideSpreadMultiBeat::EndGameMode, GameModeLength, false);
	}
}

void AWideSpreadMultiBeat::EndGameMode()
{
	Super::EndGameMode();
}
