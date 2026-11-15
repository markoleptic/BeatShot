// Copyright 2022-2026 Markoleptic Games, SP. All Rights Reserved.

/**
 * BeatShot.GameModeOptions.Integration.* -- real-game (world) integration tests.
 *
 * C1 (...Semantics.*) pins the options at the pure-data level. This suite drives the SAME options
 * through the real ATargetManager + USpawnAreaManagerComponent in a live world and asserts the
 * deterministic, non-beat-gated effects they have:
 *
 *   - Upfront-spawning: TargetSpawningPolicy::UpfrontOnly pre-spawns the grid of targets at Init;
 *     RuntimeOnly spawns nothing until a beat arrives.
 *   - Spawn-area geometry: the Grid option (GridSpacing + MaxSpawnedTargetScale) sets the spawn-area
 *     cell dimensions and produces a working grid.
 *   - RL wiring: the reinforcement-learning component is initialised to a full QTable only when the
 *     mode is RL-compatible (TargetDamageType != Tracking); otherwise it stays cleared.
 *   - Clear(): resets every live view (managed targets, caches, RL component) back to its empty state.
 *
 * The beat/timing-gated options (activation batches, per-beat spawning, target expiry) are deliberately
 * out of scope here -- they need a controllable beat/time source (see the separate design note).
 * Only the option->observable mappings that hold without a single beat or tick are asserted.
 */

#include "CoreMinimal.h"
#include "BSTestAssertions.h"
#include "../TestBase/TargetManagerTestWithWorld.h"
#include "SaveGames/SaveGamePlayerScore.h"
#include "SaveGames/SaveGamePlayerSettings.h"
#include "Target/ReinforcementLearningComponent.h"
#include "Target/SpawnAreaManagerComponent.h"
#include "Target/Target.h"
#include "Target/TargetManager.h"


namespace
{
// A 5x5 grid, upfront-spawning. MaxSpawnedTargetScale 1.0 -> cell size = GridSpacing + (1.0 * 100).
FBSConfig MakeGridConfig(const ETargetSpawningPolicy InSpawningPolicy)
{
	FBSConfig Cfg;
	Cfg.DefiningConfig = FBS_DefiningConfig(EGameModeType::Custom, EBaseGameMode::MultiBeat, TEXT("Test Grid"),
	                                        EGameModeDifficulty::None);
	Cfg.TargetConfig.TargetDistributionPolicy = ETargetDistributionPolicy::Grid;
	Cfg.TargetConfig.TargetSpawningPolicy = InSpawningPolicy;
	Cfg.TargetConfig.MaxSpawnedTargetScale = 1.f;
	Cfg.TargetConfig.MinSpawnedTargetScale = 1.f;
	Cfg.GridConfig.NumHorizontalGridTargets = 5;
	Cfg.GridConfig.NumVerticalGridTargets = 5;
	Cfg.GridConfig.GridSpacing = FVector2D(75.f, 50.f);
	return Cfg;
}
}


IMPLEMENT_CUSTOM_COMPLEX_AUTOMATION_TEST(FTestIntegrationUpfrontGrid, FTargetManagerTestWithWorld,
                                         "BeatShot.GameModeOptions.Integration.UpfrontSpawnGrid",
                                         EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                         EAutomationTestFlags_HighPriorityAndAbove | EAutomationTestFlags::
                                         ProductFilter);

void FTestIntegrationUpfrontGrid::GetTests(TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands) const
{
	OutBeautifiedNames.Add(TEXT("UpfrontOnly"));
	OutTestCommands.Add(TEXT("UpfrontOnly"));
}

bool FTestIntegrationUpfrontGrid::RunTest(const FString& Parameters)
{
	if (!Init())
	{
		return false;
	}

	BSConfig = MakeShared<FBSConfig>(MakeGridConfig(ETargetSpawningPolicy::UpfrontOnly));
	TargetManager->Init(BSConfig, FCommonScoreInfo(), FPlayerSettings_Game());

	USpawnAreaManagerComponent* Manager = GetSpawnAreaManager();
	BS_TEST_TRUE("grid produced a usable spawn area",
	             Manager->GetSpawnAreaSize().Y * Manager->GetSpawnAreaSize().Z >= 25);

	// UpfrontOnly + Grid spawns the whole grid of targets at Init (5x5 = 25).
	const int32 GridCount = 5 * 5;
	BS_TEST_EQUAL("all grid targets pre-spawned at Init", GetManagedTargets().Num(), GridCount);
	BS_TEST_EQUAL("flag cache agrees with the managed target map", Manager->GetNumManaged(), GridCount);

	// Upfront spawning registers the areas as managed but never activates them (activation is beat-driven).
	BS_TEST_EQUAL("upfront targets are not yet activated", Manager->GetNumActivated(), 0);
	BS_TEST_EQUAL("none deactivated", Manager->GetNumDeactivated(), GridCount);

	CleanUpWorld();
	return true;
}


IMPLEMENT_CUSTOM_COMPLEX_AUTOMATION_TEST(FTestIntegrationUpfrontRuntime, FTargetManagerTestWithWorld,
                                         "BeatShot.GameModeOptions.Integration.UpfrontSpawnRuntime",
                                         EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                         EAutomationTestFlags_HighPriorityAndAbove | EAutomationTestFlags::
                                         ProductFilter);

void FTestIntegrationUpfrontRuntime::GetTests(TArray<FString>& OutBeautifiedNames,
                                              TArray<FString>& OutTestCommands) const
{
	OutBeautifiedNames.Add(TEXT("RuntimeOnly"));
	OutTestCommands.Add(TEXT("RuntimeOnly"));
}

bool FTestIntegrationUpfrontRuntime::RunTest(const FString& Parameters)
{
	if (!Init())
	{
		return false;
	}

	// Same grid, but RuntimeOnly: targets only spawn on a beat. No beat -> no targets at Init.
	BSConfig = MakeShared<FBSConfig>(MakeGridConfig(ETargetSpawningPolicy::RuntimeOnly));
	TargetManager->Init(BSConfig, FCommonScoreInfo(), FPlayerSettings_Game());

	USpawnAreaManagerComponent* Manager = GetSpawnAreaManager();
	BS_TEST_TRUE("grid produced a usable spawn area",
	             Manager->GetSpawnAreaSize().Y * Manager->GetSpawnAreaSize().Z > 0);
	BS_TEST_EQUAL("RuntimeOnly spawns nothing before the first beat", GetManagedTargets().Num(), 0);
	BS_TEST_EQUAL("no spawn area is pre-flagged", Manager->GetNumManaged(), 0);
	BS_TEST_EQUAL("nothing activated", Manager->GetNumActivated(), 0);

	CleanUpWorld();
	return true;
}


IMPLEMENT_CUSTOM_COMPLEX_AUTOMATION_TEST(FTestIntegrationSpawnAreaGeometry, FTargetManagerTestWithWorld,
                                         "BeatShot.GameModeOptions.Integration.SpawnAreaGeometry",
                                         EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                         EAutomationTestFlags_HighPriorityAndAbove | EAutomationTestFlags::
                                         ProductFilter);

void FTestIntegrationSpawnAreaGeometry::GetTests(TArray<FString>& OutBeautifiedNames,
                                                 TArray<FString>& OutTestCommands) const
{
	OutBeautifiedNames.Add(TEXT("GridCells"));
	OutTestCommands.Add(TEXT("GridCells"));
}

bool FTestIntegrationSpawnAreaGeometry::RunTest(const FString& Parameters)
{
	if (!Init())
	{
		return false;
	}

	BSConfig = MakeShared<FBSConfig>(MakeGridConfig(ETargetSpawningPolicy::UpfrontOnly));
	TargetManager->Init(BSConfig, FCommonScoreInfo(), FPlayerSettings_Game());

	USpawnAreaManagerComponent* Manager = GetSpawnAreaManager();
	const FIntVector3 Cell = Manager->GetSpawnAreaDimensions();

	// Grid cell size = GridSpacing + (MaxSpawnedTargetScale * SphereTargetDiameter). With a 1.0 scale and
	// 100-unit target diameter the cells are exactly GridSpacing + 100.
	BS_TEST_EQUAL("cell width follows GridSpacing option", Cell.Y, 75 + 100);
	BS_TEST_EQUAL("cell height follows GridSpacing option", Cell.Z, 50 + 100);

	BS_TEST_TRUE("at least one spawn area", Manager->GetSpawnAreaSize().Y * Manager->GetSpawnAreaSize().Z > 0);

	CleanUpWorld();
	return true;
}


IMPLEMENT_CUSTOM_COMPLEX_AUTOMATION_TEST(FTestIntegrationRLWiringCompatible, FTargetManagerTestWithWorld,
                                         "BeatShot.GameModeOptions.Integration.RLWiringCompatible",
                                         EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                         EAutomationTestFlags_HighPriorityAndAbove | EAutomationTestFlags::
                                         ProductFilter);

void FTestIntegrationRLWiringCompatible::GetTests(TArray<FString>& OutBeautifiedNames,
                                                  TArray<FString>& OutTestCommands) const
{
	OutBeautifiedNames.Add(TEXT("Hit"));
	OutTestCommands.Add(TEXT("Hit"));
}

bool FTestIntegrationRLWiringCompatible::RunTest(const FString& Parameters)
{
	if (!Init())
	{
		return false;
	}

	// TargetDamageType::Hit is RL-compatible, so the manager initialises the RL component to a full QTable.
	FBSConfig Cfg = MakeGridConfig(ETargetSpawningPolicy::UpfrontOnly);
	Cfg.TargetConfig.TargetDamageType = ETargetDamageType::Hit;
	Cfg.AIConfig.ReinforcementLearningMode = EReinforcementLearningMode::ActiveAgent;

	BSConfig = MakeShared<FBSConfig>(Cfg);
	TargetManager->Init(BSConfig, FCommonScoreInfo(), FPlayerSettings_Game());

	UReinforcementLearningComponent* RL = GetRLComponent();
	BS_TEST_NOT_NULL("RL component exists", RL);
	if (!RL)
	{
		CleanUpWorld();
		return false;
	}

	BS_TEST_TRUE("config is RL-compatible", BSConfig->IsCompatibleWithReinforcementLearning());
	BS_TEST_TRUE("QTable initialised to the full 25x25", RL->GetTArray_FromNdArray_QTable().Num() == 25 * 25);
	BS_TEST_TRUE("requesting RL mode propagated to the component",
	             RL->GetRLMode() == EReinforcementLearningMode::ActiveAgent);
	BS_TEST_EQUAL("no training collected yet", (int32)RL->GetTotalTrainingSamples(), 0);

	CleanUpWorld();
	return true;
}


IMPLEMENT_CUSTOM_COMPLEX_AUTOMATION_TEST(FTestIntegrationRLWiringIncompatible, FTargetManagerTestWithWorld,
                                         "BeatShot.GameModeOptions.Integration.RLWiringIncompatible",
                                         EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                         EAutomationTestFlags_HighPriorityAndAbove | EAutomationTestFlags::
                                         ProductFilter);

void FTestIntegrationRLWiringIncompatible::GetTests(TArray<FString>& OutBeautifiedNames,
                                                    TArray<FString>& OutTestCommands) const
{
	OutBeautifiedNames.Add(TEXT("Tracking"));
	OutTestCommands.Add(TEXT("Tracking"));
}

bool FTestIntegrationRLWiringIncompatible::RunTest(const FString& Parameters)
{
	if (!Init())
	{
		return false;
	}

	// TargetDamageType::Tracking is NOT RL-compatible: the manager must leave the RL component cleared,
	// even if the config requested the ActiveAgent mode.
	FBSConfig Cfg = MakeGridConfig(ETargetSpawningPolicy::UpfrontOnly);
	Cfg.TargetConfig.TargetDamageType = ETargetDamageType::Tracking;
	Cfg.AIConfig.ReinforcementLearningMode = EReinforcementLearningMode::ActiveAgent;

	BSConfig = MakeShared<FBSConfig>(Cfg);
	TargetManager->Init(BSConfig, FCommonScoreInfo(), FPlayerSettings_Game());

	UReinforcementLearningComponent* RL = GetRLComponent();
	BS_TEST_NOT_NULL("RL component exists", RL);
	if (!RL)
	{
		CleanUpWorld();
		return false;
	}

	BS_TEST_FALSE("config is NOT RL-compatible", BSConfig->IsCompatibleWithReinforcementLearning());
	BS_TEST_EQUAL("QTable left empty (component not initialised)", RL->GetTArray_FromNdArray_QTable().Num(), 0);
	BS_TEST_EQUAL("RL mode reset to None by the manager", RL->GetRLMode(), EReinforcementLearningMode::None);

	CleanUpWorld();
	return true;
}


IMPLEMENT_CUSTOM_COMPLEX_AUTOMATION_TEST(FTestIntegrationClearResets, FTargetManagerTestWithWorld,
                                         "BeatShot.GameModeOptions.Integration.ClearResets",
                                         EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                         EAutomationTestFlags_HighPriorityAndAbove | EAutomationTestFlags::
                                         ProductFilter);

void FTestIntegrationClearResets::GetTests(TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands) const
{
	OutBeautifiedNames.Add(TEXT("AfterInit"));
	OutTestCommands.Add(TEXT("AfterInit"));
}

bool FTestIntegrationClearResets::RunTest(const FString& Parameters)
{
	if (!Init())
	{
		return false;
	}

	BSConfig = MakeShared<FBSConfig>(MakeGridConfig(ETargetSpawningPolicy::UpfrontOnly));
	TargetManager->Init(BSConfig, FCommonScoreInfo(), FPlayerSettings_Game());

	USpawnAreaManagerComponent* Manager = GetSpawnAreaManager();
	UReinforcementLearningComponent* RL = GetRLComponent();

	// Precondition: the upfront grid is live (25 targets, full QTable for a compatible default-damage mode).
	BS_TEST_EQUAL("pre-populated managed targets", GetManagedTargets().Num(), 25);
	BS_TEST_EQUAL("pre-populated QTable", RL ? RL->GetTArray_FromNdArray_QTable().Num() : -1, 25 * 25);

	// Clear must wipe every live view, even though the config still describes a grid of 25.
	TargetManager->Clear();
	BS_TEST_EQUAL("managed targets destroyed", GetManagedTargets().Num(), 0);
	BS_TEST_EQUAL("managed cache emptied", Manager->GetNumManaged(), 0);
	BS_TEST_EQUAL("activated cache emptied", Manager->GetNumActivated(), 0);
	BS_TEST_EQUAL("QTable cleared", RL ? RL->GetTArray_FromNdArray_QTable().Num() : -1, 0);

	CleanUpWorld();
	return true;
}
