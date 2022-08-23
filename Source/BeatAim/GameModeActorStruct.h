// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameModeActorStruct.generated.h"

UENUM(BlueprintType)
enum class EGameModeActorName : uint8 {
	None						UMETA(DisplayName, "None"),
	WideSpreadMultiBeat			UMETA(DisplayName, "WideSpreadMultiBeat"),
	NarrowSpreadMultiBeat		UMETA(DisplayName, "NarrowSpreadMultiBeat")
};

// Used to store game properties, etc.
USTRUCT(BlueprintType)
struct FGameModeActorStruct
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Properties")
	EGameModeActorName GameModeActorName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Properties")
	FTimerHandle GameModeLengthTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Properties")
	float GameModeLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Properties")
	FTimerHandle CountDownTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Properties")
	float CountdownTimerLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Properties")
	float TargetSpawnCD;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Properties")
	float MinDistanceBetweenTargets;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Properties")
	float MinTargetScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Properties")
	float MaxTargetScale;

	//TODO: Use these values to set up properties in targetspawner

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Properties")
	float TargetMaxLifeSpan;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Properties")
	FVector BoxBounds;

	FGameModeActorStruct()
	{
		GameModeActorName = EGameModeActorName::None;
		MinDistanceBetweenTargets = 100.f;
		CountdownTimerLength = 3.f;
		GameModeLength = 0;
		TargetSpawnCD = 0.35f;
		TargetMaxLifeSpan = 1.5f;
		BoxBounds.X = 0.f;
		BoxBounds.Y = 390.f;
		BoxBounds.Z = 900.f;
		MinTargetScale = 0.8f;
		MaxTargetScale = 2.f;
	}
};

// Used to load and save player scores
USTRUCT(BlueprintType)
struct FPlayerScore
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
	FString GameMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
	float Score;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
	float HighScore;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
	float Accuracy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
	float ShotsFired;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
	float TargetsHit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Score")
	float TargetsSpawned;

	FPlayerScore()
	{
		GameMode = "";
		Score = 0;
		HighScore = 0;
		Accuracy =  0;
		ShotsFired = 0;
		TargetsHit = 0;
		TargetsSpawned = 0;
	}

	void ResetStruct()
	{
		GameMode = "";
		Score = 0;
		HighScore = 0;
		Accuracy = 0;
		ShotsFired = 0;
		TargetsHit = 0;
		TargetsSpawned = 0;
	}
};

// Used to load and save player scores
USTRUCT(BlueprintType)
struct FPlayerSettings
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float Sensitivity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float MasterVolume;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float MenuVolume;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float MusicVolume;

	FPlayerSettings()
	{
		Sensitivity = 0.3f;
		MasterVolume = 50.f;
		MenuVolume = 50.f;
		MusicVolume = 10.f;
	}

	void ResetStruct()
	{
		Sensitivity = 0.3f;
		MasterVolume = 50.f;
		MenuVolume = 50.f;
		MusicVolume = 10.f;
	}
};