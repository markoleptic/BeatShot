// Copyright 2022-2026 Markoleptic Games, SP. All Rights Reserved.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Target/MatrixFunctions.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMatrixFnGet5X5OverflowArrayTest,
                                 TEXT("BeatShot.MatrixFunctions.Get5X5OverflowArray"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)
bool FMatrixFnGet5X5OverflowArrayTest::RunTest(const FString& Parameters)
{
	// Overflow 0 → all zeros.
	const nc::NdArray<int> Zero = Get5X5OverflowArray(0);
	UTEST_TRUE("overflow 0 → all zero",
	           Zero(0, 0) == 0 && Zero(0, 1) == 0 && Zero(0, 2) == 0 && Zero(0, 3) == 0 && Zero(0, 4) == 0);

	// Overflow 1 → only index 2 has a 1.
	const nc::NdArray<int> One = Get5X5OverflowArray(1);
	UTEST_TRUE("overflow 1 → only idx 2", One(0, 2) == 1 && One(0, 0) == 0 && One(0, 4) == 0);

	// Overflow 2 → idx 0 and 4 have a 1.
	const nc::NdArray<int> Two = Get5X5OverflowArray(2);
	UTEST_TRUE("overflow 2 → idx 0 & 4", Two(0, 0) == 1 && Two(0, 4) == 1 && Two(0, 2) == 0);

	// Overflow 3 → idx 0, 2, 4 have a 1.
	const nc::NdArray<int> Three = Get5X5OverflowArray(3);
	UTEST_TRUE("overflow 3 → idx 0, 2, 4", Three(0, 0) == 1 && Three(0, 2) == 1 && Three(0, 4) == 1);

	// Overflow 4 → idx 0, 1, 3, 4 have a 1 (middle is absent).
	const nc::NdArray<int> Four = Get5X5OverflowArray(4);
	UTEST_TRUE("overflow 4 → idx 0, 1, 3, 4", Four(0, 0) == 1 && Four(0, 1) == 1 && Four(0, 3) == 1 &&
	           Four(0, 4) == 1 && Four(0, 2) == 0);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMatrixFnGetTArrayFromNdArrayRoundtripTest,
                                 TEXT("BeatShot.MatrixFunctions.GetTArrayFromNdArray"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)
bool FMatrixFnGetTArrayFromNdArrayRoundtripTest::RunTest(const FString& Parameters)
{
	// 3x3 array filled with row-major 0..8, i.e. In(i, j) = 3*i + j.
	nc::NdArray<float> In = nc::arange<float>(0, 9, 1).reshape(3, 3);
	const TArray<float> Out = GetTArrayFromNdArray<float>(In);

	UTEST_EQUAL("size is 9", Out.Num(), 9);
	for (int j = 0; j < 3; j++)
	{
		for (int i = 0; i < 3; i++)
		{
			UTEST_TRUE("cell value matches",
			           FMath::IsNearlyEqual(Out[3 * j + i], static_cast<float>(3 * i + j)));
		}
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMatrixFnGetNdArrayFromTArrayRoundtripTest,
                                 TEXT("BeatShot.MatrixFunctions.GetNdArrayFromTArray"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)
bool FMatrixFnGetNdArrayFromTArrayRoundtripTest::RunTest(const FString& Parameters)
{
	// 2x3 array filled 0..5 (row-major: 0,1,2 / 3,4,5).
	TArray<float> In;
	for (int32 i = 0; i < 6; i++)
	{
		In.Add(static_cast<float>(i));
	}
	const nc::NdArray<float> Out = GetNdArrayFromTArray<float>(In, 2, 3);

	UTEST_EQUAL("numRows", static_cast<int32>(Out.numRows()), 2);
	UTEST_EQUAL("numCols", static_cast<int32>(Out.numCols()), 3);
	for (int j = 0; j < 3; j++)
	{
		for (int i = 0; i < 2; i++)
		{
			UTEST_TRUE("cell value matches", FMath::IsNearlyEqual(Out(i, j), static_cast<float>(2 * j + i)));
		}
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMatrixFnGetAveraged5X5TArray5x5Test,
                                 TEXT("BeatShot.MatrixFunctions.GetAveraged5X5TArray.5x5"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)
bool FMatrixFnGetAveraged5X5TArray5x5Test::RunTest(const FString& Parameters)
{
	// 5x5 input with unique values 1..25 (row-major). With MFloor=NFloor=1, each 5x5 output cell
	// maps to exactly one input cell, so the average of {v} is v (or sum is v when bAverage=false).
	TArray<float> In;
	In.Reserve(25);
	for (int32 i = 0; i < 25; i++)
	{
		In.Add(static_cast<float>(i + 1));
	}
	const TArray<float> OutAvg = GetAveraged5X5TArray<float>(In, 5, 5, /*bAverage*/ true);
	const TArray<float> OutSum = GetAveraged5X5TArray<float>(In, 5, 5, /*bAverage*/ false);

	UTEST_EQUAL("output size 25", OutAvg.Num(), 25);
	for (int32 i = 0; i < 25; i++)
	{
		UTEST_TRUE("average matches input", FMath::IsNearlyEqual(OutAvg[i], static_cast<float>(i + 1)));
		UTEST_TRUE("sum matches input", FMath::IsNearlyEqual(OutSum[i], static_cast<float>(i + 1)));
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMatrixFnGetAveraged5X5TArray10x10Test,
                                 TEXT("BeatShot.MatrixFunctions.GetAveraged5X5TArray.10x10"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)
bool FMatrixFnGetAveraged5X5TArray10x10Test::RunTest(const FString& Parameters)
{
	// 10x10 uniform 2 → each 5x5 output cell averages the same 2.
	TArray<float> In;
	In.Reserve(100);
	for (int32 i = 0; i < 100; i++)
	{
		In.Add(2.f);
	}
	const TArray<float> Out = GetAveraged5X5TArray<float>(In, 10, 10, /*bAverage*/ true);

	UTEST_EQUAL("output size 25", Out.Num(), 25);
	for (int32 i = 0; i < 25; i++)
	{
		UTEST_TRUE("each cell is 2", FMath::IsNearlyEqual(Out[i], 2.f));
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMatrixFnGetAveraged5X5TArrayZeroSkipTest,
                                 TEXT("BeatShot.MatrixFunctions.GetAveraged5X5TArray.ZeroSkip"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)
bool FMatrixFnGetAveraged5X5TArrayZeroSkipTest::RunTest(const FString& Parameters)
{
	// All zeros → count==0 for every block → output stays 0 (identity zero).
	TArray<float> In;
	In.Init(0.f, 25);
	const TArray<float> Out = GetAveraged5X5TArray<float>(In, 5, 5, /*bAverage*/ true);
	UTEST_EQUAL("size 25", Out.Num(), 25);
	for (int32 i = 0; i < 25; i++)
	{
		UTEST_TRUE("stays zero", Out[i] == 0.f);
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMatrixFnGetAveragedAccuracyDataTest,
                                 TEXT("BeatShot.MatrixFunctions.GetAveragedAccuracyData"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)
bool FMatrixFnGetAveragedAccuracyDataTest::RunTest(const FString& Parameters)
{
	// 5x5 spawns (all 1) and 5x5 hits (all 2) → each 5x5 output cell accumulates 1 spawn and 2 hits.
	TArray<int32> Spawn;
	Spawn.Init(1, 25);
	TArray<int32> Hit;
	Hit.Init(2, 25);
	const FAccuracyData Out = GetAveragedAccuracyData(Spawn, Hit, 5, 5);

	UTEST_EQUAL("row count", Out.AccuracyRows.Num(), 5);
	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 5; j++)
	{
		UTEST_EQUAL("spawns=1", static_cast<int32>(Out.AccuracyRows[i].TotalSpawns[j]), 1);
		UTEST_EQUAL("hits=2", static_cast<int32>(Out.AccuracyRows[i].TotalHits[j]), 2);
	}
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMatrixFnMapMatrixTo5X5_5x5Test,
                                 TEXT("BeatShot.MatrixFunctions.MapMatrixTo5X5.5x5"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)
bool FMatrixFnMapMatrixTo5X5_5x5Test::RunTest(const FString& Parameters)
{
	// 5x5 → identity mapping: each QTable index maps to exactly {itself}.
	const TMap<int32, FGenericIndexMapping> Map = MapMatrixTo5X5(5, 5);
	UTEST_EQUAL("25 mappings", Map.Num(), 25);

	for (int32 i = 0; i < 25; i++)
	{
		const FGenericIndexMapping* Entry = Map.Find(i);
		UTEST_TRUE("entry present", Entry != nullptr);
		if (Entry == nullptr)
		{
			continue;
		}
		UTEST_EQUAL("Index==i", Entry->Index, i);
		UTEST_EQUAL("one mapped index", Entry->MappedIndices.Num(), 1);
		if (Entry->MappedIndices.Num() == 1)
		{
			UTEST_EQUAL("identity mapping", Entry->MappedIndices[0], i);
		}
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMatrixFnMapMatrixTo5X5_10x10Test,
                                 TEXT("BeatShot.MatrixFunctions.MapMatrixTo5X5.10x10"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)
bool FMatrixFnMapMatrixTo5X5_10x10Test::RunTest(const FString& Parameters)
{
	// 10x10: each 5x5 output cell covers a 2x2 block → 100 total indices, each cell maps to 4 indices.
	const TMap<int32, FGenericIndexMapping> Map = MapMatrixTo5X5(10, 10);
	UTEST_EQUAL("25 mappings", Map.Num(), 25);

	int32 TotalMapped = 0;
	for (const TPair<int32, FGenericIndexMapping>& Entry : Map)
	{
		TotalMapped += Entry.Value.MappedIndices.Num();
		// Each 2x2 block maps to 4 unique indices.
		for (const int32 Idx : Entry.Value.MappedIndices)
		{
			UTEST_TRUE("index in range [0,100)", Idx >= 0 && Idx < 100);
		}
	}
	UTEST_EQUAL("total mapped = 100", TotalMapped, 100);
	return true;
}
