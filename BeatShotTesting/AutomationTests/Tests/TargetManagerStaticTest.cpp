// Copyright 2022-2026 Markoleptic Games, SP. All Rights Reserved.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "BSGameModeConfig/BSConfig.h"
#include "Target/TargetCommon.h"
#include "Target/TargetManager.h"

/** Exposes the protected statics so the tests can assert on their pure geometry logic. */
class FTargetManagerStaticExposer : public ATargetManager
{
public:
	static FVector GenerateStaticLocationEx(const FBSConfig* InCfg)
	{
		return ATargetManager::GenerateStaticLocation(InCfg);
	}

	static FVector GenerateStaticExtentsEx(const FBSConfig* InCfg)
	{
		return ATargetManager::GenerateStaticExtents(InCfg);
	}

	static FExtrema GenerateStaticExtremaEx(const FBSConfig* InCfg,
	                                        const FVector& InOrigin,
	                                        const FVector& InStaticExtents)
	{
		return ATargetManager::GenerateStaticExtrema(InCfg, InOrigin, InStaticExtents);
	}

	static FVector GenerateSpawnVolumeLocationEx(const FBSConfig* InCfg,
	                                             const FVector& InOrigin,
	                                             const FVector& InSpawnVolumeExtents)
	{
		return ATargetManager::GenerateSpawnVolumeLocation(InCfg, InOrigin, InSpawnVolumeExtents);
	}

	static FVector GenerateSpawnVolumeExtentsEx(const FBSConfig* InCfg,
	                                            const FVector& InSpawnBoxExtents,
	                                            const FVector& InStaticExtents,
	                                            const float InFactor)
	{
		return ATargetManager::GenerateSpawnVolumeExtents(InCfg, InSpawnBoxExtents, InStaticExtents, InFactor);
	}

	static FExtrema GenerateMaxSpawnVolumeExtremaEx(const FBSConfig* InCfg,
	                                                const FVector& InOrigin,
	                                                const FVector& InStaticExtents)
	{
		return ATargetManager::GenerateMaxSpawnVolumeExtrema(InCfg, InOrigin, InStaticExtents);
	}

	static float GetMaxTargetRadiusEx(const FBS_TargetConfig& InTargetCfg)
	{
		return ATargetManager::GetMaxTargetRadius(InTargetCfg);
	}

	static float GetMaxTargetDiameterEx(const FBS_TargetConfig& InTargetCfg)
	{
		return ATargetManager::GetMaxTargetDiameter(InTargetCfg);
	}

	static TArray<FVector> GetAnyDirectionMultipliersEx(const FVector& InLocationBeforeChange,
	                                                    const FVector& InOrigin)
	{
		return ATargetManager::GetAnyDirectionMultipliers(InLocationBeforeChange, InOrigin);
	}
};

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTargetManagerStaticLocationTest,
                                 TEXT("BeatShot.TargetManager.StaticLocation"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)
bool FTargetManagerStaticLocationTest::RunTest(const FString& Parameters)
{
	FBSConfig Cfg;
	UTEST_TRUE("default (non-grid) location",
	           FTargetManagerStaticExposer::GenerateStaticLocationEx(&Cfg) == FVector(3700.f, 0.f, 610.f));

	// Grid: HalfHeight = (VSpacing + VTargetWidth) * 0.5 = (50 * 4 + 4 * 200) * 0.5 = 500
	FBSConfig GridCfg;
	GridCfg.TargetConfig.TargetDistributionPolicy = ETargetDistributionPolicy::Grid;
	UTEST_TRUE("grid 5x5 location", FTargetManagerStaticExposer::GenerateStaticLocationEx(&GridCfg) == FVector(3700.f, 0.f, 610.f));

	// 3 vertical targets, vertical spacing 100 → HalfHeight = (100 * 2 + 2 * 200) * 0.5 = 300
	FBSConfig SmallGridCfg = GridCfg;
	SmallGridCfg.GridConfig.NumVerticalGridTargets = 3;
	SmallGridCfg.GridConfig.GridSpacing = FVector2D(100.f, 100.f);
	UTEST_TRUE("grid 3x3 location",
	           FTargetManagerStaticExposer::GenerateStaticLocationEx(&SmallGridCfg) == FVector(3700.f, 0.f, 410.f));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTargetManagerStaticExtentsTest,
                                 TEXT("BeatShot.TargetManager.StaticExtents"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)
bool FTargetManagerStaticExtentsTest::RunTest(const FString& Parameters)
{
	FBSConfig Cfg;
	UTEST_TRUE("default (non-grid) extents",
	           FTargetManagerStaticExposer::GenerateStaticExtentsEx(&Cfg) == FVector(0.f, 1600.f, 500.f));

	// Grid 5x5: HalfWidth = (75 * 4 + 4 * 200) * 0.5 = 550, HalfHeight = (50 * 4 + 4 * 200) * 0.5 = 500
	FBSConfig GridCfg;
	GridCfg.TargetConfig.TargetDistributionPolicy = ETargetDistributionPolicy::Grid;
	UTEST_TRUE("grid 5x5 extents", FTargetManagerStaticExposer::GenerateStaticExtentsEx(&GridCfg) == FVector(0.f, 550.f, 500.f));

	GridCfg.GridConfig.NumHorizontalGridTargets = 3;
	GridCfg.GridConfig.NumVerticalGridTargets = 3;
	GridCfg.GridConfig.GridSpacing = FVector2D(100.f, 100.f);
	UTEST_TRUE("grid 3x3 extents", FTargetManagerStaticExposer::GenerateStaticExtentsEx(&GridCfg) == FVector(0.f, 300.f, 300.f));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTargetManagerStaticExtremaTest,
                                 TEXT("BeatShot.TargetManager.StaticExtrema"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)
bool FTargetManagerStaticExtremaTest::RunTest(const FString& Parameters)
{
	const FVector Origin(3700.f, 0.f, 610.f);

	// MaxTargetDiameter = 2.0 * 100 = 200 → MinX = 3700 - 0 - 200, MaxX = 3700 + 200
	FBSConfig Cfg;
	const FExtrema NonGrid = FTargetManagerStaticExposer::GenerateStaticExtremaEx(&Cfg, Origin, FVector(0.f, 1600.f, 500.f));
	UTEST_TRUE("non-grid min", NonGrid.Min == FVector(3500.f, -1600.f, 110.f));
	UTEST_TRUE("non-grid max", NonGrid.Max == FVector(3900.f, 1600.f, 1110.f));

	FBSConfig GridCfg;
	GridCfg.TargetConfig.TargetDistributionPolicy = ETargetDistributionPolicy::Grid;
	const FExtrema Grid = FTargetManagerStaticExposer::GenerateStaticExtremaEx(&GridCfg, Origin, FVector(0.f, 550.f, 500.f));
	UTEST_TRUE("grid min", Grid.Min == FVector(3500.f, -550.f, 110.f));
	UTEST_TRUE("grid max", Grid.Max == FVector(3900.f, 550.f, 1110.f));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTargetManagerSpawnVolumeTest,
                                 TEXT("BeatShot.TargetManager.SpawnVolume"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)
bool FTargetManagerSpawnVolumeTest::RunTest(const FString& Parameters)
{
	const FVector Origin(3700.f, 0.f, 610.f);
	const FVector StaticExtents(0.f, 1600.f, 500.f);
	FBSConfig Cfg;

	// Volume back aligns with SpawnBox, shifted MaxRadius + DirBoxPadding = 105 behind
	// Out.X = 3700 - (205 - 100 - 5) = 3600
	const FVector VolumeLocation =
		FTargetManagerStaticExposer::GenerateSpawnVolumeLocationEx(&Cfg, Origin, FVector(205.f, 1702.f, 602.f));
	UTEST_TRUE("volume location aligned behind spawn box", VolumeLocation == FVector(3600.f, 0.f, 610.f));

	// Lerp(Start.X, 0, 1) is exactly 0 → Out.X = 0 + MaxRadius(100) + DirBoxPadding(5)
	const FVector VolumeExtents =
		FTargetManagerStaticExposer::GenerateSpawnVolumeExtentsEx(&Cfg, StaticExtents, StaticExtents, 1.f);
	UTEST_TRUE("volume extents", VolumeExtents == FVector(105.f, 1702.f, 602.f));

	// AbsMaxOrigin.X = 3700 - 105 = 3595; extents (105, 1702, 602) on either side
	const FExtrema MaxExtrema = FTargetManagerStaticExposer::GenerateMaxSpawnVolumeExtremaEx(&Cfg, Origin, StaticExtents);
	UTEST_TRUE("max spawn volume min", MaxExtrema.Min == FVector(3490.f, -1702.f, 8.f));
	UTEST_TRUE("max spawn volume max", MaxExtrema.Max == FVector(3700.f, 1702.f, 1212.f));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTargetManagerMaxTargetSizeTest,
                                 TEXT("BeatShot.TargetManager.MaxTargetRadiusDiameter"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)
bool FTargetManagerMaxTargetSizeTest::RunTest(const FString& Parameters)
{
	FBSConfig Cfg;
	// MaxSpawnedTargetScale = 2.0; radius = 50, diameter = 100
	UTEST_TRUE("max target radius", FTargetManagerStaticExposer::GetMaxTargetRadiusEx(Cfg.TargetConfig) == 100.f);
	UTEST_TRUE("max target diameter", FTargetManagerStaticExposer::GetMaxTargetDiameterEx(Cfg.TargetConfig) == 200.f);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTargetManagerAnyDirectionMultipliersTest,
                                 TEXT("BeatShot.TargetManager.AnyDirectionMultipliers"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)
bool FTargetManagerAnyDirectionMultipliersTest::RunTest(const FString& Parameters)
{
	const FVector Origin(0.f, 0.f, 610.f);

	auto HasDirection = [](const TArray<FVector>& Multipliers, const FVector& Direction)
	{
		return Multipliers.Contains(Direction);
	};

	// Y positive, Z below origin → (0, 1, -1) removed
	TArray<FVector> Result = FTargetManagerStaticExposer::GetAnyDirectionMultipliersEx(FVector(0.f, 10.f, 500.f), Origin);
	UTEST_EQUAL("3 of 4 directions remain", Result.Num(), 3);
	UTEST_TRUE("(0, 1, -1) removed", !HasDirection(Result, FVector(0.f, 1.f, -1.f)));
	UTEST_TRUE("(0, -1, -1) kept", HasDirection(Result, FVector(0.f, -1.f, -1.f)));
	UTEST_TRUE("(0, -1, 1) kept", HasDirection(Result, FVector(0.f, -1.f, 1.f)));
	UTEST_TRUE("(0, 1, 1) kept", HasDirection(Result, FVector(0.f, 1.f, 1.f)));

	// Y positive, Z above origin → (0, 1, 1) removed
	Result = FTargetManagerStaticExposer::GetAnyDirectionMultipliersEx(FVector(0.f, 10.f, 700.f), Origin);
	UTEST_TRUE("(0, 1, 1) removed", !HasDirection(Result, FVector(0.f, 1.f, 1.f)));
	UTEST_TRUE("(0, 1, -1) kept", HasDirection(Result, FVector(0.f, 1.f, -1.f)));

	// Y negative, Z below origin → (0, -1, -1) removed
	Result = FTargetManagerStaticExposer::GetAnyDirectionMultipliersEx(FVector(0.f, -10.f, 500.f), Origin);
	UTEST_TRUE("(0, -1, -1) removed", !HasDirection(Result, FVector(0.f, -1.f, -1.f)));
	UTEST_TRUE("(0, -1, 1) kept", HasDirection(Result, FVector(0.f, -1.f, 1.f)));

	// Y negative, Z above origin → (0, -1, 1) removed
	Result = FTargetManagerStaticExposer::GetAnyDirectionMultipliersEx(FVector(0.f, -10.f, 700.f), Origin);
	UTEST_TRUE("(0, -1, 1) removed", !HasDirection(Result, FVector(0.f, -1.f, 1.f)));
	UTEST_TRUE("(0, -1, -1) kept", HasDirection(Result, FVector(0.f, -1.f, -1.f)));

	return true;
}
