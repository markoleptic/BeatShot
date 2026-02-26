// Copyright 2022-2025 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BSGameModeConfig/DefiningConfig.h"
#include "Score/AccuracyRow.h"
#include "PlayerScore.generated.h"

/** Used to load and save player scores. */
USTRUCT(BlueprintType)
struct BEATSHOTGLOBAL_API FPlayerScore
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BeatShot|DefiningProperties")
	FBS_DefiningConfig DefiningConfig;

	/** The song title. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BeatShot|DefiningProperties")
	FString SongTitle;

	/** Length of song. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BeatShot|GameProperties")
	float SongLength;

	/** The current score at any given time during play. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BeatShot|PlayerScore")
	float Score;

	/** Only represents highest score based on previous entries, and may become outdated. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BeatShot|PlayerScore")
	float HighScore;

	/** Total Targets hit divided by Total shots fired. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BeatShot|PlayerScore")
	float Accuracy;

	/** Total Targets hit divided by Total targets spawned. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BeatShot|PlayerScore")
	float Completion;

	/** Incremented after receiving calls from FOnShotsFired delegate in DefaultCharacter. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BeatShot|PlayerScore")
	int32 ShotsFired;

	/** Total number of targets destroyed by player. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BeatShot|PlayerScore")
	int32 TargetsHit;

	/** Total number of targets spawned, incremented after receiving calls from FOnTargetSpawnSignature in
	 *  TargetManager. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BeatShot|PlayerScore")
	int32 TargetsSpawned;

	/** Total possible damage that could have been done to tracking target, also used to determine if the score object
	 *  is for Tracking game mode. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BeatShot|PlayerScore")
	float TotalPossibleDamage;

	/** Total time offset from Spawn Beat Delay for all destroyed targets. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BeatShot|PlayerScore")
	float TotalTimeOffset;

	/** Avg Time offset from Spawn Beat Delay for destroyed targets. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BeatShot|PlayerScore")
	float AvgTimeOffset;

	/** time that player completed the session, in Iso8601 UTC format. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BeatShot|PlayerScore")
	FString Time;

	/** The maximum consecutive targets hit in a row. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BeatShot|PlayerScore")
	int32 Streak;

	/** The accuracy at each point in the grid. */
	UPROPERTY()
	TArray<FAccuracyRow> LocationAccuracy;

	/** whether this instance has been saved to the database yet. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BeatShot|PlayerScore")
	bool bSavedToDatabase;

	FPlayerScore()
	{
		DefiningConfig = FBS_DefiningConfig();
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
		LocationAccuracy = TArray<FAccuracyRow>();
		bSavedToDatabase = false;
	}

	/** Verifies that the score is greater than zero and that if the mode is custom, that the Custom Game Mode Name
	 *  is not blank. */
	bool IsValidToSave() const
	{
		if (Score <= 0.f)
		{
			return false;
		}
		if (DefiningConfig.GameModeType == EGameModeType::Custom && DefiningConfig.CustomGameModeName.IsEmpty())
		{
			return false;
		}
		return true;
	}

	FORCEINLINE bool HasMatchingDefiningConfigAndSong(const FPlayerScore& Other) const
	{
		if (DefiningConfig == Other.DefiningConfig && SongTitle.Equals(Other.SongTitle))
		{
			return true;
		}
		return false;
	}
};
