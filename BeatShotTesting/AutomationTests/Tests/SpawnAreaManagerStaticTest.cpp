// Copyright 2022-2026 Markoleptic Games, SP. All Rights Reserved.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "BSTestAssertions.h"
#include "Target/SpawnAreaManagerComponent.h"

/** Exposes the protected static factor/geometry helpers so the tests can assert on their pure logic. */
class FMgrStaticExposer : public USpawnAreaManagerComponent
{
public:
	static TSet<FFactor> FindAllFactorsEx(const int32 InNumber)
	{
		return USpawnAreaManagerComponent::FindAllFactors(InNumber);
	}

	static FFactor FindLargestFactorsEx(const int32 InNumber)
	{
		return USpawnAreaManagerComponent::FindLargestFactors(InNumber);
	}

	static TSet<FFactor> FindBestFittingFactorsEx(const int32 InNumber, const int32 InConstraint1,
	                                              const int32 InConstraint2)
	{
		return USpawnAreaManagerComponent::FindBestFittingFactors(InNumber, InConstraint1, InConstraint2);
	}

	static int32 CalcManhattanDistEx(const int32 InIndex1, const int32 InIndex2, const int32 InNumCols)
	{
		return USpawnAreaManagerComponent::CalcManhattanDist(InIndex1, InIndex2, InNumCols);
	}

	static TSet<FFactor> GetPreferredRectangleDimensionsEx(const int32 InBlockSize, const int32 InNumRows,
	                                                        const int32 InNumCols)
	{
		return USpawnAreaManagerComponent::GetPreferredRectangleDimensions(InBlockSize, InNumRows, InNumCols);
	}
};

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMgrStaticFactorHelpersTest,
                                 TEXT("BeatShot.SpawnAreaManager.FactorHelpers"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)
bool FMgrStaticFactorHelpersTest::RunTest(const FString& Parameters)
{
	const TSet<FFactor> FactorsOf0 = FMgrStaticExposer::FindAllFactorsEx(0);
	BS_TEST_EQUAL("factors of 0 is empty", FactorsOf0.Num(), 0);

	const TSet<FFactor> FactorsOf1 = FMgrStaticExposer::FindAllFactorsEx(1);
	BS_TEST_EQUAL("factors of 1 has one entry", FactorsOf1.Num(), 1);
	BS_TEST_TRUE("1 → (1,1)", FactorsOf1.Contains(FFactor(1, 1)));

	// The TSet dedupes reverse pairs (e.g. (3,4) and (4,3) hash + compare equal).
	const TSet<FFactor> FactorsOf12 = FMgrStaticExposer::FindAllFactorsEx(12);
	BS_TEST_EQUAL("factors of 12 → 3 unique pairs", FactorsOf12.Num(), 3);
	BS_TEST_TRUE("12 has (1,12)", FactorsOf12.Contains(FFactor(1, 12)));
	BS_TEST_TRUE("12 has (2,6)", FactorsOf12.Contains(FFactor(2, 6)));
	BS_TEST_TRUE("12 has (3,4)", FactorsOf12.Contains(FFactor(3, 4)));

	const TSet<FFactor> Fit8 = FMgrStaticExposer::FindBestFittingFactorsEx(8, 3, 3);
	BS_TEST_EQUAL("8 with a 3x3 constraint fits nothing", Fit8.Num(), 0);

	const TSet<FFactor> Fit12 = FMgrStaticExposer::FindBestFittingFactorsEx(12, 4, 4);
	BS_TEST_EQUAL("12 with 4x4 → one best factor", Fit12.Num(), 1);
	BS_TEST_TRUE("12 4x4 → (3,4)", Fit12.Contains(FFactor(3, 4)));

	const TSet<FFactor> Fit24 = FMgrStaticExposer::FindBestFittingFactorsEx(24, 6, 6);
	BS_TEST_EQUAL("24 with 6x6 → one best factor", Fit24.Num(), 1);
	BS_TEST_TRUE("24 6x6 → (4,6)", Fit24.Contains(FFactor(4, 6)));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMgrStaticFindLargestTest,
                                 TEXT("BeatShot.SpawnAreaManager.FindLargest"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)
bool FMgrStaticFindLargestTest::RunTest(const FString& Parameters)
{
	const FFactor Largest12 = FMgrStaticExposer::FindLargestFactorsEx(12);
	BS_TEST_TRUE("12 → valid", Largest12.IsValid());
	BS_TEST_TRUE("12 → (3,4)", Largest12 == FFactor(3, 4));

	const FFactor Largest9 = FMgrStaticExposer::FindLargestFactorsEx(9);
	BS_TEST_TRUE("9 → (3,3)", Largest9 == FFactor(3, 3));
	BS_TEST_EQUAL("9 → distance 0", Largest9.Distance, 0);

	// No factors greater than 1 → loop body never runs → invalid sentinel.
	const FFactor Largest1 = FMgrStaticExposer::FindLargestFactorsEx(1);
	BS_TEST_TRUE("1 → invalid", !Largest1.IsValid());

	const FFactor Largest2 = FMgrStaticExposer::FindLargestFactorsEx(2);
	BS_TEST_TRUE("2 → invalid (no factor > 1)", !Largest2.IsValid());

	const FFactor Largest7 = FMgrStaticExposer::FindLargestFactorsEx(7);
	BS_TEST_TRUE("7 (prime) → invalid", !Largest7.IsValid());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMgrStaticManhattanTest,
                                 TEXT("BeatShot.SpawnAreaManager.CalcManhattanDist"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)
bool FMgrStaticManhattanTest::RunTest(const FString& Parameters)
{
		// IsPrime is deliberately left out: it is a constexpr static declared in the header but defined in the cpp,
	// which the C++20 toolchain does not let us odr-use from this translation unit.
	// Manhattan distance on a 4-column grid (index = row * 4 + col).
	BS_TEST_EQUAL("same index → 0", FMgrStaticExposer::CalcManhattanDistEx(6, 6, 4), 0);
	BS_TEST_EQUAL("right neighbor → 1", FMgrStaticExposer::CalcManhattanDistEx(6, 7, 4), 1);
	BS_TEST_EQUAL("left neighbor → 1", FMgrStaticExposer::CalcManhattanDistEx(6, 5, 4), 1);
	BS_TEST_EQUAL("row above (same col) → 1", FMgrStaticExposer::CalcManhattanDistEx(6, 10, 4), 1);
	BS_TEST_EQUAL("diagonal (down-left) → 2", FMgrStaticExposer::CalcManhattanDistEx(6, 1, 4), 2);
	BS_TEST_EQUAL("opposite corners of a 4x4 → 6", FMgrStaticExposer::CalcManhattanDistEx(0, 15, 4), 6);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMgrStaticPreferredDimsTest,
                                 TEXT("BeatShot.SpawnAreaManager.PreferredDimensions"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)
bool FMgrStaticPreferredDimsTest::RunTest(const FString& Parameters)
{
	const TSet<FFactor> Dims8 = FMgrStaticExposer::GetPreferredRectangleDimensionsEx(8, 5, 5);
	BS_TEST_EQUAL("8 → one option", Dims8.Num(), 1);
	BS_TEST_TRUE("8 → (2,4)", Dims8.Contains(FFactor(2, 4)));

	const TSet<FFactor> Dims9 = FMgrStaticExposer::GetPreferredRectangleDimensionsEx(9, 5, 5);
	BS_TEST_EQUAL("9 → one option", Dims9.Num(), 1);
	BS_TEST_TRUE("9 → (3,3)", Dims9.Contains(FFactor(3, 3)));

	// 7 is prime → searches 6 and 8; (2,3) [dist 1] beats (2,4) [dist 2].
	const TSet<FFactor> Dims7 = FMgrStaticExposer::GetPreferredRectangleDimensionsEx(7, 5, 5);
	BS_TEST_EQUAL("7 (prime) → one option", Dims7.Num(), 1);
	BS_TEST_TRUE("7 → (2,3)", Dims7.Contains(FFactor(2, 3)));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMgrStaticFactorStructTest,
                                 TEXT("BeatShot.SpawnAreaManager.FactorStruct"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)
bool FMgrStaticFactorStructTest::RunTest(const FString& Parameters)
{
	const FFactor Default;
	BS_TEST_TRUE("default is invalid", !Default.IsValid());
	BS_TEST_EQUAL("default F1", Default.Factor1, -1);
	BS_TEST_EQUAL("default F2", Default.Factor2, -1);
	BS_TEST_EQUAL("default distance", Default.Distance, -1);

	const FFactor Factor(3, 4);
	BS_TEST_TRUE("(3,4) is valid", Factor.IsValid());
	BS_TEST_EQUAL("Factor1", Factor.Factor1, 3);
	BS_TEST_EQUAL("Factor2", Factor.Factor2, 4);
	BS_TEST_EQUAL("distance is |3-4|", Factor.Distance, 1);

	const FFactor Reversed(4, 3);
	BS_TEST_TRUE("== is symmetric", Factor == Reversed);
	BS_TEST_TRUE("GetTypeHash is symmetric", GetTypeHash(Factor) == GetTypeHash(Reversed));

	const FFactor Close(2, 3); // distance 1
	const FFactor Far(2, 5);   // distance 3
	BS_TEST_TRUE("< same distance, lower Factor1 first", Close < Factor);
	BS_TEST_TRUE("< lower distance first", Factor < Far);

	const int32 SentinelDistance = 5;
	const FFactor Sentinel(SentinelDistance); // explicit distance ctor → invalid factor sentinel
	BS_TEST_TRUE("sentinel is invalid", !Sentinel.IsValid());
	BS_TEST_EQUAL("sentinel keeps its distance", Sentinel.Distance, 5);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMgrStaticSubRectangleTest,
                                 TEXT("BeatShot.SpawnAreaManager.SubRectangleStruct"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)
bool FMgrStaticSubRectangleTest::RunTest(const FString& Parameters)
{
	FSubRectangle::SetNumCols(5);
	FSubRectangle Sub(6, 1, 2);
	Sub.UpdateDimensions(3);
	BS_TEST_EQUAL("start index", Sub.BoundingIndices.StartIndex, 1);
	BS_TEST_EQUAL("end index", Sub.BoundingIndices.EndIndex, 8);
	BS_TEST_EQUAL("area = 3x2", Sub.Area, 6);
	BS_TEST_EQUAL("dims width", Sub.Dimensions.Width, 3);
	BS_TEST_EQUAL("dims height", Sub.Dimensions.Height, 2);
	BS_TEST_EQUAL("row start", Sub.Row.StartIndex, 0);
	BS_TEST_EQUAL("row end", Sub.Row.EndIndex, 1);
	BS_TEST_EQUAL("col start", Sub.Col.StartIndex, 1);
	BS_TEST_EQUAL("col end", Sub.Col.EndIndex, 3);
	BS_TEST_TRUE("sub rectangle is valid", Sub.IsValid());

	// Fit checks exercise Factor against the (directly assigned) available counts.
	FRectCandidate Candidate(FFactor(2, 4));
	Candidate.NumRowsAvailable = 3;
	Candidate.NumColsAvailable = 5;
	BS_TEST_TRUE("first factor combo fits", Candidate.FirstFactorComboFits());
	BS_TEST_TRUE("second (swapped) combo does not fit", !Candidate.SecondFactorComboFits());
	BS_TEST_TRUE("not all factors fit", !Candidate.AllFactorsFit());

	Candidate.NumRowsAvailable = 4; // now Factor2 also fits rows
	BS_TEST_TRUE("first combo still fits", Candidate.FirstFactorComboFits());
	BS_TEST_TRUE("second combo now fits", Candidate.SecondFactorComboFits());
	BS_TEST_TRUE("all factors fit", Candidate.AllFactorsFit());

	// SetChosenSubRectangle derives the available counts and clamps the block size to the area.
	FRectCandidate Chosen(FFactor(2, 3));
	Chosen.SetChosenSubRectangle(Sub, 8);
	BS_TEST_EQUAL("rows available", Chosen.NumRowsAvailable, 2);
	BS_TEST_EQUAL("cols available", Chosen.NumColsAvailable, 3);
	BS_TEST_EQUAL("actual block size = min(8, area 6)", Chosen.ActualBlockSize, 6);
	BS_TEST_TRUE("has a chosen sub rectangle", Chosen.HasChosenSubRectangle());

	return true;
}
