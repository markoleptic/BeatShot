// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SaveGamePlayerSettings.h"
#include "GameFramework/Actor.h"
#include "GameModeActorBase.generated.h"

class AFloatingTextActor;
class ASphereTarget;
class UAudioAnalyzerManager;
class UDefaultGameInstance;

/* Enum representing the default game mode names */
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

/* Enum representing the spread type of the targets */
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

/* Enum representing the default game mode difficulties */
UENUM(BlueprintType)
enum class EGameModeDifficulty : uint8
{
	None UMETA(DisplayName, "None"),
	Normal UMETA(DisplayName, "Normal"),
	Hard UMETA(DisplayName, "Hard"),
	Death UMETA(DisplayName, "Death")
};

ENUM_RANGE_BY_FIRST_AND_LAST(EGameModeDifficulty, EGameModeDifficulty::None, EGameModeDifficulty::Death);

/* Struct representing a game mode */
USTRUCT(BlueprintType)
struct FGameModeActorStruct
{
	GENERATED_BODY()

	/* The song title */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Defining Properties")
	FString SongTitle;

	/* The default game mode name, or custom if custom */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Defining Properties")
	EGameModeActorName GameModeActorName;

	/* Custom game mode name if custom, otherwise empty string */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Defining Properties")
	FString CustomGameModeName;

	/* Default game mode difficulties, or none if custom */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Defining Properties")
	EGameModeDifficulty GameModeDifficulty;

	/** Changes how targets are spawned relative to the spawn area. If static, it simply sets the spawn area size.
	 * If dynamic, the spawn area will gradually shrink as consecutive targets are hit */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Properties")
	ESpreadType SpreadType;

	/* Whether or not to dynamically change the size of targets as consecutive targets are hit */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Properties")
	bool UseDynamicSizing;

	/* Length of song */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Song Properties")
	float GameModeLength;

	/* Sets the minimum time between target spawns */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Properties")
	float TargetSpawnCD;

	/* Sets the minimum distance between recent target spawns */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Properties")
	float MinDistanceBetweenTargets;

	/* Min multiplier to target size */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Properties")
	float MinTargetScale;

	/* Max multiplier to target size */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Properties")
	float MaxTargetScale;

	/* Whether or not to spawn targets only at headshot height */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Properties")
	bool HeadshotHeight;

	/* Whether or not to center spawn area in the center of wall, vs as close to the ground as possible */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Properties")
	bool WallCentered;

	/* Maximum time in which target will stay on screen */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Properties")
	float TargetMaxLifeSpan;

	/* The size of the target spawn BoundingBox. Dimensions are half of the the total length/width */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Properties")
	FVector BoxBounds;

	/* Delay between AudioAnalyzer Tracker and Player. Also the same value as time between target spawn and peak green target color */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Properties")
	float PlayerDelay;

	/* The minimum speed multiplier for Tracking Game Mode */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BeatTrack Properties")
	float MinTrackingSpeed;

	/* The maximum speed multiplier for Tracking Game Mode */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BeatTrack Properties")
	float MaxTrackingSpeed;

	/* Whether or not the game mode derives from BeatTrack */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Properties")
	bool IsBeatTrackMode;

	/* Whether or not the game mode derives from SingleBeat */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Properties")
	bool IsSingleBeatMode;

	/* Whether or not the game mode derives from BeatGrid */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Properties")
	bool IsBeatGridMode;

	/* The number of BeatGrid targets, only square-able numbers */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BeatGrid Properties")
	int32 BeatGridSize;

	/* Whether or not to randomize the activation of BeatGrid targets vs only choosing adjacent targets */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BeatGrid Properties")
	bool RandomizeBeatGrid;

	/* The space between BeatGrid targets */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BeatGrid Properties")
	FVector2D BeatGridSpacing;

	/* not implemented yet */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BeatGrid Properties")
	int32 NumTargetsAtOnceBeatGrid;

	FORCEINLINE bool operator==(const FGameModeActorStruct& Other) const
	{
		if (GameModeActorName == Other.GameModeActorName &&
			SongTitle.Equals(Other.SongTitle) &&
			(CustomGameModeName.IsEmpty() && Other.CustomGameModeName.IsEmpty() ||
				CustomGameModeName.Equals(Other.CustomGameModeName)) &&
			GameModeDifficulty == Other.GameModeDifficulty)
		{
			return true;
		}
		return false;
	}

	/* Generic initialization */
	FGameModeActorStruct()
	{
		GameModeActorName = EGameModeActorName::Custom;
		SpreadType = ESpreadType::None;
		GameModeDifficulty = EGameModeDifficulty::Normal;
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

			// MultiBeat Spread Types
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
			else if (SpreadType == ESpreadType::DynamicEdgeOnly ||
				SpreadType == ESpreadType::DynamicRandom)
			{
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

			// SingleBeat Spread Types
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
			else if (SpreadType == ESpreadType::DynamicEdgeOnly ||
				SpreadType == ESpreadType::DynamicRandom)
			{
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

/* Used by PlayerScoreMap to abstract FPlayerScores */
USTRUCT(BlueprintType)
struct FPlayerScoreArrayWrapper
{
	GENERATED_BODY()

	/** originally wanted this to hold all FPlayerScores for a given EGameModeActorName, Song title, and Difficulty,
	/* but doesn't work sometimes. Saving and loading works even if equality doesn't work. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FPlayerScore> PlayerScoreArray;
};

FORCEINLINE uint32 GetTypeHash(const FGameModeActorStruct& Other)
{
	const uint32 Hash = FCrc::MemCrc32(&Other, sizeof(FGameModeActorStruct));
	return Hash;
}

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpdateScoresToHUD, FPlayerScore, NewPlayerScoreStruct);

UCLASS()
class BEATSHOT_API AGameModeActorBase : public AActor
{
	GENERATED_BODY()

public:
	AGameModeActorBase();

protected:
	virtual void BeginPlay() override;

	/* Called every frame */
	virtual void Tick(float DeltaTime) override;

public:
	/* Starts the game mode timer */
	UFUNCTION(BlueprintCallable, Category = "Game Start/End")
	void StartGameMode();

	/* called during StartGameMode */
	UFUNCTION(BlueprintCallable, Category = "Game Start/End")
	void InitializeGameModeActor();

	/* Called by DefaultGameMode. Clears any saved targets in Game Instance's sphere target array, destroys self */
	UFUNCTION(BlueprintCallable, Category = "Game Start/End")
	void EndGameMode(bool ShouldSavePlayerScores = false);

	/* Reports to DefaultGameMode when the song has finished */
	UFUNCTION(BlueprintCallable, Category = "Game Start/End")
	void OnGameModeLengthTimerComplete() const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "FloatingTextActor")
	TSubclassOf<AFloatingTextActor> FloatingTextActorToSpawn;

	/* The game mode defining properties */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Game Properties")
	FGameModeActorStruct GameModeActorStruct;

	/* Timer that spans the length of the song */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Game Properties")
	FTimerHandle GameModeLengthTimer;

#pragma region Scoring

	/* Function bound to TargetSpawner to keep track of streak */
	UFUNCTION(BlueprintCallable, Category = "FloatingTextActor")
	void OnStreakUpdate(int32 Streak, FVector Location);

	UFUNCTION(BlueprintCallable, Category = "Scoring")
	void UpdateHighScore();

	/* Called by TargetSpawner when a target takes damage */
	UFUNCTION(BlueprintCallable, Category = "Scoring")
	void UpdatePlayerScores(float TimeElapsed);

	/* Called by TargetSpawner when a SphereTarget is spawned */
	UFUNCTION(BlueprintCallable, Category = "Scoring")
	void UpdateTargetsSpawned();

	/* Called by DefaultCharacter when player shoots during an active game that is not a BeatTracking game modes */
	UFUNCTION(BlueprintCallable, Category = "Scoring")
	void UpdateShotsFired();

	/* Called by Projectile when a Player's projectile hits a SphereTarget during an active game that is not a BeatTracking game modes */
	UFUNCTION(BlueprintCallable, Category = "Scoring")
	void UpdateTargetsHit();

	/* Called when IsTrackingGameMode */
	UFUNCTION(BlueprintCallable, Category = "Scoring")
	void UpdateTrackingScore(float DamageTaken, float TotalPossibleDamage);

	/* Function called when new settings are saved in Game Instance */
	UFUNCTION(BlueprintCallable, Category = "Scoring")
	void OnPlayerSettingsChange(const FPlayerSettings& PlayerSettings);

	/* Delegate called when there is any update that should be reflected in PlayerHUD stats */
	UPROPERTY(VisibleAnywhere, BlueprintAssignable, Category = "Delegates")
	FUpdateScoresToHUD UpdateScoresToHUD;

	/* the saved score object, with accuracy, etc. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Scoring")
	FPlayerScore SavedPlayerScores;

	/* the wrapper struct that contains the array of saved player score objects */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Scoring")
	FPlayerScoreArrayWrapper PlayerScoreArrayWrapper;

	/* the "live" player score objects, which start fresh and import high score from SavedPlayerScores */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Scoring")
	FPlayerScore PlayerScores;

	/* max score per target based on total amount of targets that could spawn */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Scoring")
	float MaxScorePerTarget;

	/* Map of GameModes and Scores */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "References")
	TMap<FGameModeActorStruct, FPlayerScoreArrayWrapper> PlayerScoreMap;

#pragma endregion

private:
	/* Prepares PlayerScores for saving in GameInstance, then calls SavePlayerScores in DefaultGameInstance */
	void SavePlayerScores();

	/* Loads the player scores at the begin play */
	void LoadPlayerScores();

	/* Whether or not to show the Streak Combat Text */
	bool bShowStreakCombatText;

	/* The frequency at which to show Streak Combat Text */
	int32 CombatTextFrequency;
};
