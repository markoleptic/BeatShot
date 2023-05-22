// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "GlobalEnums.generated.h"

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
	BeatTrack UMETA(DisplayName="BeatTrack")
};
ENUM_RANGE_BY_FIRST_AND_LAST(EBaseGameMode, EBaseGameMode::SingleBeat, EBaseGameMode::BeatTrack);

/** The scaling method to apply to the bounding box bounds at runtime */
UENUM(BlueprintType)
enum class EBoundsScalingMethod : uint8
{
	/** Used if not applicable to a game mode */
	None UMETA(DisplayName="None"),
	/** The bounding box bounds size will not change throughout the game mode */
	Static UMETA(DisplayName="Static"),
	/** The bounding box bounds size will gradually increase from half the Box Bounds up to the full size of BoxBounds, based on consecutive targets hit */
	Dynamic UMETA(DisplayName="Dynamic"),
};
ENUM_RANGE_BY_FIRST_AND_LAST(EBoundsScalingMethod, EBoundsScalingMethod::Static, EBoundsScalingMethod::Dynamic);
inline bool IsDynamicSpreadType(const EBoundsScalingMethod SpreadType) { return SpreadType == EBoundsScalingMethod::Dynamic; }

/** How to distribute targets in the bounding box bounds */
UENUM(BlueprintType)
enum class ETargetDistributionMethod : uint8
{
	/** Used if not applicable to a game mode */
	None UMETA(DisplayName="None"),
	/** Only spawns targets on the edges of the bounding box */
	HeadshotHeightOnly UMETA(DisplayName="Headshot Height Only"),
	/** Only spawns targets on the edges of the bounding box */
	EdgeOnly UMETA(DisplayName="Edge Only"),
	/** Spawns targets anywhere in the bounding box */
	FullRange UMETA(DisplayName="Full Range"),
};
ENUM_RANGE_BY_FIRST_AND_LAST(ETargetDistributionMethod, ETargetDistributionMethod::HeadshotHeightOnly, ETargetDistributionMethod::FullRange);

/** The method to handle changing target scale over it's lifetime */
UENUM()
enum class ELifetimeTargetScaleMethod : uint8
{
	/** Target does not change scale over its lifetime, used if not applicable to a game mode */
	None UMETA(DisplayName="None"),
	/** Target grows from the spawn size to max target scale over its lifetime */
	Grow UMETA(DisplayName="Grow"),
	/** Target shrinks from the spawn size to min target scale over its lifetime */
	Shrink UMETA(DisplayName="Shrink")
};
ENUM_RANGE_BY_FIRST_AND_LAST(ELifetimeTargetScaleMethod, ELifetimeTargetScaleMethod::None, ELifetimeTargetScaleMethod::Shrink);

/** The method to handle changing target scale over it's lifetime */
UENUM()
enum class EConsecutiveTargetScaleMethod : uint8
{
	/** Used if not applicable to a game mode */
	None UMETA(DisplayName="None"),
	/** The starting scale/size of the target will remain constant throughout the game mode */
	Static UMETA(DisplayName="Static"),
	/** The starting scale/size of the target will be chosen randomly between min and max target scale */
	Random UMETA(DisplayName="Random"),
	/** The starting scale/size of the target will gradually shrink from max to min target scale, based on consecutive targets hit */
	SkillBased UMETA(DisplayName="Skill-Based")
};
ENUM_RANGE_BY_FIRST_AND_LAST(EConsecutiveTargetScaleMethod, EConsecutiveTargetScaleMethod::Static, EConsecutiveTargetScaleMethod::SkillBased);

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

/* The transition state describing the start state and end state of a transition */
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