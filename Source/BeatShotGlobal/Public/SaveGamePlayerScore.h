// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BSGameModeDataAsset.h"
#include "GameFramework/SaveGame.h"
#include "SaveGamePlayerScore.generated.h"

/** Struct only used to save accuracy to database */
USTRUCT()
struct FAccuracyRow
{
	GENERATED_BODY()

	friend struct FAccuracyData;

	UPROPERTY()
	int32 Size;
	
	UPROPERTY()
	TArray<float> Accuracy;

	UPROPERTY()
	TArray<int64> TotalSpawns;

	UPROPERTY()
	TArray<int64> TotalHits;

	FAccuracyRow()
	{
		Size = 0;
		Accuracy = TArray<float>();
		TotalSpawns = TArray<int64>();
		TotalHits = TArray<int64>();
	}

	FAccuracyRow(const int32 InSize)
	{
		Size = InSize;
		Accuracy.Init(-1.f, Size);
		TotalSpawns.Init(-1, Size);
		TotalHits.Init(0, Size);
	}

	/** Updates the accuracy array based on all TotalSpawns and TotalHits */
private:
	void CalculateAccuracy()
	{
		CheckForEmpty();
		for (int i = 0; i < Size; i++)
		{
			if (TotalSpawns[i] == INDEX_NONE)
			{
				continue;
			}
			Accuracy[i] = static_cast<float>(TotalHits[i]) / static_cast<float>(TotalSpawns[i]);
		}
	}

	void operator+=(const FAccuracyRow& Other)
	{
		if (Other.Size != Size)
		{
			return;
		}
		for (int i = 0; i < Size; i++)
		{
			const int64 NewSpawns = Other.TotalSpawns[i];
			const int64 NewHits = Other.TotalHits[i];
			if (NewSpawns != -1)
			{
				if (TotalSpawns[i] == -1)
				{
					TotalSpawns[i] = NewSpawns;
				}
				else
				{
					TotalSpawns[i] += NewSpawns;
				}
			}
			if (NewHits != -1)
			{
				if (TotalHits[i] == -1)
				{
					TotalHits[i] = NewHits;
				}
				else
				{
					TotalHits[i] += NewHits;
				}
			}
		}
	}

	void CheckForEmpty()
	{
		Size = FMath::Max(5, FMath::Max3(Accuracy.Num(), TotalSpawns.Num(), TotalHits.Num()));
		if (Accuracy.Num() != Size)
		{
			Accuracy.Init(-1.f, Size);
		}
		if (TotalSpawns.Num() != Size)
		{
			TotalSpawns.Init(-1, Size);
		}
		if (TotalHits.Num() != Size)
		{
			TotalHits.Init(0, Size);
		}
	}
};

/** Struct only used to save accuracy to database */
USTRUCT()
struct FAccuracyData
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FAccuracyRow> AccuracyRows;
	
	FAccuracyData()
	{
		AccuracyRows.Init(FAccuracyRow(5), 5);
	}

	FAccuracyData(const int32 InNumRows, const int32 InNumCols)
	{
		AccuracyRows.Init(FAccuracyRow(InNumCols), InNumRows);
	}
	
	/** Adds the InUpdate AccuracyRows to this struct's Accuracy rows, and recalculates the accuracy */
	void UpdateAccuracyRows(const FAccuracyData& InUpdateData)
	{
		for (int i = 0; i < AccuracyRows.Num(); i++)
		{
			if (InUpdateData.AccuracyRows.IsValidIndex(i))
			{
				AccuracyRows[i] += InUpdateData.AccuracyRows[i];
				AccuracyRows[i].CalculateAccuracy();
			}

		}
	}

	/** Calculates the accuracy for each accuracy row */
	void CalculateAccuracy()
	{
		for (int i = 0; i < AccuracyRows.Num(); i++)
		{
			AccuracyRows[i].CalculateAccuracy();
		}
	}
};

/** Struct containing any information to save between game mode sessions that does not define the game mode itself, e.g. accuracy */
USTRUCT(BlueprintType)
struct FCommonScoreInfo
{
	GENERATED_BODY()

	UPROPERTY()
	FAccuracyData AccuracyData;

	UPROPERTY()
	TArray<float> QTable;

	UPROPERTY()
	int32 QTableRowSize;
	
	UPROPERTY()
	TArray<int32> TrainingSamples;
	
	UPROPERTY()
	int64 TotalTrainingSamples;

	FCommonScoreInfo()
	{
		AccuracyData = FAccuracyData(5, 5);
		QTable = TArray<float>();
		TrainingSamples = TArray<int32>();
		QTableRowSize = 0;
		TotalTrainingSamples = 0;
	}

	FCommonScoreInfo(const int32 NumRows, const int32 NumCols)
	{
		AccuracyData = FAccuracyData(NumRows, NumCols);
		QTable = TArray<float>();
		TrainingSamples = TArray<int32>();
		QTableRowSize = 0;
		TotalTrainingSamples = 0;
	}
	
	/** Calls UpdateAccuracyRows on AccuracyData */
	void UpdateAccuracy(const FAccuracyData& InAccuracyData)
	{
		AccuracyData.UpdateAccuracyRows(InAccuracyData);
	}
	
	/** Sets the value of the QTable with InQTable */
	void UpdateQTable(const TArray<float>& InQTable, const TArray<int32>& InUpdatedTrainingSamples, const int32 InQTableRowSize, const int32 InUpdatedTotalTrainingSamples)
	{
		QTable = InQTable;
		QTableRowSize = InQTableRowSize;
		TrainingSamples = InUpdatedTrainingSamples;
		TotalTrainingSamples = InUpdatedTotalTrainingSamples;
	}

	/** Returns the average number of training samples in the TotalTrainingSamples array */
	double GetAverageTrainingSample()
	{
		double Total = 0.f;
		if (TrainingSamples.Num() < 1)
		{
			return Total;
		}
		for (const int32 Sample : TrainingSamples)
		{
			Total += Sample;
		}
		return Total / static_cast<double>(TrainingSamples.Num());
	}
};

/** Used to load and save player scores */
USTRUCT(BlueprintType)
struct FPlayerScore
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Defining Properties")
	FBS_DefiningConfig DefiningConfig;

	/** The song title */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Defining Properties")
	FString SongTitle;

	/** Length of song */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Properties")
	float SongLength;

	/** The current score at any given time during play */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
	float Score;

	/** Only represents highest score based on previous entries, and may become outdated */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
	float HighScore;

	/** Total Targets hit divided by Total shots fired */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
	float Accuracy;

	/** Total Targets hit divided by Total targets spawned */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
	float Completion;

	/** Incremented after receiving calls from FOnShotsFired delegate in DefaultCharacter */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
	int32 ShotsFired;

	/** Total number of targets destroyed by player */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
	int32 TargetsHit;

	/** Total number of targets spawned, incremented after receiving calls from FOnTargetSpawnSignature in TargetManager */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
	int32 TargetsSpawned;

	/** Total possible damage that could have been done to tracking target, also used to determine if the score object is for Tracking game mode */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
	float TotalPossibleDamage;

	/** Total time offset from Spawn Beat Delay for all destroyed targets */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
	float TotalTimeOffset;

	/** Avg Time offset from Spawn Beat Delay for destroyed targets */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
	float AvgTimeOffset;

	/** time that player completed the session, in Iso8601 UTC format */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
	FString Time;

	/** The maximum consecutive targets hit in a row */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
	int32 Streak;

	/** The accuracy at each point in the grid */
	UPROPERTY()
	TArray<FAccuracyRow> LocationAccuracy;

	/** Whether or not this instance has been saved to the database yet */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
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

	void ResetStruct()
	{
		DefiningConfig = FBS_DefiningConfig();
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
		LocationAccuracy = TArray<FAccuracyRow>();
		bSavedToDatabase = false;
	}

	FORCEINLINE bool operator==(const FPlayerScore& Other) const
	{
		if (DefiningConfig == Other.DefiningConfig && SongTitle.Equals(Other.SongTitle))
		{
			return true;
		}
		return false;
	}
};


UCLASS()
class BEATSHOTGLOBAL_API USaveGamePlayerScore : public USaveGame
{
	GENERATED_BODY()

public:
	USaveGamePlayerScore();
	
	/** Returns a copy of PlayerScoreArray */
	TArray<FPlayerScore> GetPlayerScores() const;

	/** Returns player scores not saved to database */
	TArray<FPlayerScore> GetPlayerScores_UnsavedToDatabase() const;

	/** Adds a new entry to PlayerScoreArray */
	void SavePlayerScoreInstance(const FPlayerScore& InPlayerScore);

	/** Modifies score instances in PlayerScoreArray */
	void SetAllScoresSavedToDatabase();

	/** Returns a copy of CommonScoreInfo */
	TMap<FBS_DefiningConfig, FCommonScoreInfo> GetCommonScoreInfo() const;

	/** Returns the CommonScoreInfo that matches a given DefiningConfig, or a blank one if none found  */
	FCommonScoreInfo FindCommonScoreInfo(const FBS_DefiningConfig& InDefiningConfig);

	/** Finds or Adds the CommonScoreInfo for the given Defining Config */
	void SaveCommonScoreInfo(const FBS_DefiningConfig& InDefiningConfig, const FCommonScoreInfo& InCommonScoreInfo);

	/** Prints full QTable array to log/console */
    static void PrintQTable(const FBS_DefiningConfig& InDefiningConfig, const FCommonScoreInfo& InCommonScoreInfo, const FNumberFormattingOptions& Options);

    /** Prints 5x5 accuracy to log/console */
    static void PrintAccuracy(const FBS_DefiningConfig& InDefiningConfig, const FCommonScoreInfo& InCommonScoreInfo, const FNumberFormattingOptions& Options);

    /** Prints full TrainingSamples array to log/console */
    static void PrintTrainingSamples(const FBS_DefiningConfig& InDefiningConfig, const FCommonScoreInfo& InCommonScoreInfo, const FNumberFormattingOptions& Options);

private:
	/** Returns whether or not there exists a score with the same time */
	bool ContainsExistingTime(const FPlayerScore& InPlayerScore);
	
	/** Array containing all saved score instances */
	UPROPERTY()
	TArray<FPlayerScore> PlayerScoreArray;

	/** Map containing common score info for each unique defining config */
	UPROPERTY()
	TMap<FBS_DefiningConfig, FCommonScoreInfo> CommonScoreInfo;

	FNumberFormattingOptions PercentFormat;
	FNumberFormattingOptions QTableFormat;
	FNumberFormattingOptions TrainingSamplesFormat;
};
