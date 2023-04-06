// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once
#include "GlobalEnums.h"
#include "DLSSLibrary.h"
#include "NISLibrary.h"
#include "GlobalStructs.generated.h"


/* Why aren't 2d arrays a thing */
USTRUCT()
struct F2DArray
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<float> Accuracy;
};

/* Struct representing AI parameters */
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

/* BeatGrid Configuration */
USTRUCT(BlueprintType)
struct FBeatGridConfig
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

	FBeatGridConfig()
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

/* Struct representing a game mode */
USTRUCT(BlueprintType)
struct FBSConfig
{
	GENERATED_BODY()

	/* The song title */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Defining Properties")
	FString SongTitle;

	/* The default game mode name, or custom if custom */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Defining Properties")
	EDefaultMode DefaultMode;

	/* The base game mode this game mode is based off of */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Defining Properties")
	EDefaultMode BaseGameMode;

	/* Custom game mode name if custom, otherwise empty string */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Defining Properties")
	FString CustomGameModeName;

	/* Default game mode difficulties, or none if custom */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Defining Properties")
	EGameModeDifficulty GameModeDifficulty;

	/* Whether or not to playback streamed audio */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Defining Properties")
	bool bPlaybackAudio;

	/* The audio format type used for the AudioAnalyzer */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Defining Properties")
	EAudioFormat AudioFormat;

	/* The input audio device */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Defining Properties")
	FString InAudioDevice;

	/* The output audio device */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Defining Properties")
	FString OutAudioDevice;

	/* The path to the song file */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Defining Properties")
	FString SongPath;

	/** Changes how targets are spawned relative to the spawn area. If static, it simply sets the spawn area size.
	 * If dynamic, the spawn area will gradually shrink as consecutive targets are hit */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | General")
	ESpreadType SpreadType;

	/* Whether or not to dynamically change the size of targets as consecutive targets are hit */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | General")
	bool UseDynamicSizing;

	/* Length of song */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | General")
	float GameModeLength;

	/* Sets the minimum time between target spawns */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | General")
	float TargetSpawnCD;

	/* Sets the minimum distance between recent target spawns */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | General")
	float MinDistanceBetweenTargets;

	/* Min multiplier to target size */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | General")
	float MinTargetScale;

	/* Max multiplier to target size */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | General")
	float MaxTargetScale;

	/* Whether or not to spawn targets only at headshot height */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | General")
	bool HeadshotHeight;

	/* Whether or not to center spawn area in the center of wall, vs as close to the ground as possible */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | General")
	bool WallCentered;

	/* Maximum time in which target will stay on screen */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | General")
	float TargetMaxLifeSpan;

	/* The size of the target spawn BoundingBox. Dimensions are half of the the total length/width */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | General")
	FVector BoxBounds;

	/* The min size of the target spawn BoundingBox. Dimensions are half of BoxBounds */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | General")
	FVector MinBoxBounds;

	/* Delay between AudioAnalyzer Tracker and Player. Also the same value as time between target spawn and peak green target color */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | General")
	float PlayerDelay;

	/* Whether or not to move the targets forward towards the player after spawning */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | General")
	bool bMoveTargetsForward;

	/* How far to move the target forward over its lifetime */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | General")
	float MoveForwardDistance;

	/* How far to move the target forward over its lifetime */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | AI")
	FBS_AIConfig AIConfig;

	/* The minimum speed multiplier for Tracking Game Mode */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | BeatTrack")
	float MinTrackingSpeed;

	/* The maximum speed multiplier for Tracking Game Mode */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | BeatTrack")
	float MaxTrackingSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Properties | BeatGrid")
	FBeatGridConfig BeatGridConfig;

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
		SpreadType = ESpreadType::None;
		GameModeDifficulty = EGameModeDifficulty::Normal;
		bPlaybackAudio = false;
		AudioFormat = EAudioFormat::None;
		InAudioDevice = "";
		OutAudioDevice = "";
		SongPath = "";
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
		AIConfig = FBS_AIConfig();
		SongTitle = "";
		CustomGameModeName = "";
		MinTrackingSpeed = 500.f;
		MaxTrackingSpeed = 500.f;
		BeatGridConfig = FBeatGridConfig();
		BoxBounds = FVector(0.f, 3200.f, 1000.f);
		MinBoxBounds = FVector(0.f, 3200.f, 1000.f);
	}

	FBSConfig(const EDefaultMode InDefaultMode, const EGameModeDifficulty NewGameModeDifficulty = EGameModeDifficulty::Normal, const ESpreadType NewSpreadType = ESpreadType::None)
	{
		// Parameters
		DefaultMode = InDefaultMode;
		BaseGameMode = EDefaultMode::MultiBeat;
		GameModeDifficulty = NewGameModeDifficulty;
		SpreadType = NewSpreadType;

		// Constant for all Game Modes and Difficulties
		bPlaybackAudio = false;
		AudioFormat = EAudioFormat::None;
		InAudioDevice = "";
		OutAudioDevice = "";
		SongPath = "";
		GameModeLength = 0.f;
		HeadshotHeight = false;
		SongTitle = "";
		CustomGameModeName = "";
		WallCentered = false;
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
		BeatGridConfig = FBeatGridConfig();
		BoxBounds = FVector(0.f, 3200.f, 1000.f);
		MinBoxBounds = FVector(0.f, 3200.f, 1000.f);

		switch (DefaultMode)
		{
		case EDefaultMode::SingleBeat: BaseGameMode = EDefaultMode::SingleBeat;
			UseDynamicSizing = true;
			switch (GameModeDifficulty)
			{
			case EGameModeDifficulty::Normal:
				PlayerDelay = 0.3f;
				TargetSpawnCD = 0.3f;
				TargetMaxLifeSpan = 0.8f;
				MinTargetScale = 0.75f;
				MaxTargetScale = 2.f;
				break;
			case EGameModeDifficulty::Hard:
				PlayerDelay = 0.25f;
				TargetSpawnCD = 0.25f;
				TargetMaxLifeSpan = 0.65f;
				MinTargetScale = 0.6f;
				MaxTargetScale = 1.5f;
				break;
			case EGameModeDifficulty::Death:
				PlayerDelay = 0.2f;
				TargetSpawnCD = 0.2f;
				TargetMaxLifeSpan = 0.45f;
				MinTargetScale = 0.4f;
				MaxTargetScale = 1.5f;
				break;
			case EGameModeDifficulty::None:
				break;
			}
			switch (SpreadType)
			{
			case ESpreadType::StaticNarrow:
				UseDynamicSizing = false;
				BoxBounds = FVector(0.f, 1600.f, 500.f);
				break;
			case ESpreadType::StaticWide:
				UseDynamicSizing = false;
				BoxBounds = FVector(0.f, 3200.f, 1000.f);
				break;
			default:
				SpreadType = ESpreadType::DynamicEdgeOnly;
				UseDynamicSizing = true;
				BoxBounds = FVector(0.f, 2000.f, 800.f);
				MinBoxBounds = 0.5f * BoxBounds;
				break;
			}
			break;
		case EDefaultMode::MultiBeat:
			UseDynamicSizing = true;
			BaseGameMode = EDefaultMode::MultiBeat;
			switch (GameModeDifficulty)
			{
			case EGameModeDifficulty::Normal:
				PlayerDelay = 0.35f;
				TargetSpawnCD = 0.35f;
				TargetMaxLifeSpan = 1.f;
				MinTargetScale = 0.75f;
				MaxTargetScale = 2.f;
				break;
			case EGameModeDifficulty::Hard:
				PlayerDelay = 0.3f;
				TargetSpawnCD = 0.3f;
				TargetMaxLifeSpan = 0.75f;
				MinTargetScale = 0.6f;
				MaxTargetScale = 1.5f;
				break;
			case EGameModeDifficulty::Death:
				PlayerDelay = 0.25f;
				TargetSpawnCD = 0.20f;
				TargetMaxLifeSpan = 0.5f;
				MinTargetScale = 0.4f;
				MaxTargetScale = 1.25f;
				break;
			case EGameModeDifficulty::None: break;
			}
			switch (SpreadType)
			{
			case ESpreadType::StaticNarrow:
				UseDynamicSizing = false;
				BoxBounds = FVector(0.f, 1600.f, 500.f);
				break;
			case ESpreadType::StaticWide:
				UseDynamicSizing = false;
				BoxBounds = FVector(0.f, 3200.f, 1000.f);
				break;
			default:
				SpreadType = ESpreadType::DynamicRandom;
				UseDynamicSizing = true;
				BoxBounds = FVector(0.f, 2000.f, 800.f);
				MinBoxBounds = 0.5f * BoxBounds;
				break;
			}
			break;
		case EDefaultMode::BeatGrid:
			SpreadType = ESpreadType::None;
			BaseGameMode = EDefaultMode::BeatGrid;
			BoxBounds = FVector(0.f, 3200.f, 1000.f);
			BeatGridConfig.SetConfigByDifficulty(GameModeDifficulty);
			switch (GameModeDifficulty)
			{
			case EGameModeDifficulty::Normal:
				PlayerDelay = 0.35f;
				TargetSpawnCD = 0.35f;
				TargetMaxLifeSpan = 1.2f;
				MinTargetScale = 0.85f;
				MaxTargetScale = 0.85f;
				break;
			case EGameModeDifficulty::Hard:
				PlayerDelay = 0.3f;
				TargetSpawnCD = 0.30f;
				TargetMaxLifeSpan = 1.f;
				MinTargetScale = 0.7f;
				MaxTargetScale = 0.7f;
				break;
			case EGameModeDifficulty::Death:
				PlayerDelay = 0.25f;
				TargetSpawnCD = 0.25f;
				TargetMaxLifeSpan = 0.75f;
				MinTargetScale = 0.5f;
				MaxTargetScale = 0.5f;
				break;
			case EGameModeDifficulty::None:
				break;
			}
			break;
		case EDefaultMode::BeatTrack:
			SpreadType = ESpreadType::None;
			BaseGameMode = EDefaultMode::BeatTrack;
			WallCentered = true;
			PlayerDelay = 0.f;
			TargetMaxLifeSpan = 0.f;
			MinTrackingSpeed = 500.f;
			MaxTrackingSpeed = 500.f;
			switch (GameModeDifficulty)
			{
			case EGameModeDifficulty::Normal:
				MinTrackingSpeed = 400.f;
				MaxTrackingSpeed = 500.f;
				TargetSpawnCD = 0.75f;
				MinTargetScale = 1.3f;
				MaxTargetScale = 1.3f;
				break;
			case EGameModeDifficulty::Hard:
				MinTrackingSpeed = 500.f;
				MaxTrackingSpeed = 600.f;
				TargetSpawnCD = 0.6f;
				MinTargetScale = 1.f;
				MaxTargetScale = 1.f;
				break;
			case EGameModeDifficulty::Death:
				MinTrackingSpeed = 500.f;
				MaxTrackingSpeed = 700.f;
				TargetSpawnCD = 0.45f;
				MinTargetScale = 0.75f;
				MaxTargetScale = 0.75;
				break;
			case EGameModeDifficulty::None:
				break;
			}
			break;
		case EDefaultMode::Custom: break;
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
	TArray<F2DArray> LocationAccuracy;

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

/* CrossHair settings */
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

/* Used by AASettings widget to relay Audio Analyzer settings to DefaultGameInstance */
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
