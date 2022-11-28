// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameModeActorBase.generated.h"

class AFloatingTextActor;
class ASphereTarget;
class UAudioAnalyzerManager;
class UDefaultGameInstance;

// Base game mode names
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

// Target spawning spread
UENUM(BlueprintType)
enum class ESpreadType : uint8 {
	None						UMETA(DisplayName, "None"),
	DynamicEdgeOnly				UMETA(DisplayName, "DynamicEdgeOnly"),
	DynamicRandom				UMETA(DisplayName, "DynamicRandom"),
	StaticNarrow				UMETA(DisplayName, "StaticNarrow"),
	StaticWide					UMETA(DisplayName, "StaticWide")
};
ENUM_RANGE_BY_FIRST_AND_LAST(ESpreadType, ESpreadType::None, ESpreadType::StaticWide);

// Base difficulties
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
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Properties")
		FString SongTitle;

	// Used to Spawn GameModes deriving from GameModeActorBase
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Properties")
		EGameModeActorName GameModeActorName;

	// If user creates custom mode, save it with this name
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Properties")
		FString CustomGameModeName;

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Song Properties")
		FTimerHandle GameModeLengthTimer;

	// Length of song
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Song Properties")
		float GameModeLength;

	// Countdown TimerHandle
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Song Properties")
		FTimerHandle CountDownTimer;

	// Countdown Time Length
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Static Game Properties")
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
		MinDistanceBetweenTargets = 10.f;
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
		MinDistanceBetweenTargets = 10.f;
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
				MinTargetScale = 1.f;
				MaxTargetScale = 1.f;
				BeatGridSize = 16;
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
			CenterOfSpawnBox = { 3590.f,0.f,750.f };

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
		MinDistanceBetweenTargets = 10.f;
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

// Used to load and save player scores
USTRUCT(BlueprintType)
struct FPlayerScore
{
	GENERATED_BODY()

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

// Used by PlayerScoreMap to abstract FPlayerScores
USTRUCT(BlueprintType)
struct FPlayerScoreArrayWrapper
{
	GENERATED_BODY()

		// originally wanted this to hold all FPlayerScores for a given EGameModeActorName and Song title,
		// but doesn't work sometimes. Saving and loading works even if equality doesn't work.
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FPlayerScore> PlayerScoreArray;
};

FORCEINLINE uint32 GetTypeHash(const FGameModeActorStruct& Other)
{
	uint32 Hash = FCrc::MemCrc32(&Other, sizeof(FGameModeActorStruct));
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

	UFUNCTION(BlueprintCallable, Category = "Scoring")
		void SavePlayerScores();

	UFUNCTION(BlueprintCallable, Category = "Scoring")
		void LoadPlayerScores();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// called by WBP_Countdown when player clicks anywhere on screen, including after game restart
	UFUNCTION(BlueprintCallable, Category = "Game Start/End")
		void InitializeGameModeActor();

	UFUNCTION(BlueprintCallable, Category = "Game Start/End")
		void StartGameMode();

	UFUNCTION(BlueprintCallable, Category = "Game Start/End")
		void EndGameMode(bool ShouldSavePlayerScores = false);

	UFUNCTION(BlueprintCallable, Category = "Game Start/End")
		void StartCountDownTimer();

	UFUNCTION(BlueprintCallable, Category = "Game Start/End")
		void OnGameModeLengthTimerComplete();

	UFUNCTION(BlueprintCallable, Category = "FloatingTextActor")
		void OnStreakUpdate(int32 Streak, FVector Location);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "FloatingTextActor")
		TSubclassOf<AFloatingTextActor> FloatingTextActorToSpawn;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Game Properties")
		FGameModeActorStruct GameModeActorStruct;

	UPROPERTY(VisibleAnywhere, BlueprintAssignable, Category = "Game Properties")
		FUpdateScoresToHUD UpdateScoresToHUD;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Game Properties")
		FTimerHandle GameModeLengthTimer;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Countdown")
		FTimerHandle CountDownTimer;

	// Reference Game Instance
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "References")
		UDefaultGameInstance* GI;

	// Scoring

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "References")
		TMap<FGameModeActorStruct, FPlayerScoreArrayWrapper> PlayerScoreMap;

	// the saved score object, with accuracy, etc.
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Scoring")
		FPlayerScore SavedPlayerScores;

	// the wrapper struct that contains the array of saved player score objects
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Scoring")
		FPlayerScoreArrayWrapper PlayerScoreArrayWrapper;

	// the "live" player score objects, which start fresh and import high score from SavedPlayerScores
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Scoring")
		FPlayerScore PlayerScores;

	// max score per target based on total amount of targets that could spawn
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Scoring")
		float MaxScorePerTarget;

	UFUNCTION(BlueprintCallable, Category = "Scoring")
		void UpdateHighScore();

	// Called by SphereTarget when it takes damage
	UFUNCTION(BlueprintCallable, Category = "Scoring")
		void UpdatePlayerScores(float TimeElapsed);

	// Called by TargetSpawner when a SphereTarget is spawned
	UFUNCTION(BlueprintCallable, Category = "Scoring")
		void UpdateTargetsSpawned();

	// Called by DefaultCharacter when player shoots during an active game that is not a BeatTracking game modes
	UFUNCTION(BlueprintCallable, Category = "Scoring")
		void UpdateShotsFired();

	// Called by Projectile when a Player's projectile hits a SphereTarget during an active game that is not a BeatTracking game modes
	UFUNCTION(BlueprintCallable, Category = "Scoring")
		void UpdateTargetsHit();

	// Called when IsTrackingGameMode == true
	UFUNCTION(BlueprintCallable, Category = "Scoring")
		void UpdateTrackingScore(float DamageTaken, float TotalPossibleDamage);

	//UFUNCTION(BlueprintCallable, Category = "Game Start/End")
	//	void UpdateStreak();
};
