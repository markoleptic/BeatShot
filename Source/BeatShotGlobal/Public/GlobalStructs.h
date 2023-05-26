// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once
#include "GlobalEnums.h"
#include "DLSSLibrary.h"
#include "GlobalConstants.h"
#include "NISLibrary.h"
#include "GlobalStructs.generated.h"

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
struct FBS_BeatGridConfig
{
	GENERATED_BODY()

	/** The number of horizontal BeatGrid targets*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | BeatGrid")
	int32 NumHorizontalBeatGridTargets;

	/** The number of vertical BeatGrid targets*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | BeatGrid")
	int32 NumVerticalBeatGridTargets;

	/** Whether or not to randomize the activation of BeatGrid targets vs only choosing adjacent targets */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | BeatGrid")
	bool RandomizeBeatGrid;

	/** The space between BeatGrid targets */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | BeatGrid")
	FVector2D BeatGridSpacing;

	/** not implemented yet */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | BeatGrid")
	int32 NumTargetsAtOnceBeatGrid;

	FBS_BeatGridConfig()
	{
		NumHorizontalBeatGridTargets = NumHorizontalBeatGridTargets_Normal;
		NumVerticalBeatGridTargets = NumVerticalBeatGridTargets_Normal;
		RandomizeBeatGrid = false;
		NumTargetsAtOnceBeatGrid = NumTargetsAtOnceBeatGrid_Normal;
		BeatGridSpacing = BeatGridSpacing_Normal;
	}

	void SetConfigByDifficulty(const EGameModeDifficulty Difficulty)
	{
		switch (Difficulty)
		{
		case EGameModeDifficulty::None:
			NumHorizontalBeatGridTargets = 0;
			NumVerticalBeatGridTargets = 0;
			RandomizeBeatGrid = false;
			NumTargetsAtOnceBeatGrid = -1;
			BeatGridSpacing = FVector2D::ZeroVector;
			break;
		case EGameModeDifficulty::Normal:
			NumHorizontalBeatGridTargets = NumHorizontalBeatGridTargets_Normal;
			NumVerticalBeatGridTargets = NumVerticalBeatGridTargets_Normal;
			RandomizeBeatGrid = false;
			NumTargetsAtOnceBeatGrid = NumTargetsAtOnceBeatGrid_Normal;
			BeatGridSpacing = BeatGridSpacing_Normal;
			break;
		case EGameModeDifficulty::Hard:
			NumHorizontalBeatGridTargets = NumHorizontalBeatGridTargets_Hard;
			NumVerticalBeatGridTargets = NumVerticalBeatGridTargets_Hard;
			RandomizeBeatGrid = false;
			NumTargetsAtOnceBeatGrid = NumTargetsAtOnceBeatGrid_Hard;
			BeatGridSpacing = BeatGridSpacing_Hard;
			break;
		case EGameModeDifficulty::Death:
			NumHorizontalBeatGridTargets = NumHorizontalBeatGridTargets_Death;
			NumVerticalBeatGridTargets = NumVerticalBeatGridTargets_Death;
			RandomizeBeatGrid = false;
			NumTargetsAtOnceBeatGrid = NumTargetsAtOnceBeatGrid_Death;
			BeatGridSpacing = BeatGridSpacing_Death;
			break;
		}
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

	/** Whether or not to dynamically change the size of targets as consecutive targets are hit */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | Target")
	EConsecutiveTargetScaleMethod ConsecutiveTargetScaleMethod;

	/** The method for handling changing target scale over it's lifetime */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | Target")
	ELifetimeTargetScaleMethod LifetimeTargetScaleMethod;

	/** Min multiplier to target size */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | Target")
	float MinTargetScale;

	/** Max multiplier to target size */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | Target")
	float MaxTargetScale;

	/** Sets the minimum time between target spawns */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | Target")
	float TargetSpawnCD;

	/** Maximum time in which target will stay on screen */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | Target")
	float TargetMaxLifeSpan;

	/** Delay between time between target spawn and peak green target color. Same as PlayerDelay in FBS_AudioConfig */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | Target")
	float SpawnBeatDelay;

	/** How many times the target shrinks before completely dissipating */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | ChargedBeatTrack")
	int32 NumCharges;

	/** How much to shrink the target each time a charge is consumed, if the target is charged. This is multiplied
	 *  against the last charged target scale. A fully charged target does not receive any multiplier */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | ChargedBeatTrack")
	float ConsecutiveChargeScaleMultiplier;

	FBS_TargetConfig()
	{
		ConsecutiveTargetScaleMethod = EConsecutiveTargetScaleMethod::None;
		MinTargetScale = DefaultMinTargetScale;
		MaxTargetScale = DefaultMaxTargetScale;
		LifetimeTargetScaleMethod = ELifetimeTargetScaleMethod::None;
		TargetSpawnCD = DefaultTargetSpawnCD;
		TargetMaxLifeSpan = DefaultTargetMaxLifeSpan;
		SpawnBeatDelay = DefaultSpawnBeatDelay;
		NumCharges = DefaultNumCharges;
		ConsecutiveChargeScaleMultiplier = DefaultChargeScaleMultiplier;
	}
};

USTRUCT(BlueprintType)
struct FBS_SpatialConfig
{
	GENERATED_BODY()

	/** How to scale the bounding box bounds during at runtime */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | Spatial")
	EBoundsScalingMethod BoundsScalingMethod;

	/** How to distribute targets inside the box bounds at runtime */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | Spatial")
	ETargetDistributionMethod TargetDistributionMethod;

	/** Whether or not to move the targets forward towards the player after spawning */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | Spatial")
	bool bMoveTargetsForward;

	/** Sets the minimum distance between recent target spawns */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | Spatial")
	float MinDistanceBetweenTargets;

	/** Distance from bottom of TargetSpawner BoxBounds to the floor */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | Spatial")
	float FloorDistance;

	/** How far to move the target forward over its lifetime */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | Spatial")
	float MoveForwardDistance;

	/** The size of the target spawn BoundingBox. Dimensions are half of the the total length/width */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | Spatial")
	FVector BoxBounds;

	/** Returns the location to spawn the SpawnBox at */
	FVector GenerateSpawnBoxLocation() const
	{
		FVector SpawnBoxCenter = DefaultTargetSpawnerLocation;
		if (TargetDistributionMethod == ETargetDistributionMethod::HeadshotHeightOnly)
		{
			SpawnBoxCenter.Z = HeadshotHeight;
		}
		else
		{
			SpawnBoxCenter.Z = BoxBounds.Z / 2.f + FloorDistance;
		}
		return SpawnBoxCenter;
	}

	/** Returns the actual BoxBounds that the TargetSpawner sets its BoxBounds to */
	FVector GenerateTargetSpawnerBoxBounds() const
	{
		if (TargetDistributionMethod == ETargetDistributionMethod::HeadshotHeightOnly)
		{
			return FVector(0.f, BoxBounds.Y / 2.f, 1.f);
		}
		return FVector(0.f, BoxBounds.Y / 2.f, BoxBounds.Z / 2.f);
	}

	FBS_SpatialConfig()
	{
		BoundsScalingMethod = EBoundsScalingMethod::None;
		TargetDistributionMethod = ETargetDistributionMethod::None;
		bMoveTargetsForward = false;
		MinDistanceBetweenTargets = DefaultMinDistanceBetweenTargets;
		FloorDistance = DistanceFromFloor;
		MoveForwardDistance = 0.f;
		BoxBounds = DefaultSpawnBoxBounds;
	}
};

USTRUCT(BlueprintType)
struct FBS_TrackingConfig
{
	GENERATED_BODY()

	/** The minimum speed multiplier for a moving target */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | Tracking")
	float MinTrackingSpeed;

	/** The maximum speed multiplier for a moving target */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | Tracking")
	float MaxTrackingSpeed;

	FBS_TrackingConfig()
	{
		MinTrackingSpeed = 500.f;
		MaxTrackingSpeed = 500.f;
	}

	void SetConfigByDifficulty(const EGameModeDifficulty Difficulty)
	{
		switch (Difficulty)
		{
		case EGameModeDifficulty::None:
			MinTrackingSpeed = MinTrackingSpeed_BeatTrack_Normal;
			MaxTrackingSpeed = MaxTrackingSpeed_BeatTrack_Normal;
			break;
		case EGameModeDifficulty::Normal:
			MinTrackingSpeed = MinTrackingSpeed_BeatTrack_Normal;
			MaxTrackingSpeed = MaxTrackingSpeed_BeatTrack_Normal;
			break;
		case EGameModeDifficulty::Hard:
			MinTrackingSpeed = MinTrackingSpeed_BeatTrack_Hard;
			MaxTrackingSpeed = MaxTrackingSpeed_BeatTrack_Hard;
			break;
		case EGameModeDifficulty::Death:
			MinTrackingSpeed = MinTrackingSpeed_BeatTrack_Death;
			MaxTrackingSpeed = MaxTrackingSpeed_BeatTrack_Death;
			break;
		}
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
	FBS_BeatGridConfig BeatGridConfig;

	/** Contains info for the target spawner for BeatTrack specific game modes */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | BeatTrack")
	FBS_TrackingConfig TrackingConfig;

	/** Contains info for the target spawner about how to lay out the targets in space */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | Spacing")
	FBS_SpatialConfig SpatialConfig;

	/** Contains info for the target spawner about how to spawn the targets, as well as info to give the targets */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | Target")
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
		BeatGridConfig = FBS_BeatGridConfig();
		TrackingConfig = FBS_TrackingConfig();
		SpatialConfig = FBS_SpatialConfig();
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
				Config.TargetConfig.ConsecutiveTargetScaleMethod = EConsecutiveTargetScaleMethod::SkillBased;
				Config.SpatialConfig.BoundsScalingMethod = EBoundsScalingMethod::Dynamic;
				Config.SpatialConfig.TargetDistributionMethod = ETargetDistributionMethod::EdgeOnly;
				Config.SpatialConfig.BoxBounds = BoxBounds_Dynamic_SingleBeat;
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
				Config.TargetConfig.ConsecutiveTargetScaleMethod = EConsecutiveTargetScaleMethod::SkillBased;
				Config.SpatialConfig.BoundsScalingMethod = EBoundsScalingMethod::Dynamic;
				Config.SpatialConfig.TargetDistributionMethod = ETargetDistributionMethod::FullRange;
				Config.SpatialConfig.BoxBounds = BoxBounds_Dynamic_MultiBeat;
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
						break;
					}
				case EGameModeDifficulty::Hard:
					{
						Config.TargetConfig.SpawnBeatDelay = SpawnBeatDelay_BeatGrid_Hard;
						Config.TargetConfig.TargetSpawnCD = TargetSpawnCD_BeatGrid_Hard;
						Config.TargetConfig.TargetMaxLifeSpan = TargetMaxLifeSpan_BeatGrid_Hard;
						Config.TargetConfig.MinTargetScale = MinTargetScale_BeatGrid_Hard;
						Config.TargetConfig.MaxTargetScale = MaxTargetScale_BeatGrid_Hard;
						break;
					}
				case EGameModeDifficulty::Death:
					{
						Config.TargetConfig.SpawnBeatDelay = SpawnBeatDelay_BeatGrid_Death;
						Config.TargetConfig.TargetSpawnCD = TargetSpawnCD_BeatGrid_Death;
						Config.TargetConfig.TargetMaxLifeSpan = TargetMaxLifeSpan_BeatGrid_Death;
						Config.TargetConfig.MinTargetScale = MinTargetScale_BeatGrid_Death;
						Config.TargetConfig.MaxTargetScale = MaxTargetScale_BeatGrid_Death;
						break;
					}
				case EGameModeDifficulty::None:
					break;
				}
				Config.TargetConfig.ConsecutiveTargetScaleMethod = EConsecutiveTargetScaleMethod::Static;
				Config.BeatGridConfig.SetConfigByDifficulty(Config.DefiningConfig.Difficulty);
				Config.SpatialConfig.BoxBounds = DefaultSpawnBoxBounds;
				Config.SpatialConfig.BoundsScalingMethod = EBoundsScalingMethod::Static;
				Config.SpatialConfig.TargetDistributionMethod = ETargetDistributionMethod::FullRange;
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
						break;
					}
				case EGameModeDifficulty::Hard:
					{
						Config.TargetConfig.TargetSpawnCD = TargetSpawnCD_BeatTrack_Hard;
						Config.TargetConfig.MinTargetScale = MinTargetScale_BeatTrack_Hard;
						Config.TargetConfig.MaxTargetScale = MaxTargetScale_BeatTrack_Hard;
						break;
					}
				case EGameModeDifficulty::Death:
					{
						Config.TargetConfig.TargetSpawnCD = TargetSpawnCD_BeatTrack_Death;
						Config.TargetConfig.MinTargetScale = MinTargetScale_BeatTrack_Death;
						Config.TargetConfig.MaxTargetScale = MaxTargetScale_BeatTrack_Death;
						break;
					}
				case EGameModeDifficulty::None:
					break;
				}
				Config.TargetConfig.ConsecutiveTargetScaleMethod = EConsecutiveTargetScaleMethod::Static;
				Config.TargetConfig.SpawnBeatDelay = SpawnBeatDelay_BeatTrack;
				Config.TrackingConfig.SetConfigByDifficulty(Config.DefiningConfig.Difficulty);
				Config.SpatialConfig.BoundsScalingMethod = EBoundsScalingMethod::Static;
				Config.SpatialConfig.TargetDistributionMethod = ETargetDistributionMethod::FullRange;
				Config.TargetConfig.TargetMaxLifeSpan = TargetMaxLifeSpan_BeatTrack;
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
						Config.TargetConfig.NumCharges = NumCharges_ChargedBeatTrack_Normal;
						Config.TargetConfig.ConsecutiveChargeScaleMultiplier = ChargeScaleMultiplier_ChargedBeatTrack_Normal;
						Config.TargetConfig.TargetMaxLifeSpan = TargetMaxLifeSpan_ChargedBeatTrack_Normal;
						break;
					}
				case EGameModeDifficulty::Hard:
					{
						Config.TargetConfig.TargetSpawnCD = TargetSpawnCD_ChargedBeatTrack_Hard;
						Config.TargetConfig.MinTargetScale = MinTargetScale_ChargedBeatTrack_Hard;
						Config.TargetConfig.MaxTargetScale = MaxTargetScale_ChargedBeatTrack_Hard;
						Config.TargetConfig.NumCharges = NumCharges_ChargedBeatTrack_Hard;
						Config.TargetConfig.ConsecutiveChargeScaleMultiplier = TargetMaxLifeSpan_ChargedBeatTrack_Hard;
						Config.TargetConfig.TargetMaxLifeSpan = TargetMaxLifeSpan_ChargedBeatTrack_Hard;
						break;
					}
				case EGameModeDifficulty::Death:
					{
						Config.TargetConfig.TargetSpawnCD = TargetSpawnCD_ChargedBeatTrack_Death;
						Config.TargetConfig.MinTargetScale = MinTargetScale_ChargedBeatTrack_Death;
						Config.TargetConfig.MaxTargetScale = MaxTargetScale_ChargedBeatTrack_Death;
						Config.TargetConfig.NumCharges = NumCharges_ChargedBeatTrack_Death;
						Config.TargetConfig.ConsecutiveChargeScaleMultiplier = ChargeScaleMultiplier_ChargedBeatTrack_Death;
						Config.TargetConfig.TargetMaxLifeSpan = TargetMaxLifeSpan_ChargedBeatTrack_Death;
						break;
					}
				case EGameModeDifficulty::None:
					break;
				}
				Config.TargetConfig.ConsecutiveTargetScaleMethod = EConsecutiveTargetScaleMethod::Static;
				Config.TargetConfig.SpawnBeatDelay = 0.25;
				Config.TrackingConfig.SetConfigByDifficulty(Config.DefiningConfig.Difficulty);
				Config.SpatialConfig.BoundsScalingMethod = EBoundsScalingMethod::Static;
				Config.SpatialConfig.TargetDistributionMethod = ETargetDistributionMethod::FullRange;
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

	/** Total number of targets spawned, incremented after receiving calls from FOnTargetSpawnSignature in TargetSpawner */
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
	FLinearColor BeatGridInactiveTargetColor;

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
		BeatGridInactiveTargetColor = DefaultBeatGridInactiveTargetColor;
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
		BeatGridInactiveTargetColor = DefaultBeatGridInactiveTargetColor;
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
