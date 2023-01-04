// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModeActorBase.h"
#include "DefaultCharacter.h"
#include "DefaultGameMode.h"
#include "DefaultGameInstance.h"
#include "DefaultHealthComponent.h"
#include "FloatingTextActor.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/DateTime.h"
#include "Engine/World.h"
#include "Gun_AK47.h"
#include "SphereTarget.h"
#include "Kismet/KismetTextLibrary.h"

AGameModeActorBase::AGameModeActorBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AGameModeActorBase::BeginPlay()
{
	Super::BeginPlay();
	UDefaultGameInstance* GI = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	const FPlayerSettings PlayerSettings = GI->LoadPlayerSettings();
	GI->RegisterGameModeActorBase(this);
	bShowStreakCombatText = PlayerSettings.bShowStreakCombatText;
	CombatTextFrequency = PlayerSettings.CombatTextFrequency;
	GI->OnPlayerSettingsChange.AddDynamic(this, &AGameModeActorBase::OnPlayerSettingsChange);
	GameMode = Cast<ADefaultGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
}

void AGameModeActorBase::Destroyed()
{
	/** Unbinding delegates */
	UDefaultGameInstance* GI = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	GI->OnPlayerSettingsChange.RemoveDynamic(this, &AGameModeActorBase::OnPlayerSettingsChange);
	GI->DefaultCharacterRef->Gun->OnShotFired.RemoveDynamic(this, &AGameModeActorBase::UpdateShotsFired);
	GameMode->OnBeatTrackTargetSpawned.RemoveDynamic(this, &AGameModeActorBase::OnBeatTrackTargetSpawned);
	GameMode->OnStreakUpdate.RemoveDynamic(this, &AGameModeActorBase::OnStreakUpdate);
	if (GameMode->OnTargetSpawned.IsBoundToObject(this))
	{
		GameMode->OnTargetSpawned.Unbind();
	}
	if (GameMode->OnTargetDestroyed.IsBoundToObject(this))
	{
		GameMode->OnTargetDestroyed.Unbind();
	}
	Super::Destroyed();
}

void AGameModeActorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGameModeActorBase::StartGameMode()
{
	InitializeGameModeActor();
	GetWorldTimerManager().SetTimer(GameModeLengthTimer, this, &AGameModeActorBase::OnGameModeLengthTimerComplete,
	                                GameModeActorStruct.GameModeLength, false);
	if (!GameMode->UpdateScoresToHUD.ExecuteIfBound(CurrentPlayerScore))
	{
		UE_LOG(LogTemp, Display, TEXT("Initial UpdateScoresToHUD failed."));
	}
}

void AGameModeActorBase::InitializeGameModeActor()
{
	LoadPlayerScores();
	
	/** Binding delegates */
	Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()))->DefaultCharacterRef->Gun->
		OnShotFired.AddDynamic(this, &AGameModeActorBase::UpdateShotsFired);
	GameMode->OnTargetSpawned.BindUFunction(this, FName("UpdateTargetsSpawned"));
	GameMode->OnTargetDestroyed.BindUFunction(this, FName("UpdatePlayerScores"));
	GameMode->OnStreakUpdate.AddUniqueDynamic(this, &AGameModeActorBase::OnStreakUpdate);
	if (GameModeActorStruct.IsBeatTrackMode)
	{
		Cast<ADefaultGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->OnBeatTrackTargetSpawned.AddUniqueDynamic(
			this, &AGameModeActorBase::OnBeatTrackTargetSpawned);
	}
}

void AGameModeActorBase::OnGameModeLengthTimerComplete()
{
	/** don't save scores if score is zero */
	if (CurrentPlayerScore.Score <= 0 ||
		(CurrentPlayerScore.GameModeActorName == EGameModeActorName::Custom &&
			CurrentPlayerScore.CustomGameModeName == ""))
	{
		UE_LOG(LogTemp, Display, TEXT("Shouldn't save player scores"));
		GameMode->EndGameMode(false, true);
		return;
	}
	GameMode->EndGameMode(true, true);
}

void AGameModeActorBase::OnBeatTrackTargetSpawned(ASphereTarget* TrackingTarget)
{
	/** Update tracking score if a Tracking target has spawned */
	TrackingTarget->HealthComp->OnBeatTrackTick.BindUFunction(this, FName("UpdateTrackingScore"));
}

void AGameModeActorBase::UpdatePlayerScores(const float TimeElapsed)
{
	if (GameModeActorStruct.IsBeatTrackMode == true)
	{
		UE_LOG(LogTemp, Warning, TEXT("Error: trying to call UpdateScore() with BeatTrackMode"));
		return;
	}
	UpdateTargetsHit();
	if (TimeElapsed <= GameModeActorStruct.PlayerDelay - 0.05f)
	{
		CurrentPlayerScore.Score += FMath::Lerp(MaxScorePerTarget / 2, MaxScorePerTarget,
		                                        TimeElapsed / GameModeActorStruct.PlayerDelay);
	}
	else if (TimeElapsed <= GameModeActorStruct.PlayerDelay + 0.05f)
	{
		CurrentPlayerScore.Score += MaxScorePerTarget;
	}
	else if (TimeElapsed <= GameModeActorStruct.TargetMaxLifeSpan)
	{
		CurrentPlayerScore.Score += FMath::Lerp(MaxScorePerTarget, MaxScorePerTarget / 2,
		                                        (TimeElapsed - GameModeActorStruct.PlayerDelay + 0.05f) / (
			                                        GameModeActorStruct.TargetMaxLifeSpan - (GameModeActorStruct.
				                                        PlayerDelay + 0.05f)));
		// UE_LOG(LogTemp, Display, TEXT("Last: %f"), FMath::Lerp(MaxScorePerTarget, MaxScorePerTarget / 2,
		// (TimeElapsed - GameModeActorStruct.PlayerDelay + 0.05f) /
		// (GameModeActorStruct.TargetMaxLifeSpan - (GameModeActorStruct.PlayerDelay + 0.05f))))
	}
	UpdateHighScore();
	CurrentPlayerScore.TotalTimeOffset += FMath::Abs(TimeElapsed - GameModeActorStruct.PlayerDelay);
	if (!GameMode->UpdateScoresToHUD.ExecuteIfBound(CurrentPlayerScore))
	{
		UE_LOG(LogTemp, Display, TEXT("UpdateScoresToHUD not bound."));
	}
}

void AGameModeActorBase::UpdateTrackingScore(const float DamageTaken, const float TotalPossibleDamage)
{
	CurrentPlayerScore.TotalPossibleDamage = TotalPossibleDamage;
	CurrentPlayerScore.Score += DamageTaken;
	UpdateHighScore();
	if (!GameMode->UpdateScoresToHUD.ExecuteIfBound(CurrentPlayerScore))
	{
		UE_LOG(LogTemp, Display, TEXT("UpdateScoresToHUD not bound."));
	}
}

void AGameModeActorBase::UpdateTargetsSpawned()
{
	CurrentPlayerScore.TargetsSpawned++;
	if (!GameMode->UpdateScoresToHUD.ExecuteIfBound(CurrentPlayerScore))
	{
		UE_LOG(LogTemp, Display, TEXT("UpdateScoresToHUD not bound."));
	}
}

void AGameModeActorBase::UpdateShotsFired()
{
	CurrentPlayerScore.ShotsFired++;
	if (!GameMode->UpdateScoresToHUD.ExecuteIfBound(CurrentPlayerScore))
	{
		UE_LOG(LogTemp, Display, TEXT("UpdateScoresToHUD not bound."));
	}
}

void AGameModeActorBase::OnStreakUpdate(const int32 Streak, const FVector Location)
{
	/** Only update best streak in PlayerScores and HUD */
	if (Streak > CurrentPlayerScore.Streak)
	{
		CurrentPlayerScore.Streak = Streak;
		if (!GameMode->UpdateScoresToHUD.ExecuteIfBound(CurrentPlayerScore))
		{
			UE_LOG(LogTemp, Display, TEXT("UpdateScoresToHUD not bound."));
		}
	}
	if (bShowStreakCombatText)
	{
		if (CombatTextFrequency != 0 && Streak % CombatTextFrequency == 0)
		{
			if (AFloatingTextActor* FloatingTextActor = GetWorld()->SpawnActor<AFloatingTextActor>(
				FloatingTextActorToSpawn, Location, FRotator()))
			{
				FloatingTextActor->Initialize(UKismetTextLibrary::Conv_IntToText(Streak));
			}
		}
	}
}

void AGameModeActorBase::UpdateTargetsHit()
{
	CurrentPlayerScore.TargetsHit++;
}

void AGameModeActorBase::UpdateHighScore()
{
	if (CurrentPlayerScore.Score > CurrentPlayerScore.HighScore)
	{
		CurrentPlayerScore.HighScore = CurrentPlayerScore.Score;
	}
}

void AGameModeActorBase::LoadPlayerScores()
{
	const TArray<FPlayerScore> PlayerScores = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()))
		->LoadPlayerScores();
	if (GameModeActorStruct.GameModeActorName == EGameModeActorName::Custom)
	{
		for (FPlayerScore ScoreObject : PlayerScores)
		{
			if (ScoreObject.CustomGameModeName.Equals(GameModeActorStruct.CustomGameModeName) &&
				ScoreObject.SongTitle.Equals(GameModeActorStruct.SongTitle))
			{
				if (ScoreObject.Score > CurrentPlayerScore.HighScore)
				{
					CurrentPlayerScore.HighScore = ScoreObject.Score;
				}
			}
		}
	}
	else
	{
		for (FPlayerScore ScoreObject : PlayerScores)
		{
			if (ScoreObject.GameModeActorName == GameModeActorStruct.GameModeActorName &&
				ScoreObject.SongTitle.Equals(GameModeActorStruct.SongTitle) &&
				ScoreObject.Difficulty == GameModeActorStruct.GameModeDifficulty)
			{
				if (ScoreObject.Score > CurrentPlayerScore.HighScore)
				{
					CurrentPlayerScore.HighScore = ScoreObject.Score;
				}
			}
		}
	}
	CurrentPlayerScore.GameModeActorName = GameModeActorStruct.GameModeActorName;
	CurrentPlayerScore.SongTitle = GameModeActorStruct.SongTitle;
	CurrentPlayerScore.SongLength = GameModeActorStruct.GameModeLength;
	CurrentPlayerScore.CustomGameModeName = GameModeActorStruct.CustomGameModeName;
	CurrentPlayerScore.Difficulty = GameModeActorStruct.GameModeDifficulty;
	CurrentPlayerScore.TotalPossibleDamage = 0.f;
	MaxScorePerTarget = 100000.f / ((GameModeActorStruct.GameModeLength - 1.f) / GameModeActorStruct.TargetSpawnCD);
}

void AGameModeActorBase::SavePlayerScores()
{
	/** don't save scores if score is zero */
	if (CurrentPlayerScore.Score <= 0 ||
		(CurrentPlayerScore.GameModeActorName == EGameModeActorName::Custom &&
			CurrentPlayerScore.CustomGameModeName == ""))
	{
		return;
	}

	/** save current time */
	CurrentPlayerScore.Time = FDateTime::UtcNow().ToIso8601();

	/** for BeatTrack modes */
	if (CurrentPlayerScore.TotalPossibleDamage > 0.01f)
	{
		CurrentPlayerScore.Accuracy = CheckFloatNaN(CurrentPlayerScore.Score / CurrentPlayerScore.TotalPossibleDamage, 100);
		CurrentPlayerScore.Completion = CheckFloatNaN(CurrentPlayerScore.Score / CurrentPlayerScore.TotalPossibleDamage, 100);
	}
	else
	{
		CurrentPlayerScore.AvgTimeOffset = CheckFloatNaN(CurrentPlayerScore.TotalTimeOffset / CurrentPlayerScore.TargetsHit, 1000);
		CurrentPlayerScore.Accuracy = CheckFloatNaN(CurrentPlayerScore.TargetsHit / CurrentPlayerScore.ShotsFired, 100);
		CurrentPlayerScore.Completion = CheckFloatNaN(CurrentPlayerScore.TargetsHit / CurrentPlayerScore.TargetsSpawned, 100);
	}
	CurrentPlayerScore.HighScore = CheckFloatNaN(CurrentPlayerScore.HighScore, 100);
	CurrentPlayerScore.Score = CheckFloatNaN( CurrentPlayerScore.Score, 100);
	CurrentPlayerScore.SongLength = CheckFloatNaN(CurrentPlayerScore.SongLength, 100);

	UDefaultGameInstance* GI = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	GI->SavePlayerScores(CurrentPlayerScore, true);

	UE_LOG(LogTemp, Display, TEXT("Accuracy: %f \n Completion: %f \n AvgTimeOffset: %f \n HighScore: %f \n Score: %f \n SongLength: %f"),
		CurrentPlayerScore.Accuracy, CurrentPlayerScore.Completion, CurrentPlayerScore.AvgTimeOffset, CurrentPlayerScore.HighScore,
		CurrentPlayerScore.Score, CurrentPlayerScore.SongLength);
}

void AGameModeActorBase::OnPlayerSettingsChange(const FPlayerSettings& PlayerSettings)
{
	bShowStreakCombatText = PlayerSettings.bShowStreakCombatText;
	CombatTextFrequency = PlayerSettings.CombatTextFrequency;
}

float AGameModeActorBase::CheckFloatNaN(const float ValueToCheck, const float ValueToRound)
{
	if (!isnan(roundf(ValueToRound * ValueToCheck) / ValueToRound))
	{
		return roundf(ValueToRound * ValueToCheck) / ValueToRound;
	}
	return 0;
}
