// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuGameModeBase.h"
#include "DefaultGameInstance.h"
#include "Kismet/GameplayStatics.h"


void AMainMenuGameModeBase::BeginPlay()
{
	GameModeActorName = EGameModeActorName::None;
}
