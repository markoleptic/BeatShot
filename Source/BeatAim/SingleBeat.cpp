// Fill out your copyright notice in the Description page of Project Settings.


#include "SingleBeat.h"

ASingleBeat::ASingleBeat()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ASingleBeat::BeginPlay()
{
	Super::BeginPlay();
}

void ASingleBeat::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASingleBeat::HandleGameStart()
{
	Super::HandleGameStart();
}

void ASingleBeat::HandleGameRestart()
{
	Super::HandleGameRestart();
}

void ASingleBeat::StartGameMode()
{
	Super::StartGameMode();
}

void ASingleBeat::EndGameMode()
{
	Super::EndGameMode();
}
