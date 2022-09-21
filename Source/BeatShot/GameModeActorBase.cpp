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
#include "Gun_AK47.h"

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
}

void AGameModeActorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGameModeActorBase::HandleGameStart()
{
	LoadPlayerScores();
	if (GameModeActorStruct.IsBeatTrackMode == true)
	{
		PlayerScores.IsBeatTrackMode = true;
		PlayerScores.TotalPossibleDamage = 0.f;
	}

	// Binding delegates for scoring purposes
	GI->DefaultCharacterRef->Gun->OnShotFired.AddDynamic(this, &AGameModeActorBase::UpdateShotsFired);
	GI->TargetSpawnerRef->OnTargetSpawn.AddDynamic(this, &AGameModeActorBase::UpdateTargetsSpawned);

	MaxScorePerTarget = 100000.f / ((GameModeActorStruct.GameModeLength - 1.f) / GameModeActorStruct.TargetSpawnCD);
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

	// Reset Struct to zero scores
	PlayerScores.ResetStruct();

	// Show Post Game menu
	GI->DefaultPlayerControllerRef->ShowPostGameMenu();

	Destroy();
}

void AGameModeActorBase::UpdatePlayerScores(float TimeElapsed)
{
	if (GameModeActorStruct.IsBeatTrackMode == false)
	{
		if (TimeElapsed <= GameModeActorStruct.PlayerDelay - 0.05f)
		{
			PlayerScores.Score += FMath::Lerp(MaxScorePerTarget / 2, MaxScorePerTarget, TimeElapsed / GameModeActorStruct.PlayerDelay);
		}
		else if (TimeElapsed <= GameModeActorStruct.PlayerDelay + 0.05f)
		{
			PlayerScores.Score += MaxScorePerTarget;
		}
		else if (TimeElapsed <= GameModeActorStruct.TargetMaxLifeSpan)
		{
			PlayerScores.Score += FMath::Lerp(MaxScorePerTarget, MaxScorePerTarget / 2, (TimeElapsed - GameModeActorStruct.PlayerDelay + 0.05f) / (GameModeActorStruct.TargetMaxLifeSpan - (GameModeActorStruct.PlayerDelay + 0.05f)));
		}
		UpdateHighScore();
		UpdateScoresToHUD.Broadcast(PlayerScores);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Error: trying to call UpdateScore() with BeatTrackMode"));
	}
}

void AGameModeActorBase::UpdateTrackingScore(float DamageTaken, float TotalPossibleDamage)
{
	if (GameModeActorStruct.IsBeatTrackMode==true)
	{
		PlayerScores.IsBeatTrackMode = true;
		PlayerScores.TotalPossibleDamage = TotalPossibleDamage;
		PlayerScores.Score += DamageTaken;
		UpdateHighScore();
		UpdateScoresToHUD.Broadcast(PlayerScores);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Error: trying to call UpdateTrackingScore() with non BeatTrackMode"));
	}
}

void AGameModeActorBase::UpdateTargetsSpawned()
{
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
	PlayerScores.Time = FDateTime::Now();
	PlayerScoreArrayWrapper.PlayerScoreArray.Add(PlayerScores);
	PlayerScoreMap.Remove(GameModeActorStruct);
	PlayerScoreMap.Compact();
	PlayerScoreMap.Shrink();
	PlayerScoreMap.Add(GameModeActorStruct, PlayerScoreArrayWrapper);
	GI->SavePlayerScores(PlayerScoreMap);

	// Printing saved scores to console

	// iterate through all elements in PlayerScoreMap
	for (TTuple<FGameModeActorStruct, FPlayerScoreArrayWrapper>& Elem : PlayerScoreMap)
	{
		// get array of player scores from current key value
		TArray<FPlayerScore> TempArray = Elem.Value.PlayerScoreArray;
		
		float HighScore = 0.f;
		// iterate through array of player scores to find high score for current game mode / song
		for (FPlayerScore& PlayerScoreObject : TempArray)
		{
			if (PlayerScoreObject.HighScore > HighScore)
			{
				HighScore = PlayerScoreObject.HighScore;
			}
		}
	UE_LOG(LogTemp, Display, TEXT("Enum: %s, Name: %s, SongName: %s, SongLength: %f, Highscore: %f"),
		*UEnum::GetValueAsString(Elem.Key.GameModeActorName),
		*Elem.Key.CustomGameModeName,
		*Elem.Key.SongTitle,
		Elem.Key.GameModeLength,
		HighScore);
	}
}

void AGameModeActorBase::LoadPlayerScores()
{
	PlayerScoreMap = GI->LoadPlayerScores();

	if (PlayerScoreMap.Contains(GameModeActorStruct))
	{
		UE_LOG(LogTemp, Warning, TEXT("Existing GameMode Found"));
	}

	// iterate through all elements in PlayerScoreMap
	for (TTuple<FGameModeActorStruct, FPlayerScoreArrayWrapper>& Elem : PlayerScoreMap)
	{
		// find matching Elem.Key for GameModeActorName or CustomGameModeName
		if ((Elem.Key.GameModeActorName == GameModeActorStruct.GameModeActorName &&
			Elem.Key.GameModeActorName != EGameModeActorName::Custom)||
			(Elem.Key.GameModeActorName == EGameModeActorName::Custom &&
				Elem.Key.CustomGameModeName == GameModeActorStruct.CustomGameModeName)) 
		{
			// find matching Elem.Key for Song
			for (FPlayerScore& PlayerScoreObject : Elem.Value.PlayerScoreArray)
			{
				if (PlayerScoreObject.HighScore > PlayerScores.HighScore)
				{
					SavedPlayerScores = PlayerScoreObject;
					PlayerScores.HighScore = PlayerScoreObject.HighScore;
				}
			}
		}
	}
	// save entire PlayerScoreArray(Wrapper) so we can add a new Score Struct to Array at end of game mode
	PlayerScoreArrayWrapper = PlayerScoreMap.FindRef(GameModeActorStruct);
	PlayerScores.GameModeActorName = GameModeActorStruct.GameModeActorName;
	PlayerScores.SongTitle = GameModeActorStruct.SongTitle;
	PlayerScores.SongLength = GameModeActorStruct.GameModeLength;
	PlayerScores.CustomGameModeName = GameModeActorStruct.CustomGameModeName;
	PlayerScores.TotalPossibleDamage = 0.f;
}

void AGameModeActorBase::HandleGameRestart()
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

	// Reset Struct to zero scores
	PlayerScores.ResetStruct();

	Destroy();
}

