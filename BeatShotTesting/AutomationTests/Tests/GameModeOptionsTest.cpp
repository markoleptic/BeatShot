// Copyright 2022-2026 Markoleptic Games, SP. All Rights Reserved.

/**
 * BeatShot.GameModeOptions.Semantics.* -- worldless option-semantics tests.
 *
 * These pin down what the game-mode *options* (the serialized gameplay fields of FBSConfig and its
 * sub-configs) actually do at the data level: which fields their operator== considers, which are
 * intentionally excluded, and the Reinforcement-Learning compatibility rule.
 * They complement the GameModeConfig suite (identity/creation/colors/serialization) and the
 * GameModeValidator suite (cross-field validation rules) rather than overlapping them.
 */

#include "CoreMinimal.h"
#include "BSConstants.h"
#include "BSTestAssertions.h"
#include "BSGameModeConfig/BSConfig.h"
#include "Misc/AutomationTest.h"


namespace
{
FBS_DefiningConfig MakeCustomIdentity(const FString& InName)
{
	return FBS_DefiningConfig(EGameModeType::Custom, EBaseGameMode::MultiBeat, InName, EGameModeDifficulty::None);
}
}


IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGameModeOptionTargetConfigBoolEnumArrayTest,
                                 TEXT("BeatShot.GameModeOptions.Semantics.TargetConfigEquality_BoolEnumArray"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)

bool FGameModeOptionTargetConfigBoolEnumArrayTest::RunTest(const FString& Parameters)
{
	const FBS_TargetConfig Base;
	BS_TEST_TRUE("baseline: two default configs are equal", Base == FBS_TargetConfig());

	{
		FBS_TargetConfig C = Base;
		C.bAllowActivationWhileActivated = true;
		BS_TEST_FALSE("bAllowActivationWhileActivated", Base == C);
	}
	{
		FBS_TargetConfig C = Base;
		C.bAllowSpawnWithoutActivation = true;
		BS_TEST_FALSE("bAllowSpawnWithoutActivation", Base == C);
	}
	{
		FBS_TargetConfig C = Base;
		C.bSpawnAtOriginWheneverPossible = true;
		BS_TEST_FALSE("bSpawnAtOriginWheneverPossible", Base == C);
	}
	{
		FBS_TargetConfig C = Base;
		C.bSpawnEveryOtherTargetInCenter = true;
		BS_TEST_FALSE("bSpawnEveryOtherTargetInCenter", Base == C);
	}
	{
		FBS_TargetConfig C = Base;
		C.bUseBatchSpawning = true;
		BS_TEST_FALSE("bUseBatchSpawning", Base == C);
	}
	{
		FBS_TargetConfig C = Base;
		C.BoundsScalingPolicy = EBoundsScalingPolicy::Static;
		BS_TEST_FALSE("BoundsScalingPolicy", Base == C);
	}
	{
		FBS_TargetConfig C = Base;
		C.ConsecutiveTargetScalePolicy = EConsecutiveTargetScalePolicy::SkillBased;
		BS_TEST_FALSE("ConsecutiveTargetScalePolicy", Base == C);
	}
	{
		FBS_TargetConfig C = Base;
		C.MovingTargetDirectionMode = EMovingTargetDirectionMode::HorizontalOnly;
		BS_TEST_FALSE("MovingTargetDirectionMode", Base == C);
	}
	{
		FBS_TargetConfig C = Base;
		C.RecentTargetMemoryPolicy = ERecentTargetMemoryPolicy::CustomTimeBased;
		BS_TEST_FALSE("RecentTargetMemoryPolicy", Base == C);
	}
	{
		FBS_TargetConfig C = Base;
		C.TargetActivationSelectionPolicy = ETargetActivationSelectionPolicy::Random;
		BS_TEST_FALSE("TargetActivationSelectionPolicy", Base == C);
	}
	{
		FBS_TargetConfig C = Base;
		C.TargetDamageType = ETargetDamageType::Hit;
		BS_TEST_FALSE("TargetDamageType", Base == C);
	}
	{
		FBS_TargetConfig C = Base;
		C.TargetDistributionPolicy = ETargetDistributionPolicy::FullRange;
		BS_TEST_FALSE("TargetDistributionPolicy", Base == C);
	}
	{
		FBS_TargetConfig C = Base;
		C.TargetSpawningPolicy = ETargetSpawningPolicy::UpfrontOnly;
		BS_TEST_FALSE("TargetSpawningPolicy", Base == C);
	}
	{
		FBS_TargetConfig C = Base;
		C.RuntimeTargetSpawningLocationSelectionMode = ERuntimeTargetSpawningLocationSelectionMode::Random;
		BS_TEST_FALSE("RuntimeTargetSpawningLocationSelectionMode", Base == C);
	}
	{
		FBS_TargetConfig C = Base;
		C.TargetSpawnResponses.Add(ETargetSpawnResponse::AddImmunity);
		BS_TEST_FALSE("TargetSpawnResponses", Base == C);
	}
	{
		FBS_TargetConfig C = Base;
		C.TargetActivationResponses.Add(ETargetActivationResponse::AddImmunity);
		BS_TEST_FALSE("TargetActivationResponses", Base == C);
	}
	{
		FBS_TargetConfig C = Base;
		C.TargetDeactivationConditions.Add(ETargetDeactivationCondition::OnExpiration);
		BS_TEST_FALSE("TargetDeactivationConditions", Base == C);
	}
	{
		FBS_TargetConfig C = Base;
		C.TargetDeactivationResponses.Add(ETargetDeactivationResponse::ToggleImmunity);
		BS_TEST_FALSE("TargetDeactivationResponses", Base == C);
	}
	{
		FBS_TargetConfig C = Base;
		C.TargetDestructionConditions.Add(ETargetDestructionCondition::OnHealthReachedZero);
		BS_TEST_FALSE("TargetDestructionConditions", Base == C);
	}

	return true;
}


IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGameModeOptionTargetConfigNumericTest,
                                 TEXT("BeatShot.GameModeOptions.Semantics.TargetConfigEquality_Numeric"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)

bool FGameModeOptionTargetConfigNumericTest::RunTest(const FString& Parameters)
{
	const FBS_TargetConfig Base;

	// Each numeric option is compared (floats via IsNearlyEqual, ints strictly), so changing any one breaks
	// equality. A delta of 100 is far outside any float tolerance.
	{
		FBS_TargetConfig C = Base;
		C.BasePlayerHitDamage += 100.f;
		BS_TEST_FALSE("BasePlayerHitDamage", Base == C);
	}
	{
		FBS_TargetConfig C = Base;
		C.BasePlayerTrackingDamage += 100.f;
		BS_TEST_FALSE("BasePlayerTrackingDamage", Base == C);
	}
	{
		FBS_TargetConfig C = Base;
		C.ConsecutiveChargeScaleMultiplier += 100.f;
		BS_TEST_FALSE("ConsecutiveChargeScaleMultiplier", Base == C);
	}
	{
		FBS_TargetConfig C = Base;
		C.DeactivationHealthLostThreshold += 100.f;
		BS_TEST_FALSE("DeactivationHealthLostThreshold", Base == C);
	}
	{
		FBS_TargetConfig C = Base;
		C.LifetimeTargetScaleMultiplier += 100.f;
		BS_TEST_FALSE("LifetimeTargetScaleMultiplier", Base == C);
	}
	{
		FBS_TargetConfig C = Base;
		C.ExpirationHealthPenalty += 100.f;
		BS_TEST_FALSE("ExpirationHealthPenalty", Base == C);
	}
	{
		FBS_TargetConfig C = Base;
		C.FloorDistance += 100.f;
		BS_TEST_FALSE("FloorDistance", Base == C);
	}
	{
		FBS_TargetConfig C = Base;
		C.MinDistanceBetweenTargets += 100.f;
		BS_TEST_FALSE("MinDistanceBetweenTargets", Base == C);
	}
	{
		FBS_TargetConfig C = Base;
		C.MaxHealth += 100.f;
		BS_TEST_FALSE("MaxHealth", Base == C);
	}
	{
		FBS_TargetConfig C = Base;
		C.MinSpawnedTargetScale += 100.f;
		BS_TEST_FALSE("MinSpawnedTargetScale", Base == C);
	}
	{
		FBS_TargetConfig C = Base;
		C.MaxSpawnedTargetScale += 100.f;
		BS_TEST_FALSE("MaxSpawnedTargetScale", Base == C);
	}
	{
		FBS_TargetConfig C = Base;
		C.MinSpawnedTargetSpeed += 100.f;
		BS_TEST_FALSE("MinSpawnedTargetSpeed", Base == C);
	}
	{
		FBS_TargetConfig C = Base;
		C.MaxSpawnedTargetSpeed += 100.f;
		BS_TEST_FALSE("MaxSpawnedTargetSpeed", Base == C);
	}
	{
		FBS_TargetConfig C = Base;
		C.MinActivatedTargetSpeed += 100.f;
		BS_TEST_FALSE("MinActivatedTargetSpeed", Base == C);
	}
	{
		FBS_TargetConfig C = Base;
		C.MaxActivatedTargetSpeed += 100.f;
		BS_TEST_FALSE("MaxActivatedTargetSpeed", Base == C);
	}
	{
		FBS_TargetConfig C = Base;
		C.MinDeactivatedTargetSpeed += 100.f;
		BS_TEST_FALSE("MinDeactivatedTargetSpeed", Base == C);
	}
	{
		FBS_TargetConfig C = Base;
		C.MaxDeactivatedTargetSpeed += 100.f;
		BS_TEST_FALSE("MaxDeactivatedTargetSpeed", Base == C);
	}
	{
		FBS_TargetConfig C = Base;
		C.SpawnBeatDelay += 100.f;
		BS_TEST_FALSE("SpawnBeatDelay", Base == C);
	}
	{
		FBS_TargetConfig C = Base;
		C.RecentTargetTimeLength += 100.f;
		BS_TEST_FALSE("RecentTargetTimeLength", Base == C);
	}
	{
		FBS_TargetConfig C = Base;
		C.TargetMaxLifeSpan += 100.f;
		BS_TEST_FALSE("TargetMaxLifeSpan", Base == C);
	}
	{
		FBS_TargetConfig C = Base;
		C.TargetSpawnCD += 100.f;
		BS_TEST_FALSE("TargetSpawnCD", Base == C);
	}
	{
		FBS_TargetConfig C = Base;
		C.BoxBounds += FVector(100.f, 200.f, 300.f);
		BS_TEST_FALSE("BoxBounds", Base == C);
	}
	{
		FBS_TargetConfig C = Base;
		C.MaxNumActivatedTargetsAtOnce += 1;
		BS_TEST_FALSE("MaxNumActivatedTargetsAtOnce", Base == C);
	}
	{
		FBS_TargetConfig C = Base;
		C.MaxNumRecentTargets += 1;
		BS_TEST_FALSE("MaxNumRecentTargets", Base == C);
	}
	{
		FBS_TargetConfig C = Base;
		C.MaxNumTargetsAtOnce += 1;
		BS_TEST_FALSE("MaxNumTargetsAtOnce", Base == C);
	}
	{
		FBS_TargetConfig C = Base;
		C.MinNumTargetsToActivateAtOnce += 1;
		BS_TEST_FALSE("MinNumTargetsToActivateAtOnce", Base == C);
	}
	{
		FBS_TargetConfig C = Base;
		C.MaxNumTargetsToActivateAtOnce += 1;
		BS_TEST_FALSE("MaxNumTargetsToActivateAtOnce", Base == C);
	}
	{
		FBS_TargetConfig C = Base;
		C.NumRuntimeTargetsToSpawn += 1;
		BS_TEST_FALSE("NumRuntimeTargetsToSpawn", Base == C);
	}
	{
		FBS_TargetConfig C = Base;
		C.NumUpfrontTargetsToSpawn += 1;
		BS_TEST_FALSE("NumUpfrontTargetsToSpawn", Base == C);
	}

	return true;
}


IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGameModeOptionTargetConfigColorsTest,
                                 TEXT("BeatShot.GameModeOptions.Semantics.TargetConfigEquality_ColorsExcluded"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)

bool FGameModeOptionTargetConfigColorsTest::RunTest(const FString& Parameters)
{
	const FBS_TargetConfig Base;
	FBS_TargetConfig Tinted = Base;

	// The color fields (and the outline flag) are intentionally excluded from equality -- stylistic only.
	Tinted.bUseSeparateOutlineColor = true;
	Tinted.InactiveTargetColor = FLinearColor(1.f, 0.f, 0.f, 1.f);
	Tinted.OnSpawnColor = FLinearColor(0.f, 1.f, 0.f, 1.f);
	Tinted.StartColor = FLinearColor(0.f, 0.f, 1.f, 1.f);
	Tinted.PeakColor = FLinearColor(1.f, 1.f, 0.f, 1.f);
	Tinted.EndColor = FLinearColor(1.f, 0.f, 1.f, 1.f);
	Tinted.OutlineColor = FLinearColor(0.f, 1.f, 1.f, 1.f);
	Tinted.TakingTrackingDamageColor = FLinearColor(1.f, 1.f, 1.f, 0.5f);
	Tinted.NotTakingTrackingDamageColor = FLinearColor(0.5f, 0.5f, 0.5f, 0.5f);

	BS_TEST_TRUE("colors + outline flag are intentionally excluded from equality", Base == Tinted);
	BS_TEST_TRUE("symmetric", Tinted == Base);

	return true;
}


IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGameModeOptionAIConfigTest,
                                 TEXT("BeatShot.GameModeOptions.Semantics.AIConfigEquality"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)

bool FGameModeOptionAIConfigTest::RunTest(const FString& Parameters)
{
	const FBS_AIConfig Base;
	BS_TEST_TRUE("baseline equal", Base == FBS_AIConfig());

	{
		FBS_AIConfig C = Base;
		C.bEnableReinforcementLearning = true;
		BS_TEST_FALSE("bEnableReinforcementLearning", Base == C);
	}
	{
		FBS_AIConfig C = Base;
		C.Alpha = 0.5f;
		BS_TEST_FALSE("Alpha", Base == C);
	}
	{
		FBS_AIConfig C = Base;
		C.Epsilon = 0.5f;
		BS_TEST_FALSE("Epsilon", Base == C);
	}
	{
		FBS_AIConfig C = Base;
		C.Gamma = 0.5f;
		BS_TEST_FALSE("Gamma", Base == C);
	}
	{
		FBS_AIConfig C = Base;
		C.ReinforcementLearningMode = EReinforcementLearningMode::Training;
		BS_TEST_FALSE("ReinforcementLearningMode", Base == C);
	}
	{
		FBS_AIConfig C = Base;
		C.HyperParameterMode = EReinforcementLearningHyperParameterMode::Custom;
		BS_TEST_FALSE("HyperParameterMode", Base == C);
	}

	return true;
}


IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGameModeOptionGridConfigTest,
                                 TEXT("BeatShot.GameModeOptions.Semantics.GridConfigEquality"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)

bool FGameModeOptionGridConfigTest::RunTest(const FString& Parameters)
{
	const FBS_GridConfig Base;
	BS_TEST_TRUE("baseline equal", Base == FBS_GridConfig());

	{
		FBS_GridConfig C = Base;
		C.NumHorizontalGridTargets += 1;
		BS_TEST_FALSE("NumHorizontalGridTargets", Base == C);
	}
	{
		FBS_GridConfig C = Base;
		C.NumVerticalGridTargets += 1;
		BS_TEST_FALSE("NumVerticalGridTargets", Base == C);
	}
	{
		FBS_GridConfig C = Base;
		C.GridSpacing += FVector2D(25.f, 15.f);
		BS_TEST_FALSE("GridSpacing", Base == C);
	}
	{
		FBS_GridConfig C = Base;
		C.NumGridTargetsVisibleAtOnce += 1;
		BS_TEST_FALSE("NumGridTargetsVisibleAtOnce", Base == C);
	}

	return true;
}


IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGameModeOptionDynamicTargetScalingTest,
                                 TEXT("BeatShot.GameModeOptions.Semantics.DynamicTargetScalingEquality"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)

bool FGameModeOptionDynamicTargetScalingTest::RunTest(const FString& Parameters)
{
	const FBS_Dynamic Base;
	BS_TEST_TRUE("baseline equal", Base == FBS_Dynamic());

	{
		FBS_Dynamic C = Base;
		C.StartThreshold += 1;
		BS_TEST_FALSE("StartThreshold", Base == C);
	}
	{
		FBS_Dynamic C = Base;
		C.EndThreshold += 1;
		BS_TEST_FALSE("EndThreshold", Base == C);
	}
	{
		FBS_Dynamic C = Base;
		C.bIsCubicInterpolation = true;
		BS_TEST_FALSE("bIsCubicInterpolation", Base == C);
	}
	{
		FBS_Dynamic C = Base;
		C.DecrementAmount += 1;
		BS_TEST_FALSE("DecrementAmount", Base == C);
	}

	return true;
}


IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGameModeOptionDynamicSpawnAreaTest,
                                 TEXT("BeatShot.GameModeOptions.Semantics.DynamicSpawnAreaScalingEquality"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)

bool FGameModeOptionDynamicSpawnAreaTest::RunTest(const FString& Parameters)
{
	const FBS_Dynamic_SpawnArea Base;
	BS_TEST_TRUE("baseline equal", Base == FBS_Dynamic_SpawnArea());

	// The inherited FBS_Dynamic fields are compared...
	{
		FBS_Dynamic_SpawnArea C = Base;
		C.StartThreshold += 1;
		BS_TEST_FALSE("StartThreshold", Base == C);
	}
	{
		FBS_Dynamic_SpawnArea C = Base;
		C.EndThreshold += 1;
		BS_TEST_FALSE("EndThreshold", Base == C);
	}
	{
		FBS_Dynamic_SpawnArea C = Base;
		C.bIsCubicInterpolation = true;
		BS_TEST_FALSE("bIsCubicInterpolation", Base == C);
	}
	{
		FBS_Dynamic_SpawnArea C = Base;
		C.DecrementAmount += 1;
		BS_TEST_FALSE("DecrementAmount", Base == C);
	}
	// ...as is the added StartBounds (compared with a 0.1 tolerance, so a 50 shift is well outside it).
	{
		FBS_Dynamic_SpawnArea C = Base;
		C.StartBounds += FVector(50.f, 60.f, 70.f);
		BS_TEST_FALSE("StartBounds", Base == C);
	}

	return true;
}


IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGameModeOptionRLCompatibilityTest,
                                 TEXT("BeatShot.GameModeOptions.Semantics.RLCompatibility"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)

bool FGameModeOptionRLCompatibilityTest::RunTest(const FString& Parameters)
{
	auto Compatible = [](const ETargetDamageType DamageType)
	{
		FBSConfig Cfg;
		Cfg.TargetConfig.TargetDamageType = DamageType;
		return Cfg.IsCompatibleWithReinforcementLearning();
	};

	// Tracking is the only damage type the RL agent cannot drive.
	BS_TEST_TRUE("Hit is compatible", Compatible(ETargetDamageType::Hit));
	BS_TEST_TRUE("Combined is compatible", Compatible(ETargetDamageType::Combined));
	BS_TEST_TRUE("Self is compatible", Compatible(ETargetDamageType::Self));
	BS_TEST_TRUE("None is compatible", Compatible(ETargetDamageType::None));
	BS_TEST_FALSE("Tracking is incompatible", Compatible(ETargetDamageType::Tracking));

	return true;
}


IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGameModeOptionIdentityOnlyTest,
                                 TEXT("BeatShot.GameModeOptions.Semantics.ConfigEqualityIsIdentityOnly"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)

bool FGameModeOptionIdentityOnlyTest::RunTest(const FString& Parameters)
{
	// FBSConfig equality is defined by the defining config (identity) alone: the gameplay options do NOT
	// factor in. Two configs with the same name are the "same game mode" regardless of their options...
	FBSConfig A;
	A.DefiningConfig = MakeCustomIdentity(TEXT("My Mode"));
	FBSConfig B;
	B.DefiningConfig = MakeCustomIdentity(TEXT("my mode"));
	B.TargetConfig.MaxHealth = 5000.f;
	B.TargetConfig.TargetDamageType = ETargetDamageType::Combined;
	B.AIConfig.Alpha = 0.1f;
	B.AIConfig.bEnableReinforcementLearning = true;
	B.GridConfig.NumHorizontalGridTargets = 99;
	B.DynamicSpawnAreaScaling.StartBounds = FVector(50.f, 60.f, 70.f);
	BS_TEST_TRUE("same identity, different options -> equal", A == B);
	BS_TEST_TRUE("symmetric", B == A);

	// ...while a different identity is never equal, even with byte-identical options.
	FBSConfig C;
	C.DefiningConfig = MakeCustomIdentity(TEXT("Other Mode"));
	BS_TEST_FALSE("different identity, identical options -> not equal", A == C);

	// A custom and a preset are different types, so never equal regardless of options.
	FBSConfig D;
	D.DefiningConfig = FBS_DefiningConfig(EGameModeType::Preset, EBaseGameMode::MultiBeat, TEXT(""),
	                                      EGameModeDifficulty::Normal);
	BS_TEST_FALSE("custom vs preset never equal", A == D);

	return true;
}
