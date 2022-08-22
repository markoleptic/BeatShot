// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModeActorBase.h"
#include "DefaultCharacter.h"
#include "PlayerHUD.h"
#include "SphereTarget.h"
#include "TargetSpawner.h"
#include "DefaultGameInstance.h"
#include "DefaultStatSaveGame.h"
#include "Blueprint/UserWidget.h"
#include "DefaultPlayerController.h"
#include "Kismet/GameplayStatics.h"

AGameModeActorBase::AGameModeActorBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//Default class specific values
	PrimaryActorTick.bCanEverTick = true;
	GameModeActorStruct = FGameModeActorStruct();
}

void AGameModeActorBase::BeginPlay()
{
	Super::BeginPlay();

	// Store instance of GameModeActorBase in Game Instance
	GI = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (GI)
	{
		GI->RegisterGameModeActorBase(this);
	}
}

void AGameModeActorBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	SaveGame();
}

void AGameModeActorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGameModeActorBase::HandleGameStart()
{
	// Reset Struct to zero scores
	PlayerScoreStruct.ResetStruct();

	// load save containing struct to retrieve high score
	LoadGame();

	//GI->DefaultCharacterRef->SetActorLocationAndRotation(StartLocation, StartRotation);
	if (GI->DefaultCharacterRef->OnShotFired.IsBound() == false)
	{
		GI->DefaultCharacterRef->OnShotFired.AddDynamic(this, &AGameModeActorBase::UpdateShotsFired);
	}
	if (GI->TargetSpawnerRef->OnTargetSpawn.IsBound() == false)
	{
		GI->TargetSpawnerRef->OnTargetSpawn.AddDynamic(this, &AGameModeActorBase::UpdateTargetsSpawned);
	}
}

void AGameModeActorBase::StartGameMode()
{
	LoadGame();
	GI->TargetSpawnerRef->SetShouldSpawn(true);
}

void AGameModeActorBase::EndGameMode()
{
	SaveGame();

	// Stopping Player and Tracker
	GI->GameModeBaseRef->StopAAPlayerAndTracker();

	// Deleting Targets
	GI->TargetSpawnerRef->SetShouldSpawn(false);
	if (GI->SphereTargetArray.Num() > 0)
	{
		for (ASphereTarget* Target : GI->SphereTargetArray)
		{
			if (Target)
			{
				Target->Destroy();
			}
		}
	}

	//Clearing Timers
	GameModeActorStruct.CountDownTimer.Invalidate();
	GameModeActorStruct.GameModeLengthTimer.Invalidate();
	GameModeActorStruct.CountdownTimerLength = 3.f;

	//Hide HUD and countdown
	GI->DefaultPlayerControllerRef->HidePlayerHUD();
	GI->DefaultPlayerControllerRef->HideCountdown();
}

// Called by SphereTarget when it takes damage
void AGameModeActorBase::UpdateScore(float TimeElapsed)
{
	if (TimeElapsed <= 0.5f)
	{
		PlayerScoreStruct.Score += FMath::Lerp(400.f, 1000.f, TimeElapsed / 0.5f);
	}
	else if (TimeElapsed <= 1.f)
	{
		PlayerScoreStruct.Score += FMath::Lerp(1000.f, 400.f, (TimeElapsed - 0.5f) / 0.5f);
	}
	else {
		PlayerScoreStruct.Score += 400;
	}
	UpdateHighScore();
	UpdateScoresToHUD.Broadcast(PlayerScoreStruct);
}

// Called by TargetSpawner when a SphereTarget is spawned
void AGameModeActorBase::UpdateTargetsSpawned()
{
	//maybe only include targets that have time outed or been shot
	PlayerScoreStruct.TargetsSpawned++;
	UpdateScoresToHUD.Broadcast(PlayerScoreStruct);
}

// Called by DefaultCharacter when player shoots during an active game
void AGameModeActorBase::UpdateShotsFired()
{
	PlayerScoreStruct.ShotsFired++;
	UpdateScoresToHUD.Broadcast(PlayerScoreStruct);
}

// Called by Projectile when a Player's projectile hits a SphereTarget
void AGameModeActorBase::UpdateTargetsHit()
{
	PlayerScoreStruct.TargetsHit++;
	UpdateScoresToHUD.Broadcast(PlayerScoreStruct);
}

void AGameModeActorBase::UpdateHighScore()
{
	if (PlayerScoreStruct.Score > PlayerScoreStruct.HighScore)
	{
		PlayerScoreStruct.HighScore = PlayerScoreStruct.Score;
	}
}

void AGameModeActorBase::SaveGame()
{
	if (UDefaultStatSaveGame* SaveGameInstance = Cast<UDefaultStatSaveGame>(UGameplayStatics::CreateSaveGameObject(UDefaultStatSaveGame::StaticClass())))
	{
		SaveGameInstance->InsertToPlayerScoreStructArray(PlayerScoreStruct);
		if (UGameplayStatics::SaveGameToSlot(SaveGameInstance, TEXT("MySlot"), 0))
		{
			UE_LOG(LogTemp, Display, TEXT("Save Game Succeeded."));
		}
	}
}

void AGameModeActorBase::LoadGame()
{
	if (UDefaultStatSaveGame* SaveGameInstance = Cast<UDefaultStatSaveGame>(UGameplayStatics::LoadGameFromSlot("MySlot", 0)))
	{
		TArray<FPlayerScore> ArrayOfPlayerScoreStructs = SaveGameInstance->GetArrayOfPlayerScoreStructs();
		if (ArrayOfPlayerScoreStructs.Num() > 0)
		{
			for (FPlayerScore SavedPlayerScoreStruct : ArrayOfPlayerScoreStructs)
			{
				if (SavedPlayerScoreStruct.HighScore > PlayerScoreStruct.HighScore)
				{
					PlayerScoreStruct.HighScore = SavedPlayerScoreStruct.HighScore;
				}
			}
		}
	}
}

void AGameModeActorBase::HandleGameRestart()
{
	EndGameMode();
}

