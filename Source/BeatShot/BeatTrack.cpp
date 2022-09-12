// Fill out your copyright notice in the Description page of Project Settings.


#include "BeatTrack.h"
#include "DefaultGameInstance.h"
#include "TargetSpawner.h"

ABeatTrack::ABeatTrack()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ABeatTrack::BeginPlay()
{
	Super::BeginPlay();
}

void ABeatTrack::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABeatTrack::HandleGameStart()
{
	Super::HandleGameStart();
	GI->TargetSpawnerRef->InitializeGameModeActor(GameModeActorStruct);
	GetWorldTimerManager().SetTimer(GameModeActorStruct.CountDownTimer, this, &ABeatTrack::StartGameMode, GameModeActorStruct.CountdownTimerLength, false);
}

void ABeatTrack::HandleGameRestart()
{
	Super::HandleGameRestart();
}

void ABeatTrack::StartGameMode()
{
	Super::StartGameMode();
	GetWorldTimerManager().SetTimer(GameModeActorStruct.GameModeLengthTimer, this, &ABeatTrack::EndGameMode, GameModeActorStruct.GameModeLength, false);
}

void ABeatTrack::EndGameMode()
{
	Super::EndGameMode();
}

void ABeatTrack::UpdateTrackingScore(float DamageTaken, float TotalPossibleDamage)
{
	PlayerScores.IsBeatTrackMode = true;
	PlayerScores.TotalPossibleDamage = TotalPossibleDamage;
	PlayerScores.Score += DamageTaken;
	UpdateHighScore();
	UpdateScoresToHUD.Broadcast(PlayerScores);
}
