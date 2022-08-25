// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiBeat.h"

AMultiBeat::AMultiBeat()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AMultiBeat::BeginPlay()
{
	Super::BeginPlay();
}

void AMultiBeat::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMultiBeat::HandleGameStart()
{
	Super::HandleGameStart();
}

void AMultiBeat::HandleGameRestart()
{
	Super::HandleGameRestart();
}

void AMultiBeat::StartGameMode()
{
	Super::StartGameMode();
}

void AMultiBeat::EndGameMode()
{
	Super::EndGameMode();
}
