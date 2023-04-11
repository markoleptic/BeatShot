// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once
#include "GlobalEnums.h"
#include "DLSSLibrary.h"
#include "GlobalConstants.h"
#include "NISLibrary.h"
#include "GlobalStructs.generated.h"


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

	FCommonScoreInfo()
	{
		Accuracy = TArray<float>();
		TotalSpawns = TArray<int32>();
		TotalHits = TArray<int32>();
	}

	FCommonScoreInfo(const int32 Size)
	{
		Accuracy = TArray<float>();
		Accuracy.Init(-1.f, Size);
		TotalSpawns = TArray<int32>();
		TotalSpawns.Init(-1, Size);
		TotalHits = TArray<int32>();
		TotalHits.Init(0, Size);
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

USTRUCT(BlueprintType)
struct FBS_DefiningConfig
{
	GENERATED_BODY()
	
	/* The default game mode name, or custom if custom */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties")
	EDefaultMode DefaultMode;

	/* The base game mode this game mode is based off of */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties")
	EDefaultMode BaseGameMode;

	/* Custom game mode name if custom, otherwise empty string */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties")
	FString CustomGameModeName;

	/* Default game mode difficulties, or none if custom */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties")
	EGameModeDifficulty Difficulty;

	FBS_DefiningConfig()
	{
		DefaultMode = EDefaultMode::Custom;
		BaseGameMode = EDefaultMode::MultiBeat;
		CustomGameModeName = "";
		Difficulty = EGameModeDifficulty::None;
	}

	FORCEINLINE bool operator==(const FBS_DefiningConfig& Other) const
	{
		if (DefaultMode == Other.DefaultMode && CustomGameModeName.Equals(Other.CustomGameModeName))
		{
			if (!CustomGameModeName.IsEmpty())
			{
				return true;
			}
			if (CustomGameModeName.IsEmpty() && Difficulty == Other.Difficulty)
			{
				return true;
			}
			return false;
		}
		return false;
	}
};

FORCEINLINE uint32 GetTypeHash(const FBS_DefiningConfig& MidiTime)
{
	return FCrc::MemCrc32(&MidiTime, sizeof(FBS_DefiningConfig));
}

USTRUCT(BlueprintType)
struct FBS_AIConfig
{
	GENERATED_BODY()

	/* Whether or not to enable the reinforcement learning agent to handle target spawning */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | AI")
	bool bEnableRLAgent;

	/* The stored QTable for this game mode */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | AI")
	TArray<float> QTable;

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
		bEnableRLAgent = false;
		QTable = TArray<float>();
		Alpha = 0.9f;
		Epsilon = 0.9f;
		Gamma = 0.9f;
	}
};

USTRUCT(BlueprintType)
struct FBS_BeatGridConfig
{
	GENERATED_BODY()

	/* The number of horizontal BeatGrid targets*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | BeatGrid")
	int32 NumHorizontalBeatGridTargets;

	/* The number of vertical BeatGrid targets*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | BeatGrid")
	int32 NumVerticalBeatGridTargets;

	/* Whether or not to randomize the activation of BeatGrid targets vs only choosing adjacent targets */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | BeatGrid")
	bool RandomizeBeatGrid;

	/* The space between BeatGrid targets */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | BeatGrid")
	FVector2D BeatGridSpacing;

	/* not implemented yet */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | BeatGrid")
	int32 NumTargetsAtOnceBeatGrid;

	FBS_BeatGridConfig()
	{
		NumHorizontalBeatGridTargets = 0;
		NumVerticalBeatGridTargets = 0;
		RandomizeBeatGrid = false;
		NumTargetsAtOnceBeatGrid = -1;
		BeatGridSpacing = FVector2D::ZeroVector;
	}

	void SetConfigByDifficulty(const EGameModeDifficulty Difficulty)
	{
		switch (Difficulty)
		{
		case EGameModeDifficulty::None: NumHorizontalBeatGridTargets = 0;
			NumVerticalBeatGridTargets = 0;
			RandomizeBeatGrid = false;
			NumTargetsAtOnceBeatGrid = -1;
			BeatGridSpacing = FVector2D::ZeroVector;
			break;
		case EGameModeDifficulty::Normal: NumHorizontalBeatGridTargets = 5;
			NumVerticalBeatGridTargets = 5;
			RandomizeBeatGrid = false;
			NumTargetsAtOnceBeatGrid = -1;
			BeatGridSpacing = FVector2D(75, 50);
			break;
		case EGameModeDifficulty::Hard: NumHorizontalBeatGridTargets = 10;
			NumVerticalBeatGridTargets = 5;
			RandomizeBeatGrid = false;
			NumTargetsAtOnceBeatGrid = -1;
			BeatGridSpacing = FVector2D(75, 50);
			break;
		case EGameModeDifficulty::Death: NumHorizontalBeatGridTargets = 15;
			NumVerticalBeatGridTargets = 10;
			RandomizeBeatGrid = false;
			NumTargetsAtOnceBeatGrid = -1;
			BeatGridSpacing = FVector2D(75, 50);
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

	/* Whether or not to playback streamed audio */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
	bool bPlaybackAudio;

	/* The audio format type used for the AudioAnalyzer */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
	EAudioFormat AudioFormat;

	/* The input audio device */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
	FString InAudioDevice;

	/* The output audio device */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
	FString OutAudioDevice;

	/* The path to the song file */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
	FString SongPath;

	/* Delay between AudioAnalyzer Tracker and Player. Also the same value as time between target spawn and peak green target color */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
	float PlayerDelay;

	/* Length of song */
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
		PlayerDelay = 0.3f;
	}
};

USTRUCT(BlueprintType)
struct FBS_TargetConfig
{
	GENERATED_BODY()

	/* Whether or not to dynamically change the size of targets as consecutive targets are hit */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | General")
	bool UseDynamicSizing;
	
	/* Min multiplier to target size */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | General")
	float MinTargetScale;

	/* Max multiplier to target size */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | General")
	float MaxTargetScale;
	
	/* Sets the minimum time between target spawns */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | General")
	float TargetSpawnCD;
	
	/* Maximum time in which target will stay on screen */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | General")
	float TargetMaxLifeSpan;

	FBS_TargetConfig()
	{
		UseDynamicSizing = false;
		MinTargetScale = 0.8f;
		MaxTargetScale = 2.f;
		TargetSpawnCD = 0.35f;
		TargetMaxLifeSpan = 1.5f;
	}
};

USTRUCT(BlueprintType)
struct FBS_SpatialConfig
{
	GENERATED_BODY()

	/** Changes how targets are spawned relative to the spawn area. If static, it simply sets the spawn area size.
     *  If dynamic, the spawn area will gradually shrink as consecutive targets are hit */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | Spatial")
	ESpreadType SpreadType;

	/* Whether or not to spawn targets only at headshot height */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | Spatial")
	bool bUseHeadshotHeight;

	/* Whether or not to move the targets forward towards the player after spawning */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | Spatial")
	bool bMoveTargetsForward;

	/* Sets the minimum distance between recent target spawns */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | Spatial")
	float MinDistanceBetweenTargets;

	/* Distance from bottom of TargetSpawner BoxBounds to the floor */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | Spatial")
	float FloorDistance;

	/* How far to move the target forward over its lifetime */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | Spatial")
	float MoveForwardDistance;

	/* The size of the target spawn BoundingBox. Dimensions are half of the the total length/width */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | Spatial")
	FVector BoxBounds;

	/** Returns the location to spawn the SpawnBox at */
	FVector GenerateSpawnBoxLocation() const
	{
		FVector SpawnBoxCenter = Constants::DefaultTargetSpawnerLocation;
		if (bUseHeadshotHeight)
		{
			SpawnBoxCenter.Z = Constants::HeadshotHeight;
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
		if (bUseHeadshotHeight)
		{
			return FVector(0.f, BoxBounds.Y / 2.f, 1.f);
		}
		return FVector(0.f, BoxBounds.Y / 2.f, BoxBounds.Z / 2.f);
	}

	FBS_SpatialConfig()
	{
		SpreadType = ESpreadType::None;
		bUseHeadshotHeight = false;
		bMoveTargetsForward = false;
		MinDistanceBetweenTargets = 10.f;
		FloorDistance = Constants::DistanceFromFloor;
		MoveForwardDistance = 0.f;
		BoxBounds = FVector(0.f, 3200.f, 1000.f);
	}
};

USTRUCT(BlueprintType)
struct FBS_BeatTrackConfig
{
	GENERATED_BODY()
	
	/* The minimum speed multiplier for Tracking Game Mode */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | BeatTrack")
	float MinTrackingSpeed;

	/* The maximum speed multiplier for Tracking Game Mode */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | BeatTrack")
	float MaxTrackingSpeed;

	FBS_BeatTrackConfig()
	{
		MinTrackingSpeed = 500.f;
		MaxTrackingSpeed = 500.f;
	}

	void SetConfigByDifficulty(const EGameModeDifficulty Difficulty)
	{
		switch (Difficulty)
		{
		case EGameModeDifficulty::None:
			MinTrackingSpeed = 500.f;
			MaxTrackingSpeed = 500.f;
			break;
		case EGameModeDifficulty::Normal:
			MinTrackingSpeed = 400.f;
			MaxTrackingSpeed = 500.f;
			break;
		case EGameModeDifficulty::Hard:
			MinTrackingSpeed = 500.f;
			MaxTrackingSpeed = 600.f;
			break;
		case EGameModeDifficulty::Death:
			MinTrackingSpeed = 500.f;
			MaxTrackingSpeed = 700.f;
			break;
		}
	}
};

/* Struct representing a game mode */
USTRUCT(BlueprintType)
struct FBSConfig
{
	GENERATED_BODY()
	
	/* The default game mode name, or custom if custom */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties")
	EDefaultMode DefaultMode;

	/* The base game mode this game mode is based off of */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties")
	EDefaultMode BaseGameMode;

	/* Custom game mode name if custom, otherwise empty string */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties")
	FString CustomGameModeName;

	/* Default game mode difficulties, or none if custom */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties")
	EGameModeDifficulty GameModeDifficulty;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | Audio")
	FBS_AudioConfig AudioConfig;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | Target")
	FBS_TargetConfig TargetConfig;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | Spacing")
	FBS_SpatialConfig SpatialConfig;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | AI")
	FBS_AIConfig AIConfig;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | BeatGrid")
	FBS_BeatGridConfig BeatGridConfig;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | BeatTrack")
	FBS_BeatTrackConfig BeatTrackConfig;

	FORCEINLINE bool operator==(const FBSConfig& Other) const
	{
		if (DefaultMode == Other.DefaultMode && CustomGameModeName.Equals(Other.CustomGameModeName))
		{
			return true;
		}
		return false;
	}

	/* Generic initialization */
	FBSConfig()
	{
		DefaultMode = EDefaultMode::Custom;
		BaseGameMode = EDefaultMode::MultiBeat;
		CustomGameModeName = "";
		GameModeDifficulty = EGameModeDifficulty::None;
		SpatialConfig.SpreadType = ESpreadType::None;
		
		AudioConfig = FBS_AudioConfig();
		TargetConfig = FBS_TargetConfig();
		SpatialConfig = FBS_SpatialConfig();
		AIConfig = FBS_AIConfig();
		BeatGridConfig = FBS_BeatGridConfig();
		BeatTrackConfig = FBS_BeatTrackConfig();
	}

	FBSConfig(const EDefaultMode InDefaultMode, const EGameModeDifficulty NewGameModeDifficulty = EGameModeDifficulty::Normal, const ESpreadType NewSpreadType = ESpreadType::None)
	{
		DefaultMode = InDefaultMode;
		BaseGameMode = EDefaultMode::MultiBeat;
		CustomGameModeName = "";
		GameModeDifficulty = NewGameModeDifficulty;
		SpatialConfig.SpreadType = NewSpreadType;
		
		AudioConfig = FBS_AudioConfig();
		TargetConfig = FBS_TargetConfig();
		SpatialConfig = FBS_SpatialConfig();
		AIConfig = FBS_AIConfig();
		BeatGridConfig = FBS_BeatGridConfig();
		BeatTrackConfig = FBS_BeatTrackConfig();

		switch (DefaultMode)
		{
		case EDefaultMode::SingleBeat:
			BaseGameMode = EDefaultMode::SingleBeat;
			TargetConfig.UseDynamicSizing = true;
			switch (GameModeDifficulty)
			{
			case EGameModeDifficulty::Normal:
				AudioConfig.PlayerDelay = 0.3f;
				TargetConfig.TargetSpawnCD = 0.3f;
				TargetConfig.TargetMaxLifeSpan = 0.8f;
				TargetConfig.MinTargetScale = 0.75f;
				TargetConfig.MaxTargetScale = 2.f;
				break;
			case EGameModeDifficulty::Hard:
				AudioConfig.PlayerDelay = 0.25f;
				TargetConfig.TargetSpawnCD = 0.25f;
				TargetConfig.TargetMaxLifeSpan = 0.65f;
				TargetConfig.MinTargetScale = 0.6f;
				TargetConfig.MaxTargetScale = 1.5f;
				break;
			case EGameModeDifficulty::Death:
				AudioConfig.PlayerDelay = 0.2f;
				TargetConfig.TargetSpawnCD = 0.2f;
				TargetConfig.TargetMaxLifeSpan = 0.45f;
				TargetConfig.MinTargetScale = 0.4f;
				TargetConfig.MaxTargetScale = 1.5f;
				break;
			case EGameModeDifficulty::None:
				break;
			}
			switch (SpatialConfig.SpreadType)
			{
			case ESpreadType::StaticNarrow:
				TargetConfig.UseDynamicSizing = false;
				SpatialConfig.BoxBounds = FVector(0.f, 1600.f, 500.f);
				break;
			case ESpreadType::StaticWide:
				TargetConfig.UseDynamicSizing = false;
				SpatialConfig.BoxBounds = FVector(0.f, 3200.f, 1000.f);
				break;
			default:
				SpatialConfig.SpreadType = ESpreadType::DynamicEdgeOnly;
				TargetConfig.UseDynamicSizing = true;
				SpatialConfig.BoxBounds = FVector(0.f, 2000.f, 800.f);
				break;
			}
			break;
		case EDefaultMode::MultiBeat:
			TargetConfig.UseDynamicSizing = true;
			BaseGameMode = EDefaultMode::MultiBeat;
			switch (GameModeDifficulty)
			{
			case EGameModeDifficulty::Normal:
				AudioConfig.PlayerDelay = 0.35f;
				TargetConfig.TargetSpawnCD = 0.35f;
				TargetConfig.TargetMaxLifeSpan = 1.f;
				TargetConfig.MinTargetScale = 0.75f;
				TargetConfig.MaxTargetScale = 2.f;
				break;
			case EGameModeDifficulty::Hard:
				AudioConfig.PlayerDelay = 0.3f;
				TargetConfig.TargetSpawnCD = 0.3f;
				TargetConfig.TargetMaxLifeSpan = 0.75f;
				TargetConfig.MinTargetScale = 0.6f;
				TargetConfig.MaxTargetScale = 1.5f;
				break;
			case EGameModeDifficulty::Death:
				AudioConfig.PlayerDelay = 0.25f;
				TargetConfig.TargetSpawnCD = 0.20f;
				TargetConfig.TargetMaxLifeSpan = 0.5f;
				TargetConfig.MinTargetScale = 0.4f;
				TargetConfig.MaxTargetScale = 1.25f;
				break;
			case EGameModeDifficulty::None:
				break;
			}
			switch (SpatialConfig.SpreadType)
			{
			case ESpreadType::StaticNarrow:
				TargetConfig.UseDynamicSizing = false;
				SpatialConfig.BoxBounds = FVector(0.f, 1600.f, 500.f);
				break;
			case ESpreadType::StaticWide:
				TargetConfig.UseDynamicSizing = false;
				SpatialConfig.BoxBounds = FVector(0.f, 3200.f, 1000.f);
				break;
			default:
				SpatialConfig.SpreadType = ESpreadType::DynamicRandom;
				TargetConfig.UseDynamicSizing = true;
				SpatialConfig.BoxBounds = FVector(0.f, 2000.f, 800.f);
				break;
			}
			break;
		case EDefaultMode::BeatGrid:
			SpatialConfig.SpreadType = ESpreadType::None;
			BaseGameMode = EDefaultMode::BeatGrid;
			SpatialConfig.BoxBounds = FVector(0.f, 3200.f, 1000.f);
			BeatGridConfig.SetConfigByDifficulty(GameModeDifficulty);
			switch (GameModeDifficulty)
			{
			case EGameModeDifficulty::Normal:
				AudioConfig.PlayerDelay = 0.35f;
				TargetConfig.TargetSpawnCD = 0.35f;
				TargetConfig.TargetMaxLifeSpan = 1.2f;
				TargetConfig.MinTargetScale = 0.85f;
				TargetConfig.MaxTargetScale = 0.85f;
				break;
			case EGameModeDifficulty::Hard:
				AudioConfig.PlayerDelay = 0.3f;
				TargetConfig.TargetSpawnCD = 0.30f;
				TargetConfig.TargetMaxLifeSpan = 1.f;
				TargetConfig.MinTargetScale = 0.7f;
				TargetConfig.MaxTargetScale = 0.7f;
				break;
			case EGameModeDifficulty::Death:
				AudioConfig.PlayerDelay = 0.25f;
				TargetConfig.TargetSpawnCD = 0.25f;
				TargetConfig.TargetMaxLifeSpan = 0.75f;
				TargetConfig.MinTargetScale = 0.5f;
				TargetConfig.MaxTargetScale = 0.5f;
				break;
			case EGameModeDifficulty::None:
				break;
			}
			break;
		case EDefaultMode::BeatTrack:
			SpatialConfig.SpreadType = ESpreadType::None;
			BaseGameMode = EDefaultMode::BeatTrack;
			AudioConfig.PlayerDelay = 0.f;
			TargetConfig.TargetMaxLifeSpan = 0.f;
			BeatTrackConfig.SetConfigByDifficulty(GameModeDifficulty);
			switch (GameModeDifficulty)
			{
			case EGameModeDifficulty::Normal:
				TargetConfig.TargetSpawnCD = 0.75f;
				TargetConfig.MinTargetScale = 1.3f;
				TargetConfig.MaxTargetScale = 1.3f;
				break;
			case EGameModeDifficulty::Hard:
				TargetConfig.TargetSpawnCD = 0.6f;
				TargetConfig.MinTargetScale = 1.f;
				TargetConfig.MaxTargetScale = 1.f;
				break;
			case EGameModeDifficulty::Death:
				TargetConfig.TargetSpawnCD = 0.45f;
				TargetConfig.MinTargetScale = 0.75f;
				TargetConfig.MaxTargetScale = 0.75;
				break;
			case EGameModeDifficulty::None:
				break;
			}
			break;
		case EDefaultMode::Custom:
			break;
		}
	}

	/** Returns an array of all default game modes */
	static TArray<FBSConfig> GetDefaultGameModes()
	{
		TArray<FBSConfig> DefaultModes;
		DefaultModes.Add(FBSConfig(EDefaultMode::BeatGrid, EGameModeDifficulty::Normal));
		DefaultModes.Add(FBSConfig(EDefaultMode::BeatTrack, EGameModeDifficulty::Normal));
		DefaultModes.Add(FBSConfig(EDefaultMode::SingleBeat, EGameModeDifficulty::Normal, ESpreadType::DynamicEdgeOnly));
		DefaultModes.Add(FBSConfig(EDefaultMode::MultiBeat, EGameModeDifficulty::Normal, ESpreadType::DynamicRandom));
		return DefaultModes;
	}
};

/* Used to load and save player scores */
USTRUCT(BlueprintType)
struct FPlayerScore
{
	GENERATED_BODY()

	/* The default game mode name, or custom if custom */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Defining Properties")
	EDefaultMode DefaultMode;

	/* Custom game mode name if custom, otherwise empty string */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Defining Properties")
	FString CustomGameModeName;

	/* Default game mode difficulties, or none if custom */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Defining Properties")
	EGameModeDifficulty Difficulty;

	/* The song title */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Defining Properties")
	FString SongTitle;

	/* Length of song */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Properties")
	float SongLength;

	/* The current score at any given time during play */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
	float Score;

	/* Only represents highest score based on previous entries, and may become outdated */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
	float HighScore;

	/* Total Targets hit divided by Total shots fired */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
	float Accuracy;

	/* Total Targets hit divided by Total targets spawned */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
	float Completion;

	/* Incremented after receiving calls from FOnShotsFired delegate in DefaultCharacter */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
	int32 ShotsFired;

	/* Total number of targets destroyed by player */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
	int32 TargetsHit;

	/* Total number of targets spawned, incremented after receiving calls from FOnTargetSpawnSignature in TargetSpawner */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
	int32 TargetsSpawned;

	/* Total possible damage that could have been done to tracking target, also used to determine if the score object is for Tracking game mode */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
	float TotalPossibleDamage;

	/* Total time offset from Spawn Beat Delay for all destroyed targets */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
	float TotalTimeOffset;

	/* Avg Time offset from Spawn Beat Delay for destroyed targets */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
	float AvgTimeOffset;

	/* time that player completed the session, in Iso8601 UTC format */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
	FString Time;

	/* The maximum consecutive targets hit in a row */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
	int32 Streak;

	/* The accuracy at each point in the grid */
	UPROPERTY()
	TArray<FAccuracyRow> LocationAccuracy;

	/* Whether or not this instance has been saved to the database yet */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
	bool bSavedToDatabase;

	FPlayerScore()
	{
		DefaultMode = EDefaultMode::Custom;
		Difficulty = EGameModeDifficulty::None;
		CustomGameModeName = "";
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
		DefaultMode = EDefaultMode::Custom;
		CustomGameModeName = "";
		Difficulty = EGameModeDifficulty::None;
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
		if (DefaultMode == Other.DefaultMode && CustomGameModeName.Equals(Other.CustomGameModeName) && SongTitle.Equals(Other.SongTitle))
		{
			if (!CustomGameModeName.IsEmpty())
			{
				return true;
			}
			if (CustomGameModeName.IsEmpty() && Difficulty == Other.Difficulty)
			{
				return true;
			}
			return false;
		}
		return false;
	}
};

/* Used to convert PlayerScoreArray to database scores */
USTRUCT(BlueprintType)
struct FJsonScore
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FPlayerScore> Scores;
};

/* Simple login payload */
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

/* Game settings */
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

	/* Range settings */

	UPROPERTY(BlueprintReadWrite)
	bool bShouldRecoil;

	UPROPERTY(BlueprintReadWrite)
	bool bAutomaticFire;

	UPROPERTY(BlueprintReadWrite)
	bool bShowBulletDecals;

	UPROPERTY(BlueprintReadWrite)
	bool bNightModeSelected;

	UPROPERTY(BlueprintReadWrite)
	bool bShowLightVisualizers;

	FPlayerSettings_Game()
	{
		bShowStreakCombatText = true;
		CombatTextFrequency = 5;
		StartTargetColor = FLinearColor::White;
		PeakTargetColor = FLinearColor::Green;
		EndTargetColor = FLinearColor::Red;
		bUseSeparateOutlineColor = false;
		TargetOutlineColor = FLinearColor::White;
		BeatGridInactiveTargetColor = {83.f / 255.f, 0.f, 245.f / 255.f, 1.f};
		bShouldRecoil = true;
		bAutomaticFire = true;
		bShowBulletDecals = true;
		bNightModeSelected = false;
		bShowLightVisualizers = false;
	}

	void ResetGameSettings()
	{
		bShowStreakCombatText = true;
		CombatTextFrequency = 5;
		StartTargetColor = FLinearColor::White;
		PeakTargetColor = FLinearColor::Green;
		EndTargetColor = FLinearColor::Red;
		bUseSeparateOutlineColor = false;
		TargetOutlineColor = FLinearColor::White;
		BeatGridInactiveTargetColor = {83.f / 255.f, 0.f, 245.f / 255.f, 1.f};
	}
};

/* Video and sound settings */
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
		GlobalVolume = 50.f;
		MenuVolume = 50.f;
		MusicVolume = 10.f;
		FrameRateLimitMenu = 144;
		FrameRateLimitGame = 0;
		bShowFPSCounter = false;
		DLSSMode = UDLSSMode::Auto;
		NISMode = UNISMode::Custom;
		ReflexMode = EBudgetReflexMode::Enabled;
	}
};

/* User settings */
USTRUCT(BlueprintType)
struct FPlayerSettings_User
{
	GENERATED_USTRUCT_BODY()

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
		HasLoggedInHttp = false;
		Username = "";
		LoginCookie = "";
		bNightModeUnlocked = false;
	}
};

/* CrossHair settings */
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
		LineWidth = 4;
		LineLength = 10;
		InnerOffset = 6;
		CrossHairColor = FLinearColor(63.f / 255.f, 199.f / 255.f, 235.f / 255.f, 1.f);
		OutlineOpacity = 1.f;
		OutlineWidth = 20;
	}
};

/* Used to load and save player settings */
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

	// Sensitivity of DefaultCharacter
	UPROPERTY(BlueprintReadOnly)
	float Sensitivity;

	FPlayerSettings()
	{
		User = FPlayerSettings_User();
		Game = FPlayerSettings_Game();
		VideoAndSound = FPlayerSettings_VideoAndSound();
		CrossHair = FPlayerSettings_CrossHair();
		Sensitivity = 0.3f;
	}

	void ResetGameSettings()
	{
		Game.ResetGameSettings();
	}

	void ResetVideoAndSoundSettings()
	{
		VideoAndSound = FPlayerSettings_VideoAndSound();
	}

	void ResetCrossHair()
	{
		CrossHair = FPlayerSettings_CrossHair();
	}
};

/* AudioAnalyzer Specific Settings */
USTRUCT(BlueprintType)
struct FAASettingsStruct
{
	GENERATED_BODY()

	// Number of channels to break Tracker Sound frequencies into
	UPROPERTY(BlueprintReadOnly)
	int NumBandChannels;

	// Array to store Threshold values for each active band channel
	UPROPERTY(BlueprintReadOnly)
	TArray<float> BandLimitsThreshold;

	// Array to store band frequency channels
	UPROPERTY(BlueprintReadOnly)
	TArray<FVector2D> BandLimits;

	// Time window to take frequency sample
	UPROPERTY(BlueprintReadOnly)
	float TimeWindow;

	// History size of frequency sample
	UPROPERTY(BlueprintReadOnly)
	int HistorySize;

	// Max number of band channels allowed
	int32 MaxNumBandChannels = 32;

	UPROPERTY(BlueprintReadOnly)
	FString LastSelectedInputAudioDevice;

	UPROPERTY(BlueprintReadOnly)
	FString LastSelectedOutputAudioDevice;

	FAASettingsStruct()
	{
		BandLimits = {
			FVector2d(0, 44), FVector2d(45, 88), FVector2d(89, 177), FVector2d(178, 355), FVector2d(356, 710), FVector2d(711, 1420), FVector2d(1421, 2840), FVector2d(2841, 5680),
			FVector2d(5681, 11360), FVector2d(11361, 22720),
		};
		BandLimitsThreshold = {2.1, 2.1, 2.1, 2.1, 2.1, 2.1, 2.1, 2.1, 2.1, 2.1};
		NumBandChannels = 10;
		TimeWindow = 0.02f;
		HistorySize = 30.f;
		MaxNumBandChannels = 32;
		LastSelectedInputAudioDevice = "";
		LastSelectedOutputAudioDevice = "";
	}

	void ResetStruct()
	{
		BandLimits = {
			FVector2d(0, 44), FVector2d(45, 88), FVector2d(89, 177), FVector2d(178, 355), FVector2d(356, 710), FVector2d(711, 1420), FVector2d(1421, 2840), FVector2d(2841, 5680),
			FVector2d(5681, 11360), FVector2d(11361, 22720),
		};
		BandLimitsThreshold = {2.1, 2.1, 2.1, 2.1, 2.1, 2.1, 2.1, 2.1, 2.1, 2.1};
		NumBandChannels = 10;
		TimeWindow = 0.02f;
		HistorySize = 30.f;
		MaxNumBandChannels = 32;
	}
};

/* Information about the transition state of the game */
USTRUCT()
struct FGameModeTransitionState
{
	GENERATED_BODY()

	/* The game mode transition to perform */
	ETransitionState TransitionState;

	/* Whether or not to save current scores if the transition is Restart or Quit */
	bool bSaveCurrentScores;

	/* The game mode properties, only used if Start or Restart */
	FBSConfig BSConfig;
};
