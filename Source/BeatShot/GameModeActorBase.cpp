// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModeActorBase.h"
#include "DefaultCharacter.h"
#include "SphereTarget.h"
#include "TargetSpawner.h"
#include "DefaultGameMode.h"
#include "DefaultGameInstance.h"
#include "DefaultPlayerController.h"
#include "FloatingTextActor.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/DateTime.h"
#include "Engine/World.h"
#include "Gun_AK47.h"
#include "Kismet/KismetTextLibrary.h"

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
	// register GameModeActorBase as a reference in GI
	GI->RegisterGameModeActorBase(this);
	const FPlayerSettings PlayerSettings = GI->LoadPlayerSettings();
	bShowStreakCombatText = PlayerSettings.bShowStreakCombatText;
	CombatTextFrequency = PlayerSettings.CombatTextFrequency;
	GI->OnPlayerSettingsChange.AddDynamic(this, &AGameModeActorBase::OnPlayerSettingsChange);
}

void AGameModeActorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGameModeActorBase::InitializeGameModeActor()
{
	LoadPlayerScores();
	if (GameModeActorStruct.IsBeatTrackMode == true)
	{
		PlayerScores.TotalPossibleDamage = 0.f;
	}
	MaxScorePerTarget = 100000.f / ((GameModeActorStruct.GameModeLength - 1.f) / GameModeActorStruct.TargetSpawnCD);

	// Binding delegates for scoring purposes
	Cast<AGun_AK47>(GI->DefaultCharacterRef->GunActorComp->GetChildActor())->OnShotFired.AddDynamic(this, &AGameModeActorBase::UpdateShotsFired);
	GI->TargetSpawnerRef->OnTargetSpawn.AddDynamic(this, &AGameModeActorBase::UpdateTargetsSpawned);
	GI->TargetSpawnerRef->OnStreakUpdate.AddDynamic(this, &AGameModeActorBase::OnStreakUpdate);
}

void AGameModeActorBase::StartGameMode()
{
	InitializeGameModeActor();
	GetWorldTimerManager().SetTimer(GameModeLengthTimer, this, &AGameModeActorBase::OnGameModeLengthTimerComplete, GameModeActorStruct.GameModeLength, false);
	GI->TargetSpawnerRef->SetShouldSpawn(true);
	UpdateScoresToHUD.Broadcast(PlayerScores);
}

void AGameModeActorBase::EndGameMode(const bool ShouldSavePlayerScores)
{
	if (ShouldSavePlayerScores)
	{
		SavePlayerScores();
	}

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
		GI->SphereTargetArray.Empty();
		GI->SphereTargetArray.Shrink();
	}

	//Clearing Timers
	GetWorldTimerManager().ClearTimer(GameModeLengthTimer);
	GameModeLengthTimer.Invalidate();

	// Reset Struct to zero scores
	PlayerScores.ResetStruct();

	Destroy();
}

void AGameModeActorBase::OnGameModeLengthTimerComplete() const
{
	// Show Post Game menu
	// don't save scores if score is zero
	if (PlayerScores.Score <= 0 || 
		(PlayerScores.GameModeActorName == EGameModeActorName::Custom &&
			PlayerScores.CustomGameModeName == ""))
	{
		GI->DefaultPlayerControllerRef->ShowPostGameMenu(false);
	}
	else
	{
		GI->DefaultPlayerControllerRef->ShowPostGameMenu(true);
	}
	Cast<ADefaultGameMode>(GI->GameModeBaseRef)->EndGameMode(true);
}

void AGameModeActorBase::OnStreakUpdate(const int32 Streak, const FVector Location)
{
	// Only update best streak in PlayerScores and HUD
	if (Streak > PlayerScores.Streak)
	{
		PlayerScores.Streak = Streak;
		UpdateScoresToHUD.Broadcast(PlayerScores);
	}
	if (bShowStreakCombatText)
	{
		if (CombatTextFrequency!= 0 && Streak % CombatTextFrequency == 0)
		{
			if (AFloatingTextActor* FloatingTextActor = GetWorld()->SpawnActor<AFloatingTextActor>(FloatingTextActorToSpawn, Location, FRotator()))
			{
				FloatingTextActor->Initialize(UKismetTextLibrary::Conv_IntToText(Streak));
			}
		}
	}
}

void AGameModeActorBase::UpdatePlayerScores(const float TimeElapsed)
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
			//UE_LOG(LogTemp, Display, TEXT("Last: %f"), FMath::Lerp(MaxScorePerTarget, MaxScorePerTarget / 2, (TimeElapsed - GameModeActorStruct.PlayerDelay + 0.05f) / (GameModeActorStruct.TargetMaxLifeSpan - (GameModeActorStruct.PlayerDelay + 0.05f))))
		}
		UpdateHighScore();
		PlayerScores.TotalTimeOffset += FMath::Abs(TimeElapsed - GameModeActorStruct.PlayerDelay);
		UpdateScoresToHUD.Broadcast(PlayerScores);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Error: trying to call UpdateScore() with BeatTrackMode"));
	}
}

void AGameModeActorBase::UpdateTrackingScore(const float DamageTaken, const float TotalPossibleDamage)
{
	if (GameModeActorStruct.IsBeatTrackMode==true)
	{
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

void AGameModeActorBase::OnPlayerSettingsChange(const FPlayerSettings PlayerSettings)
{
	bShowStreakCombatText = PlayerSettings.bShowStreakCombatText;
	CombatTextFrequency = PlayerSettings.CombatTextFrequency;
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
	// don't save scores if score is zero
	if (PlayerScores.Score <= 0 || 
		(PlayerScores.GameModeActorName == EGameModeActorName::Custom &&
			PlayerScores.CustomGameModeName == ""))
	{
		return;
	}

	// save current time
	PlayerScores.Time = FDateTime::UtcNow().ToIso8601();

	// for BeatTrack modes
	if (PlayerScores.TotalPossibleDamage > 0.01f)
	{
		PlayerScores.Accuracy = FMath::RoundHalfToZero(100.0 * PlayerScores.Score / PlayerScores.TotalPossibleDamage) / 100.0;
		PlayerScores.Completion = FMath::RoundHalfToZero(100.0 * PlayerScores.Score / PlayerScores.TotalPossibleDamage) / 100.0;
	}
	else
	{
		PlayerScores.AvgTimeOffset = FMath::RoundHalfToZero(1000.0 * (PlayerScores.TotalTimeOffset / PlayerScores.TargetsHit)) / 1000.0;
		PlayerScores.Accuracy = FMath::RoundHalfToZero(100.0 * PlayerScores.TargetsHit / PlayerScores.ShotsFired) / 100.0;
		PlayerScores.Completion = FMath::RoundHalfToZero(100.0 * PlayerScores.TargetsHit / PlayerScores.TargetsSpawned) / 100.0;
	}
	PlayerScores.HighScore = FMath::RoundHalfToZero(100.0 * PlayerScores.HighScore) / 100.0;
	PlayerScores.Score = FMath::RoundHalfToZero(100.0 * PlayerScores.Score) / 100.0;
	PlayerScores.SongLength = FMath::RoundHalfToZero(100.0 * PlayerScores.SongLength) / 100.0;
	PlayerScoreArrayWrapper.PlayerScoreArray.Add(PlayerScores);

	// just in case ScoreMap comparison is working
	if (PlayerScoreMap.Contains(GameModeActorStruct))
	{
		PlayerScoreMap.Remove(GameModeActorStruct);
		PlayerScoreMap.Compact();
		PlayerScoreMap.Shrink();
		UE_LOG(LogTemp, Display, TEXT("An existing gamemodeactorstruct was found and replaced"));
	}
	PlayerScoreMap.Add(GameModeActorStruct, PlayerScoreArrayWrapper);
	GI->SavePlayerScores(PlayerScoreMap, true);

	// Printing saved scores to console
	for (const TTuple<FGameModeActorStruct, FPlayerScoreArrayWrapper>& Elem : PlayerScoreMap)
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
			UE_LOG(LogTemp, Display, TEXT("Score for %s %s %s: %f High Score: %f"),
				*PlayerScoreObject.SongTitle,
				*UEnum::GetValueAsString(Elem.Key.GameModeActorName),
				*PlayerScoreObject.CustomGameModeName,
				PlayerScoreObject.Score,
				HighScore);
		}
	}
}

void AGameModeActorBase::LoadPlayerScores()
{
	PlayerScoreMap = GI->LoadPlayerScores();

	// iterate through all elements in PlayerScoreMap
	for (TTuple<FGameModeActorStruct, FPlayerScoreArrayWrapper>& Elem : PlayerScoreMap)
	{
		// find matching GameModeActorName or CustomGameModeName
		if ((Elem.Key.GameModeActorName == GameModeActorStruct.GameModeActorName &&
			Elem.Key.GameModeActorName != EGameModeActorName::Custom &&
			Elem.Key.GameModeDifficulty == GameModeActorStruct.GameModeDifficulty) ||
			(Elem.Key.GameModeActorName == EGameModeActorName::Custom &&
				Elem.Key.CustomGameModeName == GameModeActorStruct.CustomGameModeName)) 
		{
			// find matching Song
			for (const FPlayerScore& PlayerScoreObject : Elem.Value.PlayerScoreArray)
			{
				if (PlayerScoreObject.SongTitle.Equals(GameModeActorStruct.SongTitle))
				{
					// get high score for PlayerHUD
					if (PlayerScores.HighScore < PlayerScoreObject.HighScore)
					{
						PlayerScores.HighScore = PlayerScoreObject.HighScore;
					}
				}
			}
		}
	}
	PlayerScores.GameModeActorName = GameModeActorStruct.GameModeActorName;
	PlayerScores.SongTitle = GameModeActorStruct.SongTitle;
	PlayerScores.SongLength = GameModeActorStruct.GameModeLength;
	PlayerScores.CustomGameModeName = GameModeActorStruct.CustomGameModeName;
	PlayerScores.Difficulty = GameModeActorStruct.GameModeDifficulty;
	PlayerScores.TotalPossibleDamage = 0.f;

	// just in case ScoreMap comparison is working
	if (PlayerScoreMap.Contains(GameModeActorStruct))
	{
		PlayerScoreArrayWrapper = PlayerScoreMap.FindRef(GameModeActorStruct);
	}
}
