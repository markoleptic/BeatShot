// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "GlobalEnums.generated.h"

/** Enum representing the default game mode names */
UENUM(BlueprintType)
enum class EDefaultMode : uint8
{
	Custom UMETA(DisplayName="Custom"),
	SingleBeat UMETA(DisplayName="SingleBeat"),
	MultiBeat UMETA(DisplayName="MultiBeat"),
	BeatGrid UMETA(DisplayName="BeatGrid"),
	BeatTrack UMETA(DisplayName="BeatTrack")
};
ENUM_RANGE_BY_FIRST_AND_LAST(EDefaultMode, EDefaultMode::Custom, EDefaultMode::BeatTrack);

/** Enum representing the spread type of the targets */
UENUM(BlueprintType)
enum class ESpreadType : uint8
{
	None UMETA(DisplayName="None"),
	DynamicEdgeOnly UMETA(DisplayName="Dynamic Edge Only"),
	DynamicRandom UMETA(DisplayName="Dynamic Random"),
	StaticNarrow UMETA(DisplayName="Static Narrow"),
	StaticWide UMETA(DisplayName="Static Wide")
};
ENUM_RANGE_BY_FIRST_AND_LAST(ESpreadType, ESpreadType::None, ESpreadType::StaticWide);
inline bool IsDynamicSpreadType(const ESpreadType SpreadType) { return (SpreadType == ESpreadType::DynamicEdgeOnly || SpreadType == ESpreadType::DynamicRandom); }

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
	Capture UMETA(DisplayName="Capture")
};
ENUM_RANGE_BY_FIRST_AND_LAST(EAudioFormat, EAudioFormat::File, EAudioFormat::Capture);

/** The method to handle changing target scale over it's lifetime */
UENUM()
enum class ELifetimeTargetScaleMethod : uint8
{
	None UMETA(DisplayName="None"),
	Grow UMETA(DisplayName="Grow"),
	Shrink UMETA(DisplayName="Shrink")
};
ENUM_RANGE_BY_FIRST_AND_LAST(ELifetimeTargetScaleMethod, ELifetimeTargetScaleMethod::None, ELifetimeTargetScaleMethod::Shrink);

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