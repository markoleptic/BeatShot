// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "GlobalEnums.generated.h"

/** The player chosen audio format for the current game mode */
UENUM()
enum class EAudioFormat : uint8
{
	None UMETA(DisplayName="None"),
	File UMETA(DisplayName="File"),
	Capture UMETA(DisplayName="Capture"),
	Loopback UMETA(DisplayName="Loopback")
};
ENUM_RANGE_BY_FIRST_AND_LAST(EAudioFormat, EAudioFormat::File, EAudioFormat::Loopback);

/** Current player login state */
UENUM()
enum class ELoginState : uint8
{
	None UMETA(DisplayName="None"),
	NewUser UMETA(DisplayName="NewUser"),
	LoggedInHttp UMETA(DisplayName="LoggedInHttp"),
	LoggedInHttpAndBrowser UMETA(DisplayName="LoggedInHttpAndBrowser"),
	InvalidHttp UMETA(DisplayName="InvalidHttp"),
	InvalidBrowser UMETA(DisplayName="InvalidBrowser"),
	InvalidCredentials UMETA(DisplayName="InvalidCredentials"),
	TimeOut UMETA(DisplayName="TimeOut"),
};
ENUM_RANGE_BY_FIRST_AND_LAST(ELoginState, ELoginState::None, ELoginState::TimeOut);

/** Nvidia Reflex Mode */
UENUM(BlueprintType)
enum class EBudgetReflexMode : uint8
{
	Disabled = 0 UMETA(DisplayName="Disabled"),
	Enabled = 1 UMETA(DisplayName="Enabled"),
	EnabledPlusBoost = 3 UMETA(DisplayName="Enabled + Boost")
};
ENUM_RANGE_BY_FIRST_AND_LAST(EBudgetReflexMode, EBudgetReflexMode::Disabled, EBudgetReflexMode::EnabledPlusBoost);

/** The transition state describing the start state and end state of a transition */
UENUM()
enum class ETransitionState : uint8
{
	StartFromMainMenu UMETA(DisplayName="StartFromMainMenu"),
	StartFromPostGameMenu UMETA(DisplayName="StartFromPostGameMenu"),
	Restart UMETA(DisplayName="Restart"),
	QuitToMainMenu UMETA(DisplayName="QuitToMainMenu"),
	QuitToDesktop UMETA(DisplayName="QuitToDesktop")
};
ENUM_RANGE_BY_FIRST_AND_LAST(ETransitionState, ETransitionState::StartFromMainMenu, ETransitionState::QuitToDesktop);


/* --------------------- */
/* -- Game Mode Stuff -- */
/* --------------------- */


/** Enum representing the type of game mode */
UENUM(BlueprintType)
enum class EGameModeType : uint8
{
	None UMETA(DisplayName="None"),
	Preset UMETA(DisplayName="Preset"),
	Custom UMETA(DisplayName="Custom")
};
ENUM_RANGE_BY_FIRST_AND_LAST(EGameModeType, EGameModeType::Preset, EGameModeType::Custom);


/** Enum representing the base game modes. Preset game modes aren't necessarily a BaseGameMode */
UENUM(BlueprintType)
enum class EBaseGameMode : uint8
{
	None UMETA(DisplayName="None"),
	SingleBeat UMETA(DisplayName="SingleBeat"),
	MultiBeat UMETA(DisplayName="MultiBeat"),
	BeatGrid UMETA(DisplayName="BeatGrid"),
	BeatTrack UMETA(DisplayName="BeatTrack"),
	ChargedBeatTrack UMETA(DisplayName="ChargedBeatTrack"),
};
ENUM_RANGE_BY_FIRST_AND_LAST(EBaseGameMode, EBaseGameMode::SingleBeat, EBaseGameMode::ChargedBeatTrack);


/** Enum representing the preset, non-mutable game modes. Preset game modes aren't necessarily a BaseGameMode */
UENUM(BlueprintType)
enum class EPresetGameMode : uint8
{
	None UMETA(DisplayName="None"),
	SingleBeat UMETA(DisplayName="SingleBeat"),
	MultiBeat UMETA(DisplayName="MultiBeat"),
	BeatGrid UMETA(DisplayName="BeatGrid"),
	BeatTrack UMETA(DisplayName="BeatTrack"),
	ChargedBeatTrack UMETA(DisplayName="ChargedBeatTrack"),
};
ENUM_RANGE_BY_FIRST_AND_LAST(EPresetGameMode, EPresetGameMode::SingleBeat, EPresetGameMode::ChargedBeatTrack);


/** Enum representing the default game mode difficulties */
UENUM(BlueprintType)
enum class EGameModeDifficulty : uint8
{
	None UMETA(DisplayName="None"),
	Normal UMETA(DisplayName="Normal"),
	Hard UMETA(DisplayName="Hard"),
	Death UMETA(DisplayName="Death")
};
ENUM_RANGE_BY_FIRST_AND_LAST(EGameModeDifficulty, EGameModeDifficulty::None, EGameModeDifficulty::Death);


/** How to scale the bounding box bounds (spawn area where targets are spawned), at runtime */
UENUM(BlueprintType)
enum class EBoundsScalingPolicy : uint8
{
	None UMETA(DisplayName="None"),
	/** The bounding box bounds size will not change throughout the game mode */
	Static UMETA(DisplayName="Static"),
	/** The bounding box bounds size will gradually increase from half the Box Bounds up to the full size of BoxBounds, based on consecutive targets hit */
	Dynamic UMETA(DisplayName="Dynamic"),
};
ENUM_RANGE_BY_FIRST_AND_LAST(EBoundsScalingPolicy, EBoundsScalingPolicy::Static, EBoundsScalingPolicy::Dynamic);
inline bool IsDynamicBoundsScalingPolicy(const EBoundsScalingPolicy SpreadType) { return SpreadType == EBoundsScalingPolicy::Dynamic; }


/** Where to spawn/activate targets in the bounding box bounds (spawn area) */
UENUM(BlueprintType)
enum class ETargetDistributionPolicy : uint8
{
	None UMETA(DisplayName="None"),
	/** Only spawns targets at headshot height, so the bounding box has no height */
	HeadshotHeightOnly UMETA(DisplayName="Headshot Height Only"),
	/** Only spawns targets on the edges of the bounding box */
	EdgeOnly UMETA(DisplayName="Edge Only"),
	/** Spawns targets anywhere in the bounding box */
	FullRange UMETA(DisplayName="Full Range"),
	/** Spawns targets in a grid */
	Grid UMETA(DisplayName="Full Range"),
};
ENUM_RANGE_BY_FIRST_AND_LAST(ETargetDistributionPolicy, ETargetDistributionPolicy::HeadshotHeightOnly, ETargetDistributionPolicy::Grid);


/** How to handle changing target scale over its damageable lifetime */
UENUM()
enum class ELifetimeTargetScalePolicy : uint8
{
	/** Target does not change scale over its damageable lifetime, used if not applicable to a game mode */
	None UMETA(DisplayName="None"),
	/** Target grows from the spawn size to max target scale over its damageable lifetime */
	Grow UMETA(DisplayName="Grow"),
	/** Target shrinks from the spawn size to min target scale over its damageable lifetime */
	Shrink UMETA(DisplayName="Shrink")
};
ENUM_RANGE_BY_FIRST_AND_LAST(ELifetimeTargetScalePolicy, ELifetimeTargetScalePolicy::None, ELifetimeTargetScalePolicy::Shrink);


/** How to handle changing the target scale between consecutively activated targets */
UENUM()
enum class EConsecutiveTargetScalePolicy : uint8
{
	None UMETA(DisplayName="None"),
	/** The starting scale/size of the target will remain constant throughout the game mode */
	Static UMETA(DisplayName="Static"),
	/** The starting scale/size of the target will be chosen randomly between min and max target scale */
	Random UMETA(DisplayName="Random"),
	/** The starting scale/size of the target will gradually shrink from max to min target scale, based on consecutive targets hit */
	SkillBased UMETA(DisplayName="Skill-Based")
};
ENUM_RANGE_BY_FIRST_AND_LAST(EConsecutiveTargetScalePolicy, EConsecutiveTargetScalePolicy::Static, EConsecutiveTargetScalePolicy::SkillBased);


/** When to spawn the targets */
UENUM()
enum class ETargetSpawningPolicy : uint8
{
	None UMETA(DisplayName="None"),
	/** Spawn all the targets before the game mode starts */
	Upfront UMETA(DisplayName="Upfront"),
	/** Spawn targets when the TargetManager receives the signal from AudioAnalyzer */
	Runtime UMETA(DisplayName="Runtime"),
};
ENUM_RANGE_BY_FIRST_AND_LAST(ETargetSpawningPolicy, ETargetSpawningPolicy::Upfront, ETargetSpawningPolicy::Runtime);


/** When to activate the target (make it damageable) */
UENUM()
enum class ETargetActivationPolicy : uint8
{
	None UMETA(DisplayName="None"),
	/** Activate the target immediately on spawn */
	OnSpawn UMETA(DisplayName="On Spawn"),
	/** Activate the target when the TargetManager receives the signal from AudioAnalyzer */
	OnCooldown UMETA(DisplayName="On Cooldown"),
};
ENUM_RANGE_BY_FIRST_AND_LAST(ETargetActivationPolicy, ETargetActivationPolicy::OnSpawn, ETargetActivationPolicy::OnCooldown);


/** How to handle deactivating targets */
UENUM()
enum class ETargetDeactivationPolicy : uint8
{
	None UMETA(DisplayName="None"),
	/** Targets are never deactivated, even if their health reaches zero */
	Persistant UMETA(DisplayName="Persistant"),
	/** Targets are deactivated ONLY after their health reaches zero */
	OnHealthReachedZero UMETA(DisplayName="On Health Reached Zero"),
	/** Targets are deactivated ONLY after their damageable window closes */
	OnExpiration UMETA(DisplayName="On Expiration"),
	/** Targets are deactivated after their health reaches zero OR their damageable window closes */
	OnHealthReachedZeroOrExpiration UMETA(DisplayName="On Health Reached Zero Or Expiration"),
	/** Targets are deactivated after ANY damage event OR their damageable window closes */
	OnAnyDamageEventOrExpiration UMETA(DisplayName="On Any Damage Event Or Expiration")
};
ENUM_RANGE_BY_FIRST_AND_LAST(ETargetDeactivationPolicy, ETargetDeactivationPolicy::Persistant, ETargetDeactivationPolicy::OnAnyDamageEventOrExpiration);


/** The current activation state of a target */
UENUM()
enum class ETargetActivationState : uint8
{
	None UMETA(DisplayName="None"),
	/** Target NOT damageable, but still being managed by TargetManager */
	Inactive UMETA(DisplayName="Inactive"),
	/** Target IS damageable, managed by TargetManager */
	Active UMETA(DisplayName="Active"),
};
ENUM_RANGE_BY_FIRST_AND_LAST(ETargetActivationState, ETargetActivationState::Inactive, ETargetActivationState::Active);


/** How the player damages the target and receives score */
UENUM()
enum class ETargetDamageType : uint8
{
	None UMETA(DisplayName="None"),
	/** Having the CrossHair over the target damages the target and awards score */
	Tracking UMETA(DisplayName="Tracking"),
	/** Firing the gun and hitting the target damages the target and awards score */
	Hit UMETA(DisplayName="Hit"),
	/** Having the CrossHair over the target and hitting the target both damage the target and award score */
	Combined UMETA(DisplayName="Combined"),
};
ENUM_RANGE_BY_FIRST_AND_LAST(ETargetDamageType, ETargetDamageType::Tracking, ETargetDamageType::Hit);


/** How to handle destroying targets */
UENUM()
enum class ETargetDestructionPolicy : uint8
{
	None UMETA(DisplayName="None"),
	/** Targets are never destroyed, even if their health reaches zero. Can be reactivated/deactivated */
	Persistant UMETA(DisplayName="Persistant"),
	/** Targets are destroyed ONLY after their health reaches zero */
	OnHealthReachedZero UMETA(DisplayName="On Health Reached Zero"),
	/** Targets are destroyed ONLY after their damageable window closes */
	OnExpiration UMETA(DisplayName="On Expiration"),
	/** Targets are destroyed after their health reaches zero or their damageable window closes */
	OnHealthReachedZeroOrExpiration UMETA(DisplayName="On Health Reached Zero Or Expiration"),
	/** Targets are destroyed after any damage event or their damageable window closes */
	OnAnyDamageEventOrExpiration UMETA(DisplayName="On Any Damage Event Or Expiration")
};
ENUM_RANGE_BY_FIRST_AND_LAST(ETargetDestructionPolicy, ETargetDestructionPolicy::Persistant, ETargetDestructionPolicy::OnAnyDamageEventOrExpiration);