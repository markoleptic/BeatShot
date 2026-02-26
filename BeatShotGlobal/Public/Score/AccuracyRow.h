// Copyright 2022-2025 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AccuracyRow.generated.h"

/** Struct only used to save accuracy to database. */
USTRUCT()
struct BEATSHOTGLOBAL_API FAccuracyRow
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

	/** Updates the accuracy array based on all TotalSpawns and TotalHits. */
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
