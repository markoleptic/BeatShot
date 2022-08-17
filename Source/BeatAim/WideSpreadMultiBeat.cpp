// Fill out your copyright notice in the Description page of Project Settings.


#include "WideSpreadMultiBeat.h"
#include "DefaultGameInstance.h"
#include "TargetSpawner.h"
#include "SphereTarget.h"
#include "DefaultCharacter.h"
#include "PlayerHUD.h"
#include "EngineUtils.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "BeatAimGameModeBase.h"

AWideSpreadMultiBeat::AWideSpreadMultiBeat()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AWideSpreadMultiBeat::BeginPlay()
{
	Super::BeginPlay();
	GI->RegisterGameModeActorBase(this);
	HandleGameStart();
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
	SetGameModeSelected(true);
	//ResetPlayerStats();
	//GI->DefaultCharacterRef->LoadGame();
	//GI->DefaultCharacterRef->ShowPlayerHUD(true);
	//GI->DefaultCharacterRef->ShowCountdown();
	GetWorldTimerManager().SetTimer(CountDownTimer, this, &AWideSpreadMultiBeat::StartGameMode, 3.f, false);
}

void AWideSpreadMultiBeat::HandleGameRestart()
{
	EndGameMode();
	StartGameMode();
}

void AWideSpreadMultiBeat::StartGameMode()
{
	if (IsGameModeSelected() == true)
	{
		GI->TargetSpawnerRef->SetShouldSpawn(true);
		GetWorldTimerManager().SetTimer(GameModeLengthTimer, this, &AWideSpreadMultiBeat::EndGameMode, GameModeLength, false);
	}
}

void AWideSpreadMultiBeat::EndGameMode()
{
	//Updating Scoring
	if (GI->GetScore() > GI->GetHighScore())
	{
		GI->UpdateHighScore(GI->GetScore());
	}

	//Deleting Targets
	GI->TargetSpawnerRef->SetShouldSpawn(false);
	StopAAPlayerAndTracker();
	if (GI->SphereTargetArray.Num() > 0)
	{
		for (ASphereTarget* Target : GI->SphereTargetArray)
		{
			if (Target)
			{
				Target->HandleDestruction();
			}
		}
	}

	//Clearing Timers
	GetWorldTimerManager().ClearAllTimersForObject(this);

	//Saving Score
	GI->DefaultCharacterRef->SaveGame();

	//Update HUD
	GI->DefaultCharacterRef->ShowPlayerHUD(false);
}
