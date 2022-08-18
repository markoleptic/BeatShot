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

// Sets default values
AGameModeActorBase::AGameModeActorBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//Default class specific values
	PrimaryActorTick.bCanEverTick = true;
	GameModeSelected = false;
	CountdownTimerLength = 3.f;
}

// Called when the game starts or when spawned
void AGameModeActorBase::BeginPlay()
{
	Super::BeginPlay();
	// Store instance of GameModeActorBase in Game Instance
	GI = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (GI)
	{
		GI->RegisterGameModeActorBase(this);
	}
	SetGameModeSelected(false);
}

// Called every frame
void AGameModeActorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGameModeActorBase::SetGameModeSelected(bool IsSelected)
{
	GameModeSelected = IsSelected;
}

bool AGameModeActorBase::IsGameModeSelected()
{
	return GameModeSelected;
}

AGameModeActorBase* AGameModeActorBase::GetCurrentGameModeClass()
{
	return this;
}

void AGameModeActorBase::SetCurrentGameModeClass(AGameModeActorBase* GameModeActor)
{
	CurrentGameModeClass = GameModeActor;
}

void AGameModeActorBase::HandleGameStart()
{
	// Reset Struct to zero scores
	PlayerScoreStruct.ResetStruct();
	// load save containing struct to retrieve high score
	GI->DefaultCharacterRef->SetActorLocationAndRotation(StartLocation, StartRotation);
	GI->DefaultCharacterRef->OnShotFired.AddDynamic(this, &AGameModeActorBase::UpdateShotsFired);
	GI->TargetSpawnerRef->OnTargetSpawn.AddDynamic(this, &AGameModeActorBase::UpdateTargetsSpawned);
}

void AGameModeActorBase::StartGameMode()
{
	LoadGame();
}

void AGameModeActorBase::EndGameMode()
{
	SaveGame();

	// Stopping Player and Tracker
	StopAAPlayerAndTracker();

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
	GetWorldTimerManager().ClearAllTimersForObject(this);

	//Hide PlayerHUD
	GI->DefaultPlayerControllerRef->HidePlayerHUD();
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
				UE_LOG(LogTemp, Warning, TEXT("PlayerHighScoreValue:"), SavedPlayerScoreStruct.HighScore);
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
	//TODO: theres probably a lot more stuff i can move from blueprint to here, maybe using the struct
	GI->DefaultCharacterRef->SetActorLocationAndRotation(StartLocation, StartRotation);
	GI->TargetSpawnerRef->OnTargetSpawn.RemoveAll(this);
	GI->DefaultCharacterRef->OnShotFired.RemoveAll(this);
	EndGameMode();
	StartGameMode();
}

