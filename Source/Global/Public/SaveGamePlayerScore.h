// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SaveGameCustomGameMode.h"
#include "GameFramework/SaveGame.h"
#include "SaveGamePlayerScore.generated.h"

/* Why aren't 2d arrays a thing */
USTRUCT()
struct F2DArray
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<float> Accuracy;
};

/* Used to load and save player scores */
USTRUCT(BlueprintType)
struct FPlayerScore
{
	GENERATED_BODY()

	/* The default game mode name, or custom if custom */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Defining Properties")
	EDefaultMode DefaultMode;

	/* Custom game mode name if custom, otherwise empty string */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Defining Properties")
	FString CustomGameModeName;

	/* Default game mode difficulties, or none if custom */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Defining Properties")
	EGameModeDifficulty Difficulty;

	/* The song title */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Defining Properties")
	FString SongTitle;

	/* Length of song */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Properties")
	float SongLength;

	/* The current score at any given time during play */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
	float Score;

	/* Only represents highest score based on previous entries, and may become outdated */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
	float HighScore;

	/* Total Targets hit divided by Total shots fired */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
	float Accuracy;

	/* Total Targets hit divided by Total targets spawned */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
	float Completion;

	/* Incremented after receiving calls from FOnShotsFired delegate in DefaultCharacter */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
	int32 ShotsFired;

	/* Total number of targets destroyed by player */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
	int32 TargetsHit;

	/* Total number of targets spawned, incremented after receiving calls from FOnTargetSpawnSignature in TargetSpawner */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
	int32 TargetsSpawned;

	/* Total possible damage that could have been done to tracking target, also used to determine if the score object is for Tracking game mode */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
	float TotalPossibleDamage;

	/* Total time offset from Spawn Beat Delay for all destroyed targets */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
	float TotalTimeOffset;

	/* Avg Time offset from Spawn Beat Delay for destroyed targets */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
	float AvgTimeOffset;

	/* time that player completed the session, in Iso8601 UTC format */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
	FString Time;

	/* The maximum consecutive targets hit in a row */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
	int32 Streak;

	/* The accuracy at each point in the grid */
	UPROPERTY()
	TArray<F2DArray> LocationAccuracy;

	/* Whether or not this instance has been saved to the database yet */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
	bool bSavedToDatabase;

	FPlayerScore()
	{
		DefaultMode = EDefaultMode::Custom;
		Difficulty = EGameModeDifficulty::None;
		CustomGameModeName = "";
		SongTitle = "";
		SongLength = 0.f;
		Score = 0;
		HighScore = 0;
		Completion = 0;
		Accuracy = 0;
		ShotsFired = 0;
		TargetsHit = 0;
		TotalTimeOffset = 0;
		AvgTimeOffset = 0;
		TargetsSpawned = 0;
		TotalPossibleDamage = 0.f;
		Streak = 0;
		bSavedToDatabase = false;
	}

	void ResetStruct()
	{
		DefaultMode = EDefaultMode::Custom;
		CustomGameModeName = "";
		Difficulty = EGameModeDifficulty::None;
		SongTitle = "";
		SongLength = 0.f;
		Score = 0;
		HighScore = 0;
		Accuracy = 0;
		Completion = 0;
		ShotsFired = 0;
		TargetsHit = 0;
		AvgTimeOffset = 0;
		TargetsSpawned = 0;
		TotalPossibleDamage = 0.f;
		Streak = 0;
		bSavedToDatabase = false;
	}
};

/* Used to convert PlayerScoreArray to database scores */
USTRUCT(BlueprintType)
struct FJsonScore
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FPlayerScore> Scores;
};

UCLASS()
class GLOBAL_API USaveGamePlayerScore : public USaveGame
{
	GENERATED_BODY()

public:
	/* Array containing all saved score instances */
	UPROPERTY()
	TArray<FPlayerScore> PlayerScoreArray;
};
