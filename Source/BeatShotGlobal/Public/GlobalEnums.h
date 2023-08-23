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


/** Describes if player scores were posted or not */
UENUM()
enum class EPostScoresResponse : uint8
{
	ZeroScore UMETA(DisplayName="ZeroScore"),
	UnsavedGameMode UMETA(DisplayName="UnsavedGameMode"),
	NoAccount UMETA(DisplayName="NoAccount"),
	HttpError UMETA(DisplayName="HttpError"),
	HttpSuccess UMETA(DisplayName="HttpSuccess"),
};
ENUM_RANGE_BY_FIRST_AND_LAST(EPostScoresResponse, EPostScoresResponse::ZeroScore, EPostScoresResponse::HttpSuccess);


/** The type of ScoreBrowserWidget */
UENUM()
enum class EScoreBrowserType : uint8
{
	MainMenuScores UMETA(DisplayName="MainMenuScores"),
	PostGameModeMenuScores UMETA(DisplayName="PostGameModeMenuScores"),
	PatchNotes UMETA(DisplayName="PatchNotes"),
};
ENUM_RANGE_BY_FIRST_AND_LAST(EScoreBrowserType, EScoreBrowserType::MainMenuScores, EScoreBrowserType::PatchNotes);


/** Nvidia Reflex Mode */
UENUM(BlueprintType)
enum class EBudgetReflexMode : uint8
{
	Disabled = 0 UMETA(DisplayName="Disabled"),
	Enabled = 1 UMETA(DisplayName="Enabled"),
	EnabledPlusBoost = 3 UMETA(DisplayName="Enabled + Boost")
};
ENUM_RANGE_BY_FIRST_AND_LAST(EBudgetReflexMode, EBudgetReflexMode::Disabled, EBudgetReflexMode::EnabledPlusBoost);


/** Nvidia DLSS Enabled Mode */
UENUM(BlueprintType)
enum class EDLSSEnabledMode : uint8
{
	Off UMETA(DisplayName = "Off"),
	On UMETA(DisplayName = "On"),
};
ENUM_RANGE_BY_FIRST_AND_LAST(EDLSSEnabledMode, EDLSSEnabledMode::Off, EDLSSEnabledMode::On);


/** Nvidia NIS Enabled Mode */
UENUM(BlueprintType)
enum class ENISEnabledMode : uint8
{
	Off UMETA(DisplayName = "Off"),
	On UMETA(DisplayName = "On"),
};
ENUM_RANGE_BY_FIRST_AND_LAST(ENISEnabledMode, ENISEnabledMode::Off, ENISEnabledMode::On);


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
	ClusterBeat UMETA(DisplayName="ClusterBeat"),
};
ENUM_RANGE_BY_FIRST_AND_LAST(EBaseGameMode, EBaseGameMode::SingleBeat, EBaseGameMode::ClusterBeat);


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
	/** Spawns targets in an evenly-spaced grid. Able to fit more targets */
	Grid UMETA(DisplayName="Grid"),
};
ENUM_RANGE_BY_FIRST_AND_LAST(ETargetDistributionPolicy, ETargetDistributionPolicy::HeadshotHeightOnly, ETargetDistributionPolicy::Grid);


/** Which direction to move a target in */
UENUM(BlueprintType)
enum class EMovingTargetDirectionMode : uint8
{
	None UMETA(DisplayName="None"),
	/** Only move targets left and right */
	HorizontalOnly UMETA(DisplayName="Horizontal Only"),
	/** Only move targets up and down */
	VerticalOnly UMETA(DisplayName="Vertical Only"),
	/** Only move targets up and down */
	AlternateHorizontalVertical UMETA(DisplayName="Alternate Horizontal Vertical"),
	/** Move targets in any direction */
	Any UMETA(DisplayName="Any"),
};
ENUM_RANGE_BY_FIRST_AND_LAST(EMovingTargetDirectionMode, EMovingTargetDirectionMode::HorizontalOnly, EMovingTargetDirectionMode::Any);


/** How to handle changing target scale over its damageable lifetime. This can override ConsecutiveTargetScalePolicy only if
 *  the scale is changed while the target is already active */
UENUM(BlueprintType)
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
UENUM(BlueprintType)
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


/** How the player damages the target and receives score */
UENUM(BlueprintType)
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


/** When to spawn the targets. For now, limited to just all upfront or all at runtime */
UENUM(BlueprintType)
enum class ETargetSpawningPolicy : uint8
{
	None UMETA(DisplayName="None"),
	/** Spawn all the targets before the game mode starts */
	UpfrontOnly UMETA(DisplayName="Upfront Only"),
	/** Spawn targets when the TargetManager receives the signal from AudioAnalyzer */
	RuntimeOnly UMETA(DisplayName="Runtime Only"),
};
ENUM_RANGE_BY_FIRST_AND_LAST(ETargetSpawningPolicy, ETargetSpawningPolicy::UpfrontOnly, ETargetSpawningPolicy::RuntimeOnly);


/** How to choose the target(s) to activate */
UENUM(BlueprintType)
enum class ETargetActivationSelectionPolicy : uint8
{
	None UMETA(DisplayName="None"),
	/** Only choose spawn points that border the previous point (BeatGrid is an example) */
	Bordering UMETA(DisplayName="Bordering"),
	/** Randomly chooses a target within the available spawn points */
	Random UMETA(DisplayName="Random"),
};
ENUM_RANGE_BY_FIRST_AND_LAST(ETargetActivationSelectionPolicy, ETargetActivationSelectionPolicy::Bordering, ETargetActivationSelectionPolicy::Random);


/** Specifies the method to remove targets from recent memory, allowing targets to spawn in that location again */
UENUM(BlueprintType)
enum class ERecentTargetMemoryPolicy : uint8
{
	/** Does not remember any recent targets */
	None UMETA(DisplayName="None"),
	/** Uses a specified time to remove recent targets, starting after the target has been destroyed or deactivated */
	CustomTimeBased UMETA(DisplayName="Custom Time Based"),
	/** Removes a recent target after TargetSpawnCD length of time has passed since the target has been destroyed or deactivated */
	UseTargetSpawnCD UMETA(DisplayName="Use TargetSpawnCD"),
	/** Removes recent targets only when the number of recent targets exceeds specified capacity */
	NumTargetsBased UMETA(DisplayName="Num Targets Based"),
};
ENUM_RANGE_BY_FIRST_AND_LAST(ERecentTargetMemoryPolicy, ERecentTargetMemoryPolicy::None, ERecentTargetMemoryPolicy::NumTargetsBased);


/** Each represents one way that a target can be deactivated */
UENUM(BlueprintType)
enum class ETargetDeactivationCondition : uint8
{
	None UMETA(DisplayName="None"),
	/** Targets are never deactivated, even if their health reaches zero */
	Persistant UMETA(DisplayName="Persistant"),
	/** Target is deactivated when it receives any damage from the player */
	OnAnyExternalDamageTaken UMETA(DisplayName="On Any External Damage Taken"),
	/** Target is deactivated after its damageable window closes */
	OnExpiration UMETA(DisplayName="On Expiration"),
	/** DEPRECATED */
	OnHealthReachedZero UMETA(DisplayName="On Health Reached Zero"),
};
ENUM_RANGE_BY_FIRST_AND_LAST(ETargetDeactivationCondition, ETargetDeactivationCondition::Persistant, ETargetDeactivationCondition::OnExpiration);


/** Each represents one way that a target can be destroyed */
UENUM(BlueprintType)
enum class ETargetDestructionCondition : uint8
{
	None UMETA(DisplayName="None"),
	/** Targets are never destroyed, even if their health reaches zero. Can still be reactivated/deactivated */
	Persistant UMETA(DisplayName="Persistant"),
	/** Target is deactivated after its damageable window closes */
	OnExpiration UMETA(DisplayName="On Expiration"),
	/** Target is destroyed when it receives any damage from the player */
	OnAnyExternalDamageTaken UMETA(DisplayName="On Any External Damage Taken"),
	/** Target is destroyed when its health reaches zero */
	OnHealthReachedZero UMETA(DisplayName="On Health Reached Zero"),
	/** Target is destroyed when any of its deactivation conditions are met. This essentially makes any deactivation condition a destruction condition */
	OnDeactivation UMETA(DisplayName="On Deactivation")
};
ENUM_RANGE_BY_FIRST_AND_LAST(ETargetDestructionCondition, ETargetDestructionCondition::Persistant, ETargetDestructionCondition::OnDeactivation);


/** What does the target do when its activated: change directions, make damageable, etc */
UENUM(BlueprintType)
enum class ETargetActivationResponse : uint8
{
	None UMETA(DisplayName="None"),
	/** Immunity is removed from the target, damageable window begins if not already damageable */
	RemoveImmunity UMETA(DisplayName="Remove Immunity"),
	/** Immunity is granted to the target */
	AddImmunity UMETA(DisplayName="Add Immunity"),
	/** Immunity is added or removed from the target, depending on its existing state. Same with damageable window */
	ToggleImmunity UMETA(DisplayName="Toggle Immunity"),
	/** If a moving target, the direction is changed */
	ChangeDirection UMETA(DisplayName="Change Direction"),
	/** The scale is set according to ConsecutiveTargetScale. LifetimeTargetScale can still override this */
	ChangeScale UMETA(DisplayName="Change Scale"),
	/** If a moving target, the velocity of the target is changed */
	ChangeVelocity UMETA(DisplayName="Change Velocity"),
};
ENUM_RANGE_BY_FIRST_AND_LAST(ETargetActivationResponse, ETargetActivationResponse::RemoveImmunity, ETargetActivationResponse::ChangeVelocity);


/** What does the target do when its deactivated */
UENUM(BlueprintType)
enum class ETargetDeactivationResponse : uint8
{
	None UMETA(DisplayName="None"),
	/** Immunity is removed from the target, damageable window begins if not already damageable */
	RemoveImmunity UMETA(DisplayName="Remove Immunity"),
	/** Immunity is granted to the target */
	AddImmunity UMETA(DisplayName="Add Immunity"),
	/** Immunity is added or removed from the target, depending on its existing state. Same with damageable window */
	ToggleImmunity UMETA(DisplayName="Toggle Immunity"),
	/** The scale is set according to ConsecutiveTargetScale. LifetimeTargetScale can still override this */
	ApplyDeactivatedTargetScaleMultiplier UMETA(DisplayName="Apply Deactivated Target Scale Multiplier"),
	/** Reset the scale of the target to the scale it was initialized with */
	ResetScale UMETA(DisplayName="Reset Scale"),
	/** Reset the position of the target to the position it was initialized with */
	ResetPosition UMETA(DisplayName="Reset Position"),
	/** Reset the color of the target to the inactive color */
	ResetColorToInactiveColor UMETA(DisplayName="Reset Color To Inactive Color"),
	/** Reset the scale of the target to the scale it was initialized with */
	ShrinkQuickGrowSlow UMETA(DisplayName="Shrink Quick Grow Slow"),
	/** Play an explosion effect */
	PlayExplosionEffect UMETA(DisplayName="Play Explosion Effect"),
	/** Destroy the target permanently */
	Destroy UMETA(DisplayName="Destroy"),
	/** Hide the target */
	HideTarget UMETA(DisplayName="Hide Target"),
	/** DEPRECATED */
	Deprecated1 UMETA(DisplayName="Hide"),
	/** DEPRECATED */
	ChangeDirection UMETA(DisplayName="Change Direction"),
	/** DEPRECATED */
	ChangeVelocity UMETA(DisplayName="Change Velocity"),
	/** DEPRECATED */
	ChangeScale UMETA(DisplayName="Change Scale"),
};
ENUM_RANGE_BY_FIRST_AND_LAST(ETargetDeactivationResponse, ETargetDeactivationResponse::RemoveImmunity, ETargetDeactivationResponse::HideTarget);

/** How to handle activating a target. For example, if a target was spawned, activated, and deactivated but not destroyed, should another target be allowed to spawned/activated? */
/*UENUM(BlueprintType)
enum class ETargetActivationPolicy : uint8
{
	None UMETA(DisplayName="None"),
	/** Activate the target immediately on spawn #1#
	ReactivateUntilDestruction UMETA(DisplayName="Reactivate Until Destruction"),
	/** Activate the target when the TargetManager receives the signal from AudioAnalyzer #1#
	DoNotInterrupt UMETA(DisplayName="Do Not Interrupt"),
};
ENUM_RANGE_BY_FIRST_AND_LAST(ETargetActivationPolicy, ETargetActivationPolicy::Interrupt, ETargetActivationPolicy::DoNotInterrupt);*/