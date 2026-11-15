// Copyright 2022-2026 Markoleptic Games, SP. All Rights Reserved.

#include "CoreMinimal.h"
#include "BSTestAssertions.h"
#include "BSGameModeConfig/BSConfig.h"
#include "BSGameModeConfig/BSGameModeValidator.h"
#include "Misc/AutomationTest.h"

namespace
{
/** Finds the first result (succeeded or failed) whose check carries InCheckId. */
const FValidationCheckResult* FindResult(const FValidationResult& Result,
                                         const EValidationCheckId InCheckId,
                                         const bool bInSucceeded)
{
	const FValidationCheckResultSet& CheckSet = bInSucceeded ? Result.GetSucceeded() : Result.GetFailed();
	for (const FValidationCheckResult& CheckResult : CheckSet)
	{
		if (CheckResult.ValidationCheckPtr && CheckResult.ValidationCheckPtr->CheckId == InCheckId)
		{
			return &CheckResult;
		}
	}
	return nullptr;
}

bool Succeeds(const FValidationResult& Result, const EValidationCheckId InCheckId)
{
	return FindResult(Result, InCheckId, true) != nullptr;
}

bool Fails(const FValidationResult& Result, const EValidationCheckId InCheckId)
{
	return FindResult(Result, InCheckId, false) != nullptr;
}

/** Validates only the property identified by InPropertyHash of InConfig. */
FValidationResult ValidateProperty(UBSGameModeValidator& Validator,
                                   const FBSConfig& InConfig,
                                   const uint32 InPropertyHash)
{
	const TSharedPtr<FBSConfig> SharedConfig = MakeShareable(new FBSConfig(InConfig));
	TSet<uint32> PropertyHashes;
	PropertyHashes.Add(InPropertyHash);
	return Validator.Validate(SharedConfig, PropertyHashes);
}

/** Human-readable name of a check id, for assertion messages. */
const TCHAR* CheckIdToName(const EValidationCheckId InCheckId)
{
	switch (InCheckId)
	{
	case EValidationCheckId::None:
		return TEXT("None");
	case EValidationCheckId::SpawnEveryOtherTargetInCenter_BatchSpawning:
		return TEXT("SpawnEveryOtherTargetInCenter_BatchSpawning");
	case EValidationCheckId::BatchSpawning_SpawnEveryOtherTargetInCenter:
		return TEXT("BatchSpawning_SpawnEveryOtherTargetInCenter");
	case EValidationCheckId::SpawnEveryOtherTargetInCenter_AllowSpawnWithoutActivation:
		return TEXT("SpawnEveryOtherTargetInCenter_AllowSpawnWithoutActivation");
	case EValidationCheckId::AllowSpawnWithoutActivation_SpawnEveryOtherTargetInCenter:
		return TEXT("AllowSpawnWithoutActivation_SpawnEveryOtherTargetInCenter");
	case EValidationCheckId::ChangeVelocity_MTDNone_MTDM:
		return TEXT("ChangeVelocity_MTDNone_MTDM");
	case EValidationCheckId::ChangeDirection_MTDNone_MTDM:
		return TEXT("ChangeDirection_MTDNone_MTDM");
	case EValidationCheckId::ChangeVelocity_MTDNone_SpawnResponses:
		return TEXT("ChangeVelocity_MTDNone_SpawnResponses");
	case EValidationCheckId::ChangeDirection_MTDNone_SpawnResponses:
		return TEXT("ChangeDirection_MTDNone_SpawnResponses");
	case EValidationCheckId::ChangeVelocity_MTDNone_ActivationResponses:
		return TEXT("ChangeVelocity_MTDNone_ActivationResponses");
	case EValidationCheckId::ChangeDirection_MTDNone_ActivationResponses:
		return TEXT("ChangeDirection_MTDNone_ActivationResponses");
	case EValidationCheckId::ChangeVelocity_MTDNone_DeactivationResponses:
		return TEXT("ChangeVelocity_MTDNone_DeactivationResponses");
	case EValidationCheckId::ChangeDirection_MTDNone_DeactivationResponses:
		return TEXT("ChangeDirection_MTDNone_DeactivationResponses");
	case EValidationCheckId::ZeroForwardDistance_ForwardOnly_MTDM:
		return TEXT("ZeroForwardDistance_ForwardOnly_MTDM");
	case EValidationCheckId::ZeroForwardDistance_ForwardOnly_BoxBoundsX:
		return TEXT("ZeroForwardDistance_ForwardOnly_BoxBoundsX");
	case EValidationCheckId::Grid_NumHorizontalTargets:
		return TEXT("Grid_NumHorizontalTargets");
	case EValidationCheckId::Grid_NumVerticalTargets:
		return TEXT("Grid_NumVerticalTargets");
	case EValidationCheckId::Grid_HorizontalSpacing:
		return TEXT("Grid_HorizontalSpacing");
	case EValidationCheckId::Grid_VerticalSpacing:
		return TEXT("Grid_VerticalSpacing");
	case EValidationCheckId::Grid_MaxSpawnedTargetScale:
		return TEXT("Grid_MaxSpawnedTargetScale");
	case EValidationCheckId::HeadshotHeightOnly_ReinforcementLearning_Policy:
		return TEXT("HeadshotHeightOnly_ReinforcementLearning_Policy");
	case EValidationCheckId::HeadshotHeightOnly_ReinforcementLearning_EnableRL:
		return TEXT("HeadshotHeightOnly_ReinforcementLearning_EnableRL");
	case EValidationCheckId::Tracking_ReinforcementLearning_EnableRL:
		return TEXT("Tracking_ReinforcementLearning_EnableRL");
	case EValidationCheckId::Tracking_ReinforcementLearning_TargetDamageType:
		return TEXT("Tracking_ReinforcementLearning_TargetDamageType");
	}
	return TEXT("Unknown");
}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGameModeValidatorSpawnEveryOtherBatchSpawningTest,
                                 TEXT("BeatShot.GameModeValidator.SpawnEveryOtherTargetInCenter_BatchSpawning"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)

bool FGameModeValidatorSpawnEveryOtherBatchSpawningTest::RunTest(const FString& Parameters)
{
	UBSGameModeValidator* Validator = NewObject<UBSGameModeValidator>();
	const uint32 PropertyHash = UBSGameModeValidator::FindBSConfigProperty(
		TEXT("TargetConfig"), TEXT("bSpawnEveryOtherTargetInCenter"));
	const EValidationCheckId CheckId = EValidationCheckId::SpawnEveryOtherTargetInCenter_BatchSpawning;
	const EValidationCheckId DependentCheckId = EValidationCheckId::BatchSpawning_SpawnEveryOtherTargetInCenter;

	auto ValidateWith = [&](const bool InSpawnEveryOther, const bool InBatchSpawning)
	{
		FBSConfig Cfg;
		Cfg.TargetConfig.bSpawnEveryOtherTargetInCenter = InSpawnEveryOther;
		Cfg.TargetConfig.bUseBatchSpawning = InBatchSpawning;
		return ValidateProperty(*Validator, Cfg, PropertyHash);
	};

	// Both set: every copy of the rule fails.
	const FValidationResult Triggered = ValidateWith(true, true);
	const FValidationCheckResult* TriggeredCheck = FindResult(Triggered, CheckId, false);
	BS_TEST_REQUIRE("both set: fails", TriggeredCheck != nullptr);
	BS_TEST_EQUAL("warning severity", TriggeredCheck->WarningType, EGameModeWarningType::Warning);
	BS_TEST_TRUE("both set: dependent copy fails", Fails(Triggered, DependentCheckId));

	// Only one side set: the rule disarms.
	BS_TEST_TRUE("SpawnEveryOther alone: passes", Succeeds(ValidateWith(true, false), CheckId));
	BS_TEST_TRUE("BatchSpawning alone: passes", Succeeds(ValidateWith(false, true), CheckId));
	BS_TEST_TRUE("neither: passes", Succeeds(ValidateWith(false, false), CheckId));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGameModeValidatorSpawnEveryOtherAllowSpawnTest,
                                 TEXT(
	                                 "BeatShot.GameModeValidator.SpawnEveryOtherTargetInCenter_AllowSpawnWithoutActivation"
                                 ), EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)

bool FGameModeValidatorSpawnEveryOtherAllowSpawnTest::RunTest(const FString& Parameters)
{
	UBSGameModeValidator* Validator = NewObject<UBSGameModeValidator>();
	const uint32 PropertyHash = UBSGameModeValidator::FindBSConfigProperty(
		TEXT("TargetConfig"), TEXT("bAllowSpawnWithoutActivation"));
	const EValidationCheckId CheckId = EValidationCheckId::SpawnEveryOtherTargetInCenter_AllowSpawnWithoutActivation;
	const EValidationCheckId DependentCheckId =
		EValidationCheckId::AllowSpawnWithoutActivation_SpawnEveryOtherTargetInCenter;

	auto ValidateWith = [&](const bool InSpawnEveryOther, const bool InAllowSpawnWithoutActivation)
	{
		FBSConfig Cfg;
		Cfg.TargetConfig.bSpawnEveryOtherTargetInCenter = InSpawnEveryOther;
		Cfg.TargetConfig.bAllowSpawnWithoutActivation = InAllowSpawnWithoutActivation;
		return ValidateProperty(*Validator, Cfg, PropertyHash);
	};

	// Both set: every copy of the rule fails.
	const FValidationResult Triggered = ValidateWith(true, true);
	const FValidationCheckResult* TriggeredCheck = FindResult(Triggered, CheckId, false);
	BS_TEST_REQUIRE("both set: fails", TriggeredCheck != nullptr);
	BS_TEST_EQUAL("warning severity", TriggeredCheck->WarningType, EGameModeWarningType::Warning);
	BS_TEST_TRUE("both set: dependent copy fails", Fails(Triggered, DependentCheckId));

	// Only one side set: the rule disarms.
	BS_TEST_TRUE("SpawnEveryOther alone: passes", Succeeds(ValidateWith(true, false), CheckId));
	BS_TEST_TRUE("AllowSpawnWithoutActivation alone: passes", Succeeds(ValidateWith(false, true), CheckId));
	BS_TEST_TRUE("neither: passes", Succeeds(ValidateWith(false, false), CheckId));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGameModeValidatorMtdmChangeVelocityTest,
                                 TEXT("BeatShot.GameModeValidator.MovingTargetNone_ChangeVelocity"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)

bool FGameModeValidatorMtdmChangeVelocityTest::RunTest(const FString& Parameters)
{
	UBSGameModeValidator* Validator = NewObject<UBSGameModeValidator>();
	const uint32 PropertyHash = UBSGameModeValidator::FindBSConfigProperty(
		TEXT("TargetConfig"), TEXT("MovingTargetDirectionMode"));
	const EValidationCheckId CheckId = EValidationCheckId::ChangeVelocity_MTDNone_SpawnResponses;
	const EValidationCheckId DependentCheckId = EValidationCheckId::ChangeVelocity_MTDNone_MTDM;
	const EValidationCheckId DirectionCheckId = EValidationCheckId::ChangeDirection_MTDNone_SpawnResponses;

	// A ChangeVelocity response in any of the three arrays with no moving-target direction mode fails.
	FBSConfig SpawnTriggered;
	SpawnTriggered.TargetConfig.TargetSpawnResponses = {ETargetSpawnResponse::ChangeVelocity};
	const FValidationResult SpawnResult = ValidateProperty(*Validator, SpawnTriggered, PropertyHash);
	const FValidationCheckResult* SpawnCheck = FindResult(SpawnResult, CheckId, false);
	BS_TEST_REQUIRE("spawn response: fails", SpawnCheck != nullptr);
	BS_TEST_EQUAL("caution severity", SpawnCheck->WarningType, EGameModeWarningType::Caution);
	BS_TEST_TRUE("Mtdm side: fails", Fails(SpawnResult, DependentCheckId));
	BS_TEST_TRUE("direction rule stays passing", Succeeds(SpawnResult, DirectionCheckId));

	FBSConfig ActivationTriggered;
	ActivationTriggered.TargetConfig.TargetActivationResponses = {ETargetActivationResponse::ChangeVelocity};
	BS_TEST_TRUE("activation response: fails",
	             Fails(ValidateProperty(*Validator, ActivationTriggered, PropertyHash), EValidationCheckId::
		             ChangeVelocity_MTDNone_ActivationResponses));

	FBSConfig DeactivationTriggered;
	DeactivationTriggered.TargetConfig.TargetDeactivationResponses = {ETargetDeactivationResponse::ChangeVelocity};
	BS_TEST_TRUE("deactivation response: fails",
	             Fails(ValidateProperty(*Validator, DeactivationTriggered, PropertyHash), EValidationCheckId::
		             ChangeVelocity_MTDNone_DeactivationResponses));

	// A non-None moving-target direction mode disarms the rule.
	FBSConfig ArmedMode;
	ArmedMode.TargetConfig.MovingTargetDirectionMode = EMovingTargetDirectionMode::Any;
	ArmedMode.TargetConfig.TargetSpawnResponses = {ETargetSpawnResponse::ChangeVelocity};
	BS_TEST_TRUE("direction mode Any: passes",
	             Succeeds(ValidateProperty(*Validator, ArmedMode, PropertyHash), CheckId));

	// No responses at all: the rule disarms.
	FBSConfig NoResponses;
	BS_TEST_TRUE("no responses: passes", Succeeds(ValidateProperty(*Validator, NoResponses, PropertyHash), CheckId));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGameModeValidatorMtdmChangeDirectionTest,
                                 TEXT("BeatShot.GameModeValidator.MovingTargetNone_ChangeDirection"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)

bool FGameModeValidatorMtdmChangeDirectionTest::RunTest(const FString& Parameters)
{
	UBSGameModeValidator* Validator = NewObject<UBSGameModeValidator>();
	const uint32 PropertyHash = UBSGameModeValidator::FindBSConfigProperty(
		TEXT("TargetConfig"), TEXT("MovingTargetDirectionMode"));
	const EValidationCheckId CheckId = EValidationCheckId::ChangeDirection_MTDNone_SpawnResponses;
	const EValidationCheckId DependentCheckId = EValidationCheckId::ChangeDirection_MTDNone_MTDM;
	const EValidationCheckId VelocityCheckId = EValidationCheckId::ChangeVelocity_MTDNone_SpawnResponses;

	// A ChangeDirection response in any of the three arrays with no moving-target direction mode fails.
	FBSConfig SpawnTriggered;
	SpawnTriggered.TargetConfig.TargetSpawnResponses = {ETargetSpawnResponse::ChangeDirection};
	const FValidationResult SpawnResult = ValidateProperty(*Validator, SpawnTriggered, PropertyHash);
	const FValidationCheckResult* SpawnCheck = FindResult(SpawnResult, CheckId, false);
	BS_TEST_REQUIRE("spawn response: fails", SpawnCheck != nullptr);
	BS_TEST_EQUAL("caution severity", SpawnCheck->WarningType, EGameModeWarningType::Caution);
	BS_TEST_TRUE("Mtdm side: fails", Fails(SpawnResult, DependentCheckId));
	BS_TEST_TRUE("velocity rule stays passing", Succeeds(SpawnResult, VelocityCheckId));

	FBSConfig ActivationTriggered;
	ActivationTriggered.TargetConfig.TargetActivationResponses = {ETargetActivationResponse::ChangeDirection};
	BS_TEST_TRUE("activation response: fails",
	             Fails(ValidateProperty(*Validator, ActivationTriggered, PropertyHash), EValidationCheckId::
		             ChangeDirection_MTDNone_ActivationResponses));

	FBSConfig DeactivationTriggered;
	DeactivationTriggered.TargetConfig.TargetDeactivationResponses = {ETargetDeactivationResponse::ChangeDirection};
	BS_TEST_TRUE("deactivation response: fails",
	             Fails(ValidateProperty(*Validator, DeactivationTriggered, PropertyHash), EValidationCheckId::
		             ChangeDirection_MTDNone_DeactivationResponses));

	// A non-None moving-target direction mode disarms the rule.
	FBSConfig ArmedMode;
	ArmedMode.TargetConfig.MovingTargetDirectionMode = EMovingTargetDirectionMode::HorizontalOnly;
	ArmedMode.TargetConfig.TargetSpawnResponses = {ETargetSpawnResponse::ChangeDirection};
	BS_TEST_TRUE("direction mode HorizontalOnly: passes",
	             Succeeds(ValidateProperty(*Validator, ArmedMode, PropertyHash), CheckId));

	// No responses at all: the rule disarms.
	FBSConfig NoResponses;
	BS_TEST_TRUE("no responses: passes", Succeeds(ValidateProperty(*Validator, NoResponses, PropertyHash), CheckId));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGameModeValidatorBoxBoundsForwardOnlyTest,
                                 TEXT("BeatShot.GameModeValidator.BoxBoundsX_Zero_ForwardOnly"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)

bool FGameModeValidatorBoxBoundsForwardOnlyTest::RunTest(const FString& Parameters)
{
	UBSGameModeValidator* Validator = NewObject<UBSGameModeValidator>();
	const uint32 PropertyHash = UBSGameModeValidator::FindBSConfigProperty(TEXT("TargetConfig"), TEXT("BoxBounds"),
	                                                                       TEXT("X"));
	const EValidationCheckId CheckId = EValidationCheckId::ZeroForwardDistance_ForwardOnly_MTDM;
	const EValidationCheckId BoxBoundsXCheckId = EValidationCheckId::ZeroForwardDistance_ForwardOnly_BoxBoundsX;

	auto ValidateWith = [&](const float InBoxBoundsX, const EMovingTargetDirectionMode InMode)
	{
		FBSConfig Cfg;
		Cfg.TargetConfig.BoxBounds = FVector(InBoxBoundsX, 3200.f, 1000.f);
		Cfg.TargetConfig.MovingTargetDirectionMode = InMode;
		return ValidateProperty(*Validator, Cfg, PropertyHash);
	};

	// Zero forward distance with the ForwardOnly movement mode fails.
	const FValidationResult Triggered = ValidateWith(0.f, EMovingTargetDirectionMode::ForwardOnly);
	const FValidationCheckResult* TriggeredCheck = FindResult(Triggered, CheckId, false);
	BS_TEST_REQUIRE("X=0 + ForwardOnly: fails", TriggeredCheck != nullptr);
	BS_TEST_EQUAL("caution severity", TriggeredCheck->WarningType, EGameModeWarningType::Caution);
	BS_TEST_TRUE("X=0 + ForwardOnly: BoxBoundsX copy fails", Fails(Triggered, BoxBoundsXCheckId));

	// Either half of the condition disarms the rule.
	BS_TEST_TRUE("X>0 + ForwardOnly: passes",
	             Succeeds(ValidateWith(100.f, EMovingTargetDirectionMode::ForwardOnly), CheckId));
	BS_TEST_TRUE("X=0 + HorizontalOnly: passes",
	             Succeeds(ValidateWith(0.f, EMovingTargetDirectionMode::HorizontalOnly), CheckId));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGameModeValidatorGridNumHorizontalTest,
                                 TEXT("BeatShot.GameModeValidator.GridNumHorizontalTargets"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)

bool FGameModeValidatorGridNumHorizontalTest::RunTest(const FString& Parameters)
{
	UBSGameModeValidator* Validator = NewObject<UBSGameModeValidator>();
	const uint32 PropertyHash = UBSGameModeValidator::FindBSConfigProperty(
		TEXT("GridConfig"), TEXT("NumHorizontalGridTargets"));
	const EValidationCheckId CheckId = EValidationCheckId::Grid_NumHorizontalTargets;

	auto MakeGridConfig = [](const int32 InNumHorizontal, const float InSpacingX)
	{
		FBSConfig Cfg;
		Cfg.TargetConfig.TargetDistributionPolicy = ETargetDistributionPolicy::Grid;
		Cfg.GridConfig.NumHorizontalGridTargets = InNumHorizontal;
		Cfg.GridConfig.GridSpacing.X = InSpacingX;
		return Cfg;
	};

	// Off-grid distribution policy: the check bypasses (does not run) and counts as passing.
	FBSConfig Bypassed;
	Bypassed.TargetConfig.TargetDistributionPolicy = ETargetDistributionPolicy::FullRange;
	const FValidationResult BypassedResult = ValidateProperty(*Validator, Bypassed, PropertyHash);
	const FValidationCheckResult* BypassedCheck = FindResult(BypassedResult, CheckId, true);
	BS_TEST_REQUIRE("off-grid: recorded as passing", BypassedCheck != nullptr);
	BS_TEST_TRUE("off-grid: bypassed", BypassedCheck->bBypassed);

	// Triggered: 13 targets but only 12 fit: 3200 / (75 + 200) + 1 = 12.
	const FValidationResult TriggeredResult = ValidateProperty(*Validator, MakeGridConfig(13, 75.f), PropertyHash);
	const FValidationCheckResult* TriggeredCheck = FindResult(TriggeredResult, CheckId, false);
	BS_TEST_REQUIRE("13 targets (12 allowed): fails", TriggeredCheck != nullptr);
	const FValidationCheckData& TriggeredData = TriggeredCheck->ValidationCheckPtr->ValidationCheckData;
	BS_TEST_EQUAL("warning severity", TriggeredCheck->WarningType, EGameModeWarningType::Warning);
	BS_TEST_EQUAL("MaxAllowed reported", TriggeredData.CalculatedValues.Num(), 1);
	BS_TEST_TRUE("MaxAllowed value",
	             TriggeredData.CalculatedValues.Num() == 1 && FMath::IsNearlyEqual(TriggeredData.CalculatedValues[0],
		             12.f));
	BS_TEST_FALSE("MaxAllowed above minimum: no flag", TriggeredData.bRequireOtherPropertiesToBeChanged);

	// Triggered: MaxAllowed (1) below the minimum of 2; validation demands another property to change.
	const FValidationResult BelowMinResult = ValidateProperty(*Validator, MakeGridConfig(10, 3500.f), PropertyHash);
	const FValidationCheckResult* BelowMinCheck = FindResult(BelowMinResult, CheckId, false);
	BS_TEST_REQUIRE("MaxAllowed below minimum: fails", BelowMinCheck != nullptr);
	const FValidationCheckData& BelowMinData = BelowMinCheck->ValidationCheckPtr->ValidationCheckData;
	BS_TEST_TRUE("demands another property change", BelowMinData.bRequireOtherPropertiesToBeChanged);
	BS_TEST_EQUAL("no MaxAllowed reported", BelowMinData.CalculatedValues.Num(), 0);

	// Passing boundary: exactly 12 targets fit.
	BS_TEST_TRUE("12 targets (12 allowed): passes",
	             Succeeds(ValidateProperty(*Validator, MakeGridConfig(12, 75.f), PropertyHash), CheckId));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGameModeValidatorGridNumVerticalTest,
                                 TEXT("BeatShot.GameModeValidator.GridNumVerticalTargets"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)

bool FGameModeValidatorGridNumVerticalTest::RunTest(const FString& Parameters)
{
	UBSGameModeValidator* Validator = NewObject<UBSGameModeValidator>();
	const uint32 PropertyHash = UBSGameModeValidator::FindBSConfigProperty(
		TEXT("GridConfig"), TEXT("NumVerticalGridTargets"));
	const EValidationCheckId CheckId = EValidationCheckId::Grid_NumVerticalTargets;

	auto MakeGridConfig = [](const int32 InNumVertical, const float InSpacingY)
	{
		FBSConfig Cfg;
		Cfg.TargetConfig.TargetDistributionPolicy = ETargetDistributionPolicy::Grid;
		Cfg.GridConfig.NumVerticalGridTargets = InNumVertical;
		Cfg.GridConfig.GridSpacing.Y = InSpacingY;
		return Cfg;
	};

	// Off-grid distribution policy: the check bypasses (does not run) and counts as passing.
	FBSConfig Bypassed;
	Bypassed.TargetConfig.TargetDistributionPolicy = ETargetDistributionPolicy::EdgeOnly;
	const FValidationResult BypassedResult = ValidateProperty(*Validator, Bypassed, PropertyHash);
	const FValidationCheckResult* BypassedCheck = FindResult(BypassedResult, CheckId, true);
	BS_TEST_REQUIRE("off-grid: recorded as passing", BypassedCheck != nullptr);
	BS_TEST_TRUE("off-grid: bypassed", BypassedCheck->bBypassed);

	// Triggered: 6 targets but only 5 fit: 1000 / (50 + 200) + 1 = 5.
	const FValidationResult TriggeredResult = ValidateProperty(*Validator, MakeGridConfig(6, 50.f), PropertyHash);
	const FValidationCheckResult* TriggeredCheck = FindResult(TriggeredResult, CheckId, false);
	BS_TEST_REQUIRE("6 targets (5 allowed): fails", TriggeredCheck != nullptr);
	const FValidationCheckData& TriggeredData = TriggeredCheck->ValidationCheckPtr->ValidationCheckData;
	BS_TEST_EQUAL("warning severity", TriggeredCheck->WarningType, EGameModeWarningType::Warning);
	BS_TEST_TRUE("MaxAllowed value",
	             TriggeredData.CalculatedValues.Num() == 1 && FMath::IsNearlyEqual(TriggeredData.CalculatedValues[0],
		             5.f));

	// Triggered: MaxAllowed (1) below the minimum of 2.
	const FValidationResult BelowMinResult = ValidateProperty(*Validator, MakeGridConfig(10, 1200.f), PropertyHash);
	const FValidationCheckResult* BelowMinCheck = FindResult(BelowMinResult, CheckId, false);
	BS_TEST_REQUIRE("MaxAllowed below minimum: fails", BelowMinCheck != nullptr);
	BS_TEST_TRUE("demands another property change",
	             BelowMinCheck->ValidationCheckPtr->ValidationCheckData.bRequireOtherPropertiesToBeChanged);

	// Passing boundary: exactly 5 targets fit.
	BS_TEST_TRUE("5 targets (5 allowed): passes",
	             Succeeds(ValidateProperty(*Validator, MakeGridConfig(5, 50.f), PropertyHash), CheckId));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGameModeValidatorGridSpacingXTest, TEXT("BeatShot.GameModeValidator.GridSpacingX"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)

bool FGameModeValidatorGridSpacingXTest::RunTest(const FString& Parameters)
{
	UBSGameModeValidator* Validator = NewObject<UBSGameModeValidator>();
	const uint32 PropertyHash = UBSGameModeValidator::FindBSConfigProperty(TEXT("GridConfig"), TEXT("GridSpacing"),
	                                                                       TEXT("X"));
	const EValidationCheckId CheckId = EValidationCheckId::Grid_HorizontalSpacing;

	auto MakeGridConfig = [](const float InSpacingX, const int32 InNumHorizontal, const float InMaxSpawnedScale)
	{
		FBSConfig Cfg;
		Cfg.TargetConfig.TargetDistributionPolicy = ETargetDistributionPolicy::Grid;
		Cfg.TargetConfig.MaxSpawnedTargetScale = InMaxSpawnedScale;
		Cfg.GridConfig.NumHorizontalGridTargets = InNumHorizontal;
		Cfg.GridConfig.GridSpacing.X = InSpacingX;
		return Cfg;
	};

	// Off-grid distribution policy: the check bypasses (does not run) and counts as passing.
	FBSConfig Bypassed;
	Bypassed.TargetConfig.TargetDistributionPolicy = ETargetDistributionPolicy::HeadshotHeightOnly;
	const FValidationResult BypassedResult = ValidateProperty(*Validator, Bypassed, PropertyHash);
	const FValidationCheckResult* BypassedCheck = FindResult(BypassedResult, CheckId, true);
	BS_TEST_REQUIRE("off-grid: recorded as passing", BypassedCheck != nullptr);
	BS_TEST_TRUE("off-grid: bypassed", BypassedCheck->bBypassed);

	// Triggered: max allowed is 3200 / 11 - 200 = 90.909..., stored rounded to 2 decimals (90.91).
	const float MaxAllowed = 3200.f / (12 - 1) - 200.f;
	const float ExpectedCalculated = FMath::RoundHalfToZero(MaxAllowed * 100) / 100.f;
	const FValidationResult TriggeredResult = ValidateProperty(*Validator, MakeGridConfig(100.f, 12, 2.f),
	                                                           PropertyHash);
	const FValidationCheckResult* TriggeredCheck = FindResult(TriggeredResult, CheckId, false);
	BS_TEST_REQUIRE("spacing 100 (90.91 allowed): fails", TriggeredCheck != nullptr);
	const FValidationCheckData& TriggeredData = TriggeredCheck->ValidationCheckPtr->ValidationCheckData;
	BS_TEST_TRUE("MaxAllowed reported",
	             TriggeredData.CalculatedValues.Num() == 1 && FMath::IsNearlyEqual(TriggeredData.CalculatedValues[0],
		             ExpectedCalculated));

	// Triggered: target diameter (30 * 100) leaves less than the minimum spacing of 10.
	const FValidationResult BelowMinResult =
		ValidateProperty(*Validator, MakeGridConfig(100.f, 12, 30.f), PropertyHash);
	const FValidationCheckResult* BelowMinCheck = FindResult(BelowMinResult, CheckId, false);
	BS_TEST_REQUIRE("MaxAllowed below minimum: fails", BelowMinCheck != nullptr);
	BS_TEST_TRUE("demands another property change",
	             BelowMinCheck->ValidationCheckPtr->ValidationCheckData.bRequireOtherPropertiesToBeChanged);

	// Passing: 75 within the allowed 90.91.
	BS_TEST_TRUE("spacing 75 (90.91 allowed): passes",
	             Succeeds(ValidateProperty(*Validator, MakeGridConfig(75.f, 12, 2.f), PropertyHash), CheckId));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGameModeValidatorGridSpacingYTest, TEXT("BeatShot.GameModeValidator.GridSpacingY"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)

bool FGameModeValidatorGridSpacingYTest::RunTest(const FString& Parameters)
{
	UBSGameModeValidator* Validator = NewObject<UBSGameModeValidator>();
	const uint32 PropertyHash = UBSGameModeValidator::FindBSConfigProperty(TEXT("GridConfig"), TEXT("GridSpacing"),
	                                                                       TEXT("Y"));
	const EValidationCheckId CheckId = EValidationCheckId::Grid_VerticalSpacing;

	auto MakeGridConfig = [](const float InSpacingY, const int32 InNumVertical, const float InMaxSpawnedScale)
	{
		FBSConfig Cfg;
		Cfg.TargetConfig.TargetDistributionPolicy = ETargetDistributionPolicy::Grid;
		Cfg.TargetConfig.MaxSpawnedTargetScale = InMaxSpawnedScale;
		Cfg.GridConfig.NumVerticalGridTargets = InNumVertical;
		Cfg.GridConfig.GridSpacing.Y = InSpacingY;
		return Cfg;
	};

	// Off-grid distribution policy: the check bypasses (does not run) and counts as passing.
	FBSConfig Bypassed;
	Bypassed.TargetConfig.TargetDistributionPolicy = ETargetDistributionPolicy::FullRange;
	const FValidationResult BypassedResult = ValidateProperty(*Validator, Bypassed, PropertyHash);
	const FValidationCheckResult* BypassedCheck = FindResult(BypassedResult, CheckId, true);
	BS_TEST_REQUIRE("off-grid: recorded as passing", BypassedCheck != nullptr);
	BS_TEST_TRUE("off-grid: bypassed", BypassedCheck->bBypassed);

	// Triggered: max allowed is 1000 / 4 - 200 = 50, below the requested 60.
	const float MaxAllowed = 1000.f / (5 - 1) - 200.f;
	const float ExpectedCalculated = FMath::RoundHalfToZero(MaxAllowed * 100) / 100.f;
	const FValidationResult TriggeredResult = ValidateProperty(*Validator, MakeGridConfig(60.f, 5, 2.f), PropertyHash);
	const FValidationCheckResult* TriggeredCheck = FindResult(TriggeredResult, CheckId, false);
	BS_TEST_REQUIRE("spacing 60 (50 allowed): fails", TriggeredCheck != nullptr);
	const FValidationCheckData& TriggeredData = TriggeredCheck->ValidationCheckPtr->ValidationCheckData;
	BS_TEST_TRUE("MaxAllowed reported",
	             TriggeredData.CalculatedValues.Num() == 1 && FMath::IsNearlyEqual(TriggeredData.CalculatedValues[0],
		             ExpectedCalculated));

	// Triggered: target diameter (13 * 100) leaves less than the minimum spacing of 10.
	const FValidationResult BelowMinResult = ValidateProperty(*Validator, MakeGridConfig(50.f, 5, 13.f), PropertyHash);
	const FValidationCheckResult* BelowMinCheck = FindResult(BelowMinResult, CheckId, false);
	BS_TEST_REQUIRE("MaxAllowed below minimum: fails", BelowMinCheck != nullptr);
	BS_TEST_TRUE("demands another property change",
	             BelowMinCheck->ValidationCheckPtr->ValidationCheckData.bRequireOtherPropertiesToBeChanged);

	// Passing boundary: exactly 50 allowed.
	BS_TEST_TRUE("spacing 50 (50 allowed): passes",
	             Succeeds(ValidateProperty(*Validator, MakeGridConfig(50.f, 5, 2.f), PropertyHash), CheckId));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGameModeValidatorGridMaxSpawnedScaleTest,
                                 TEXT("BeatShot.GameModeValidator.GridMaxSpawnedTargetScale"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)

bool FGameModeValidatorGridMaxSpawnedScaleTest::RunTest(const FString& Parameters)
{
	UBSGameModeValidator* Validator = NewObject<UBSGameModeValidator>();
	const uint32 PropertyHash = UBSGameModeValidator::FindBSConfigProperty(
		TEXT("TargetConfig"), TEXT("MaxSpawnedTargetScale"));
	const EValidationCheckId CheckId = EValidationCheckId::Grid_MaxSpawnedTargetScale;

	auto MakeGridConfig = [&](const float InMinSpawnedScale,
	                          const float InMaxSpawnedScale,
	                          const float InSpacingX,
	                          const float InSpacingY,
	                          const int32 InNumHorizontal,
	                          const int32 InNumVertical)
	{
		FBSConfig Cfg;
		Cfg.TargetConfig.TargetDistributionPolicy = ETargetDistributionPolicy::Grid;
		Cfg.TargetConfig.MinSpawnedTargetScale = InMinSpawnedScale;
		Cfg.TargetConfig.MaxSpawnedTargetScale = InMaxSpawnedScale;
		Cfg.GridConfig.NumHorizontalGridTargets = InNumHorizontal;
		Cfg.GridConfig.NumVerticalGridTargets = InNumVertical;
		Cfg.GridConfig.GridSpacing = FVector2D(InSpacingX, InSpacingY);
		return Cfg;
	};

	// Off-grid distribution policy: the check bypasses (does not run) and counts as passing.
	FBSConfig Bypassed;
	Bypassed.TargetConfig.TargetDistributionPolicy = ETargetDistributionPolicy::EdgeOnly;
	const FValidationResult BypassedResult = ValidateProperty(*Validator, Bypassed, PropertyHash);
	const FValidationCheckResult* BypassedCheck = FindResult(BypassedResult, CheckId, true);
	BS_TEST_REQUIRE("off-grid: recorded as passing", BypassedCheck != nullptr);
	BS_TEST_TRUE("off-grid: bypassed", BypassedCheck->bBypassed);

	// Max allowed scale for the grid below:
	//   horizontal: (3200 - 75 * 11) / (11 * 100) = 2.16
	//   vertical:   (1000 - 50 * 4) / (4 * 100) = 2.00
	const float MaxAllowed = FMath::Min((3200.f - 75.f * (12 - 1)) / ((12 - 1) * 100.f),
	                                    (1000.f - 50.f * (5 - 1)) / ((5 - 1) * 100.f));

	// Passing boundary: max spawned scale of exactly 2.0.
	const FValidationResult PassingResult = ValidateProperty(*Validator, MakeGridConfig(0.8f, 2.f, 75.f, 50.f, 12, 5),
	                                                         PropertyHash);
	const FValidationCheckResult* PassingCheck = FindResult(PassingResult, CheckId, true);
	BS_TEST_REQUIRE("scale 2.0 (2.0 allowed): passing", PassingCheck != nullptr);
	const FValidationCheckData& PassingData = PassingCheck->ValidationCheckPtr->ValidationCheckData;
	BS_TEST_TRUE("MaxAllowed reported",
	             PassingData.CalculatedValues.Num() == 1 && FMath::IsNearlyEqual( PassingData.CalculatedValues[0], FMath
		             ::RoundHalfToZero(MaxAllowed * 100) / 100.f));

	// Triggered via the max scale: 2.5 above the allowed 2.0.
	const FValidationResult MaxTriggeredResult = ValidateProperty(
		*Validator, MakeGridConfig(0.8f, 2.5f, 75.f, 50.f, 12, 5), PropertyHash);
	const FValidationCheckResult* MaxTriggeredCheck = FindResult(MaxTriggeredResult, CheckId, false);
	BS_TEST_REQUIRE("max scale 2.5: fails", MaxTriggeredCheck != nullptr);
	BS_TEST_EQUAL("warning severity", MaxTriggeredCheck->WarningType, EGameModeWarningType::Warning);

	// Triggered via the min scale: the larger of the two bounds is what must fit.
	BS_TEST_TRUE("min scale 2.2: fails",
	             Fails(ValidateProperty(*Validator, MakeGridConfig(2.2f, 0.5f, 75.f, 50.f, 12, 5), PropertyHash),
		             CheckId));

	// Triggered: allowed scale below the minimum of 0.25.
	const FValidationResult BelowMinResult = ValidateProperty(
		*Validator, MakeGridConfig(0.8f, 2.f, 1000.f, 1000.f, 3, 3), PropertyHash);
	const FValidationCheckResult* BelowMinCheck = FindResult(BelowMinResult, CheckId, false);
	BS_TEST_REQUIRE("MaxAllowed below minimum: fails", BelowMinCheck != nullptr);
	BS_TEST_TRUE("demands another property change",
	             BelowMinCheck->ValidationCheckPtr->ValidationCheckData.bRequireOtherPropertiesToBeChanged);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGameModeValidatorHeadshotHeightOnlyRLTest,
                                 TEXT("BeatShot.GameModeValidator.HeadshotHeightOnly_ReinforcementLearning"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)

bool FGameModeValidatorHeadshotHeightOnlyRLTest::RunTest(const FString& Parameters)
{
	UBSGameModeValidator* Validator = NewObject<UBSGameModeValidator>();
	const uint32 PropertyHash = UBSGameModeValidator::FindBSConfigProperty(
		TEXT("AIConfig"), TEXT("bEnableReinforcementLearning"));
	const EValidationCheckId CheckId = EValidationCheckId::HeadshotHeightOnly_ReinforcementLearning_EnableRL;
	const EValidationCheckId PolicySideCheckId = EValidationCheckId::HeadshotHeightOnly_ReinforcementLearning_Policy;

	auto ValidateWith = [&](const ETargetDistributionPolicy InPolicy, const bool InEnableRL)
	{
		FBSConfig Cfg;
		Cfg.TargetConfig.TargetDistributionPolicy = InPolicy;
		Cfg.AIConfig.bEnableReinforcementLearning = InEnableRL;
		return ValidateProperty(*Validator, Cfg, PropertyHash);
	};

	// Headshot-height distribution with the RL agent enabled fails.
	const FValidationResult Triggered = ValidateWith(ETargetDistributionPolicy::HeadshotHeightOnly, true);
	const FValidationCheckResult* TriggeredCheck = FindResult(Triggered, CheckId, false);
	BS_TEST_REQUIRE("HeadshotHeightOnly + RL: fails", TriggeredCheck != nullptr);
	BS_TEST_EQUAL("warning severity", TriggeredCheck->WarningType, EGameModeWarningType::Warning);
	BS_TEST_TRUE("policy side: fails", Fails(Triggered, PolicySideCheckId));

	// Either half of the condition disarms the rule.
	BS_TEST_TRUE("Grid + RL: passes", Succeeds(ValidateWith(ETargetDistributionPolicy::Grid, true), CheckId));
	BS_TEST_TRUE("HeadshotHeightOnly + no RL: passes",
	             Succeeds(ValidateWith(ETargetDistributionPolicy::HeadshotHeightOnly, false), CheckId));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGameModeValidatorTrackingRLTest,
                                 TEXT("BeatShot.GameModeValidator.Tracking_ReinforcementLearning"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)

bool FGameModeValidatorTrackingRLTest::RunTest(const FString& Parameters)
{
	UBSGameModeValidator* Validator = NewObject<UBSGameModeValidator>();
	const uint32 PropertyHash = UBSGameModeValidator::FindBSConfigProperty(
		TEXT("AIConfig"), TEXT("bEnableReinforcementLearning"));
	const EValidationCheckId CheckId = EValidationCheckId::Tracking_ReinforcementLearning_EnableRL;
	const EValidationCheckId DamageTypeSideCheckId =
		EValidationCheckId::Tracking_ReinforcementLearning_TargetDamageType;

	auto ValidateWith = [&](const ETargetDamageType InDamageType, const bool InEnableRL)
	{
		FBSConfig Cfg;
		Cfg.TargetConfig.TargetDamageType = InDamageType;
		Cfg.AIConfig.bEnableReinforcementLearning = InEnableRL;
		return ValidateProperty(*Validator, Cfg, PropertyHash);
	};

	// Tracking damage with the RL agent enabled fails.
	const FValidationResult Triggered = ValidateWith(ETargetDamageType::Tracking, true);
	const FValidationCheckResult* TriggeredCheck = FindResult(Triggered, CheckId, false);
	BS_TEST_REQUIRE("Tracking + RL: fails", TriggeredCheck != nullptr);
	BS_TEST_EQUAL("warning severity", TriggeredCheck->WarningType, EGameModeWarningType::Warning);
	BS_TEST_TRUE("damage type side: fails", Fails(Triggered, DamageTypeSideCheckId));

	// Either half of the condition disarms the rule.
	BS_TEST_TRUE("Hit + RL: passes", Succeeds(ValidateWith(ETargetDamageType::Hit, true), CheckId));
	BS_TEST_TRUE("Tracking + no RL: passes", Succeeds(ValidateWith(ETargetDamageType::Tracking, false), CheckId));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGameModeValidatorDefaultConfigTest,
                                 TEXT("BeatShot.GameModeValidator.DefaultConfig_NoFailures"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)

bool FGameModeValidatorDefaultConfigTest::RunTest(const FString& Parameters)
{
	UBSGameModeValidator* Validator = NewObject<UBSGameModeValidator>();
	const FBSConfig Config;
	const TSharedPtr<FBSConfig> SharedConfig = MakeShareable(new FBSConfig(Config));
	const FValidationResult Result = Validator->Validate(SharedConfig);

	const FValidationCheckResultSet SucceededSet = Result.GetSucceeded();
	const FValidationCheckResultSet FailedSet = Result.GetFailed();
	BS_TEST_TRUE("default config: no failures", FailedSet.Num() == 0);

	// The default config's distribution policy is not Grid, so exactly the 5 grid checks bypass.
	int32 BypassedCount = 0;
	for (const FValidationCheckResult& CheckResult : SucceededSet)
	{
		if (CheckResult.bBypassed)
		{
			BypassedCount++;
		}
	}
	BS_TEST_EQUAL("non-grid policy: the 5 grid checks bypass", BypassedCount, 5);

	// Full validation surface: every registered check.
	BS_TEST_EQUAL("full validation surface checked", SucceededSet.Num() + FailedSet.Num(),
	              (int32)Validator->GetValidationChecks().Num());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGameModeValidatorCoverageAllChecksTest,
                                 TEXT("BeatShot.GameModeValidator.CoverageAllChecks"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)

bool FGameModeValidatorCoverageAllChecksTest::RunTest(const FString& Parameters)
{
	UBSGameModeValidator* Validator = NewObject<UBSGameModeValidator>();
	const TArray<FValidationCheckPtr> Checks = Validator->GetValidationChecks();
	BS_TEST_TRUE("validator has checks", Checks.Num() > 0);

	// Fails validation of exactly one check when applied to the default config.
	struct FCheckFailureCase
	{
		EValidationCheckId CheckId;
		TFunction<void(FBSConfig&)> Mutator;
	};

	// One failing config per check id, each backed by the config that triggers the matching per-rule test above.
	// The two copies of a rule share their config.
	const TArray<FCheckFailureCase> FailureCases = {
		{
			EValidationCheckId::SpawnEveryOtherTargetInCenter_BatchSpawning,
			[](FBSConfig& Cfg)
			{
				Cfg.TargetConfig.bSpawnEveryOtherTargetInCenter = true;
				Cfg.TargetConfig.bUseBatchSpawning = true;
			}
		},
		{
			EValidationCheckId::BatchSpawning_SpawnEveryOtherTargetInCenter,
			[](FBSConfig& Cfg)
			{
				Cfg.TargetConfig.bSpawnEveryOtherTargetInCenter = true;
				Cfg.TargetConfig.bUseBatchSpawning = true;
			}
		},
		{
			EValidationCheckId::SpawnEveryOtherTargetInCenter_AllowSpawnWithoutActivation,
			[](FBSConfig& Cfg)
			{
				Cfg.TargetConfig.bSpawnEveryOtherTargetInCenter = true;
				Cfg.TargetConfig.bAllowSpawnWithoutActivation = true;
			}
		},
		{
			EValidationCheckId::AllowSpawnWithoutActivation_SpawnEveryOtherTargetInCenter,
			[](FBSConfig& Cfg)
			{
				Cfg.TargetConfig.bSpawnEveryOtherTargetInCenter = true;
				Cfg.TargetConfig.bAllowSpawnWithoutActivation = true;
			}
		},
		{
			EValidationCheckId::ChangeVelocity_MTDNone_MTDM,
			[](FBSConfig& Cfg)
			{
				Cfg.TargetConfig.TargetSpawnResponses = {ETargetSpawnResponse::ChangeVelocity};
			}
		},
		{
			EValidationCheckId::ChangeDirection_MTDNone_MTDM,
			[](FBSConfig& Cfg)
			{
				Cfg.TargetConfig.TargetSpawnResponses = {ETargetSpawnResponse::ChangeDirection};
			}
		},
		{
			EValidationCheckId::ChangeVelocity_MTDNone_SpawnResponses,
			[](FBSConfig& Cfg)
			{
				Cfg.TargetConfig.TargetSpawnResponses = {ETargetSpawnResponse::ChangeVelocity};
			}
		},
		{
			EValidationCheckId::ChangeDirection_MTDNone_SpawnResponses,
			[](FBSConfig& Cfg)
			{
				Cfg.TargetConfig.TargetSpawnResponses = {ETargetSpawnResponse::ChangeDirection};
			}
		},
		{
			EValidationCheckId::ChangeVelocity_MTDNone_ActivationResponses,
			[](FBSConfig& Cfg)
			{
				Cfg.TargetConfig.TargetActivationResponses = {ETargetActivationResponse::ChangeVelocity};
			}
		},
		{
			EValidationCheckId::ChangeDirection_MTDNone_ActivationResponses,
			[](FBSConfig& Cfg)
			{
				Cfg.TargetConfig.TargetActivationResponses = {ETargetActivationResponse::ChangeDirection};
			}
		},
		{
			EValidationCheckId::ChangeVelocity_MTDNone_DeactivationResponses,
			[](FBSConfig& Cfg)
			{
				Cfg.TargetConfig.TargetDeactivationResponses = {ETargetDeactivationResponse::ChangeVelocity};
			}
		},
		{
			EValidationCheckId::ChangeDirection_MTDNone_DeactivationResponses,
			[](FBSConfig& Cfg)
			{
				Cfg.TargetConfig.TargetDeactivationResponses = {ETargetDeactivationResponse::ChangeDirection};
			}
		},
		{
			EValidationCheckId::ZeroForwardDistance_ForwardOnly_MTDM,
			[](FBSConfig& Cfg)
			{
				Cfg.TargetConfig.BoxBounds = FVector(0.f, 3200.f, 1000.f);
				Cfg.TargetConfig.MovingTargetDirectionMode = EMovingTargetDirectionMode::ForwardOnly;
			}
		},
		{
			EValidationCheckId::ZeroForwardDistance_ForwardOnly_BoxBoundsX,
			[](FBSConfig& Cfg)
			{
				Cfg.TargetConfig.BoxBounds = FVector(0.f, 3200.f, 1000.f);
				Cfg.TargetConfig.MovingTargetDirectionMode = EMovingTargetDirectionMode::ForwardOnly;
			}
		},
		{
			EValidationCheckId::Grid_NumHorizontalTargets,
			[](FBSConfig& Cfg)
			{
				Cfg.TargetConfig.TargetDistributionPolicy = ETargetDistributionPolicy::Grid;
				Cfg.GridConfig.NumHorizontalGridTargets = 13;
				Cfg.GridConfig.GridSpacing.X = 75.f;
			}
		},
		{
			EValidationCheckId::Grid_NumVerticalTargets,
			[](FBSConfig& Cfg)
			{
				Cfg.TargetConfig.TargetDistributionPolicy = ETargetDistributionPolicy::Grid;
				Cfg.GridConfig.NumVerticalGridTargets = 6;
				Cfg.GridConfig.GridSpacing.Y = 50.f;
			}
		},
		{
			EValidationCheckId::Grid_HorizontalSpacing,
			[](FBSConfig& Cfg)
			{
				Cfg.TargetConfig.TargetDistributionPolicy = ETargetDistributionPolicy::Grid;
				Cfg.TargetConfig.MaxSpawnedTargetScale = 2.f;
				Cfg.GridConfig.NumHorizontalGridTargets = 12;
				Cfg.GridConfig.GridSpacing.X = 100.f;
			}
		},
		{
			EValidationCheckId::Grid_VerticalSpacing,
			[](FBSConfig& Cfg)
			{
				Cfg.TargetConfig.TargetDistributionPolicy = ETargetDistributionPolicy::Grid;
				Cfg.GridConfig.NumVerticalGridTargets = 5;
				Cfg.GridConfig.GridSpacing.Y = 60.f;
			}
		},
		{
			EValidationCheckId::Grid_MaxSpawnedTargetScale,
			[](FBSConfig& Cfg)
			{
				Cfg.TargetConfig.TargetDistributionPolicy = ETargetDistributionPolicy::Grid;
				Cfg.TargetConfig.MinSpawnedTargetScale = 0.8f;
				Cfg.TargetConfig.MaxSpawnedTargetScale = 2.5f;
				Cfg.GridConfig.NumHorizontalGridTargets = 12;
				Cfg.GridConfig.NumVerticalGridTargets = 5;
				Cfg.GridConfig.GridSpacing = FVector2D(75.f, 50.f);
			}
		},
		{
			EValidationCheckId::HeadshotHeightOnly_ReinforcementLearning_Policy,
			[](FBSConfig& Cfg)
			{
				Cfg.TargetConfig.TargetDistributionPolicy = ETargetDistributionPolicy::HeadshotHeightOnly;
				Cfg.AIConfig.bEnableReinforcementLearning = true;
			}
		},
		{
			EValidationCheckId::HeadshotHeightOnly_ReinforcementLearning_EnableRL,
			[](FBSConfig& Cfg)
			{
				Cfg.TargetConfig.TargetDistributionPolicy = ETargetDistributionPolicy::HeadshotHeightOnly;
				Cfg.AIConfig.bEnableReinforcementLearning = true;
			}
		},
		{
			EValidationCheckId::Tracking_ReinforcementLearning_EnableRL,
			[](FBSConfig& Cfg)
			{
				Cfg.TargetConfig.TargetDamageType = ETargetDamageType::Tracking;
				Cfg.AIConfig.bEnableReinforcementLearning = true;
			}
		},
		{
			EValidationCheckId::Tracking_ReinforcementLearning_TargetDamageType,
			[](FBSConfig& Cfg)
			{
				Cfg.TargetConfig.TargetDamageType = ETargetDamageType::Tracking;
				Cfg.AIConfig.bEnableReinforcementLearning = true;
			}
		}
	};

	TSet<EValidationCheckId> RegisteredIds;
	for (const FValidationCheckPtr& Check : Checks)
	{
		RegisteredIds.Add(Check->CheckId);
	}
	BS_TEST_EQUAL("check ids are unique", RegisteredIds.Num(), Checks.Num());
	BS_TEST_EQUAL("failure cases cover the full validation surface", FailureCases.Num(), Checks.Num());

	TSet<EValidationCheckId> CaseIds;
	for (const FCheckFailureCase& Case : FailureCases)
	{
		CaseIds.Add(Case.CheckId);
	}
	BS_TEST_EQUAL("failure cases target distinct checks", CaseIds.Num(), FailureCases.Num());

	// Every registered check must be covered by a failing config.
	FString MissingIds;
	for (const FValidationCheckPtr& Check : Checks)
	{
		if (!CaseIds.Contains(Check->CheckId))
		{
			if (!MissingIds.IsEmpty())
			{
				MissingIds += TEXT(", ");
			}
			MissingIds += CheckIdToName(Check->CheckId);
		}
	}
	BS_TEST_TRUE(FString::Printf(TEXT("every registered check has a failing config, missing: [%s]"), *MissingIds),
	             MissingIds.IsEmpty());

	// Each failing config must actually fail its check...
	for (const FCheckFailureCase& Case : FailureCases)
	{
		const FValidationCheckPtr* OwningCheck = Checks.FindByPredicate(
			[InCheckId = Case.CheckId](const FValidationCheckPtr& Check) { return Check->CheckId == InCheckId; });
		BS_TEST_REQUIRE(FString::Printf(TEXT("check %s is registered"), CheckIdToName(Case.CheckId)),
		                OwningCheck != nullptr);

		FBSConfig Cfg;
		Case.Mutator(Cfg);
		const FValidationResult Result = ValidateProperty(*Validator, Cfg, (*OwningCheck)->OwningPropertyHash);
		const FValidationCheckResult* FailedCheck = FindResult(Result, Case.CheckId, false);
		BS_TEST_TRUE(FString::Printf(TEXT("check %s triggers under its failing config"), CheckIdToName(Case.CheckId)),
		             FailedCheck != nullptr);
	}

	return true;
}
