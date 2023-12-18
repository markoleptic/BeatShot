// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "SaveGamePlayerScore.h"
THIRD_PARTY_INCLUDES_START
#pragma push_macro("check")
#undef check
#pragma warning (push)
#pragma warning (disable : 4191)
#pragma warning (disable : 4686)
#include "NumCpp/Public/NumCpp.hpp"
#pragma warning (pop)
#pragma pop_macro("check")
THIRD_PARTY_INCLUDES_END

/** Converts an NdArray of floats to a TArray of floats */
template <typename T>
static TArray<T> GetTArrayFromNdArray(const nc::NdArray<T>& InArray)
{
	const int32 RowSize = InArray.numRows();
	const int32 ColSize = InArray.numCols();

	TArray<T> Out;
	Out.Init(0.f, InArray.size());

	for (int j = 0; j < ColSize; j++)
	{
		for (int i = 0; i < RowSize; i++)
		{
			Out[RowSize * j + i] = InArray(i, j);
		}
	}
	return Out;
}

/** Converts a TArray of floats to an NdArray of floats */
template <typename T>
static nc::NdArray<T> GetNdArrayFromTArray(const TArray<T>& InTArray, const int32 InRows, const int32 InCols)
{
	nc::NdArray<T> Out = nc::zeros<T>(InRows, InCols);

	for (int j = 0; j < InCols; j++)
	{
		for (int i = 0; i < InRows; i++)
		{
			Out(i, j) = InTArray[InRows * j + i];
		}
	}
	return Out;
}

/** Returns a flattened array of 5 values containing which indices should take in additional values in their average */
static nc::NdArray<int> Get5X5OverflowArray(const int32 Overflow)
{
	return nc::NdArray<int>({
		Overflow == 2 || Overflow == 3 || Overflow == 4 ? 1 : 0, Overflow == 4 ? 1 : 0,
		Overflow == 1 || Overflow == 3 ? 1 : 0, Overflow == 4 ? 1 : 0,
		Overflow == 2 || Overflow == 3 || Overflow == 4 ? 1 : 0
	});
}

/** Returns a 5x5 NdArray, averaging each value from the input array using the smallest amount of surrounding values
 *  as possible. Does not re-use any values from the input array */
template <typename T>
nc::NdArray<T> GetAveraged5X5NdArray(const TArray<T>& In, const int32 InRows, const int32 InCols, const bool bAverage)
{
	// Define the output size of the array (m x n)
	constexpr int SmallM = 5;
	constexpr int SmallN = 5;

	nc::NdArray<T> Out = nc::zeros<T>(nc::Shape(SmallM, SmallN));;

	// Define the minimum number of elements that are averaged from input array
	const int MFloor = FMath::Floor(InRows / SmallM);
	const int NFloor = FMath::Floor(InCols / SmallN);

	// Define which columns/rows will avg extra values if not divisible by 5
	nc::NdArray<int> MPad = Get5X5OverflowArray(InRows % SmallM);
	nc::NdArray<int> NPad = Get5X5OverflowArray(InCols % SmallN);

	int MPadSum = 0;
	for (int i = 0; i < SmallM; ++i)
	{
		int NPadSum = 0;
		for (int j = 0; j < SmallN; ++j)
		{
			T Sum = 0;
			int Count = 0;

			const int StartM = i * MFloor + MPadSum;
			const int EndM = StartM + MFloor + MPad(0, i) - 1;

			const int StartN = j * NFloor + NPadSum;
			const int EndN = StartN + NFloor + NPad(0, j) - 1;

			for (int x = StartM; x <= EndM; ++x)
			{
				for (int y = StartN; y <= EndN; ++y)
				{
					T Value = In[x * InCols + y];

					if (Value > 0.f)
					{
						Sum += Value;
						Count += 1;
					}
				}
			}
			if (Count > 0)
			{
				Out(i, j) = static_cast<T>(bAverage ? static_cast<float>(Sum) / Count : Sum);
			}
			NPadSum += NPad(0, j);
		}
		MPadSum += MPad(0, i);
	}
	return Out;
}

/** Returns a 5x5 TArray, averaging each value from the input array using the smallest amount of surrounding values
 *  as possible. Does not re-use any values from the input array */
template <typename T>
TArray<T> GetAveraged5X5TArray(const TArray<T>& In, const int32 InRows, const int32 InCols, const bool bAverage)
{
	// Define the output size of the array (m x n)
	constexpr int SmallM = 5;
	constexpr int SmallN = 5;

	TArray<T> Out = TArray<T>();
	Out.Init(0, SmallM * SmallN);

	// Define the minimum number of elements that are averaged from input array
	const int MFloor = FMath::Floor(InRows / SmallM);
	const int NFloor = FMath::Floor(InCols / SmallN);

	// Define which columns/rows will avg extra values if not divisible by 5
	nc::NdArray<int> MPad = Get5X5OverflowArray(InRows % SmallM);
	nc::NdArray<int> NPad = Get5X5OverflowArray(InCols % SmallN);

	int MPadSum = 0;
	for (int i = 0; i < SmallM; ++i)
	{
		int NPadSum = 0;
		for (int j = 0; j < SmallN; ++j)
		{
			T Sum = 0;
			int Count = 0;

			const int StartM = i * MFloor + MPadSum;
			const int EndM = StartM + MFloor + MPad(0, i) - 1;

			const int StartN = j * NFloor + NPadSum;
			const int EndN = StartN + NFloor + NPad(0, j) - 1;

			for (int x = StartM; x <= EndM; ++x)
			{
				for (int y = StartN; y <= EndN; ++y)
				{
					T Value = In[x * InCols + y];

					if (Value > 0.f)
					{
						Sum += Value;
						Count += 1;
					}
				}
			}
			if (Count > 0)
			{
				Out[i * SmallN + j] = static_cast<T>(bAverage ? static_cast<float>(Sum) / Count : Sum);
			}
			NPadSum += NPad(0, j);
		}
		MPadSum += MPad(0, i);
	}
	return Out;
}

/** Returns an FAccuracyData struct with 5 rows and 5 columns. Distributes the TotalSpawns and TotalHits into smaller blocks,
 *  where each block is an element in the 5x5 AccuracyRows matrix. Sums using the smallest amount of surrounding values
 *  as possible. Does not re-use any values from the input array */
inline FAccuracyData GetAveragedAccuracyData(const TArray<int32>& InTotalSpawns, const TArray<int32>& InTotalHits,
	const int32 InRows, const int32 InCols)
{
	// Define the output size of the array (m x n)
	constexpr int SmallM = 5;
	constexpr int SmallN = 5;

	FAccuracyData OutData = FAccuracyData(SmallM, SmallN);

	// Define the minimum number of elements that are summed from input array
	const int MFloor = FMath::Floor(InRows / SmallM);
	const int NFloor = FMath::Floor(InCols / SmallN);

	// Define which columns/rows will sum extra values if not divisible by 5
	nc::NdArray<int> MPad = Get5X5OverflowArray(InRows % SmallM);
	nc::NdArray<int> NPad = Get5X5OverflowArray(InCols % SmallN);

	int MPadSum = 0;
	for (int i = 0; i < SmallM; ++i)
	{
		int NPadSum = 0;
		for (int j = 0; j < SmallN; ++j)
		{
			auto TotalSpawnsSum = 0;
			auto TotalSpawnsCount = 0;

			auto TotalHitsSum = 0;
			auto TotalHitsCount = 0;

			const int StartM = i * MFloor + MPadSum;
			const int EndM = StartM + MFloor + MPad(0, i) - 1;

			const int StartN = j * NFloor + NPadSum;
			const int EndN = StartN + NFloor + NPad(0, j) - 1;

			for (int x = StartM; x <= EndM; ++x)
			{
				for (int y = StartN; y <= EndN; ++y)
				{
					const int32 CurrentTotalSpawns = InTotalSpawns[x * InCols + y];
					const int32 CurrentTotalHits = InTotalHits[x * InCols + y];

					if (CurrentTotalSpawns > 0.f)
					{
						TotalSpawnsSum += CurrentTotalSpawns;
						TotalSpawnsCount += 1;
					}
					if (CurrentTotalHits > 0.f)
					{
						TotalHitsSum += CurrentTotalHits;
						TotalHitsCount += 1;
					}
				}
			}
			if (TotalSpawnsCount > 0)
			{
				OutData.AccuracyRows[i].TotalSpawns[j] = static_cast<int64>(TotalSpawnsSum);
			}
			if (TotalHitsCount > 0)
			{
				OutData.AccuracyRows[i].TotalHits[j] = static_cast<int64>(TotalHitsSum);
			}
			NPadSum += NPad(0, j);
		}
		MPadSum += MPad(0, i);
	}
	return OutData;
}

/**
 *	Returns a 25 element map that maps all indices of the input matrix to a 5X5 matrix. Each map element is an index of
 *  the 5X5 matrix which contains an array of indices that it represents. \n\n No two indices of the larger are
 *  represented more than once.
 *  
 *  @param InRows Number of rows in original matrix
 *  @param InCols Number of columns in original matrix
 */
inline TMap<int32, FGenericIndexMapping> MapMatrixTo5X5(const int32 InRows, const int32 InCols)
{
	// Define the output size of the array (m x n)
	constexpr int OutM = 5;
	constexpr int OutN = 5;

	TMap<int32, FGenericIndexMapping> IndexMappings = TMap<int32, FGenericIndexMapping>();

	// Define the minimum number of elements that are combined from input array
	const int MFloor = FMath::Floor(InRows / OutM);
	const int NFloor = FMath::Floor(InCols / OutN);

	// Define which columns/rows will get extra values if not divisible by 5
	nc::NdArray<int> MPad = Get5X5OverflowArray(InRows % OutM);
	nc::NdArray<int> NPad = Get5X5OverflowArray(InCols % OutN);

	int MPadSum = 0;
	for (int i = 0; i < OutM; ++i)
	{
		int NPadSum = 0;
		for (int j = 0; j < OutN; ++j)
		{
			FGenericIndexMapping MappingInst;

			const int StartM = i * MFloor + MPadSum;
			const int EndM = StartM + MFloor + MPad(0, i) - 1;

			const int StartN = j * NFloor + NPadSum;
			const int EndN = StartN + NFloor + NPad(0, j) - 1;

			for (int x = StartM; x <= EndM; ++x)
			{
				for (int y = StartN; y <= EndN; ++y)
				{
					int Index = x * InCols + y;
					MappingInst.MappedIndices.Add(Index);
				}
			}

			MappingInst.Index = i * OutN + j;
			IndexMappings.Add(MappingInst.Index, MappingInst);

			NPadSum += NPad(0, j);
		}
		MPadSum += MPad(0, i);
	}

	TArray<int32> UniqueIndices = TArray<int32>();
	for (const TPair<int32, FGenericIndexMapping>& IndexMapping : IndexMappings)
	{
		for (const int32 Value : IndexMapping.Value.MappedIndices)
		{
			UniqueIndices.AddUnique(Value);
		}
	}
	UE_LOG(LogTemp, Display, TEXT("Unique Indices across entire mapping: %d Input Size: %d"), UniqueIndices.Num(),
		InRows * InCols);

	return IndexMappings;
}