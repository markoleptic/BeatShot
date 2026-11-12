// Copyright 2022-2026 Markoleptic Games, SP. All Rights Reserved.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "BSTestAssertions.h"
#include "Target/SpawnArea.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSpawnAreaFindIndexTypeTest,
                                 TEXT("BeatShot.SpawnArea.FindIndexType"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)
bool FSpawnAreaFindIndexTypeTest::RunTest(const FString& Parameters)
{
	const int32 Size = 16;
	const int32 Width = 4;

	BS_TEST_TRUE("idx 0 → BottomLeftCorner", USpawnArea::FindIndexType(0, Size, Width) ==
	                                        EGridIndexType::BottomLeftCorner);
	BS_TEST_TRUE("idx 1 → Bottom", USpawnArea::FindIndexType(1, Size, Width) == EGridIndexType::Bottom);
	BS_TEST_TRUE("idx 2 → Bottom", USpawnArea::FindIndexType(2, Size, Width) == EGridIndexType::Bottom);
	BS_TEST_TRUE("idx 3 → BottomRightCorner", USpawnArea::FindIndexType(3, Size, Width) ==
	                                        EGridIndexType::BottomRightCorner);
	BS_TEST_TRUE("idx 4 → Left", USpawnArea::FindIndexType(4, Size, Width) == EGridIndexType::Left);
	BS_TEST_TRUE("idx 5 → Middle", USpawnArea::FindIndexType(5, Size, Width) == EGridIndexType::Middle);
	BS_TEST_TRUE("idx 6 → Middle", USpawnArea::FindIndexType(6, Size, Width) == EGridIndexType::Middle);
	BS_TEST_TRUE("idx 7 → Right", USpawnArea::FindIndexType(7, Size, Width) == EGridIndexType::Right);
	BS_TEST_TRUE("idx 8 → Left", USpawnArea::FindIndexType(8, Size, Width) == EGridIndexType::Left);
	BS_TEST_TRUE("idx 9 → Middle", USpawnArea::FindIndexType(9, Size, Width) == EGridIndexType::Middle);
	BS_TEST_TRUE("idx 10 → Middle", USpawnArea::FindIndexType(10, Size, Width) == EGridIndexType::Middle);
	BS_TEST_TRUE("idx 11 → Right", USpawnArea::FindIndexType(11, Size, Width) == EGridIndexType::Right);
	BS_TEST_TRUE("idx 12 → TopLeftCorner", USpawnArea::FindIndexType(12, Size, Width) ==
	                                      EGridIndexType::TopLeftCorner);
	BS_TEST_TRUE("idx 13 → Top", USpawnArea::FindIndexType(13, Size, Width) == EGridIndexType::Top);
	BS_TEST_TRUE("idx 14 → Top", USpawnArea::FindIndexType(14, Size, Width) == EGridIndexType::Top);
	BS_TEST_TRUE("idx 15 → TopRightCorner", USpawnArea::FindIndexType(15, Size, Width) ==
	                                       EGridIndexType::TopRightCorner);

	const int32 RowSize = 5;
	BS_TEST_TRUE("single row: idx 0 → SingleRowLeft",
	           USpawnArea::FindIndexType(0, RowSize, RowSize) == EGridIndexType::SingleRowLeft);
	BS_TEST_TRUE("single row: idx 2 → SingleRowMiddle",
	           USpawnArea::FindIndexType(2, RowSize, RowSize) == EGridIndexType::SingleRowMiddle);
	BS_TEST_TRUE("single row: idx 4 → SingleRowRight",
	           USpawnArea::FindIndexType(4, RowSize, RowSize) == EGridIndexType::SingleRowRight);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSpawnAreaCreateAdjacentIndicesTest,
                                 TEXT("BeatShot.SpawnArea.CreateAdjacentIndices"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)
bool FSpawnAreaCreateAdjacentIndicesTest::RunTest(const FString& Parameters)
{
	const int32 Width = 4;

	auto Has = [](const TMap<EAdjacentDirection, int32>& Map, const EAdjacentDirection Direction,
	              const int32 Expected)
	{
		const int32* Found = Map.Find(Direction);
		return Found && *Found == Expected;
	};

	const TMap<EAdjacentDirection, int32> Middle = USpawnArea::CreateAdjacentIndices(EGridIndexType::Middle, 6,
	                                                                                  Width);
	BS_TEST_EQUAL("Middle has 8 neighbors", Middle.Num(), 8);
	BS_TEST_TRUE("Middle neighbors",
	           Has(Middle, EAdjacentDirection::UpLeft, 9) && Has(Middle, EAdjacentDirection::Up, 10) &&
	           Has(Middle, EAdjacentDirection::UpRight, 11) && Has(Middle, EAdjacentDirection::Right, 7) &&
	           Has(Middle, EAdjacentDirection::DownRight, 3) && Has(Middle, EAdjacentDirection::Down, 2) &&
	           Has(Middle, EAdjacentDirection::DownLeft, 1) && Has(Middle, EAdjacentDirection::Left, 5));

	const TMap<EAdjacentDirection, int32> TopLeft = USpawnArea::CreateAdjacentIndices(EGridIndexType::TopLeftCorner,
	                                                                                   12, Width);
	BS_TEST_EQUAL("TopLeftCorner has 3 neighbors", TopLeft.Num(), 3);
	BS_TEST_TRUE("TopLeftCorner neighbors",
	           Has(TopLeft, EAdjacentDirection::Right, 13) && Has(TopLeft, EAdjacentDirection::Down, 8) &&
	           Has(TopLeft, EAdjacentDirection::DownRight, 9));

	const TMap<EAdjacentDirection, int32> TopRight = USpawnArea::CreateAdjacentIndices(
		EGridIndexType::TopRightCorner, 15, Width);
	BS_TEST_EQUAL("TopRightCorner has 3 neighbors", TopRight.Num(), 3);
	BS_TEST_TRUE("TopRightCorner neighbors",
	           Has(TopRight, EAdjacentDirection::Left, 14) && Has(TopRight, EAdjacentDirection::Down, 11) &&
	           Has(TopRight, EAdjacentDirection::DownLeft, 10));

	const TMap<EAdjacentDirection, int32> BottomLeft = USpawnArea::CreateAdjacentIndices(
		EGridIndexType::BottomLeftCorner, 0, Width);
	BS_TEST_EQUAL("BottomLeftCorner has 3 neighbors", BottomLeft.Num(), 3);
	BS_TEST_TRUE("BottomLeftCorner neighbors",
	           Has(BottomLeft, EAdjacentDirection::Right, 1) && Has(BottomLeft, EAdjacentDirection::Up, 4) &&
	           Has(BottomLeft, EAdjacentDirection::UpRight, 5));

	const TMap<EAdjacentDirection, int32> BottomRight = USpawnArea::CreateAdjacentIndices(
		EGridIndexType::BottomRightCorner, 3, Width);
	BS_TEST_EQUAL("BottomRightCorner has 3 neighbors", BottomRight.Num(), 3);
	BS_TEST_TRUE("BottomRightCorner neighbors",
	           Has(BottomRight, EAdjacentDirection::Left, 2) && Has(BottomRight, EAdjacentDirection::Up, 7) &&
	           Has(BottomRight, EAdjacentDirection::UpLeft, 6));

	const TMap<EAdjacentDirection, int32> Top = USpawnArea::CreateAdjacentIndices(EGridIndexType::Top, 13, Width);
	BS_TEST_EQUAL("Top has 5 neighbors", Top.Num(), 5);
	BS_TEST_TRUE("Top neighbors",
	           Has(Top, EAdjacentDirection::Left, 12) && Has(Top, EAdjacentDirection::Right, 14) &&
	           Has(Top, EAdjacentDirection::DownRight, 10) && Has(Top, EAdjacentDirection::Down, 9) &&
	           Has(Top, EAdjacentDirection::DownLeft, 8));

	const TMap<EAdjacentDirection, int32> Right = USpawnArea::CreateAdjacentIndices(EGridIndexType::Right, 7,
	                                                                                 Width);
	BS_TEST_EQUAL("Right has 5 neighbors", Right.Num(), 5);
	BS_TEST_TRUE("Right neighbors",
	           Has(Right, EAdjacentDirection::UpLeft, 10) && Has(Right, EAdjacentDirection::Up, 11) &&
	           Has(Right, EAdjacentDirection::Left, 6) && Has(Right, EAdjacentDirection::Down, 3) &&
	           Has(Right, EAdjacentDirection::DownLeft, 2));

	const TMap<EAdjacentDirection, int32> Bottom = USpawnArea::CreateAdjacentIndices(EGridIndexType::Bottom, 1,
	                                                                                  Width);
	BS_TEST_EQUAL("Bottom has 5 neighbors", Bottom.Num(), 5);
	BS_TEST_TRUE("Bottom neighbors",
	           Has(Bottom, EAdjacentDirection::UpLeft, 4) && Has(Bottom, EAdjacentDirection::Up, 5) &&
	           Has(Bottom, EAdjacentDirection::UpRight, 6) && Has(Bottom, EAdjacentDirection::Right, 2) &&
	           Has(Bottom, EAdjacentDirection::Left, 0));

	const TMap<EAdjacentDirection, int32> Left = USpawnArea::CreateAdjacentIndices(EGridIndexType::Left, 8, Width);
	BS_TEST_EQUAL("Left has 5 neighbors", Left.Num(), 5);
	BS_TEST_TRUE("Left neighbors",
	           Has(Left, EAdjacentDirection::Up, 12) && Has(Left, EAdjacentDirection::UpRight, 13) &&
	           Has(Left, EAdjacentDirection::Right, 9) && Has(Left, EAdjacentDirection::DownRight, 5) &&
	           Has(Left, EAdjacentDirection::Down, 4));

	const TMap<EAdjacentDirection, int32> SingleRowLeft = USpawnArea::CreateAdjacentIndices(
		EGridIndexType::SingleRowLeft, 0, 5);
	BS_TEST_EQUAL("SingleRowLeft has 1 neighbor", SingleRowLeft.Num(), 1);
	BS_TEST_TRUE("SingleRowLeft neighbor", Has(SingleRowLeft, EAdjacentDirection::Right, 1));

	const TMap<EAdjacentDirection, int32> SingleRowRight = USpawnArea::CreateAdjacentIndices(
		EGridIndexType::SingleRowRight, 4, 5);
	BS_TEST_EQUAL("SingleRowRight has 1 neighbor", SingleRowRight.Num(), 1);
	BS_TEST_TRUE("SingleRowRight neighbor", Has(SingleRowRight, EAdjacentDirection::Left, 3));

	const TMap<EAdjacentDirection, int32> SingleRowMiddle = USpawnArea::CreateAdjacentIndices(
		EGridIndexType::SingleRowMiddle, 2, 5);
	BS_TEST_EQUAL("SingleRowMiddle has 2 neighbors", SingleRowMiddle.Num(), 2);
	BS_TEST_TRUE("SingleRowMiddle neighbors",
	           Has(SingleRowMiddle, EAdjacentDirection::Left, 1) &&
	           Has(SingleRowMiddle, EAdjacentDirection::Right, 3));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSpawnAreaCalcTraceRadiusTest,
                                 TEXT("BeatShot.SpawnArea.CalcTraceRadius"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)
bool FSpawnAreaCalcTraceRadiusTest::RunTest(const FString& Parameters)
{
	// MinRadius = Max(Width, Height) * 0.5 = 5
	USpawnArea::SetWidth(10);
	USpawnArea::SetHeight(10);

	const float ExpectedRadius_1 = ceil(1.0f * 50.f / 5.f) * 5.f * FMath::Sqrt(2.f) * 2.f;
	BS_TEST_TRUE("scale 1 snaps to 50 and doubles",
	           FMath::IsNearlyEqual(USpawnArea::CalcTraceRadius(FVector(1.f, 1.f, 1.f)), ExpectedRadius_1));

	// 0.45 * 50 / 5 = 4.5 → ceil → 5 → 25 (exercises the snapping)
	const float ExpectedRadius_45 = ceil(0.45f * 50.f / 5.f) * 5.f * FMath::Sqrt(2.f) * 2.f;
	BS_TEST_TRUE("scale 0.45 ceils up",
	           FMath::IsNearlyEqual(USpawnArea::CalcTraceRadius(FVector(0.45f, 0.45f, 0.45f)), ExpectedRadius_45));

	const float ExpectedRadius_8 = ceil(0.8f * 50.f / 5.f) * 5.f * FMath::Sqrt(2.f) * 2.f;
	BS_TEST_TRUE("scale 0.8", FMath::IsNearlyEqual(USpawnArea::CalcTraceRadius(FVector(0.8f, 0.8f, 0.8f)),
	                                             ExpectedRadius_8));

	const float ExpectedRadius_2 = ceil(2.0f * 50.f / 5.f) * 5.f * FMath::Sqrt(2.f) * 2.f;
	BS_TEST_TRUE("scale 2", FMath::IsNearlyEqual(USpawnArea::CalcTraceRadius(FVector(2.f, 2.f, 2.f)),
	                                           ExpectedRadius_2));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSpawnAreaGenerateRandomOffsetTest,
                                 TEXT("BeatShot.SpawnArea.GenerateRandomOffset"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)
bool FSpawnAreaGenerateRandomOffsetTest::RunTest(const FString& Parameters)
{
	USpawnArea::SetWidth(31);
	USpawnArea::SetHeight(57);

	const TSet<FVector> Expected = {
		FVector(0.f, 0.f, 0.f),
		FVector(0.f, 30.f, 0.f),
		FVector(0.f, 0.f, 56.f),
		FVector(0.f, 30.f, 56.f)
	};

	bool bAllValid = true;
	for (int i = 0; i < 10; i++)
	{
		const FVector Offset = USpawnArea::GenerateRandomOffset();
		if (!Expected.Contains(Offset) || Offset.X != 0.f)
		{
			bAllValid = false;
			break;
		}
	}
	BS_TEST_TRUE("all offsets are within the corner set", bAllValid);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSpawnAreaInitAndIncrementTest,
                                 TEXT("BeatShot.SpawnArea.InitAndIncrement"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)
bool FSpawnAreaInitAndIncrementTest::RunTest(const FString& Parameters)
{
	USpawnArea::SetSize(16);
	USpawnArea::SetWidth(10);
	USpawnArea::SetHeight(20);
	USpawnArea::SetTotalNumHorizontalSpawnAreas(4);

	USpawnArea* SpawnArea = NewObject<USpawnArea>(GetTransientPackage());
	SpawnArea->Init(6, FVector(0.f, 6.f, 2.f));

	BS_TEST_EQUAL("index stored", SpawnArea->GetIndex(), 6);
	BS_TEST_TRUE("index type is Middle", SpawnArea->GetIndexType() == EGridIndexType::Middle);
	BS_TEST_EQUAL("8 adjacent indices", SpawnArea->GetAdjacentIndices().Num(), 8);
	BS_TEST_TRUE("bottom left vertex", SpawnArea->GetBottomLeftVertex() == FVector(0.f, 6.f, 2.f));
	BS_TEST_TRUE("center point", SpawnArea->GetCenterPoint() == FVector(0.f, 11.f, 12.f));

	BS_TEST_EQUAL("initial spawns is INDEX_NONE", SpawnArea->GetTotalSpawns(), INDEX_NONE);
	BS_TEST_EQUAL("initial hits is 0", SpawnArea->GetTotalHits(), 0);
	BS_TEST_EQUAL("initial tracking damage possible is INDEX_NONE", SpawnArea->GetTotalTrackingDamagePossible(),
	            INDEX_NONE);
	BS_TEST_EQUAL("initial tracking damage is 0", SpawnArea->GetTotalTrackingDamage(), 0);

	SpawnArea->IncrementTotalSpawns();
	BS_TEST_EQUAL("first spawn counts 1 (INDEX_NONE special case)", SpawnArea->GetTotalSpawns(), 1);
	SpawnArea->IncrementTotalSpawns();
	BS_TEST_EQUAL("second spawn counts 2", SpawnArea->GetTotalSpawns(), 2);

	SpawnArea->IncrementTotalHits();
	BS_TEST_EQUAL("first hit", SpawnArea->GetTotalHits(), 1);
	SpawnArea->IncrementTotalHits();
	BS_TEST_EQUAL("second hit", SpawnArea->GetTotalHits(), 2);

	SpawnArea->IncrementTotalTrackingDamagePossible();
	BS_TEST_EQUAL("first damage possible (INDEX_NONE special case)", SpawnArea->GetTotalTrackingDamagePossible(), 1);
	SpawnArea->IncrementTotalTrackingDamagePossible();
	BS_TEST_EQUAL("second damage possible", SpawnArea->GetTotalTrackingDamagePossible(), 2);

	SpawnArea->IncrementTotalTrackingDamage();
	BS_TEST_EQUAL("first damage", SpawnArea->GetTotalTrackingDamage(), 1);

	return true;
}
