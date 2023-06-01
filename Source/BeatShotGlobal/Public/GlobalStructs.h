// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once
#include "GlobalEnums.h"
#include "DLSSLibrary.h"
#include "GameplayTagContainer.h"
#include "GlobalConstants.h"
#include "NISLibrary.h"
#include "GlobalStructs.generated.h"

struct FGameplayTagContainer;
using namespace Constants;

/** Struct only used to save accuracy to database */
USTRUCT()
struct FAccuracyRow
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<float> Accuracy;
	TArray<int32> TotalSpawns;
	TArray<int32> TotalHits;

	FAccuracyRow()
	{
		Accuracy = TArray<float>();
		TotalSpawns = TArray<int32>();
		TotalHits = TArray<int32>();
	}

	FAccuracyRow(const int32 Size)
	{
		Accuracy = TArray<float>();
		Accuracy.Init(-1.f, Size);
		TotalSpawns = TArray<int32>();
		TotalSpawns.Init(-1, Size);
		TotalHits = TArray<int32>();
		TotalHits.Init(0, Size);
	}

	/** Updates the accuracy array based on all TotalSpawns and TotalHits */
	void UpdateAccuracy()
	{
		for (int i = 0; i < Accuracy.Num(); i++)
		{
			if (TotalSpawns[i] == INDEX_NONE)
			{
				continue;
			}
			Accuracy[i] = static_cast<float>(TotalHits[i]) / static_cast<float>(TotalSpawns[i]);
		}
	}
};

/** Struct containing any information to save between game mode sessions that does not define the game mode itself, e.g. accuracy */
USTRUCT(BlueprintType)
struct FCommonScoreInfo
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<float> Accuracy;

	UPROPERTY()
	TArray<int32> TotalSpawns;

	UPROPERTY()
	TArray<int32> TotalHits;

	UPROPERTY()
	TArray<float> QTable;

	FCommonScoreInfo()
	{
		Accuracy = TArray<float>();
		TotalSpawns = TArray<int32>();
		TotalHits = TArray<int32>();
		QTable = TArray<float>();
	}

	FCommonScoreInfo(const int32 Size)
	{
		Accuracy = TArray<float>();
		Accuracy.Init(-1.f, Size);
		TotalSpawns = TArray<int32>();
		TotalSpawns.Init(-1, Size);
		TotalHits = TArray<int32>();
		TotalHits.Init(0, Size);
		QTable.Init(0.f, Size * Size);
	}

	/** Updates TotalSpawns, TotalHits, and calls UpdateAccuracy */
	void UpdateCommonValues(const TArray<int32>& InTotalSpawns, const TArray<int32>& InTotalHits)
	{
		if (InTotalSpawns.Num() != TotalSpawns.Num() || InTotalHits.Num() != TotalHits.Num())
		{
			return;
		}
		for (int32 i = 0; i < InTotalSpawns.Num(); i++)
		{
			if (InTotalSpawns[i] != INDEX_NONE)
			{
				CheckTotalSpawns(i);
				TotalSpawns[i] += InTotalSpawns[i];
			}
			TotalHits[i] += InTotalHits[i];
		}
		UpdateAccuracy();
	}

	/** Do not call directly, called inside UpdateCommonValues */
	void UpdateAccuracy()
	{
		for (int i = 0; i < Accuracy.Num(); i++)
		{
			if (TotalSpawns[i] == INDEX_NONE)
			{
				continue;
			}
			Accuracy[i] = static_cast<float>(TotalHits[i]) / static_cast<float>(TotalSpawns[i]);
		}
	}

	/** Switches a TotalSpawn index to zero if it has not been activated yet */
	void CheckTotalSpawns(const int32 IndexToActivate)
	{
		if (TotalSpawns[IndexToActivate] == INDEX_NONE)
		{
			TotalSpawns[IndexToActivate] = 0;
		}
	}
};

/** Small Struct containing the information needed to distinguish between unique default game modes and unique custom game modes.
 *  This info persists across different songs, which is why it is separate from FPlayerScore */
USTRUCT(BlueprintType)
struct FBS_DefiningConfig
{
	GENERATED_BODY()

	/** The type of game mode: either preset or custom */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | Defining Config")
	EGameModeType GameModeType;

	/** The base game mode this game mode is based off of */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | Defining Config")
	EBaseGameMode BaseGameMode;

	/** Custom game mode name if custom, otherwise empty string */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | Defining Config")
	FString CustomGameModeName;

	/** Default game mode difficulties, or none if custom */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | Defining Config")
	EGameModeDifficulty Difficulty;

	FBS_DefiningConfig()
	{
		GameModeType = EGameModeType::None;
		BaseGameMode = EBaseGameMode::None;
		CustomGameModeName = "";
		Difficulty = EGameModeDifficulty::None;
	}

	FBS_DefiningConfig(const EGameModeType& InGameModeType, const EBaseGameMode& InBaseGameMode, const FString& InCustomGameModeName, const EGameModeDifficulty& InGameModeDifficulty)
	{
		GameModeType = InGameModeType;
		BaseGameMode = InBaseGameMode;
		CustomGameModeName = InCustomGameModeName;
		Difficulty = InGameModeDifficulty;
	}

	FORCEINLINE bool operator==(const FBS_DefiningConfig& Other) const
	{
		/* Preset vs Custom */
		if (GameModeType == Other.GameModeType)
		{
			/* Custom game modes don't depend on difficulty */
			if (GameModeType == EGameModeType::Custom && CustomGameModeName.Equals(Other.CustomGameModeName, ESearchCase::IgnoreCase))
			{
				return true;
			}
			/* Default game modes must match the difficulty to be considered equal */
			if (GameModeType == EGameModeType::Preset && Difficulty == Other.Difficulty)
			{
				return true;
			}
		}
		return false;
	}
};

FORCEINLINE uint32 GetTypeHash(const FBS_DefiningConfig& Config)
{
	return FCrc::MemCrc32(&Config, sizeof(FBS_DefiningConfig));
}

USTRUCT(BlueprintType)
struct FBS_AIConfig
{
	GENERATED_BODY()

	/** Whether or not to enable the reinforcement learning agent to handle target spawning */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | AI")
	bool bEnableReinforcementLearning;

	/** Learning rate, or how much to update the Q-Table rewards when a reward is received */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | AI")
	float Alpha;

	/** The exploration/exploitation balance factor. A value = 1 will result in only choosing random values (explore),
	 *  while a value of zero will result in only choosing the max Q-value (exploitation) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | AI")
	float Epsilon;

	/** Discount factor, or how much to value future rewards vs immediate rewards */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | AI")
	float Gamma;

	FBS_AIConfig()
	{
		bEnableReinforcementLearning = false;
		Alpha = DefaultAlpha;
		Epsilon = DefaultEpsilon;
		Gamma = DefaultGamma;
	}
};

USTRUCT(BlueprintType)
struct FBS_GridConfig
{
	GENERATED_BODY()

	/** The number of horizontal grid targets*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | BeatGrid")
	int32 NumHorizontalGridTargets;

	/** The number of vertical grid targets*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | BeatGrid")
	int32 NumVerticalGridTargets;

	/** Whether or not to randomize the activation of grid targets vs only choosing adjacent targets */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | BeatGrid")
	bool bRandomizeGridTargetActivation;

	/** The space between grid targets */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | BeatGrid")
	FVector2D GridSpacing;

	/** Number of grid target visible at any one time */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | BeatGrid")
	int32 NumGridTargetsVisibleAtOnce;

	FBS_GridConfig()
	{
		NumHorizontalGridTargets = NumHorizontalBeatGridTargets_Normal;
		NumVerticalGridTargets = NumVerticalBeatGridTargets_Normal;
		bRandomizeGridTargetActivation = false;
		NumGridTargetsVisibleAtOnce = NumTargetsAtOnceBeatGrid_Normal;
		GridSpacing = BeatGridSpacing_Normal;
	}
};

USTRUCT(BlueprintType)
struct FBS_AudioConfig
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
	FString SongTitle;

	/** Whether or not to playback streamed audio */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
	bool bPlaybackAudio;

	/** The audio format type used for the AudioAnalyzer */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
	EAudioFormat AudioFormat;

	/** The input audio device */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
	FString InAudioDevice;

	/** The output audio device */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
	FString OutAudioDevice;

	/** The path to the song file */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
	FString SongPath;

	/** Delay between AudioAnalyzer Tracker and Player. Also the same value as time between target spawn and peak green target color */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
	float PlayerDelay;

	/** Length of song */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
	float SongLength;

	FBS_AudioConfig()
	{
		bPlaybackAudio = false;
		AudioFormat = EAudioFormat::None;
		InAudioDevice = "";
		OutAudioDevice = "";
		SongPath = "";
		SongTitle = "";
		SongLength = 0.f;
		PlayerDelay = DefaultPlayerDelay;
	}
};

USTRUCT(BlueprintType)
struct FBS_TargetConfig
{
	GENERATED_BODY()

	/** Should the target be immune to damage when spawned? */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bApplyImmunityOnSpawn;

	/** Continuously spawn targets or wait until a TargetDamageEvent/expiration to continue spawning */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bContinuouslySpawn;

	/** Whether or not the targets ever move */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bMoveTargets;
	
	/** Whether or not to move the targets forward towards the player after spawning */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bMoveTargetsForward;
	
	/** Whether or not to spawn at the origin if it isn't blocked by a recent target whenever possible */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bSpawnAtOriginWheneverPossible;
	
	/** Whether or not alternate every target spawn in the very center */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bSpawnEveryOtherTargetInCenter;

	/** Whether or not to reverse the direction of a moving target after it stops overlapping the OverlapSpawnBox */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bUseOverlapSpawnBox;
	
	/** Whether or not to use separate outline color */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bUseSeparateOutlineColor;
	
	/** How to scale the bounding box bounds (spawn area where targets are spawned), at runtime */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EBoundsScalingPolicy BoundsScalingPolicy;

	/** Whether or not to dynamically change the size of targets as consecutive targets are hit */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EConsecutiveTargetScalePolicy ConsecutiveTargetScalePolicy;

	/** The method for handling changing target scale over it's lifetime */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ELifetimeTargetScalePolicy LifetimeTargetScalePolicy;

	/** Specifies the method to remove targets from recent memory, allowing targets to spawn in that location again */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ERecentTargetMemoryPolicy RecentTargetMemoryPolicy;

	/** How to choose the target(s) to activate */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ETargetActivationSelectionPolicy TargetActivationSelectionPolicy;

	/** How the player damages the target and receives score */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ETargetDamageType TargetDamageType;

	/** Where to spawn/activate targets in the bounding box bounds (spawn area) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ETargetDistributionPolicy TargetDistributionPolicy;
	
	/** When to spawn targets */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ETargetSpawningPolicy TargetSpawningPolicy;

	/** The possible outcomes that a target can do when its activated: change directions, make damageable, etc */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<ETargetActivationResponse> TargetActivationResponses;

	/** Any condition that should deactivate the target (make it immune to damage) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<ETargetDeactivationCondition> TargetDeactivationConditions;

	/** Anything the target should do when it deactivates */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<ETargetDeactivationResponse> TargetDeactivationResponses;
	
	/** Any condition that should permanently destroy a the target */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<ETargetDestructionCondition> TargetDestructionConditions;
	
	/** How much to shrink the target each time a charge is consumed, if the target is charged. This is multiplied
	 *  against the last charged target scale. A fully charged target does not receive any multiplier */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ConsecutiveChargeScaleMultiplier;

	/** Amount of health to take away from the target if the DamageableWindow timer expires */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ExpirationHealthPenalty;

	/** Distance from bottom of TargetManager BoxBounds to the floor */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float FloorDistance;
	
	/** Sets the minimum distance between recent target spawns */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MinDistanceBetweenTargets;

	/** Value to set the MaxHealth attribute value to */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxHealth;

	/** How far to move the target forward over its lifetime */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MoveForwardDistance;

	/** Min multiplier to target size */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MinTargetScale;

	/** Max multiplier to target size */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxTargetScale;
	
	/** Minimum speed multiplier for a moving target */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MinTargetSpeed;

	/** Maximum speed multiplier for a moving target */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxTargetSpeed;

	/** Length of time to keep targets flags as recent, if not using MaxNumRecentTargets */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float RecentTargetTimeLength;

	/** Delay between time between target spawn and peak green target color. Same as PlayerDelay in FBS_AudioConfig */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float SpawnBeatDelay;

	/** Maximum time in which target will stay on screen */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float TargetMaxLifeSpan;
	
	/** Sets the minimum time between target spawns */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float TargetSpawnCD;
	
	/** Gameplay tags applied to the target ASC when spawned */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTagContainer OnSpawn_ApplyTags;

	/** The size of the target spawn BoundingBox. Dimensions are half of the the total length/width */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FVector BoxBounds;

	/** How many recent targets to keep in memory, if not using RecentTargetTimeLength */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 MaxNumRecentTargets;

	/** TODO: Minimum number of targets to activate at one time */
	int32 MinNumTargetsToActivateAtOnce;
	
	/** TODO: Maximum number of targets to activate at one time */
	int32 MaxNumTargetsToActivateAtOnce;
	
	/** How many targets to spawn before the game mode begins */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 NumUpfrontTargetsToSpawn;
	
	/** Color to applied to the actor if inactive */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FLinearColor InactiveTargetColor;

	/** Color applied to target on spawn */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FLinearColor OnSpawnColor;

	/** Color interpolated from at start of DamageableWindow timer */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FLinearColor StartColor;
	
	/** Color interpolated to from StartColor */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FLinearColor PeakColor;

	/** Color interpolated to from PeakColor */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FLinearColor EndColor;

	/** Separate outline color if specified */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FLinearColor OutlineColor;

	FBS_TargetConfig()
	{
		bApplyImmunityOnSpawn = false;
		bContinuouslySpawn = false;
		bMoveTargets = false;
		bMoveTargetsForward = false;
		bSpawnAtOriginWheneverPossible = false;
		bSpawnEveryOtherTargetInCenter = false;
		bUseOverlapSpawnBox = false;
		bUseSeparateOutlineColor = false;
		
		BoundsScalingPolicy = EBoundsScalingPolicy::None;
		ConsecutiveTargetScalePolicy = EConsecutiveTargetScalePolicy::None;
		LifetimeTargetScalePolicy = ELifetimeTargetScalePolicy::None;
		RecentTargetMemoryPolicy = ERecentTargetMemoryPolicy::None;
		TargetActivationSelectionPolicy = ETargetActivationSelectionPolicy::None;
		TargetDamageType = ETargetDamageType::None;
		TargetDistributionPolicy = ETargetDistributionPolicy::None;
		TargetSpawningPolicy = ETargetSpawningPolicy::None;

		TargetActivationResponses = TArray<ETargetActivationResponse>();
		TargetDeactivationConditions = TArray<ETargetDeactivationCondition>();
		TargetDeactivationResponses = TArray<ETargetDeactivationResponse>();
		TargetDestructionConditions = TArray<ETargetDestructionCondition>();

		ConsecutiveChargeScaleMultiplier = DefaultChargeScaleMultiplier;
		ExpirationHealthPenalty = BaseTargetHealth;
		FloorDistance = DistanceFromFloor;
		MinDistanceBetweenTargets = DefaultMinDistanceBetweenTargets;
		MaxHealth = BaseTargetHealth;
		MoveForwardDistance = 0.f;
		MinTargetScale = DefaultMinTargetScale;
		MaxTargetScale = DefaultMaxTargetScale;
		MinTargetSpeed = 0.f;
		MaxTargetSpeed = 0.f;
		SpawnBeatDelay = DefaultSpawnBeatDelay;
		RecentTargetTimeLength = 0.f;
		TargetMaxLifeSpan = DefaultTargetMaxLifeSpan;
		TargetSpawnCD = DefaultTargetSpawnCD;
		
		OnSpawn_ApplyTags = FGameplayTagContainer();
		BoxBounds = DefaultSpawnBoxBounds;

		MaxNumRecentTargets = 0;
		MinNumTargetsToActivateAtOnce = 1;
		MaxNumTargetsToActivateAtOnce = 1;
		NumUpfrontTargetsToSpawn = 0;
		
		InactiveTargetColor = FLinearColor();
		OnSpawnColor = FLinearColor();
		StartColor = FLinearColor();
		PeakColor = FLinearColor();
		EndColor = FLinearColor();
		OutlineColor = FLinearColor();
	}


	/** Returns the location to spawn the SpawnBox at */
	FVector GenerateSpawnBoxLocation() const
	{
		FVector SpawnBoxCenter = DefaultTargetManagerLocation;
		if (TargetDistributionPolicy == ETargetDistributionPolicy::HeadshotHeightOnly)
		{
			SpawnBoxCenter.Z = HeadshotHeight;
		}
		else
		{
			SpawnBoxCenter.Z = BoxBounds.Z / 2.f + FloorDistance;
		}
		return SpawnBoxCenter;
	}

	/** Returns the actual BoxBounds that the TargetManager sets its BoxBounds to */
	FVector GenerateTargetManagerBoxBounds() const
	{
		if (TargetDistributionPolicy == ETargetDistributionPolicy::HeadshotHeightOnly)
		{
			return FVector(0.f, BoxBounds.Y / 2.f, 1.f);
		}
		return FVector(0.f, BoxBounds.Y / 2.f, BoxBounds.Z / 2.f);
	}
};

/** Struct representing a game mode */
USTRUCT(BlueprintType)
struct FBSConfig
{
	GENERATED_BODY()

	/** The defining config for a game mode, containing the names, base, difficulty */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | Defining Config")
	FBS_DefiningConfig DefiningConfig;

	/** Contains info for the target spawner about how to handle the RLAgent */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | AI")
	FBS_AIConfig AIConfig;

	/** Contains info for the AudioAnalyzer and PlayerHUD */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | Audio")
	FBS_AudioConfig AudioConfig;

	/** Contains info for the target spawner for BeatGrid specific game modes */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | BeatGrid")
	FBS_GridConfig GridConfig;

	/** Contains info for the target spawner about how to spawn the targets, as well as info to give the targets */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FBS_TargetConfig TargetConfig;

	FORCEINLINE bool operator==(const FBSConfig& Other) const
	{
		if (DefiningConfig == Other.DefiningConfig)
		{
			return true;
		}
		return false;
	}

	/** Generic constructor */
	FBSConfig()
	{
		DefiningConfig = FBS_DefiningConfig();
		AIConfig = FBS_AIConfig();
		AudioConfig = FBS_AudioConfig();
		GridConfig = FBS_GridConfig();
		TargetConfig = FBS_TargetConfig();
	}

	/** Returns a preset FBSConfig based on the preset base game mode and difficulty */
	static FBSConfig MakePresetConfig(const EBaseGameMode& InPresetGameMode, const EGameModeDifficulty& InDifficulty)
	{
		FBSConfig Config;
		Config.DefiningConfig = GetConfigForPreset(InPresetGameMode, InDifficulty);
		switch (Config.DefiningConfig.BaseGameMode)
		{
		case EBaseGameMode::SingleBeat:
			{
				switch (Config.DefiningConfig.Difficulty)
				{
				case EGameModeDifficulty::Normal:
					{
						Config.TargetConfig.SpawnBeatDelay = SpawnBeatDelay_SingleBeat_Normal;
						Config.TargetConfig.TargetSpawnCD = TargetSpawnCD_SingleBeat_Normal;
						Config.TargetConfig.TargetMaxLifeSpan = TargetMaxLifeSpan_SingleBeat_Normal;
						Config.TargetConfig.MinTargetScale = MinTargetScale_SingleBeat_Normal;
						Config.TargetConfig.MaxTargetScale = MaxTargetScale_SingleBeat_Normal;
						break;
					}
				case EGameModeDifficulty::Hard:
					{
						Config.TargetConfig.SpawnBeatDelay = SpawnBeatDelay_SingleBeat_Hard;
						Config.TargetConfig.TargetSpawnCD = TargetSpawnCD_SingleBeat_Hard;
						Config.TargetConfig.TargetMaxLifeSpan = TargetMaxLifeSpan_SingleBeat_Hard;
						Config.TargetConfig.MinTargetScale = MinTargetScale_SingleBeat_Hard;
						Config.TargetConfig.MaxTargetScale = MaxTargetScale_SingleBeat_Hard;
						break;
					}
				case EGameModeDifficulty::Death:
					{
						Config.TargetConfig.SpawnBeatDelay = SpawnBeatDelay_SingleBeat_Death;
						Config.TargetConfig.TargetSpawnCD = TargetSpawnCD_SingleBeat_Death;
						Config.TargetConfig.TargetMaxLifeSpan = TargetMaxLifeSpan_SingleBeat_Death;
						Config.TargetConfig.MinTargetScale = MinTargetScale_SingleBeat_Death;
						Config.TargetConfig.MaxTargetScale = MaxTargetScale_SingleBeat_Death;
						break;
					}
				case EGameModeDifficulty::None:
					break;
				}
				Config.TargetConfig.bApplyImmunityOnSpawn = false;
				Config.TargetConfig.bContinuouslySpawn = false;
				Config.TargetConfig.bMoveTargets = false;
				Config.TargetConfig.bMoveTargetsForward = false;
				Config.TargetConfig.bSpawnAtOriginWheneverPossible = true;
				Config.TargetConfig.bSpawnEveryOtherTargetInCenter = true;
				Config.TargetConfig.bUseOverlapSpawnBox = false;

				Config.TargetConfig.BoundsScalingPolicy = EBoundsScalingPolicy::Dynamic;
				Config.TargetConfig.ConsecutiveTargetScalePolicy = EConsecutiveTargetScalePolicy::SkillBased;
				Config.TargetConfig.LifetimeTargetScalePolicy = ELifetimeTargetScalePolicy::None;
				Config.TargetConfig.RecentTargetMemoryPolicy = ERecentTargetMemoryPolicy::UseTargetSpawnCD;
				Config.TargetConfig.TargetActivationSelectionPolicy = ETargetActivationSelectionPolicy::Random;
				Config.TargetConfig.TargetDamageType = ETargetDamageType::Hit;
				Config.TargetConfig.TargetDistributionPolicy = ETargetDistributionPolicy::EdgeOnly;
				Config.TargetConfig.TargetSpawningPolicy = ETargetSpawningPolicy::RuntimeOnly;

				Config.TargetConfig.TargetActivationResponses.Add(ETargetActivationResponse::RemoveImmunity);
				
				Config.TargetConfig.TargetDeactivationConditions.Add(ETargetDeactivationCondition::OnAnyExternalDamageTaken);
				Config.TargetConfig.TargetDeactivationConditions.Add(ETargetDeactivationCondition::OnExpiration);
				Config.TargetConfig.TargetDeactivationResponses.Add(ETargetDeactivationResponse::PlayExplosionEffect);
				Config.TargetConfig.TargetDeactivationResponses.Add(ETargetDeactivationResponse::Destroy);
				
				Config.TargetConfig.TargetDestructionConditions.Add(ETargetDestructionCondition::OnDeactivation);
				
				Config.TargetConfig.ConsecutiveChargeScaleMultiplier = DefaultChargeScaleMultiplier;
				Config.TargetConfig.ExpirationHealthPenalty = BaseTargetHealth;
				Config.TargetConfig.FloorDistance = DistanceFromFloor;
				Config.TargetConfig.MinDistanceBetweenTargets = DefaultMinDistanceBetweenTargets;
				Config.TargetConfig.MaxHealth = BaseTargetHealth;
				Config.TargetConfig.MoveForwardDistance = 0.f;
				Config.TargetConfig.MinTargetSpeed = 0.f;
				Config.TargetConfig.MaxTargetSpeed = 0.f;
				
				Config.TargetConfig.OnSpawn_ApplyTags = FGameplayTagContainer();
				Config.TargetConfig.BoxBounds = BoxBounds_Dynamic_SingleBeat;
				Config.TargetConfig.MinNumTargetsToActivateAtOnce = 1;
				Config.TargetConfig.MaxNumTargetsToActivateAtOnce = 1;
				Config.TargetConfig.NumUpfrontTargetsToSpawn = 0;
			}
			break;
		case EBaseGameMode::MultiBeat:
			{
				switch (Config.DefiningConfig.Difficulty)
				{
				case EGameModeDifficulty::Normal:
					{
						Config.TargetConfig.SpawnBeatDelay = SpawnBeatDelay_MultiBeat_Normal;
						Config.TargetConfig.TargetSpawnCD = TargetSpawnCD_MultiBeat_Normal;
						Config.TargetConfig.TargetMaxLifeSpan = TargetMaxLifeSpan_MultiBeat_Normal;
						Config.TargetConfig.MinTargetScale = MinTargetScale_MultiBeat_Normal;
						Config.TargetConfig.MaxTargetScale = MaxTargetScale_MultiBeat_Normal;
						break;
					}
				case EGameModeDifficulty::Hard:
					{
						Config.TargetConfig.SpawnBeatDelay = SpawnBeatDelay_MultiBeat_Hard;
						Config.TargetConfig.TargetSpawnCD = TargetSpawnCD_MultiBeat_Hard;
						Config.TargetConfig.TargetMaxLifeSpan = TargetMaxLifeSpan_MultiBeat_Hard;
						Config.TargetConfig.MinTargetScale = MinTargetScale_MultiBeat_Hard;
						Config.TargetConfig.MaxTargetScale = MaxTargetScale_MultiBeat_Hard;
						break;
					}
				case EGameModeDifficulty::Death:
					{
						Config.TargetConfig.SpawnBeatDelay = SpawnBeatDelay_MultiBeat_Death;
						Config.TargetConfig.TargetSpawnCD = TargetSpawnCD_MultiBeat_Death;
						Config.TargetConfig.TargetMaxLifeSpan = TargetMaxLifeSpan_MultiBeat_Death;
						Config.TargetConfig.MinTargetScale = MinTargetScale_MultiBeat_Death;
						Config.TargetConfig.MaxTargetScale = MaxTargetScale_MultiBeat_Death;
						break;
					}
				case EGameModeDifficulty::None:
					break;
				}
				Config.TargetConfig.bApplyImmunityOnSpawn = false;
				Config.TargetConfig.bContinuouslySpawn = true;
				Config.TargetConfig.bMoveTargets = false;
				Config.TargetConfig.bMoveTargetsForward = false;
				Config.TargetConfig.bSpawnAtOriginWheneverPossible = true;
				Config.TargetConfig.bSpawnEveryOtherTargetInCenter = false;
				Config.TargetConfig.bUseOverlapSpawnBox = false;

				Config.TargetConfig.BoundsScalingPolicy = EBoundsScalingPolicy::Dynamic;
				Config.TargetConfig.ConsecutiveTargetScalePolicy = EConsecutiveTargetScalePolicy::SkillBased;
				Config.TargetConfig.LifetimeTargetScalePolicy = ELifetimeTargetScalePolicy::None;
				Config.TargetConfig.RecentTargetMemoryPolicy = ERecentTargetMemoryPolicy::UseTargetSpawnCD;
				Config.TargetConfig.TargetActivationSelectionPolicy = ETargetActivationSelectionPolicy::Random;
				Config.TargetConfig.TargetDamageType = ETargetDamageType::Hit;
				Config.TargetConfig.TargetDistributionPolicy = ETargetDistributionPolicy::FullRange;
				Config.TargetConfig.TargetSpawningPolicy = ETargetSpawningPolicy::RuntimeOnly;

				Config.TargetConfig.TargetActivationResponses.Add(ETargetActivationResponse::RemoveImmunity);
				
				Config.TargetConfig.TargetDeactivationConditions.Add(ETargetDeactivationCondition::OnAnyExternalDamageTaken);
				Config.TargetConfig.TargetDeactivationConditions.Add(ETargetDeactivationCondition::OnExpiration);
				Config.TargetConfig.TargetDeactivationResponses.Add(ETargetDeactivationResponse::PlayExplosionEffect);
				Config.TargetConfig.TargetDeactivationResponses.Add(ETargetDeactivationResponse::Destroy);

				Config.TargetConfig.TargetDestructionConditions.Add(ETargetDestructionCondition::OnDeactivation);
				
				Config.TargetConfig.ConsecutiveChargeScaleMultiplier = DefaultChargeScaleMultiplier;
				Config.TargetConfig.ExpirationHealthPenalty = BaseTargetHealth;
				Config.TargetConfig.FloorDistance = DistanceFromFloor;
				Config.TargetConfig.MinDistanceBetweenTargets = DefaultMinDistanceBetweenTargets;
				Config.TargetConfig.MaxHealth = BaseTargetHealth;
				Config.TargetConfig.MoveForwardDistance = 0.f;
				Config.TargetConfig.MinTargetSpeed = 0.f;
				Config.TargetConfig.MaxTargetSpeed = 0.f;
				
				Config.TargetConfig.OnSpawn_ApplyTags = FGameplayTagContainer();
				Config.TargetConfig.BoxBounds = BoxBounds_Dynamic_MultiBeat;
				Config.TargetConfig.MinNumTargetsToActivateAtOnce = 1;
				Config.TargetConfig.MaxNumTargetsToActivateAtOnce = 1;
				Config.TargetConfig.NumUpfrontTargetsToSpawn = 0;
			}
			break;
		case EBaseGameMode::BeatGrid:
			{
				switch (Config.DefiningConfig.Difficulty)
				{
				case EGameModeDifficulty::Normal:
					{
						Config.TargetConfig.SpawnBeatDelay = SpawnBeatDelay_BeatGrid_Normal;
						Config.TargetConfig.TargetSpawnCD = TargetSpawnCD_BeatGrid_Normal;
						Config.TargetConfig.TargetMaxLifeSpan = TargetMaxLifeSpan_BeatGrid_Normal;
						Config.TargetConfig.MinTargetScale = MinTargetScale_BeatGrid_Normal;
						Config.TargetConfig.MaxTargetScale = MaxTargetScale_BeatGrid_Normal;
						Config.GridConfig.NumHorizontalGridTargets = NumHorizontalBeatGridTargets_Normal;
						Config.GridConfig.NumVerticalGridTargets = NumVerticalBeatGridTargets_Normal;
						Config.GridConfig.bRandomizeGridTargetActivation = false;
						Config.GridConfig.NumGridTargetsVisibleAtOnce = NumTargetsAtOnceBeatGrid_Normal;
						Config.GridConfig.GridSpacing = BeatGridSpacing_Normal;
						break;
					}
				case EGameModeDifficulty::Hard:
					{
						Config.TargetConfig.SpawnBeatDelay = SpawnBeatDelay_BeatGrid_Hard;
						Config.TargetConfig.TargetSpawnCD = TargetSpawnCD_BeatGrid_Hard;
						Config.TargetConfig.TargetMaxLifeSpan = TargetMaxLifeSpan_BeatGrid_Hard;
						Config.TargetConfig.MinTargetScale = MinTargetScale_BeatGrid_Hard;
						Config.TargetConfig.MaxTargetScale = MaxTargetScale_BeatGrid_Hard;
						Config.GridConfig.NumHorizontalGridTargets = NumHorizontalBeatGridTargets_Hard;
						Config.GridConfig.NumVerticalGridTargets = NumVerticalBeatGridTargets_Hard;
						Config.GridConfig.bRandomizeGridTargetActivation = false;
						Config.GridConfig.NumGridTargetsVisibleAtOnce = NumTargetsAtOnceBeatGrid_Hard;
						Config.GridConfig.GridSpacing = BeatGridSpacing_Hard;
						break;
					}
				case EGameModeDifficulty::Death:
					{
						Config.TargetConfig.SpawnBeatDelay = SpawnBeatDelay_BeatGrid_Death;
						Config.TargetConfig.TargetSpawnCD = TargetSpawnCD_BeatGrid_Death;
						Config.TargetConfig.TargetMaxLifeSpan = TargetMaxLifeSpan_BeatGrid_Death;
						Config.TargetConfig.MinTargetScale = MinTargetScale_BeatGrid_Death;
						Config.TargetConfig.MaxTargetScale = MaxTargetScale_BeatGrid_Death;
						Config.GridConfig.NumHorizontalGridTargets = NumHorizontalBeatGridTargets_Death;
						Config.GridConfig.NumVerticalGridTargets = NumVerticalBeatGridTargets_Death;
						Config.GridConfig.bRandomizeGridTargetActivation = false;
						Config.GridConfig.NumGridTargetsVisibleAtOnce = NumTargetsAtOnceBeatGrid_Death;
						Config.GridConfig.GridSpacing = BeatGridSpacing_Death;
						break;
					}
				case EGameModeDifficulty::None:
					break;
				}
				Config.TargetConfig.bApplyImmunityOnSpawn = true;
				// Might need to change
				Config.TargetConfig.bContinuouslySpawn = false;
				Config.TargetConfig.bMoveTargets = false;
				Config.TargetConfig.bMoveTargetsForward = false;
				Config.TargetConfig.bSpawnAtOriginWheneverPossible = false;
				Config.TargetConfig.bSpawnEveryOtherTargetInCenter = false;
				Config.TargetConfig.bUseOverlapSpawnBox = false;

				Config.TargetConfig.BoundsScalingPolicy = EBoundsScalingPolicy::Static;
				Config.TargetConfig.ConsecutiveTargetScalePolicy = EConsecutiveTargetScalePolicy::Static;
				Config.TargetConfig.LifetimeTargetScalePolicy = ELifetimeTargetScalePolicy::None;
				Config.TargetConfig.RecentTargetMemoryPolicy = ERecentTargetMemoryPolicy::UseTargetSpawnCD;
				Config.TargetConfig.TargetActivationSelectionPolicy = ETargetActivationSelectionPolicy::Bordering;
				Config.TargetConfig.TargetDamageType = ETargetDamageType::Hit;
				Config.TargetConfig.TargetDistributionPolicy = ETargetDistributionPolicy::Grid;
				Config.TargetConfig.TargetSpawningPolicy = ETargetSpawningPolicy::UpfrontOnly;

				Config.TargetConfig.TargetActivationResponses.Add(ETargetActivationResponse::RemoveImmunity);
				
				Config.TargetConfig.TargetDeactivationConditions.Add(ETargetDeactivationCondition::OnAnyExternalDamageTaken);
				Config.TargetConfig.TargetDeactivationConditions.Add(ETargetDeactivationCondition::OnExpiration);
				
				Config.TargetConfig.TargetDeactivationResponses.Add(ETargetDeactivationResponse::PlayExplosionEffect);
				Config.TargetConfig.TargetDeactivationResponses.Add(ETargetDeactivationResponse::AddImmunity);
				Config.TargetConfig.TargetDeactivationResponses.Add(ETargetDeactivationResponse::ResetPosition);
				Config.TargetConfig.TargetDeactivationResponses.Add(ETargetDeactivationResponse::ShrinkQuickGrowSlow);

				Config.TargetConfig.TargetDestructionConditions.Add(ETargetDestructionCondition::Persistant);
				
				Config.TargetConfig.ConsecutiveChargeScaleMultiplier = DefaultChargeScaleMultiplier;
				Config.TargetConfig.ExpirationHealthPenalty = BaseTargetHealth;
				Config.TargetConfig.FloorDistance = DistanceFromFloor;
				Config.TargetConfig.MinDistanceBetweenTargets = DefaultMinDistanceBetweenTargets;
				Config.TargetConfig.MaxHealth = BaseTargetHealth;
				Config.TargetConfig.MoveForwardDistance = 0.f;
				Config.TargetConfig.MinTargetSpeed = 0.f;
				Config.TargetConfig.MaxTargetSpeed = 0.f;
				
				Config.TargetConfig.OnSpawn_ApplyTags = FGameplayTagContainer();
				Config.TargetConfig.BoxBounds = DefaultSpawnBoxBounds;
				Config.TargetConfig.MinNumTargetsToActivateAtOnce = 1;
				Config.TargetConfig.MaxNumTargetsToActivateAtOnce = 1;
				Config.TargetConfig.NumUpfrontTargetsToSpawn = Config.GridConfig.NumHorizontalGridTargets * Config.GridConfig.NumVerticalGridTargets;
			}
			break;
		case EBaseGameMode::BeatTrack:
			{
				switch (Config.DefiningConfig.Difficulty)
				{
				case EGameModeDifficulty::Normal:
					{
						Config.TargetConfig.TargetSpawnCD = TargetSpawnCD_BeatTrack_Normal;
						Config.TargetConfig.MinTargetScale = MinTargetScale_BeatTrack_Normal;
						Config.TargetConfig.MaxTargetScale = MaxTargetScale_BeatTrack_Normal;
						Config.TargetConfig.MinTargetSpeed = MinTrackingSpeed_BeatTrack_Normal;
						Config.TargetConfig.MaxTargetSpeed = MaxTrackingSpeed_BeatTrack_Normal;
						break;
					}
				case EGameModeDifficulty::Hard:
					{
						Config.TargetConfig.TargetSpawnCD = TargetSpawnCD_BeatTrack_Hard;
						Config.TargetConfig.MinTargetScale = MinTargetScale_BeatTrack_Hard;
						Config.TargetConfig.MaxTargetScale = MaxTargetScale_BeatTrack_Hard;
						Config.TargetConfig.MinTargetSpeed = MinTrackingSpeed_BeatTrack_Hard;
						Config.TargetConfig.MaxTargetSpeed = MaxTrackingSpeed_BeatTrack_Hard;
						break;
					}
				case EGameModeDifficulty::Death:
					{
						Config.TargetConfig.TargetSpawnCD = TargetSpawnCD_BeatTrack_Death;
						Config.TargetConfig.MinTargetScale = MinTargetScale_BeatTrack_Death;
						Config.TargetConfig.MaxTargetScale = MaxTargetScale_BeatTrack_Death;
						Config.TargetConfig.MinTargetSpeed = MinTrackingSpeed_BeatTrack_Death;
						Config.TargetConfig.MaxTargetSpeed = MaxTrackingSpeed_BeatTrack_Death;
						break;
					}
				case EGameModeDifficulty::None:
					break;
				}
				Config.TargetConfig.bApplyImmunityOnSpawn = true;
				Config.TargetConfig.bContinuouslySpawn = false;
				Config.TargetConfig.bMoveTargets = false;
				Config.TargetConfig.bMoveTargetsForward = false;
				Config.TargetConfig.bSpawnAtOriginWheneverPossible = false;
				Config.TargetConfig.bSpawnEveryOtherTargetInCenter = false;
				Config.TargetConfig.bUseOverlapSpawnBox = true;

				Config.TargetConfig.BoundsScalingPolicy = EBoundsScalingPolicy::Static;
				Config.TargetConfig.ConsecutiveTargetScalePolicy = EConsecutiveTargetScalePolicy::Static;
				Config.TargetConfig.LifetimeTargetScalePolicy = ELifetimeTargetScalePolicy::None;
				Config.TargetConfig.RecentTargetMemoryPolicy = ERecentTargetMemoryPolicy::UseTargetSpawnCD;
				Config.TargetConfig.TargetActivationSelectionPolicy = ETargetActivationSelectionPolicy::Random;
				Config.TargetConfig.TargetDamageType = ETargetDamageType::Tracking;
				Config.TargetConfig.TargetDistributionPolicy = ETargetDistributionPolicy::FullRange;
				Config.TargetConfig.TargetSpawningPolicy = ETargetSpawningPolicy::UpfrontOnly;

				Config.TargetConfig.TargetActivationResponses.Add(ETargetActivationResponse::RemoveImmunity);
				Config.TargetConfig.TargetActivationResponses.Add(ETargetActivationResponse::ChangeDirection);
				
				Config.TargetConfig.TargetDeactivationConditions.Add(ETargetDeactivationCondition::Persistant);
				Config.TargetConfig.TargetDeactivationResponses.Add(ETargetDeactivationResponse::None);

				Config.TargetConfig.TargetDestructionConditions.Add(ETargetDestructionCondition::Persistant);
				
				Config.TargetConfig.ConsecutiveChargeScaleMultiplier = DefaultChargeScaleMultiplier;
				Config.TargetConfig.ExpirationHealthPenalty = 0.f;
				Config.TargetConfig.FloorDistance = DistanceFromFloor;
				Config.TargetConfig.MinDistanceBetweenTargets = DefaultMinDistanceBetweenTargets;
				Config.TargetConfig.MaxHealth = BaseTrackingTargetHealth;
				Config.TargetConfig.MoveForwardDistance = 0.f;
				Config.TargetConfig.SpawnBeatDelay = SpawnBeatDelay_BeatTrack;
				Config.TargetConfig.TargetMaxLifeSpan = TargetMaxLifeSpan_BeatTrack;
				
				Config.TargetConfig.OnSpawn_ApplyTags = FGameplayTagContainer();
				Config.TargetConfig.BoxBounds = DefaultSpawnBoxBounds;
				Config.TargetConfig.MinNumTargetsToActivateAtOnce = 1;
				Config.TargetConfig.MaxNumTargetsToActivateAtOnce = 1;
				Config.TargetConfig.NumUpfrontTargetsToSpawn = 1;
			}
			break;
		case EBaseGameMode::ChargedBeatTrack:
			{
				switch (Config.DefiningConfig.Difficulty)
				{
				case EGameModeDifficulty::Normal:
					{
						Config.TargetConfig.TargetSpawnCD = TargetSpawnCD_ChargedBeatTrack_Normal;
						Config.TargetConfig.MinTargetScale = MinTargetScale_ChargedBeatTrack_Normal;
						Config.TargetConfig.MaxTargetScale = MaxTargetScale_ChargedBeatTrack_Normal;
						Config.TargetConfig.MaxHealth = BaseTargetHealth * NumCharges_ChargedBeatTrack_Normal;
						Config.TargetConfig.ConsecutiveChargeScaleMultiplier = ChargeScaleMultiplier_ChargedBeatTrack_Normal;
						Config.TargetConfig.TargetMaxLifeSpan = TargetMaxLifeSpan_ChargedBeatTrack_Normal;
						Config.TargetConfig.MinTargetSpeed = MinTrackingSpeed_BeatTrack_Normal;
						Config.TargetConfig.MaxTargetSpeed = MaxTrackingSpeed_BeatTrack_Normal;
						break;
					}
				case EGameModeDifficulty::Hard:
					{
						Config.TargetConfig.TargetSpawnCD = TargetSpawnCD_ChargedBeatTrack_Hard;
						Config.TargetConfig.MinTargetScale = MinTargetScale_ChargedBeatTrack_Hard;
						Config.TargetConfig.MaxTargetScale = MaxTargetScale_ChargedBeatTrack_Hard;
						Config.TargetConfig.MaxHealth = BaseTargetHealth * NumCharges_ChargedBeatTrack_Hard;
						Config.TargetConfig.ConsecutiveChargeScaleMultiplier = ChargeScaleMultiplier_ChargedBeatTrack_Hard;
						Config.TargetConfig.TargetMaxLifeSpan = TargetMaxLifeSpan_ChargedBeatTrack_Hard;
						Config.TargetConfig.MinTargetSpeed = MinTrackingSpeed_BeatTrack_Hard;
						Config.TargetConfig.MaxTargetSpeed = MaxTrackingSpeed_BeatTrack_Hard;
						break;
					}
				case EGameModeDifficulty::Death:
					{
						Config.TargetConfig.TargetSpawnCD = TargetSpawnCD_ChargedBeatTrack_Death;
						Config.TargetConfig.MinTargetScale = MinTargetScale_ChargedBeatTrack_Death;
						Config.TargetConfig.MaxTargetScale = MaxTargetScale_ChargedBeatTrack_Death;
						Config.TargetConfig.MaxHealth = BaseTargetHealth * NumCharges_ChargedBeatTrack_Death;
						Config.TargetConfig.ConsecutiveChargeScaleMultiplier = ChargeScaleMultiplier_ChargedBeatTrack_Death;
						Config.TargetConfig.TargetMaxLifeSpan = TargetMaxLifeSpan_ChargedBeatTrack_Death;
						Config.TargetConfig.MinTargetSpeed = MinTrackingSpeed_BeatTrack_Death;
						Config.TargetConfig.MaxTargetSpeed = MaxTrackingSpeed_BeatTrack_Death;
						break;
					}
				case EGameModeDifficulty::None:
					break;
				}
				Config.TargetConfig.bApplyImmunityOnSpawn = true;
				Config.TargetConfig.bContinuouslySpawn = false;
				Config.TargetConfig.bMoveTargets = false;
				Config.TargetConfig.bMoveTargetsForward = false;
				Config.TargetConfig.bSpawnAtOriginWheneverPossible = false;
				Config.TargetConfig.bSpawnEveryOtherTargetInCenter = false;
				Config.TargetConfig.bUseOverlapSpawnBox = true;

				Config.TargetConfig.BoundsScalingPolicy = EBoundsScalingPolicy::Static;
				Config.TargetConfig.ConsecutiveTargetScalePolicy = EConsecutiveTargetScalePolicy::Static;
				Config.TargetConfig.LifetimeTargetScalePolicy = ELifetimeTargetScalePolicy::None;
				Config.TargetConfig.RecentTargetMemoryPolicy = ERecentTargetMemoryPolicy::UseTargetSpawnCD;
				Config.TargetConfig.TargetActivationSelectionPolicy = ETargetActivationSelectionPolicy::Random;
				Config.TargetConfig.TargetDamageType = ETargetDamageType::Hit;
				Config.TargetConfig.TargetDistributionPolicy = ETargetDistributionPolicy::FullRange;
				Config.TargetConfig.TargetSpawningPolicy = ETargetSpawningPolicy::RuntimeOnly;

				Config.TargetConfig.TargetActivationResponses.Add(ETargetActivationResponse::RemoveImmunity);
				Config.TargetConfig.TargetActivationResponses.Add(ETargetActivationResponse::ChangeDirection);
				
				Config.TargetConfig.TargetDeactivationConditions.Add(ETargetDeactivationCondition::OnAnyExternalDamageTaken);
				Config.TargetConfig.TargetDeactivationConditions.Add(ETargetDeactivationCondition::OnExpiration);
				Config.TargetConfig.TargetDeactivationResponses.Add(ETargetDeactivationResponse::PlayExplosionEffect);
				Config.TargetConfig.TargetDeactivationResponses.Add(ETargetDeactivationResponse::AddImmunity);
				Config.TargetConfig.TargetDeactivationResponses.Add(ETargetDeactivationResponse::ResetPosition);
				Config.TargetConfig.TargetDeactivationResponses.Add(ETargetDeactivationResponse::ShrinkQuickGrowSlow);

				Config.TargetConfig.TargetDestructionConditions.Add(ETargetDestructionCondition::OnHealthReachedZero);
				
				Config.TargetConfig.ExpirationHealthPenalty = BaseTargetHealth;
				Config.TargetConfig.FloorDistance = DistanceFromFloor;
				Config.TargetConfig.MinDistanceBetweenTargets = DefaultMinDistanceBetweenTargets;
				Config.TargetConfig.MoveForwardDistance = 0.f;
				Config.TargetConfig.SpawnBeatDelay = 0.25f;
				
				Config.TargetConfig.OnSpawn_ApplyTags = FGameplayTagContainer();
				Config.TargetConfig.BoxBounds = DefaultSpawnBoxBounds;
				Config.TargetConfig.MinNumTargetsToActivateAtOnce = 1;
				Config.TargetConfig.MaxNumTargetsToActivateAtOnce = 1;
				Config.TargetConfig.NumUpfrontTargetsToSpawn = 1;
			}
			break;
		case EBaseGameMode::None:
			break;
		}
		/* SpawnBeatDelay is the same as PlayerDelay */
		Config.AudioConfig.PlayerDelay = Config.TargetConfig.SpawnBeatDelay;
		return Config;
	}

	/** Returns an array of all preset game modes, all set to normal difficulty */
	static TArray<FBSConfig> GetPresetGameModes()
	{
		TArray<FBSConfig> DefaultModes;
		DefaultModes.Add(MakePresetConfig(EBaseGameMode::BeatGrid, EGameModeDifficulty::Normal));
		DefaultModes.Add(MakePresetConfig(EBaseGameMode::BeatTrack, EGameModeDifficulty::Normal));
		DefaultModes.Add(MakePresetConfig(EBaseGameMode::SingleBeat, EGameModeDifficulty::Normal));
		DefaultModes.Add(MakePresetConfig(EBaseGameMode::MultiBeat, EGameModeDifficulty::Normal));
		DefaultModes.Add(MakePresetConfig(EBaseGameMode::ChargedBeatTrack, EGameModeDifficulty::Normal));
		return DefaultModes;
	}

	/** Returns the defining config for a preset base game mode and difficulty */
	static FBS_DefiningConfig GetConfigForPreset(const EBaseGameMode& InBaseGameMode, const EGameModeDifficulty& InDifficulty)
	{
		FBS_DefiningConfig Config;
		Config.BaseGameMode = InBaseGameMode;
		Config.Difficulty = InDifficulty;
		Config.GameModeType = EGameModeType::Preset;
		Config.CustomGameModeName = "";
		return Config;
	}
};

/** Used to load and save player scores */
USTRUCT(BlueprintType)
struct FPlayerScore
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Defining Properties")
	FBS_DefiningConfig DefiningConfig;

	/** The song title */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Defining Properties")
	FString SongTitle;

	/** Length of song */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Properties")
	float SongLength;

	/** The current score at any given time during play */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
	float Score;

	/** Only represents highest score based on previous entries, and may become outdated */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
	float HighScore;

	/** Total Targets hit divided by Total shots fired */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
	float Accuracy;

	/** Total Targets hit divided by Total targets spawned */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
	float Completion;

	/** Incremented after receiving calls from FOnShotsFired delegate in DefaultCharacter */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
	int32 ShotsFired;

	/** Total number of targets destroyed by player */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
	int32 TargetsHit;

	/** Total number of targets spawned, incremented after receiving calls from FOnTargetSpawnSignature in TargetManager */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
	int32 TargetsSpawned;

	/** Total possible damage that could have been done to tracking target, also used to determine if the score object is for Tracking game mode */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
	float TotalPossibleDamage;

	/** Total time offset from Spawn Beat Delay for all destroyed targets */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
	float TotalTimeOffset;

	/** Avg Time offset from Spawn Beat Delay for destroyed targets */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
	float AvgTimeOffset;

	/** time that player completed the session, in Iso8601 UTC format */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
	FString Time;

	/** The maximum consecutive targets hit in a row */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
	int32 Streak;

	/** The accuracy at each point in the grid */
	UPROPERTY()
	TArray<FAccuracyRow> LocationAccuracy;

	/** Whether or not this instance has been saved to the database yet */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
	bool bSavedToDatabase;

	FPlayerScore()
	{
		DefiningConfig = FBS_DefiningConfig();
		SongTitle = "";
		SongLength = 0.f;
		Score = 0;
		HighScore = 0;
		Completion = 0;
		Accuracy = 0;
		ShotsFired = 0;
		TargetsHit = 0;
		TotalTimeOffset = 0;
		AvgTimeOffset = 0;
		TargetsSpawned = 0;
		TotalPossibleDamage = 0.f;
		Streak = 0;
		bSavedToDatabase = false;
	}

	void ResetStruct()
	{
		DefiningConfig = FBS_DefiningConfig();
		SongTitle = "";
		SongLength = 0.f;
		Score = 0;
		HighScore = 0;
		Accuracy = 0;
		Completion = 0;
		ShotsFired = 0;
		TargetsHit = 0;
		AvgTimeOffset = 0;
		TargetsSpawned = 0;
		TotalPossibleDamage = 0.f;
		Streak = 0;
		bSavedToDatabase = false;
	}

	FORCEINLINE bool operator==(const FPlayerScore& Other) const
	{
		if (DefiningConfig == Other.DefiningConfig && SongTitle.Equals(Other.SongTitle))
		{
			return true;
		}
		return false;
	}
};

/** Simple login payload */
USTRUCT(BlueprintType)
struct FLoginPayload
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Login")
	FString Username;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Login")
	FString Email;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Login")
	FString Password;

	FLoginPayload()
	{
		Username = "";
		Email = "";
		Password = "";
	}

	FLoginPayload(const FString& InUsername, const FString& InEmail, const FString& InPassword)
	{
		Username = InUsername;
		Email = InEmail;
		Password = InPassword;
	}
};

/** Game settings */
USTRUCT(BlueprintType)
struct FPlayerSettings_Game
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadOnly)
	FLinearColor StartTargetColor;

	UPROPERTY(BlueprintReadOnly)
	FLinearColor PeakTargetColor;

	UPROPERTY(BlueprintReadOnly)
	FLinearColor EndTargetColor;

	UPROPERTY(BlueprintReadOnly)
	bool bUseSeparateOutlineColor;

	UPROPERTY(BlueprintReadOnly)
	FLinearColor TargetOutlineColor;

	UPROPERTY(BlueprintReadOnly)
	FLinearColor InactiveTargetColor;

	UPROPERTY(BlueprintReadOnly)
	bool bShowStreakCombatText;

	UPROPERTY(BlueprintReadOnly)
	int32 CombatTextFrequency;

	/* Wall Menu settings */

	UPROPERTY(BlueprintReadWrite)
	bool bShouldRecoil;

	UPROPERTY(BlueprintReadWrite)
	bool bAutomaticFire;

	UPROPERTY(BlueprintReadWrite)
	bool bShowBulletDecals;

	UPROPERTY(BlueprintReadWrite)
	bool bShowBulletTracers;

	UPROPERTY(BlueprintReadWrite)
	bool bNightModeSelected;

	UPROPERTY(BlueprintReadWrite)
	bool bShowLightVisualizers;

	FPlayerSettings_Game()
	{
		bShowStreakCombatText = true;
		CombatTextFrequency = DefaultCombatTextFrequency;
		StartTargetColor = DefaultStartTargetColor;
		PeakTargetColor = DefaultPeakTargetColor;
		EndTargetColor = DefaultEndTargetColor;
		bUseSeparateOutlineColor = false;
		TargetOutlineColor = DefaultTargetOutlineColor;
		InactiveTargetColor = DefaultBeatGridInactiveTargetColor;
		bShouldRecoil = true;
		bAutomaticFire = true;
		bShowBulletDecals = true;
		bShowBulletTracers = true;
		bNightModeSelected = false;
		bShowLightVisualizers = false;
	}

	/** Resets all game settings not on the wall menu */
	void ResetToDefault()
	{
		bShowStreakCombatText = true;
		CombatTextFrequency = DefaultCombatTextFrequency;
		StartTargetColor = DefaultStartTargetColor;
		PeakTargetColor = DefaultPeakTargetColor;
		EndTargetColor = DefaultEndTargetColor;
		bUseSeparateOutlineColor = false;
		TargetOutlineColor = DefaultTargetOutlineColor;
		InactiveTargetColor = DefaultBeatGridInactiveTargetColor;
	}
};

/** Video and sound settings */
USTRUCT(BlueprintType)
struct FPlayerSettings_VideoAndSound
{
	GENERATED_USTRUCT_BODY()

	// GlobalVolume, which also affects Menu and Music volume
	UPROPERTY(BlueprintReadOnly)
	float GlobalVolume;

	// Volume of the Main Menu Music
	UPROPERTY(BlueprintReadOnly)
	float MenuVolume;

	// Volume of the AudioAnalyzer Tracker
	UPROPERTY(BlueprintReadOnly)
	float MusicVolume;

	UPROPERTY(BlueprintReadOnly)
	int32 FrameRateLimitMenu;

	UPROPERTY(BlueprintReadOnly)
	int32 FrameRateLimitGame;

	UPROPERTY(BlueprintReadOnly)
	bool bShowFPSCounter;

	UPROPERTY(BlueprintReadOnly)
	UDLSSMode DLSSMode;

	UPROPERTY(BlueprintReadOnly)
	UNISMode NISMode;

	UPROPERTY(BlueprintReadWrite)
	EBudgetReflexMode ReflexMode;

	FPlayerSettings_VideoAndSound()
	{
		GlobalVolume = DefaultGlobalVolume;
		MenuVolume = DefaultMenuVolume;
		MusicVolume = DefaultMusicVolume;
		FrameRateLimitMenu = DefaultFrameRateLimitMenu;
		FrameRateLimitGame = DefaultFrameRateLimitGame;
		bShowFPSCounter = false;
		DLSSMode = UDLSSMode::Auto;
		NISMode = UNISMode::Custom;
		ReflexMode = EBudgetReflexMode::Enabled;
	}
};

/** User settings */
USTRUCT(BlueprintType)
struct FPlayerSettings_User
{
	GENERATED_USTRUCT_BODY()

	/** Sensitivity of DefaultCharacter */
	UPROPERTY(BlueprintReadOnly)
	float Sensitivity;

	UPROPERTY(BlueprintReadOnly)
	FString Username;

	UPROPERTY(BlueprintReadOnly)
	bool HasLoggedInHttp;

	UPROPERTY(BlueprintReadOnly)
	FString LoginCookie;

	UPROPERTY(BlueprintReadOnly)
	bool bNightModeUnlocked;

	FPlayerSettings_User()
	{
		Sensitivity = DefaultSensitivity;
		HasLoggedInHttp = false;
		Username = "";
		LoginCookie = "";
		bNightModeUnlocked = false;
	}
};

/** CrossHair settings */
USTRUCT(BlueprintType)
struct FPlayerSettings_CrossHair
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadOnly)
	int32 LineWidth;

	UPROPERTY(BlueprintReadOnly)
	int32 LineLength;

	UPROPERTY(BlueprintReadOnly)
	int32 InnerOffset;

	UPROPERTY(BlueprintReadOnly)
	FLinearColor CrossHairColor;

	UPROPERTY(BlueprintReadOnly)
	float OutlineOpacity;

	UPROPERTY(BlueprintReadOnly)
	int32 OutlineWidth;

	FPlayerSettings_CrossHair()
	{
		LineWidth = DefaultLineWidth;
		LineLength = DefaultLineLength;
		InnerOffset = DefaultInnerOffset;
		CrossHairColor = DefaultCrossHairColor;
		OutlineOpacity = DefaultOutlineOpacity;
		OutlineWidth = DefaultOutlineWidth;
	}
};

/** Audio Analyzer specific settings */
USTRUCT(BlueprintType)
struct FPlayerSettings_AudioAnalyzer
{
	GENERATED_BODY()

	/** Number of channels to break Tracker Sound frequencies into */
	UPROPERTY(BlueprintReadOnly)
	int NumBandChannels;

	/** Array to store Threshold values for each active band channel */
	UPROPERTY(BlueprintReadOnly)
	TArray<float> BandLimitsThreshold;

	/** Array to store band frequency channels */
	UPROPERTY(BlueprintReadOnly)
	TArray<FVector2D> BandLimits;

	/** Time window to take frequency sample */
	UPROPERTY(BlueprintReadOnly)
	float TimeWindow;

	/** History size of frequency sample */
	UPROPERTY(BlueprintReadOnly)
	int HistorySize;

	/** Max number of band channels allowed */
	int32 MaxNumBandChannels;

	UPROPERTY(BlueprintReadOnly)
	FString LastSelectedInputAudioDevice;

	UPROPERTY(BlueprintReadOnly)
	FString LastSelectedOutputAudioDevice;

	FPlayerSettings_AudioAnalyzer()
	{
		BandLimits = DefaultBandLimits;
		BandLimitsThreshold = TArray<float>();
		BandLimitsThreshold.Init(DefaultBandLimitThreshold, DefaultNumBandChannels);
		NumBandChannels = DefaultNumBandChannels;
		TimeWindow = DefaultTimeWindow;
		HistorySize = DefaultHistorySize;
		MaxNumBandChannels = DefaultMaxNumBandChannels;
		LastSelectedInputAudioDevice = "";
		LastSelectedOutputAudioDevice = "";
	}

	/** Resets all settings to default, but keeps audio device information */
	void ResetToDefault()
	{
		BandLimits = DefaultBandLimits;
		BandLimitsThreshold = TArray<float>();
		BandLimitsThreshold.Init(DefaultBandLimitThreshold, DefaultNumBandChannels);
		NumBandChannels = DefaultNumBandChannels;
		TimeWindow = DefaultTimeWindow;
		HistorySize = DefaultHistorySize;
		MaxNumBandChannels = DefaultMaxNumBandChannels;
	}
};

/** Wrapper holding all player settings sub-structs */
USTRUCT(BlueprintType)
struct FPlayerSettings
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadOnly)
	FPlayerSettings_User User;

	UPROPERTY(BlueprintReadWrite)
	FPlayerSettings_Game Game;

	UPROPERTY(BlueprintReadOnly)
	FPlayerSettings_VideoAndSound VideoAndSound;

	UPROPERTY(BlueprintReadOnly)
	FPlayerSettings_CrossHair CrossHair;

	UPROPERTY(BlueprintReadOnly)
	FPlayerSettings_AudioAnalyzer AudioAnalyzer;

	FPlayerSettings()
	{
		User = FPlayerSettings_User();
		Game = FPlayerSettings_Game();
		VideoAndSound = FPlayerSettings_VideoAndSound();
		CrossHair = FPlayerSettings_CrossHair();
		AudioAnalyzer = FPlayerSettings_AudioAnalyzer();
	}

	void ResetGameSettings()
	{
		Game.ResetToDefault();
	}

	void ResetVideoAndSoundSettings()
	{
		VideoAndSound = FPlayerSettings_VideoAndSound();
	}

	void ResetCrossHair()
	{
		CrossHair = FPlayerSettings_CrossHair();
	}

	void ResetAudioAnalyzer()
	{
		AudioAnalyzer.ResetToDefault();
	}
};

/** Information about the transition state of the game */
USTRUCT()
struct FGameModeTransitionState
{
	GENERATED_BODY()

	/** The game mode transition to perform */
	ETransitionState TransitionState;

	/** Whether or not to save current scores if the transition is Restart or Quit */
	bool bSaveCurrentScores;

	/** The game mode properties, only used if Start or Restart */
	FBSConfig BSConfig;
};
