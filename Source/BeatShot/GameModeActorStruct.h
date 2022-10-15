// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/UserDefinedStruct.h"
#include "GameModeActorStruct.generated.h"

UENUM(BlueprintType)
enum class EGameModeActorName : uint8 {
	Custom						UMETA(DisplayName, "Custom"),
	SingleBeat					UMETA(DisplayName, "SingleBeat"),
	MultiBeat					UMETA(DisplayName, "MultiBeat"),
	NarrowSpreadSingleBeat		UMETA(DisplayName, "NarrowSpreadSingleBeat"),
	WideSpreadSingleBeat		UMETA(DisplayName, "WideSpreadSingleBeat"),
	NarrowSpreadMultiBeat		UMETA(DisplayName, "NarrowSpreadMultiBeat"),
	WideSpreadMultiBeat			UMETA(DisplayName, "WideSpreadMultiBeat"),
	BeatGrid					UMETA(DisplayName, "BeatGrid"),
	// REMEMBER TO UPDATE ENUM_RANGE_BY_FIRST_AND_LAST AS GAME MODES ARE ADDED!!!
	BeatTrack					UMETA(DisplayName, "BeatTrack")
};

// REMEMBER TO UPDATE THIS AS GAME MODES ARE ADDED!!!
ENUM_RANGE_BY_FIRST_AND_LAST(EGameModeActorName, EGameModeActorName::Custom, EGameModeActorName::BeatTrack);

// Used to store game properties, etc.
USTRUCT(BlueprintType)
struct FGameModeActorStruct
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Properties")
		FString SongTitle;

	// Used to Spawn GameModes deriving from GameModeActorBase
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Properties")
		EGameModeActorName GameModeActorName;

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
			CustomGameModeName.Equals(Other.CustomGameModeName))
		{
			return true;
		}
		return false;
	}

	// Generic initialization
	FGameModeActorStruct()
	{
		GameModeActorName = EGameModeActorName::Custom;
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
		BoxBounds.Y = 1600.f;
		// vertical
		BoxBounds.Z = 500.f;
	}

	void ResetStruct()
	{
		GameModeActorName = EGameModeActorName::Custom;
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
		BoxBounds.X = 0.f;
		// horizontal
		BoxBounds.Y = 1600.f;
		// vertical
		BoxBounds.Z = 500.f;
	}
};

FORCEINLINE uint32 GetTypeHash(const FGameModeActorStruct& Other)
{
	uint32 Hash = FCrc::MemCrc32(&Other, sizeof(FGameModeActorStruct));
	return Hash;
}

// Used by GameModeActorBase to load and save player scores
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

	// Incremented after receiving calls from FOnShotsFired delegate in DefaultCharacter
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
		float ShotsFired;

	// Total number of targets destroyed by player, called by Projectile
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
		float TargetsHit;

	// Total number of targets spawned, incremented after receiving calls from FOnTargetSpawnSignature in TargetSpawner
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
		float TargetsSpawned;

	// Tracking changes the way score is calculated
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
		bool IsBeatTrackMode;

	// Total possible damage that could have been done to tracking target
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
		float TotalPossibleDamage;

	// Array that contains the time offset from Spawn Beat Delay for destroyed targets
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
		TArray<float> ReactionTime;

	// time that player completed the session
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
		FDateTime Time;

	FPlayerScore()
	{
		GameModeActorName = EGameModeActorName::Custom;
		CustomGameModeName = "";
		SongTitle = "";
		SongLength = 0.f;
		Score = 0;
		HighScore = 0;
		Accuracy = 0;
		ShotsFired = 0;
		TargetsHit = 0;
		TargetsSpawned = 0;
		IsBeatTrackMode = false;
		TotalPossibleDamage = 0.f;
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
		ShotsFired = 0;
		TargetsHit = 0;
		TargetsSpawned = 0;
		IsBeatTrackMode = false;
		TotalPossibleDamage = 0.f;
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

	// MasterVolume, which also affects Menu and Music volume
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
		float MasterVolume;

	// Volume of the Main Menu Music
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
		float MenuVolume;

	// Volume of the AudioAnalyzer Tracker
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
		float MusicVolume;

	FPlayerSettings()
	{
		Sensitivity = 0.3f;
		MasterVolume = 50.f;
		MenuVolume = 50.f;
		MusicVolume = 10.f;
	}
};

// Used by PlayerScoreMap
USTRUCT(BlueprintType)
struct FPlayerScoreArrayWrapper
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
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

USTRUCT(BlueprintType)
struct FJsonGameModeScore
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	FString GameMode_SongTitle;

	UPROPERTY()
	TArray<FPlayerScore> PlayerScoreArray;
};

USTRUCT(BlueprintType)
struct FJsonScore
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	TArray<FJsonGameModeScore> GameModeScorePairs;
};