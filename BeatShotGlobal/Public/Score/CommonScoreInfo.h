// Copyright 2022-2025 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AccuracyData.h"
#include "BSConstants.h"
#include "CommonScoreInfo.generated.h"

/** Struct containing any information to save between game mode sessions that does not define the game mode itself, e.g.
 *  accuracy. */
USTRUCT(BlueprintType)
struct BEATSHOTGLOBAL_API FCommonScoreInfo
{
	GENERATED_BODY()

	UPROPERTY()
	FAccuracyData AccuracyData;

	UPROPERTY()
	TArray<float> QTable;

	UPROPERTY()
	int32 NumQTableRows;

	UPROPERTY()
	int32 NumQTableColumns;

	UPROPERTY()
	TArray<int32> TrainingSamples;

	UPROPERTY()
	int64 TotalTrainingSamples;

	/** Generic constructor. */
	FCommonScoreInfo()
	{
		AccuracyData = FAccuracyData(Constants::DefaultNumberOfAccuracyDataRows,
		                             Constants::DefaultNumberOfAccuracyDataColumns);
		QTable = TArray<float>();
		QTable.Init(0.f, Constants::DefaultQTableSize);
		TrainingSamples = TArray<int32>();
		TrainingSamples.Init(0.f, Constants::DefaultQTableSize);
		NumQTableRows = Constants::DefaultNumberOfQTableRows;
		NumQTableColumns = Constants::DefaultNumberOfQTableColumns;
		TotalTrainingSamples = 0;
	}

	/** Calls UpdateAccuracyRows on AccuracyData which recalculates the accuracy for each entry. */
	void UpdateAccuracy(const FAccuracyData& InAccuracyData)
	{
		AccuracyData.UpdateAccuracyRows(InAccuracyData);
	}

	/** Sets the value of the QTable with InQTable. */
	void UpdateQTable(const TArray<float>& InQTable,
	                  const int32 InNumQTableRows,
	                  const int32 InNumQTableColumns,
	                  const TArray<int32>& InUpdatedTrainingSamples,
	                  const int32 InUpdatedTotalTrainingSamples)
	{
		QTable = InQTable;
		NumQTableRows = InNumQTableRows;
		NumQTableColumns = InNumQTableColumns;
		TrainingSamples = InUpdatedTrainingSamples;
		TotalTrainingSamples = InUpdatedTotalTrainingSamples;
	}

	/** Resets the QTable, TrainingSamples, NumQTableRows & NumQTableColumns, and TotalTrainingSamples to default
	 *  values. */
	void ResetQTable()
	{
		QTable = TArray<float>();
		QTable.Init(0.f, Constants::DefaultQTableSize);
		TrainingSamples = TArray<int32>();
		TrainingSamples.Init(0.f, Constants::DefaultQTableSize);
		NumQTableRows = Constants::DefaultNumberOfQTableRows;
		NumQTableColumns = Constants::DefaultNumberOfQTableColumns;
		TotalTrainingSamples = 0;
	}

	/** Returns the average number of training samples in the TotalTrainingSamples array. */
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
