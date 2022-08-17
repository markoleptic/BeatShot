// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameModeActorStruct.generated.h"

UENUM(BlueprintType)
enum class GameModeActorName : uint8 {
	WideSpreadMultiBeat			UMETA(DisplayName = "WideSpreadMultiBeat"),
	NarrowSpreadMultiBeat		UMETA(DisplayName = "NarrowSpreadMultiBeat")
};

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

	UPROPERTY(VisibleAnywhere, Category = "Settings Menu")
	float TargetSpawnCD = 0.35f;

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