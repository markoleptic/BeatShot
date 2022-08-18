// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameModeActorStruct.generated.h"

UENUM(BlueprintType)
enum class GameModeActorName : uint8 {
	WideSpreadMultiBeat			UMETA(DisplayName = "WideSpreadMultiBeat"),
	NarrowSpreadMultiBeat		UMETA(DisplayName = "NarrowSpreadMultiBeat")
};

// Used to store game properties, etc.
USTRUCT(BlueprintType)
struct FGameModeActorStruct
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Start Conditions")
	bool GameModeSelected;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Game Properties")
	FTimerHandle GameModeLengthTimer;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Game Properties")
	float GameModeLength;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Game Properties")
	FTimerHandle CountDownTimer;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Game Properties")
	float TargetSpawnCD = 0.35f;

	// can set this property in targetspawner
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Game Properties")
	float TargetMaxLifeSpan = 1.5f;

	FGameModeActorStruct()
	{
		GameModeSelected = false;
		GameModeLength = 0;
		TargetSpawnCD = 0.35f;
	}

	void SetGameModeSelected(bool IsSelected)
	{
		GameModeSelected = IsSelected;
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