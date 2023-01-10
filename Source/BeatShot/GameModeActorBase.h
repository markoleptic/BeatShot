// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SaveGamePlayerSettings.h"
#include "GameFramework/Actor.h"
#include "GameModeActorBase.generated.h"

class ASphereTarget;
class AFloatingTextActor;
class ADefaultGameMode;

#pragma region Enums

/** Enum representing the default game mode names */
UENUM(BlueprintType)
enum class EGameModeActorName : uint8
{
	Custom UMETA(DisplayName, "Custom"),
	SingleBeat UMETA(DisplayName, "SingleBeat"),
	MultiBeat UMETA(DisplayName, "MultiBeat"),
	BeatGrid UMETA(DisplayName, "BeatGrid"),
	BeatTrack UMETA(DisplayName, "BeatTrack")
};

ENUM_RANGE_BY_FIRST_AND_LAST(EGameModeActorName, EGameModeActorName::Custom, EGameModeActorName::BeatTrack);

/** Enum representing the spread type of the targets */
UENUM(BlueprintType)
enum class ESpreadType : uint8
{
	None UMETA(DisplayName, "None"),
	DynamicEdgeOnly UMETA(DisplayName, "DynamicEdgeOnly"),
	DynamicRandom UMETA(DisplayName, "DynamicRandom"),
	StaticNarrow UMETA(DisplayName, "StaticNarrow"),
	StaticWide UMETA(DisplayName, "StaticWide")
};

ENUM_RANGE_BY_FIRST_AND_LAST(ESpreadType, ESpreadType::None, ESpreadType::StaticWide);

/** Enum representing the default game mode difficulties */
UENUM(BlueprintType)
enum class EGameModeDifficulty : uint8
{
	None UMETA(DisplayName, "None"),
	Normal UMETA(DisplayName, "Normal"),
	Hard UMETA(DisplayName, "Hard"),
	Death UMETA(DisplayName, "Death")
};

ENUM_RANGE_BY_FIRST_AND_LAST(EGameModeDifficulty, EGameModeDifficulty::None, EGameModeDifficulty::Death);

#pragma endregion

#pragma region Structs

/* Struct representing a game mode */
USTRUCT(BlueprintType)
struct FGameModeActorStruct
{
	GENERATED_BODY()

	/* The song title */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Defining Properties")
	FString SongTitle;

	/* The default game mode name, or custom if custom */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Defining Properties")
	EGameModeActorName GameModeActorName;

	/* Custom game mode name if custom, otherwise empty string */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Defining Properties")
	FString CustomGameModeName;

	/* Default game mode difficulties, or none if custom */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Defining Properties")
	EGameModeDifficulty GameModeDifficulty;

	/* Whether or not the game mode derives from BeatTrack */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Properties | BaseGameMode")
	bool IsBeatTrackMode;

	/* Whether or not the game mode derives from SingleBeat */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Properties | BaseGameMode")
	bool IsSingleBeatMode;

	/* Whether or not the game mode derives from BeatGrid */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Properties | BaseGameMode")
	bool IsBeatGridMode;

	/** Changes how targets are spawned relative to the spawn area. If static, it simply sets the spawn area size.
	 * If dynamic, the spawn area will gradually shrink as consecutive targets are hit */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Properties | General")
	ESpreadType SpreadType;

	/* Whether or not to dynamically change the size of targets as consecutive targets are hit */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Properties | General")
	bool UseDynamicSizing;

	/* Length of song */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Properties | General")
	float GameModeLength;

	/* Sets the minimum time between target spawns */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Properties | General")
	float TargetSpawnCD;

	/* Sets the minimum distance between recent target spawns */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Properties | General")
	float MinDistanceBetweenTargets;

	/* Min multiplier to target size */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Properties | General")
	float MinTargetScale;

	/* Max multiplier to target size */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Properties | General")
	float MaxTargetScale;

	/* Whether or not to spawn targets only at headshot height */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Properties | General")
	bool HeadshotHeight;

	/* Whether or not to center spawn area in the center of wall, vs as close to the ground as possible */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Properties | General")
	bool WallCentered;

	/* Maximum time in which target will stay on screen */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Properties | General")
	float TargetMaxLifeSpan;

	/* The size of the target spawn BoundingBox. Dimensions are half of the the total length/width */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Properties | General")
	FVector BoxBounds;

	/* Delay between AudioAnalyzer Tracker and Player. Also the same value as time between target spawn and peak green target color */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Properties | General")
	float PlayerDelay;

	/* Whether or not to move the targets forward towards the player after spawning */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Properties | General")
	bool bMoveTargetsForward;

	/* Whether or not to move the targets forward towards the player after spawning */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Properties | General")
	float MoveForwardDistance;

	/* The minimum speed multiplier for Tracking Game Mode */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Properties | BeatTrack")
	float MinTrackingSpeed;

	/* The maximum speed multiplier for Tracking Game Mode */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Properties | BeatTrack")
	float MaxTrackingSpeed;

	/* The number of BeatGrid targets, only square-able numbers */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Properties | BeatGrid")
	int32 BeatGridSize;

	/* Whether or not to randomize the activation of BeatGrid targets vs only choosing adjacent targets */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Properties | BeatGrid")
	bool RandomizeBeatGrid;

	/* The space between BeatGrid targets */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Properties | BeatGrid")
	FVector2D BeatGridSpacing;

	/* not implemented yet */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Properties | BeatGrid")
	int32 NumTargetsAtOnceBeatGrid;

	FORCEINLINE bool operator==(const FGameModeActorStruct& Other) const
	{
		if (GameModeActorName == Other.GameModeActorName &&
			CustomGameModeName.Equals(Other.CustomGameModeName))
		{
			return true;
		}
		return false;
	}

	/*FORCEINLINE uint32 GetTypeHash(const FGameModeActorStruct& Other)
	{
		const uint32 Hash = FCrc::MemCrc32(&Other, sizeof(FGameModeActorStruct));
		return Hash;
	}*/

	/* Generic initialization */
	FGameModeActorStruct()
	{
		GameModeActorName = EGameModeActorName::Custom;
		SpreadType = ESpreadType::None;
		GameModeDifficulty = EGameModeDifficulty::Normal;
		IsBeatTrackMode = false;
		IsSingleBeatMode = false;
		IsBeatGridMode = false;
		UseDynamicSizing = false;
		MinDistanceBetweenTargets = 10.f;
		GameModeLength = 0.f;
		TargetSpawnCD = 0.35f;
		TargetMaxLifeSpan = 1.5f;
		MinTargetScale = 0.8f;
		MaxTargetScale = 2.f;
		HeadshotHeight = false;
		WallCentered = false;
		PlayerDelay = 0.3f;
		bMoveTargetsForward = false;
		MoveForwardDistance = 0.f;
		SongTitle = "";
		CustomGameModeName = "";
		MinTrackingSpeed = 500.f;
		MaxTrackingSpeed = 500.f;
		RandomizeBeatGrid = false;
		NumTargetsAtOnceBeatGrid = -1;
		BeatGridSpacing = FVector2D::ZeroVector;
		BeatGridSize = 0.f;
		BoxBounds = FVector(0.f, 3200.f, 1000.f);
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
		MinDistanceBetweenTargets = 10.f;
		PlayerDelay = 0.3f;
		bMoveTargetsForward = false;
		MoveForwardDistance = 0.f;
		TargetSpawnCD = 0.35f;
		TargetMaxLifeSpan = 1.5f;
		MinTargetScale = 0.8f;
		MaxTargetScale = 2.f;
		MinTrackingSpeed = 500.f;
		MaxTrackingSpeed = 500.f;
		BeatGridSize = 0.f;
		BoxBounds = FVector(0.f, 3200.f, 1000.f);

		// BeatGrid
		if (GameModeActor == EGameModeActorName::BeatGrid)
		{
			SpreadType = ESpreadType::None;
			IsBeatGridMode = true;
			BoxBounds = FVector(0.f, 1000.f, 1000.f);

			// BeatGrid Difficulties
			if (GameModeDifficulty == EGameModeDifficulty::Normal)
			{
				PlayerDelay = 0.35f;
				TargetSpawnCD = 0.35f;
				TargetMaxLifeSpan = 1.2f;
				MinTargetScale = 0.85f;
				MaxTargetScale = 0.85f;
				BeatGridSize = 25;
			}
			else if (GameModeDifficulty == EGameModeDifficulty::Hard)
			{
				PlayerDelay = 0.3f;
				TargetSpawnCD = 0.30f;
				TargetMaxLifeSpan = 1.f;
				MinTargetScale = 0.7f;
				MaxTargetScale = 0.7f;
				BeatGridSize = 25;
			}
			else if (GameModeDifficulty == EGameModeDifficulty::Death)
			{
				PlayerDelay = 0.25f;
				TargetSpawnCD = 0.25f;
				TargetMaxLifeSpan = 0.75f;
				MinTargetScale = 0.5f;
				MaxTargetScale = 0.5f;
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

			// BeatTrack Difficulties
			if (GameModeDifficulty == EGameModeDifficulty::Normal)
			{
				MinTrackingSpeed = 400.f;
				MaxTrackingSpeed = 500.f;
				TargetSpawnCD = 0.75f;
				MinTargetScale = 1.3f;
				MaxTargetScale = 1.3f;
			}
			else if (GameModeDifficulty == EGameModeDifficulty::Hard)
			{
				MinTrackingSpeed = 500.f;
				MaxTrackingSpeed = 600.f;
				TargetSpawnCD = 0.6f;
				MinTargetScale = 1.f;
				MaxTargetScale = 1.f;
			}
			else if (GameModeDifficulty == EGameModeDifficulty::Death)
			{
				MinTrackingSpeed = 500.f;
				MaxTrackingSpeed = 700.f;
				TargetSpawnCD = 0.45f;
				MinTargetScale = 0.75f;
				MaxTargetScale = 0.75;
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

			// MultiBeat Spread Types, defaults to DynamicRandom
			if (SpreadType == ESpreadType::StaticNarrow)
			{
				UseDynamicSizing = false;
				BoxBounds = FVector(0.f, 1600.f, 500.f);
			}
			else if (SpreadType == ESpreadType::StaticWide)
			{
				UseDynamicSizing = false;
				BoxBounds = FVector(0.f, 3200.f, 1000.f);
			}
			else
			{
				SpreadType = ESpreadType::DynamicRandom;
				UseDynamicSizing = true;
				BoxBounds = FVector(0.f, 2000.f, 800.f);
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
				TargetMaxLifeSpan = 0.8f;
				MinTargetScale = 0.75f;
				MaxTargetScale = 2.f;
			}
			else if (GameModeDifficulty == EGameModeDifficulty::Hard)
			{
				PlayerDelay = 0.25f;
				TargetSpawnCD = 0.25f;
				TargetMaxLifeSpan = 0.65f;
				MinTargetScale = 0.6f;
				MaxTargetScale = 1.5f;
			}
			else if (GameModeDifficulty == EGameModeDifficulty::Death)
			{
				PlayerDelay = 0.2f;
				TargetSpawnCD = 0.2f;
				TargetMaxLifeSpan = 0.45f;
				MinTargetScale = 0.4f;
				MaxTargetScale = 1.5f;
			}

			// SingleBeat Spread Types, defaults to DynamicEdgeOnly
			if (SpreadType == ESpreadType::StaticNarrow)
			{
				UseDynamicSizing = false;
				BoxBounds = FVector(0.f, 1600.f, 500.f);
			}
			else if (SpreadType == ESpreadType::StaticWide)
			{
				UseDynamicSizing = false;
				BoxBounds = FVector(0.f, 3200.f, 1000.f);
			}
			else
			{
				SpreadType = ESpreadType::DynamicEdgeOnly;
				UseDynamicSizing = true;
				BoxBounds = FVector(0.f, 2000.f, 800.f);
			}
		}
	}

	void ResetStruct()
	{
		GameModeActorName = EGameModeActorName::Custom;
		SpreadType = ESpreadType::None;
		MinDistanceBetweenTargets = 10.f;
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

/* Used to load and save player scores */
USTRUCT(BlueprintType)
struct FPlayerScore
{
	GENERATED_BODY()

	/* The default game mode name, or custom if custom */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Defining Properties")
	EGameModeActorName GameModeActorName;

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

	/* Whether or not this instance has been saved to the database yet */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
	bool bSavedToDatabase;

	FPlayerScore()
	{
		GameModeActorName = EGameModeActorName::Custom;
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
		GameModeActorName = EGameModeActorName::Custom;
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
};

#pragma endregion

UCLASS()
class BEATSHOT_API AGameModeActorBase : public AActor
{
	GENERATED_BODY()

public:
	AGameModeActorBase();

protected:
	virtual void BeginPlay() override;

	virtual void Destroyed() override;

	virtual void Tick(float DeltaTime) override;

public:
	/** Starts the game mode timer */
	UFUNCTION()
	void StartGameMode();

	/** Timer that spans the length of the song */
	UPROPERTY(VisibleAnywhere)
	FTimerHandle GameModeLengthTimer;

	/** The game mode defining properties */
	UPROPERTY(VisibleAnywhere)
	FGameModeActorStruct GameModeActorStruct;

private:
	/** called during StartGameMode. Loads player scores, sets the score amount for each target, and binds to TargetSpawner's
	 *  UpdateTargetSpawned and OnStreakUpdate */
	UFUNCTION()
	void InitializeGameModeActor();

	/** Reports to DefaultGameMode when the song has finished */
	UFUNCTION()
	void OnGameModeLengthTimerComplete();

	/** Function bound to DefaultGameMode's OnBeatTrackTargetSpawned delegate.
	 *  Binds the tracking target's health component's OnBeatTrackTick delegate to UpdateTrackingScore */
	UFUNCTION()
	void OnBeatTrackTargetSpawned(ASphereTarget* TrackingTarget);

	/** The floating combat text class */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AFloatingTextActor> FloatingTextActorToSpawn;

#pragma region HUDUpdate


	/** Function bound to DefaultGameMode's OnTargetDestroyed delegate, which is executed by TargetSpawner.
	 *  Passes the time that the target was alive for */
	UFUNCTION()
	void UpdatePlayerScores(const float TimeElapsed);

	/** Function bound to the tracking target's health component's OnBeatTrackTick delegate,
	 *  which passes the current damage taken, and the total possible damage. Executed on tick
	 *  by SphereTarget */
	UFUNCTION()
	void UpdateTrackingScore(float DamageTaken, float TotalPossibleDamage);

	/** Function bound to DefaultGameMode's OnTargetSpawned delegate to keep track of number of targets spawned.
	 *  Executed by TargetSpawner */
	UFUNCTION()
	void UpdateTargetsSpawned();

	/** Function bound to Gun_AK47's FOnShotFired delegate to keep track of number of targets spawned.
	 *  Executed by Gun_AK47 */
	UFUNCTION()
	void UpdateShotsFired();

	/** Function bound to DefaultGameMode's OnStreakUpdate delegate to keep track of streak.
	 *  Executed by TargetSpawner */
	UFUNCTION()
	void OnStreakUpdate(int32 Streak, FVector Location);

	/* Called by UpdatePlayerScores since everytime that function is called, a target has been hit */
	void UpdateTargetsHit();

	/** Called by UpdatePlayerScores or UpdatingTrackingScores to recalculate the high score if needed  */
	void UpdateHighScore();

#pragma endregion

#pragma region Scoring

	/** Loads the player scores and finds the highest score that matches the GameModeActorBase, Difficulty, and song title */
	void LoadPlayerScores();

public:
	/** Prepares PlayerScores for saving in GameInstance, then calls SavePlayerScores in DefaultGameInstance */
	void SavePlayerScores();

private:
	/** The "live" player score objects, which start fresh and import high score from SavedPlayerScores */
	UPROPERTY(VisibleAnywhere)
	FPlayerScore CurrentPlayerScore;

	/** Max score per target based on total amount of targets that could spawn */
	UPROPERTY(VisibleAnywhere)
	float MaxScorePerTarget;

#pragma endregion

	/** Function called when new settings are saved in Game Instance */
	UFUNCTION()
	void OnPlayerSettingsChange(const FPlayerSettings& PlayerSettings);

	float CheckFloatNaN(const float ValueToCheck, const float ValueToRound);

	/** Whether or not to show the Streak Combat Text */
	bool bShowStreakCombatText;

	/** The frequency at which to show Streak Combat Text */
	int32 CombatTextFrequency;

	/** Reference to the GameMode because it frequently needs to be called */
	UPROPERTY()
	ADefaultGameMode* GameMode;
};
