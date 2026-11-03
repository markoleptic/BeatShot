// Copyright 2022-2025 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BSGameModeConfig/DefiningConfig.h"
#include "Score/AccuracyRow.h"
#include "Utilities/SongMetadata.h"
#include "PlayerScore.generated.h"

struct FBS_AudioConfig;

/** Used to load and save player scores. */
USTRUCT(BlueprintType)
struct BEATSHOTGLOBAL_API FPlayerScore
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BeatShot|DefiningProperties")
	FBS_DefiningConfig DefiningConfig;

	/** @deprecated The song title. */
	UPROPERTY(meta = (DeprecatedProperty, DeprecationMessage = "Use SongMetadata.Title instead."))
	FString SongTitle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BeatShot|DefiningProperties")
	FSongMetadata SongMetadata;

	/** @deprecated Length of song. */
	UPROPERTY(meta = (DeprecatedProperty, DeprecationMessage = "Use SongMetadata.Length instead."))
	float SongLength = 0.f;

	/** The current score at any given time during play. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BeatShot|PlayerScore")
	float Score = 0.f;

	/** Only represents highest score based on previous entries, and may become outdated. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BeatShot|PlayerScore")
	float HighScore = 0.f;

	/** Total Targets hit divided by Total shots fired. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BeatShot|PlayerScore")
	float Accuracy = 0.f;

	/** Total Targets hit divided by Total targets spawned. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BeatShot|PlayerScore")
	float Completion = 0.f;

	/** Incremented after receiving calls from FOnShotsFired delegate in DefaultCharacter. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BeatShot|PlayerScore")
	int32 ShotsFired = 0;

	/** Total number of targets destroyed by player. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BeatShot|PlayerScore")
	int32 TargetsHit = 0;

	/** Total number of targets spawned, incremented after receiving calls from FOnTargetSpawnSignature in
	 *  TargetManager. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BeatShot|PlayerScore")
	int32 TargetsSpawned = 0;

	/** Total possible damage that could have been done to tracking target, also used to determine if the score object
	 *  is for Tracking game mode. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BeatShot|PlayerScore")
	float TotalPossibleDamage = 0.f;

	/** Total time offset from Spawn Beat Delay for all destroyed targets. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BeatShot|PlayerScore")
	float TotalTimeOffset = 0.f;

	/** Avg Time offset from Spawn Beat Delay for destroyed targets. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BeatShot|PlayerScore")
	float AvgTimeOffset = 0.f;

	/** time that player completed the session, in Iso8601 UTC format. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BeatShot|PlayerScore")
	FString Time;

	FDateTime LocalDateTime;

	/** The maximum consecutive targets hit in a row. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BeatShot|PlayerScore")
	int32 Streak = 0;

	/** The accuracy at each point in the grid. */
	UPROPERTY()
	TArray<FAccuracyRow> LocationAccuracy;

	FPlayerScore() = default;

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
		return DefiningConfig == Other.DefiningConfig && SongMetadata == Other.SongMetadata;
	}

	void PopulateAudioInfo(const FBS_AudioConfig&);
};
