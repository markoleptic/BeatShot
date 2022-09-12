// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModeActorBase.h"
#include "DefaultCharacter.h"
#include "PlayerHUD.h"
#include "SphereTarget.h"
#include "TargetSpawner.h"
#include "DefaultGameInstance.h"
#include "DefaultGameMode.h"
#include "Blueprint/UserWidget.h"
#include "DefaultPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "SaveGamePlayerScore.h"

class USaveGamePlayerScore;

AGameModeActorBase::AGameModeActorBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AGameModeActorBase::BeginPlay()
{
	Super::BeginPlay();

	// Store instance of GameModeActorBase in Game Instance
	GI = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (GI)
	{
		GI->RegisterGameModeActorBase(this);
		// Retrieve GameMode data storage object from Game Instance
		GameModeActorStruct = GI->GameModeActorStruct;
	}

	if (UGameplayStatics::DoesSaveGameExist(TEXT("ScoreSlot"), 1))
	{
		SaveGamePlayerScore = Cast<USaveGamePlayerScore>(UGameplayStatics::LoadGameFromSlot(TEXT("ScoreSlot"), 1));
	}
	else
	{
		SaveGamePlayerScore = Cast<USaveGamePlayerScore>(UGameplayStatics::CreateSaveGameObject(USaveGamePlayerScore::StaticClass()));
	}
}

void AGameModeActorBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	SavePlayerScores();
}

void AGameModeActorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGameModeActorBase::HandleGameStart()
{
	LoadPlayerScores();

	if (GameModeActorStruct.GameModeActorName == EGameModeActorName::BeatTrack)
	{
		PlayerScores.IsBeatTrackMode = true;
	}

	// Binding delegates for scoring purposes
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
	GI->TargetSpawnerRef->SetShouldSpawn(true);
	UpdateScoresToHUD.Broadcast(PlayerScores);
}

void AGameModeActorBase::EndGameMode()
{
	SavePlayerScores();

	// Stopping Player and Tracker
	Cast<ADefaultGameMode>(GI->GameModeBaseRef)->StopAAPlayerAndTracker();

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

void AGameModeActorBase::UpdatePlayerScores(float TimeElapsed)
{
	if (TimeElapsed <= 0.5f)
	{
		PlayerScores.Score += FMath::Lerp(400.f, 1000.f, TimeElapsed / 0.5f);
	}
	else if (TimeElapsed <= 1.f)
	{
		PlayerScores.Score += FMath::Lerp(1000.f, 400.f, (TimeElapsed - 0.5f) / 0.5f);
	}
	else {
		PlayerScores.Score += 400;
	}
	UpdateHighScore();
	UpdateScoresToHUD.Broadcast(PlayerScores);
}

void AGameModeActorBase::UpdateTargetsSpawned()
{
	//maybe only include targets that have time outed or been shot
	PlayerScores.TargetsSpawned++;
	UpdateScoresToHUD.Broadcast(PlayerScores);
}

void AGameModeActorBase::UpdateShotsFired()
{
	PlayerScores.ShotsFired++;
	UpdateScoresToHUD.Broadcast(PlayerScores);
}

void AGameModeActorBase::UpdateTargetsHit()
{
	PlayerScores.TargetsHit++;
	UpdateScoresToHUD.Broadcast(PlayerScores);
}

void AGameModeActorBase::UpdateHighScore()
{
	if (PlayerScores.Score > PlayerScores.HighScore)
	{
		PlayerScores.HighScore = PlayerScores.Score;
	}
}

void AGameModeActorBase::SavePlayerScores()
{
	if (USaveGamePlayerScore* SaveGamePlayerScores = Cast<USaveGamePlayerScore>(UGameplayStatics::CreateSaveGameObject(USaveGamePlayerScore::StaticClass())))
	{
		if (PlayerScores.HighScore > SavedPlayerScores.HighScore)
		{
			SavedPlayerScores.HighScore = PlayerScores.HighScore;
		}
		PlayerScoreMap.Add(GameModeActorStruct, PlayerScores);
		SaveGamePlayerScores->PlayerScoreMap = PlayerScoreMap;

		if (UGameplayStatics::SaveGameToSlot(SaveGamePlayerScores, TEXT("ScoreSlot"), 1))
		{
			UE_LOG(LogTemp, Warning, TEXT("SavePlayerScores Succeeded"));
		}
	}
}

void AGameModeActorBase::LoadPlayerScores()
{
	if (SaveGamePlayerScore)
	{
		PlayerScoreMap = SaveGamePlayerScore->PlayerScoreMap;
		UE_LOG(LogTemp, Warning, TEXT("PlayerScores loaded to Game Instance"));

		SavedPlayerScores = PlayerScoreMap.FindRef(GameModeActorStruct);
		PlayerScores.GameModeActorName = GameModeActorStruct.GameModeActorName;
		if (SavedPlayerScores.HighScore > PlayerScores.HighScore)
		{
			PlayerScores.HighScore = SavedPlayerScores.HighScore;
		}
		
		//if (PlayerScoreArray.Num() > 0)
		//{
		//	for (FPlayerScore SavedPlayerScores : PlayerScoreArray)
		//	{
		//		UE_LOG(LogTemp, Display, TEXT("Score %f"), SavedPlayerScores.HighScore);
		//		if (SavedPlayerScores.HighScore > PlayerScores.HighScore)
		//		{
		//			PlayerScores.HighScore = SavedPlayerScores.HighScore;
		//		}
		//	}
		//}
	}
}

void AGameModeActorBase::HandleGameRestart()
{
	// Reset Struct to zero scores
	PlayerScores.ResetStruct();

	EndGameMode();
}

