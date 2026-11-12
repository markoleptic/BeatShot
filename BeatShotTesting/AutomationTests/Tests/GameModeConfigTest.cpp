// Copyright 2022-2026 Markoleptic Games, SP. All Rights Reserved.

#include "CoreMinimal.h"
#include "BSGameModeConfig/BSConfig.h"
#include "Misc/AutomationTest.h"
#include "Misc/EnumRange.h"
#include "BSTestAssertions.h"

namespace
{
/** The three real preset difficulties (None is excluded). */
const TArray<EGameModeDifficulty> PresetDifficulties = {
	EGameModeDifficulty::Normal,
	EGameModeDifficulty::Hard,
	EGameModeDifficulty::Death
};

FBS_DefiningConfig MakePreset(const EBaseGameMode InBase, const EGameModeDifficulty InDifficulty,
                              const FString& InName = FString())
{
	FBS_DefiningConfig Cfg = FBSConfig::GetConfigForPreset(InBase, InDifficulty);
	Cfg.CustomGameModeName = InName;
	return Cfg;
}

FBS_DefiningConfig MakeCustom(const FString& InName,
                              const EBaseGameMode InBase = EBaseGameMode::MultiBeat,
                              const EGameModeDifficulty InDifficulty = EGameModeDifficulty::Normal)
{
	return FBS_DefiningConfig(EGameModeType::Custom, InBase, InName, InDifficulty);
}

// Distinct sentinel colors, each with its own identity, for the InitColors assertions.
const FLinearColor InactiveSentinel(1.f, 0.f, 0.f, 1.f);
const FLinearColor OutlineSentinel(1.f, 1.f, 0.f, 1.f);
const FLinearColor StartSentinel(0.f, 1.f, 0.f, 1.f);
const FLinearColor PeakSentinel(0.f, 1.f, 1.f, 1.f);
const FLinearColor EndSentinel(0.f, 0.f, 1.f, 1.f);
const FLinearColor TrackingDamSentinel(1.f, 0.f, 1.f, 1.f);
const FLinearColor NotTrackingDamSentinel(1.f, 1.f, 1.f, 0.5f);
const FLinearColor TouchedSentinel(0.9f, 0.8f, 0.7f, 0.6f);

void ApplySentinelColors(FBSConfig& InCfg)
{
	InCfg.TargetConfig.bUseSeparateOutlineColor = true;
	InCfg.TargetConfig.InactiveTargetColor = InactiveSentinel;
	InCfg.TargetConfig.OutlineColor = OutlineSentinel;
	InCfg.TargetConfig.StartColor = StartSentinel;
	InCfg.TargetConfig.PeakColor = PeakSentinel;
	InCfg.TargetConfig.EndColor = EndSentinel;
	InCfg.TargetConfig.TakingTrackingDamageColor = TrackingDamSentinel;
	InCfg.TargetConfig.NotTakingTrackingDamageColor = NotTrackingDamSentinel;
}

/** Fills every serialized field with a value distinct from the defaults, and the transient fields
 *  with values of their own that must NOT survive a serialization round trip. */
FBSConfig MakeDistinctiveConfig()
{
	FBSConfig Cfg;
	Cfg.DefiningConfig = FBS_DefiningConfig(EGameModeType::Custom, EBaseGameMode::MultiBeat,
	                                        TEXT("Round Trip Mode"), EGameModeDifficulty::Death);

	Cfg.AIConfig.bEnableReinforcementLearning = true;
	Cfg.AIConfig.Alpha = 0.5f;
	Cfg.AIConfig.Epsilon = 0.25f;
	Cfg.AIConfig.Gamma = 0.75f;
	Cfg.AIConfig.ReinforcementLearningMode = EReinforcementLearningMode::Exploration;
	Cfg.AIConfig.HyperParameterMode = EReinforcementLearningHyperParameterMode::Custom;

	Cfg.GridConfig.NumHorizontalGridTargets = 7;
	Cfg.GridConfig.NumVerticalGridTargets = 9;
	Cfg.GridConfig.GridSpacing = FVector2D(120.f, 65.f);
	Cfg.GridConfig.NumGridTargetsVisibleAtOnce = 4;

	Cfg.DynamicTargetScaling.StartThreshold = 12;
	Cfg.DynamicTargetScaling.EndThreshold = 80;
	Cfg.DynamicTargetScaling.bIsCubicInterpolation = true;
	Cfg.DynamicTargetScaling.DecrementAmount = 7;

	Cfg.DynamicSpawnAreaScaling.StartThreshold = 20;
	Cfg.DynamicSpawnAreaScaling.EndThreshold = 150;
	Cfg.DynamicSpawnAreaScaling.bIsCubicInterpolation = false;
	Cfg.DynamicSpawnAreaScaling.DecrementAmount = 9;
	Cfg.DynamicSpawnAreaScaling.StartBounds = FVector(25.f, 300.f, 600.f);

	FBS_TargetConfig& Target = Cfg.TargetConfig;
	Target.bAllowActivationWhileActivated = true;
	Target.bAllowSpawnWithoutActivation = true;
	Target.bSpawnAtOriginWheneverPossible = true;
	Target.bSpawnEveryOtherTargetInCenter = true;
	Target.bUseBatchSpawning = true;
	Target.BoundsScalingPolicy = EBoundsScalingPolicy::Static;
	Target.ConsecutiveTargetScalePolicy = EConsecutiveTargetScalePolicy::SkillBased;
	Target.MovingTargetDirectionMode = EMovingTargetDirectionMode::AlternateHorizontalVertical;
	Target.RecentTargetMemoryPolicy = ERecentTargetMemoryPolicy::NumTargetsBased;
	Target.TargetActivationSelectionPolicy = ETargetActivationSelectionPolicy::Bordering;
	Target.TargetDamageType = ETargetDamageType::Combined;
	Target.TargetDistributionPolicy = ETargetDistributionPolicy::Grid;
	Target.TargetSpawningPolicy = ETargetSpawningPolicy::UpfrontOnly;
	Target.RuntimeTargetSpawningLocationSelectionMode =
		ERuntimeTargetSpawningLocationSelectionMode::NearbyGridBlock;
	Target.TargetSpawnResponses = {ETargetSpawnResponse::ChangeVelocity, ETargetSpawnResponse::AddImmunity};
	Target.TargetActivationResponses = {ETargetActivationResponse::AddImmunity,
	                                    ETargetActivationResponse::ApplyConsecutiveTargetScale,
	                                    ETargetActivationResponse::ApplyLifetimeTargetScaling};
	Target.TargetDeactivationConditions = {ETargetDeactivationCondition::OnExpiration,
	                                       ETargetDeactivationCondition::OnSpecificHealthLost};
	Target.TargetDeactivationResponses = {ETargetDeactivationResponse::ToggleImmunity,
	                                      ETargetDeactivationResponse::Reactivate};
	Target.TargetDestructionConditions = {ETargetDestructionCondition::OnHealthReachedZero,
	                                      ETargetDestructionCondition::OnDeactivation};
	Target.BasePlayerHitDamage = 125.f;
	Target.BasePlayerTrackingDamage = 2.5f;
	Target.ConsecutiveChargeScaleMultiplier = 0.4f;
	Target.DeactivationHealthLostThreshold = 450.f;
	Target.LifetimeTargetScaleMultiplier = 1.4f;
	Target.ExpirationHealthPenalty = 330.f;
	Target.FloorDistance = 550.f;
	Target.MinDistanceBetweenTargets = 25.f;
	Target.MaxHealth = 250;
	Target.MinSpawnedTargetScale = 0.7f;
	Target.MaxSpawnedTargetScale = 1.7f;
	Target.MinSpawnedTargetSpeed = 55.f;
	Target.MaxSpawnedTargetSpeed = 430.f;
	Target.MinActivatedTargetSpeed = 25.f;
	Target.MaxActivatedTargetSpeed = 350.f;
	Target.MinDeactivatedTargetSpeed = 15.f;
	Target.MaxDeactivatedTargetSpeed = 150.f;
	Target.RecentTargetTimeLength = 12.5f;
	Target.SpawnBeatDelay = 0.45f;
	Target.TargetMaxLifeSpan = 4.25f;
	Target.TargetSpawnCD = 0.9f;
	Target.BoxBounds = FVector(15.f, 2400.f, 850.f);
	Target.MaxNumActivatedTargetsAtOnce = 5;
	Target.MaxNumRecentTargets = 17;
	Target.MaxNumTargetsAtOnce = 9;
	Target.MinNumTargetsToActivateAtOnce = 2;
	Target.MaxNumTargetsToActivateAtOnce = 6;
	Target.NumRuntimeTargetsToSpawn = -1;
	Target.NumUpfrontTargetsToSpawn = 49;

	// Transient / SkipSerialization fields: distinct before, expected default after the round trip.
	Cfg.AudioConfig.SongFilePath = TEXT("/tmp/a.mp3");
	Cfg.AudioConfig.SongMetadata.Title = TEXT("Round Trip");
	Cfg.AudioConfig.bPlaybackAudio = false;
	Cfg.AudioConfig.AudioFormat = EAudioFormat::Capture;
	Cfg.AudioConfig.InAudioDevice = TEXT("Device 0");
	ApplySentinelColors(Cfg);

	return Cfg;
}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGameModeConfigPresetIdentityTest,
                                 TEXT("BeatShot.GameModeConfig.PresetIdentity"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)

bool FGameModeConfigPresetIdentityTest::RunTest(const FString& Parameters)
{
	for (const EBaseGameMode Base : TEnumRange<EBaseGameMode>())
	{
		for (const EGameModeDifficulty Difficulty : PresetDifficulties)
		{
			const FBS_DefiningConfig Cfg = FBSConfig::GetConfigForPreset(Base, Difficulty);
			BS_TEST_TRUE("GameModeType is Preset", Cfg.GameModeType == EGameModeType::Preset);
			BS_TEST_TRUE("BaseGameMode matches", Cfg.BaseGameMode == Base);
			BS_TEST_TRUE("Difficulty matches", Cfg.Difficulty == Difficulty);
			BS_TEST_TRUE("name is empty for presets", Cfg.CustomGameModeName.IsEmpty());
		}
	}

	const FBS_DefiningConfig Default;
	BS_TEST_TRUE("default type is None", Default.GameModeType == EGameModeType::None);
	BS_TEST_TRUE("default base is None", Default.BaseGameMode == EBaseGameMode::None);
	BS_TEST_TRUE("default difficulty is None", Default.Difficulty == EGameModeDifficulty::None);
	BS_TEST_TRUE("default name is empty", Default.CustomGameModeName.IsEmpty());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGameModeConfigPresetEqualityTest,
                                 TEXT("BeatShot.GameModeConfig.PresetEquality"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)

bool FGameModeConfigPresetEqualityTest::RunTest(const FString& Parameters)
{
	const FBS_DefiningConfig SingleNormal =
		MakePreset(EBaseGameMode::SingleBeat, EGameModeDifficulty::Normal);

	BS_TEST_TRUE("same base and difficulty", SingleNormal ==
	                                                MakePreset(EBaseGameMode::SingleBeat,
	                                                           EGameModeDifficulty::Normal));
	BS_TEST_TRUE("name is ignored for presets", SingleNormal ==
	                                                MakePreset(EBaseGameMode::SingleBeat,
	                                                           EGameModeDifficulty::Normal,
	                                                           TEXT("AnyNameAtAll")));
	BS_TEST_FALSE("different difficulty", SingleNormal ==
	                                              MakePreset(EBaseGameMode::SingleBeat,
	                                                         EGameModeDifficulty::Hard));
	BS_TEST_FALSE("different base", SingleNormal ==
	                                       MakePreset(EBaseGameMode::MultiBeat, EGameModeDifficulty::Normal));
	BS_TEST_FALSE("preset vs custom", SingleNormal == MakeCustom(TEXT("Anything")));

	// A defining config with no type (the default) is not equal to the same base/difficulty preset.
	const FBS_DefiningConfig Untyped =
		FBS_DefiningConfig(EGameModeType::None, EBaseGameMode::SingleBeat, FString(),
		                   EGameModeDifficulty::Normal);
	BS_TEST_FALSE("untyped (None) vs preset", SingleNormal == Untyped);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGameModeConfigCustomEqualityTest,
                                 TEXT("BeatShot.GameModeConfig.CustomEquality"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)

bool FGameModeConfigCustomEqualityTest::RunTest(const FString& Parameters)
{
	const FBS_DefiningConfig MyMode =
		MakeCustom(TEXT("My Mode"), EBaseGameMode::SingleBeat, EGameModeDifficulty::Normal);

	BS_TEST_TRUE("case-insensitive name, base/difficulty ignored",
	             MyMode == MakeCustom(TEXT("my mode"), EBaseGameMode::MultiBeat, EGameModeDifficulty::Death));
	BS_TEST_FALSE("different names", MyMode == MakeCustom(TEXT("Other Mode")));
	BS_TEST_FALSE("custom vs preset, even with matching name",
	              MyMode == MakePreset(EBaseGameMode::SingleBeat, EGameModeDifficulty::Normal, TEXT("my mode")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGameModeConfigEqualImpliesSameHashTest,
                                 TEXT("BeatShot.GameModeConfig.EqualImpliesSameHash"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)

bool FGameModeConfigEqualImpliesSameHashTest::RunTest(const FString& Parameters)
{
	// Two independently constructed copies of every preset must be equal AND hash identically
	// (a TMap keyed on FBS_DefiningConfig would otherwise split them into two buckets).
	for (const EBaseGameMode Base : TEnumRange<EBaseGameMode>())
	{
		for (const EGameModeDifficulty Difficulty : PresetDifficulties)
		{
			const FBS_DefiningConfig A = MakePreset(Base, Difficulty);
			const FBS_DefiningConfig B = FBSConfig::GetConfigForPreset(Base, Difficulty);
			BS_TEST_TRUE("presets constructed two ways are equal", A == B);
			BS_TEST_TRUE("equal presets hash the same", GetTypeHash(A) == GetTypeHash(B));
		}
	}

	// Case-insensitive names are equal by definition, so they must also agree on hash.
	const FBS_DefiningConfig CustomA = MakeCustom(TEXT("My Mode"));
	const FBS_DefiningConfig CustomB = MakeCustom(TEXT("my mode"));
	BS_TEST_TRUE("custom names differing only in case are equal", CustomA == CustomB);
	BS_TEST_TRUE("case-insensitive-name customs hash the same", GetTypeHash(CustomA) == GetTypeHash(CustomB));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGameModeConfigOnCreateCustomTest,
                                 TEXT("BeatShot.GameModeConfig.OnCreateCustom"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)

bool FGameModeConfigOnCreateCustomTest::RunTest(const FString& Parameters)
{
	FBSConfig Cfg = FBSConfig(EBaseGameMode::MultiBeat, EGameModeDifficulty::Hard);
	BS_TEST_REQUIRE("starts as a preset", Cfg.DefiningConfig.GameModeType == EGameModeType::Preset);

	Cfg.DefiningConfig.CustomGameModeName = TEXT("My Mode");
	Cfg.TargetConfig.SpawnBeatDelay = 0.3f;
	Cfg.AudioConfig.AudioFormat = EAudioFormat::Capture;
	Cfg.AIConfig.bEnableReinforcementLearning = true;

	Cfg.OnCreate_Custom();

	BS_TEST_TRUE("type forced to Custom", Cfg.DefiningConfig.GameModeType == EGameModeType::Custom);
	BS_TEST_TRUE("difficulty forced to None", Cfg.DefiningConfig.Difficulty == EGameModeDifficulty::None);
	BS_TEST_TRUE("base preserved", Cfg.DefiningConfig.BaseGameMode == EBaseGameMode::MultiBeat);
	BS_TEST_TRUE("name preserved", Cfg.DefiningConfig.CustomGameModeName == TEXT("My Mode"));
	// OnCreate_Custom also runs OnCreate:
	BS_TEST_TRUE("capture audio zeroes spawn beat delay", FMath::IsNearlyEqual(Cfg.TargetConfig.SpawnBeatDelay, 0.f));
	BS_TEST_TRUE("RL mode is ActiveAgent (enabled + compatible)",
	             Cfg.AIConfig.ReinforcementLearningMode == EReinforcementLearningMode::ActiveAgent);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGameModeConfigOnCreateRLModeTest,
                                 TEXT("BeatShot.GameModeConfig.OnCreateRLMode"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)

bool FGameModeConfigOnCreateRLModeTest::RunTest(const FString& Parameters)
{
	auto RLModeFor = [](const bool bEnabled, const ETargetDamageType DamageType)
	{
		FBSConfig Cfg;
		Cfg.AIConfig.bEnableReinforcementLearning = bEnabled;
		Cfg.TargetConfig.TargetDamageType = DamageType;
		Cfg.OnCreate();
		return Cfg.AIConfig.ReinforcementLearningMode;
	};

	// Tracking damage is the only incompatible damage type.
	BS_TEST_EQUAL("compatible + enabled -> ActiveAgent", RLModeFor(true, ETargetDamageType::Hit),
	              EReinforcementLearningMode::ActiveAgent);
	BS_TEST_EQUAL("compatible + disabled -> Training", RLModeFor(false, ETargetDamageType::Hit),
	              EReinforcementLearningMode::Training);
	BS_TEST_EQUAL("Combined + enabled -> ActiveAgent", RLModeFor(true, ETargetDamageType::Combined),
	              EReinforcementLearningMode::ActiveAgent);
	BS_TEST_EQUAL("Tracking + enabled -> None", RLModeFor(true, ETargetDamageType::Tracking),
	              EReinforcementLearningMode::None);
	BS_TEST_EQUAL("Tracking + disabled -> None", RLModeFor(false, ETargetDamageType::Tracking),
	              EReinforcementLearningMode::None);
	BS_TEST_EQUAL("Self + disabled -> Training", RLModeFor(false, ETargetDamageType::Self),
	              EReinforcementLearningMode::Training);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGameModeConfigOnCreateAudioDelayTest,
                                 TEXT("BeatShot.GameModeConfig.OnCreateAudioDelay"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)

bool FGameModeConfigOnCreateAudioDelayTest::RunTest(const FString& Parameters)
{
	auto SpawnDelayFor = [](const EAudioFormat Format)
	{
		FBSConfig Cfg;
		Cfg.TargetConfig.SpawnBeatDelay = 0.3f;
		Cfg.AudioConfig.AudioFormat = Format;
		Cfg.OnCreate();
		return Cfg.TargetConfig.SpawnBeatDelay;
	};

	// Live-audio formats have no file-relative start, so the delay must drop to zero; file audio keeps it.
	BS_TEST_TRUE("Capture zeroes the delay", FMath::IsNearlyEqual(SpawnDelayFor(EAudioFormat::Capture), 0.f));
	BS_TEST_TRUE("Loopback zeroes the delay", FMath::IsNearlyEqual(SpawnDelayFor(EAudioFormat::Loopback), 0.f));
	BS_TEST_TRUE("File preserves the delay", FMath::IsNearlyEqual(SpawnDelayFor(EAudioFormat::File), 0.3f));
	BS_TEST_TRUE("None preserves the delay", FMath::IsNearlyEqual(SpawnDelayFor(EAudioFormat::None), 0.3f));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGameModeConfigInitColorsTest,
                                 TEXT("BeatShot.GameModeConfig.InitColors"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)

bool FGameModeConfigInitColorsTest::RunTest(const FString& Parameters)
{
	auto InitWith = [](const ETargetSpawningPolicy Spawning, const ETargetDamageType Damage,
	                   const bool bAllowSpawnWithoutActivation)
	{
		FBSConfig Cfg;
		Cfg.TargetConfig.TargetSpawningPolicy = Spawning;
		Cfg.TargetConfig.TargetDamageType = Damage;
		Cfg.TargetConfig.bAllowSpawnWithoutActivation = bAllowSpawnWithoutActivation;
		Cfg.InitColors(true, InactiveSentinel, OutlineSentinel, StartSentinel, PeakSentinel, EndSentinel,
		               TrackingDamSentinel, NotTrackingDamSentinel);
		return Cfg;
	};
	const FBS_TargetConfig& Canonical = InitWith(ETargetSpawningPolicy::UpfrontOnly, ETargetDamageType::Hit,
	                                             false).TargetConfig;
	BS_TEST_TRUE("outline flag set", Canonical.bUseSeparateOutlineColor);
	BS_TEST_TRUE("inactive color", Canonical.InactiveTargetColor == InactiveSentinel);
	BS_TEST_TRUE("outline color", Canonical.OutlineColor == OutlineSentinel);
	BS_TEST_TRUE("start color", Canonical.StartColor == StartSentinel);
	BS_TEST_TRUE("peak color", Canonical.PeakColor == PeakSentinel);
	BS_TEST_TRUE("end color", Canonical.EndColor == EndSentinel);
	BS_TEST_TRUE("tracking-damage color", Canonical.TakingTrackingDamageColor == TrackingDamSentinel);
	BS_TEST_TRUE("not-taking-damage color", Canonical.NotTakingTrackingDamageColor == NotTrackingDamSentinel);
	BS_TEST_TRUE("upfront + hit spawns inactive", Canonical.OnSpawnColor == InactiveSentinel);

	// Branch-dependent OnSpawnColor:
	const FBS_TargetConfig& UpfrontTracking = InitWith(ETargetSpawningPolicy::UpfrontOnly, ETargetDamageType::Tracking,
	                                                   false).TargetConfig;
	BS_TEST_TRUE("upfront + tracking spawns not-taking-damage",
	             UpfrontTracking.OnSpawnColor == NotTrackingDamSentinel);
	const FBS_TargetConfig& RuntimeTracking = InitWith(ETargetSpawningPolicy::RuntimeOnly, ETargetDamageType::Tracking,
	                                                   false).TargetConfig;
	BS_TEST_TRUE("runtime + tracking spawns not-taking-damage",
	             RuntimeTracking.OnSpawnColor == NotTrackingDamSentinel);
	const FBS_TargetConfig& RuntimeNoActivation =
		InitWith(ETargetSpawningPolicy::RuntimeOnly, ETargetDamageType::Hit, true).TargetConfig;
	BS_TEST_TRUE("runtime + spawn-without-activation spawns inactive",
	              RuntimeNoActivation.OnSpawnColor == InactiveSentinel);
	const FBS_TargetConfig& RuntimeActivated = InitWith(ETargetSpawningPolicy::RuntimeOnly, ETargetDamageType::Hit,
	                                                    false).TargetConfig;
	BS_TEST_TRUE("runtime + activated spawns start color", RuntimeActivated.OnSpawnColor == StartSentinel);

	// No spawning policy: OnSpawnColor must be left untouched.
	FBSConfig Cfg;
	Cfg.TargetConfig.OnSpawnColor = TouchedSentinel;
	Cfg.InitColors(false, InactiveSentinel, OutlineSentinel, StartSentinel, PeakSentinel, EndSentinel,
	               TrackingDamSentinel, NotTrackingDamSentinel);
	BS_TEST_TRUE("no spawning policy leaves OnSpawnColor alone",
	             Cfg.TargetConfig.OnSpawnColor == TouchedSentinel);
	BS_TEST_TRUE("outline flag passthrough (false)", !Cfg.TargetConfig.bUseSeparateOutlineColor);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGameModeConfigSerializationRoundTripTest,
                                 TEXT("BeatShot.GameModeConfig.SerializationRoundTrip"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)

bool FGameModeConfigSerializationRoundTripTest::RunTest(const FString& Parameters)
{
	const FBSConfig Source = MakeDistinctiveConfig();
	const FString Encoded = Source.EncodeToString();
	BS_TEST_REQUIRE("encode produced output", !Encoded.IsEmpty());

	FBSConfig Decoded;
	BS_TEST_REQUIRE("decode succeeded", FBSConfig::DecodeFromString(Encoded, Decoded));

	// Every serialized field comes back identical (each sub-struct operator== compares its own fields).
	BS_TEST_TRUE("DefiningConfig", Decoded.DefiningConfig == Source.DefiningConfig);
	BS_TEST_TRUE("AIConfig", Decoded.AIConfig == Source.AIConfig);
	BS_TEST_TRUE("GridConfig", Decoded.GridConfig == Source.GridConfig);
	BS_TEST_TRUE("DynamicTargetScaling", Decoded.DynamicTargetScaling == Source.DynamicTargetScaling);
	BS_TEST_TRUE("DynamicSpawnAreaScaling", Decoded.DynamicSpawnAreaScaling == Source.DynamicSpawnAreaScaling);
	BS_TEST_TRUE("TargetConfig", Decoded.TargetConfig == Source.TargetConfig);

	// The transient fields must NOT have been serialized (decoded side holds the defaults).
	BS_TEST_TRUE("AudioConfig.SongFilePath dropped", Decoded.AudioConfig.SongFilePath.IsEmpty());
	BS_TEST_TRUE("AudioConfig.Title dropped", Decoded.AudioConfig.SongMetadata.Title.IsEmpty());
	BS_TEST_TRUE("AudioConfig.InAudioDevice dropped", Decoded.AudioConfig.InAudioDevice.IsEmpty());
	BS_TEST_TRUE("AudioConfig.AudioFormat dropped", Decoded.AudioConfig.AudioFormat == EAudioFormat::None);
	BS_TEST_TRUE("bUseSeparateOutlineColor dropped", !Decoded.TargetConfig.bUseSeparateOutlineColor);
	BS_TEST_TRUE("InactiveTargetColor dropped", Decoded.TargetConfig.InactiveTargetColor == FLinearColor());
	BS_TEST_TRUE("OutlineColor dropped", Decoded.TargetConfig.OutlineColor == FLinearColor());
	BS_TEST_TRUE("StartColor dropped", Decoded.TargetConfig.StartColor == FLinearColor());
	BS_TEST_TRUE("PeakColor dropped", Decoded.TargetConfig.PeakColor == FLinearColor());
	BS_TEST_TRUE("EndColor dropped", Decoded.TargetConfig.EndColor == FLinearColor());
	BS_TEST_TRUE("OnSpawnColor dropped", Decoded.TargetConfig.OnSpawnColor == FLinearColor());
	BS_TEST_TRUE("TakingTrackingDamageColor dropped",
	             Decoded.TargetConfig.TakingTrackingDamageColor == FLinearColor());
	BS_TEST_TRUE("NotTakingTrackingDamageColor dropped",
	             Decoded.TargetConfig.NotTakingTrackingDamageColor == FLinearColor());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGameModeConfigDecodeFailureTest,
                                 TEXT("BeatShot.GameModeConfig.DecodeFailure"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)

bool FGameModeConfigDecodeFailureTest::RunTest(const FString& Parameters)
{
	FBSConfig Out;
	BS_TEST_FALSE("invalid base64 fails", FBSConfig::DecodeFromString(TEXT("%%no-base64%%"), Out));
	BS_TEST_FALSE("empty input fails", FBSConfig::DecodeFromString(TEXT(""), Out));
	BS_TEST_FALSE("valid base64 of non-JSON fails", FBSConfig::DecodeFromString(TEXT("aGVsbG8gd29ybGQ="), Out));

	return true;
}
