// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/UserDefinedStruct.h"
#include "GameModeActorStruct.generated.h"

UENUM(BlueprintType)
enum class EGameModeActorName : uint8 {
	Custom						UMETA(DisplayName, "Custom"),
	SingleBeat					UMETA(DisplayName, "SingleBeat"),
	MultiBeat					UMETA(DisplayName, "MultiBeat"),
	BeatGrid					UMETA(DisplayName, "BeatGrid"),
	// REMEMBER TO UPDATE ENUM_RANGE_BY_FIRST_AND_LAST AS GAME MODES ARE ADDED!!!
	BeatTrack					UMETA(DisplayName, "BeatTrack")
};

// REMEMBER TO UPDATE THIS AS GAME MODES ARE ADDED!!!
// Allows iterating through EGameModeActorName
ENUM_RANGE_BY_FIRST_AND_LAST(EGameModeActorName, EGameModeActorName::Custom, EGameModeActorName::BeatTrack);

UENUM(BlueprintType)
enum class ESpreadType : uint8 {
	None						UMETA(DisplayName, "None"),
	DynamicEdgeOnly				UMETA(DisplayName, "DynamicEdgeOnly"),
	DynamicRandom				UMETA(DisplayName, "DynamicRandom"),
	StaticNarrow				UMETA(DisplayName, "StaticNarrow"),
	StaticWide					UMETA(DisplayName, "StaticWide")
};
ENUM_RANGE_BY_FIRST_AND_LAST(ESpreadType, ESpreadType::None, ESpreadType::StaticWide);

UENUM(BlueprintType)
enum class EGameModeDifficulty : uint8 {
	Normal						UMETA(DisplayName, "Normal"),
	Hard						UMETA(DisplayName, "Hard"),
	Death						UMETA(DisplayName, "Death")
};
ENUM_RANGE_BY_FIRST_AND_LAST(EGameModeDifficulty, EGameModeDifficulty::Normal, EGameModeDifficulty::Death);

// Used to store game properties
USTRUCT(BlueprintType)
struct FGameModeActorStruct
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Properties")
		FString SongTitle;

	// Used to Spawn GameModes deriving from GameModeActorBase
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Properties")
		EGameModeActorName GameModeActorName;

	// Used to dynamically adjust the spawn area and sphere size
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Properties")
		ESpreadType SpreadType;

	// Used to dynamically adjust the sphere size
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Properties")
		bool UseDynamicSizing;

	// Changes default difficulty values
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Properties")
		EGameModeDifficulty GameModeDifficulty;

	// TimerHandle for Song Length
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Properties")
		FTimerHandle GameModeLengthTimer;

	// Length of song
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Properties")
		float GameModeLength;

	// Countdown TimerHandle
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Properties")
		FTimerHandle CountDownTimer;

	// Countdown Time Length
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Properties")
		float CountdownTimerLength;

	// Sets the minimum time between target spawns
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Properties")
		float TargetSpawnCD;

	// Sets the minimum distance between recent target spawns
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Properties")
		float MinDistanceBetweenTargets;

	// Used by TargetSpawner to set the center of target spawn area
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Properties")
		FVector CenterOfSpawnBox;

	// Min multiplier to target size
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Properties")
		float MinTargetScale;

	// Max multiplier to target size
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Properties")
		float MaxTargetScale;

	// Targets only spawn at headshot height
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Properties")
		bool HeadshotHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Properties")
		bool WallCentered;

	// Maximum time in which target will stay on screen
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Properties")
		float TargetMaxLifeSpan;

	// The size of the target spawn BoundingBox. Dimensions are half of the the total length/width
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Properties")
		FVector BoxBounds;

	// If user creates custom mode, save it with this name
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Properties")
		FString CustomGameModeName;

	// Delay between AudioAnalyzer Tracker and Player. Also the same value as time between target spawn and peak green target color
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Properties")
		float PlayerDelay;

	// the minimum speed multiplier for Tracking Game Mode
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BeatTrack Properties")
		float MinTrackingSpeed;

	// the maximum speed multiplier for Tracking Game Mode
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BeatTrack Properties")
		float MaxTrackingSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Properties")
		bool IsBeatTrackMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Properties")
		bool IsSingleBeatMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Properties")
		bool IsBeatGridMode;

	// not implemented yet
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BeatGrid Properties")
		FVector2D BeatGridSpacing;

	// min 4, only squarable numbers
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BeatGrid Properties")
		int32 BeatGridSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BeatGrid Properties")
		bool RandomizeBeatGrid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BeatGrid Properties")
		int32 NumTargetsAtOnceBeatGrid;

	FORCEINLINE bool operator== (const FGameModeActorStruct& Other) const
	{
		if (GameModeActorName == Other.GameModeActorName &&
			SongTitle.Equals(Other.SongTitle) &&
			(CustomGameModeName.IsEmpty() && Other.CustomGameModeName.IsEmpty() ||
				CustomGameModeName.Equals(Other.CustomGameModeName)))
		{
			return true;
		}
		return false;
	}

	// Generic initialization
	FGameModeActorStruct()
	{
		GameModeActorName = EGameModeActorName::Custom;
		SpreadType = ESpreadType::None;
		GameModeDifficulty = EGameModeDifficulty::Normal;
		UseDynamicSizing = false;
		MinDistanceBetweenTargets = 100.f;
		CenterOfSpawnBox = { 3590.f,0.f,160.f };
		CountdownTimerLength = 3.f;
		GameModeLength = 0.f;
		TargetSpawnCD = 0.35f;
		TargetMaxLifeSpan = 1.5f;
		MinTargetScale = 0.8f;
		MaxTargetScale = 2.f;
		HeadshotHeight = false;
		WallCentered = false;
		PlayerDelay = 0.3f;
		SongTitle = "";
		CustomGameModeName = "";
		MinTrackingSpeed = 500.f;
		MaxTrackingSpeed = 500.f;
		IsBeatTrackMode = false;
		IsSingleBeatMode = false;
		IsBeatGridMode = false;
		RandomizeBeatGrid = false;
		NumTargetsAtOnceBeatGrid = -1;
		BeatGridSpacing = FVector2D::ZeroVector;
		BeatGridSize = 0.f;
		BoxBounds.X = 0.f;
		// horizontal
		BoxBounds.Y = 3200.f;
		// vertical
		BoxBounds.Z = 1000.f;
	}

	FGameModeActorStruct(EGameModeActorName GameModeActor,
		EGameModeDifficulty NewGameModeDifficulty = EGameModeDifficulty::Normal,
		ESpreadType NewSpreadType = ESpreadType::None)
	{
		// Parameters
		GameModeActorName = GameModeActor;
		GameModeDifficulty = NewGameModeDifficulty;
		SpreadType = NewSpreadType;

		// Constant for all Game Modes and Difficulties
		CountdownTimerLength = 3.f;
		GameModeLength = 0.f;
		HeadshotHeight = false;
		RandomizeBeatGrid = false;
		SongTitle = "";
		NumTargetsAtOnceBeatGrid = -1;
		BeatGridSpacing = FVector2D::ZeroVector;
		CustomGameModeName = "";

		WallCentered = false;
		IsBeatTrackMode = false;
		IsSingleBeatMode = false;
		IsBeatGridMode = false;
		UseDynamicSizing = false;
		MinDistanceBetweenTargets = 100.f;
		PlayerDelay = 0.3f;
		TargetSpawnCD = 0.35f;
		TargetMaxLifeSpan = 1.5f;
		MinTargetScale = 0.8f;
		MaxTargetScale = 2.f;
		MinTrackingSpeed = 500.f;
		MaxTrackingSpeed = 500.f;
		BeatGridSize = 0.f;
		BoxBounds.X = 0.f;
		BoxBounds.Y = 3200.f;
		BoxBounds.Z = 1000.f;
		CenterOfSpawnBox = { 3590.f,0.f,160.f };

		// BeatGrid
		if (GameModeActor == EGameModeActorName::BeatGrid)
		{
			SpreadType = ESpreadType::None;
			IsBeatGridMode = true;
			WallCentered = true;
			BoxBounds.Y = 1000.f;
			BoxBounds.Z = 1000.f;
			CenterOfSpawnBox = { 3590.f,0.f,750.f };

			// BeatGrid Difficulties
			if (GameModeDifficulty == EGameModeDifficulty::Normal)
			{
				PlayerDelay = 0.35f;
				TargetSpawnCD = 0.35f;
				TargetMaxLifeSpan = 1.2f;
				MinTargetScale = 1.2f;
				MaxTargetScale = 1.2f;
				BeatGridSize = 16;
			}
			else if (GameModeDifficulty == EGameModeDifficulty::Hard)
			{
				PlayerDelay = 0.3f;
				TargetSpawnCD = 0.30f;
				TargetMaxLifeSpan = 1.f;
				MinTargetScale = 1.f;
				MaxTargetScale = 1.f;
				BeatGridSize = 25;
			}
			else if (GameModeDifficulty == EGameModeDifficulty::Death)
			{
				PlayerDelay = 0.25f;
				TargetSpawnCD = 0.25f;
				TargetMaxLifeSpan = 0.75f;
				MinTargetScale = 0.75f;
				MaxTargetScale = 0.75f;
				BeatGridSize = 36;
			}
		}
		// BeatTrack
		else if (GameModeActor == EGameModeActorName::BeatTrack)
		{
			SpreadType = ESpreadType::None;
			WallCentered = true;
			IsBeatTrackMode = true;
			PlayerDelay = 0.f;
			TargetMaxLifeSpan = 0.f;
			MinTrackingSpeed = 500.f;
			MaxTrackingSpeed = 500.f;
			CenterOfSpawnBox = { 3590.f,0.f,750.f };

			// BeatTrack Difficulties
			if (GameModeDifficulty == EGameModeDifficulty::Normal)
			{
				MinTrackingSpeed = 500.f;
				MaxTrackingSpeed = 500.f;
				TargetSpawnCD = 0.5f;
				MinTargetScale = 1.f;
				MaxTargetScale = 1.f;
			}
			else if (GameModeDifficulty == EGameModeDifficulty::Hard)
			{
				MinTrackingSpeed = 600.f;
				MaxTrackingSpeed = 600.f;
				TargetSpawnCD = 0.4f;
				MinTargetScale = 0.75f;
				MaxTargetScale = 0.75f;
			}
			else if (GameModeDifficulty == EGameModeDifficulty::Death)
			{
				MinTrackingSpeed = 700.f;
				MaxTrackingSpeed = 700.f;
				TargetSpawnCD = 0.3f;
				MinTargetScale = 0.5f;
				MaxTargetScale = 0.5f;
			}
		}
		// MultiBeat
		else if (GameModeActor == EGameModeActorName::MultiBeat)
		{
			// MultiBeat Difficulties
			if (GameModeDifficulty == EGameModeDifficulty::Normal)
			{
				PlayerDelay = 0.35f;
				TargetSpawnCD = 0.35f;
				TargetMaxLifeSpan = 1.f;
				MinTargetScale = 0.75f;
				MaxTargetScale = 2.f;
			}
			else if (GameModeDifficulty == EGameModeDifficulty::Hard)
			{
				PlayerDelay = 0.3f;
				TargetSpawnCD = 0.3f;
				TargetMaxLifeSpan = 0.75f;
				MinTargetScale = 0.6f;
				MaxTargetScale = 1.5f;
			}
			else if (GameModeDifficulty == EGameModeDifficulty::Death)
			{
				PlayerDelay = 0.25f;
				TargetSpawnCD = 0.20f;
				TargetMaxLifeSpan = 0.5f;
				MinTargetScale = 0.4f;
				MaxTargetScale = 1.25f;
			}

			// MultiBeat Spread Types
			if (SpreadType == ESpreadType::StaticNarrow)
			{
				UseDynamicSizing = false;
				BoxBounds.Y = 1600.f;
				BoxBounds.Z = 500.f;
			}
			else if (SpreadType == ESpreadType::StaticWide)
			{
				UseDynamicSizing = false;
				BoxBounds.Y = 3200.f;
				BoxBounds.Z = 1000.f;
			}
			else if (SpreadType == ESpreadType::DynamicEdgeOnly ||
				SpreadType == ESpreadType::DynamicRandom)
			{
				UseDynamicSizing = true;
				BoxBounds.Y = 2000.f;
				BoxBounds.Z = 800.f;
			}
		}
		// SingleBeat
		else if (GameModeActor == EGameModeActorName::SingleBeat)
		{
			IsSingleBeatMode = true;

			// SingleBeat Difficulties
			if (GameModeDifficulty == EGameModeDifficulty::Normal)
			{
				PlayerDelay = 0.3f;
				TargetSpawnCD = 0.3f;
				TargetMaxLifeSpan = 1.f;
				MinTargetScale = 0.75f;
				MaxTargetScale = 2.f;
			}
			else if (GameModeDifficulty == EGameModeDifficulty::Hard)
			{
				PlayerDelay = 0.25f;
				TargetSpawnCD = 0.25f;
				TargetMaxLifeSpan = 0.75f;
				MinTargetScale = 0.6f;
				MaxTargetScale = 1.5f;
			}
			else if (GameModeDifficulty == EGameModeDifficulty::Death)
			{
				PlayerDelay = 0.2f;
				TargetSpawnCD = 0.2f;
				TargetMaxLifeSpan = 0.5f;
				MinTargetScale = 0.4f;
				MaxTargetScale = 1.5f;
			}

			// SingleBeat Spread Types
			if (SpreadType == ESpreadType::StaticNarrow)
			{
				UseDynamicSizing = false;
				BoxBounds.Y = 1600.f;
				BoxBounds.Z = 500.f;
			}
			else if (SpreadType == ESpreadType::StaticWide)
			{
				UseDynamicSizing = false;
				BoxBounds.Y = 3200.f;
				BoxBounds.Z = 1000.f;
			}
			else if (SpreadType == ESpreadType::DynamicEdgeOnly ||
				SpreadType == ESpreadType::DynamicRandom)
			{
				UseDynamicSizing = true;
				BoxBounds.Y = 2000.f;
				BoxBounds.Z = 800.f;
			}
		}
	}

	void ResetStruct()
	{
		GameModeActorName = EGameModeActorName::Custom;
		SpreadType = ESpreadType::None;
		MinDistanceBetweenTargets = 100.f;
		CenterOfSpawnBox = { 3590.f,0.f,160.f };
		CountdownTimerLength = 3.f;
		GameModeLength = 0.f;
		TargetSpawnCD = 0.35f;
		TargetMaxLifeSpan = 1.5f;
		MinTargetScale = 0.8f;
		MaxTargetScale = 1.5f;
		HeadshotHeight = false;
		WallCentered = false;
		IsBeatTrackMode = false;
		IsSingleBeatMode = false;
		IsBeatGridMode = false;
		RandomizeBeatGrid = false;
		UseDynamicSizing = false;
		PlayerDelay = 0.3f;
		SongTitle = "";
		CustomGameModeName = "";
		MinTrackingSpeed = 500.f;
		MaxTrackingSpeed = 500.f;
		NumTargetsAtOnceBeatGrid = -1;
		BeatGridSpacing = FVector2D::ZeroVector;
		BoxBounds.X = 0.f;
		BoxBounds.Y = 1600.f;
		BoxBounds.Z = 500.f;
	}
};

FORCEINLINE uint32 GetTypeHash(const FGameModeActorStruct& Other)
{
	uint32 Hash = FCrc::MemCrc32(&Other, sizeof(FGameModeActorStruct));
	return Hash;
}

// Used to load and save player scores
USTRUCT(BlueprintType)
struct FPlayerScore
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
		EGameModeActorName GameModeActorName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Properties")
		FString CustomGameModeName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Properties")
		FString SongTitle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Properties")
		float SongLength;

	// Used by PlayerHUD to display current score
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
		float Score;

	// Displayed with PlayerHUD
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
		float HighScore;

	// Total Targets hit / Total shots fired
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
		float Accuracy;

	// Total Targets hit / Total targets spawned
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
		float Completion;

	// Incremented after receiving calls from FOnShotsFired delegate in DefaultCharacter
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
		int32 ShotsFired;

	// Total number of targets destroyed by player, called by Projectile
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
		int32 TargetsHit;

	// Total number of targets spawned, incremented after receiving calls from FOnTargetSpawnSignature in TargetSpawner
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
		int32 TargetsSpawned;

	// Total possible damage that could have been done to tracking target,
	// also used to determine if the score object is for Tracking game mode
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
		float TotalPossibleDamage;

	// Total time offset from Spawn Beat Delay for all destroyed targets
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
		float TotalTimeOffset;

	// Avg Time offset from Spawn Beat Delay for destroyed targets
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
		float AvgTimeOffset;

	// time that player completed the session
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
		FString Time;

	// The maximum consecutive targets hit in a row
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
		int32 Streak;

	FPlayerScore()
	{
		GameModeActorName = EGameModeActorName::Custom;
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
	}

	void ResetStruct()
	{
		GameModeActorName = EGameModeActorName::Custom;
		CustomGameModeName = "";
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
	}
};

// Used by DefaultGameInstance to load and save player settings
USTRUCT(BlueprintType)
struct FPlayerSettings
{
	GENERATED_USTRUCT_BODY()

		// Sensitivity of DefaultCharacter
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
		float Sensitivity;

	// GlobalVolume, which also affects Menu and Music volume
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
		float GlobalVolume;

	// Volume of the Main Menu Music
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
		float MenuVolume;

	// Volume of the AudioAnalyzer Tracker
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
		float MusicVolume;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Login")
		FString Username;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Login")
		bool HasLoggedInHttp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Login")
		bool HasLoggedInBrowser;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Login")
		FString LoginCookie;

	FPlayerSettings()
	{
		Sensitivity = 0.3f;
		GlobalVolume = 50.f;
		MenuVolume = 50.f;
		MusicVolume = 10.f;
		HasLoggedInHttp = false;
		HasLoggedInBrowser = false;
		Username = "";
		LoginCookie = "";
	}

	void ResetStruct()
	{
		Sensitivity = 0.3f;
		GlobalVolume = 50.f;
		MenuVolume = 50.f;
		MusicVolume = 10.f;
		HasLoggedInHttp = false;
		HasLoggedInBrowser = false;
		Username = "";
		LoginCookie = "";
	}
};

// Used by PlayerScoreMap to abstract FPlayerScores
USTRUCT(BlueprintType)
struct FPlayerScoreArrayWrapper
{
	GENERATED_USTRUCT_BODY()

		// originally wanted this to hold all FPlayerScores for a given EGameModeActorName and Song title,
		// but doesn't work sometimes. Saving and loading works even if equality doesn't work.
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FPlayerScore> PlayerScoreArray;
};

// Used by AASettings widget to relay Audio Analyzer settings to DefaultGameInstance
USTRUCT(BlueprintType)
struct FAASettingsStruct
{
	GENERATED_USTRUCT_BODY()

		// Number of channels to break Tracker Sound frequencies into
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AA Settings")
		int NumBandChannels;

	// Array to store Threshold values for each active band channel
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AA Settings")
		TArray<float> BandLimitsThreshold;

	// Array to store band frequency channels
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AA Settings")
		TArray<FVector2D> BandLimits;

	// Time window to take frequency sample
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AA Settings")
		float TimeWindow;

	// History size of frequency sample
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AA Settings")
		int HistorySize;

	FAASettingsStruct()
	{
		NumBandChannels = 3;
		BandLimitsThreshold = { 2.1f, 2.1f, 2.1f };
		BandLimits = { FVector2D(0.f, 87.f),
			FVector2D(500.f, 700.f),
			FVector2D(5000.f, 12000.f) };
		TimeWindow = 0.02f;
		HistorySize = 30.f;
	}

	void ResetStruct()
	{
		NumBandChannels = 3;
		BandLimitsThreshold = { 2.1f, 2.1f, 2.1f };
		BandLimits = { FVector2D(0.f, 87.f),
			FVector2D(500.f, 700.f),
			FVector2D(5000.f, 12000.f) };
		TimeWindow = 0.02f;
		HistorySize = 30.f;
	}
};

// Used to convert PlayerScoreMap to database scores
USTRUCT(BlueprintType)
struct FJsonScore
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY()
		TArray<FPlayerScore> Scores;
};

// Simple login payload
USTRUCT(BlueprintType)
struct FLoginPayload
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Login")
		FString Username;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Login")
		FString Email;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Login")
		FString Password;

};