// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SaveGameAASettings.generated.h"

// Used by AASettings widget to relay Audio Analyzer settings to DefaultGameInstance
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

	FAASettingsStruct()
	{
		NumBandChannels = 3;
		BandLimitsThreshold = {2.1f, 2.1f, 2.1f};
		BandLimits = {
			FVector2D(0.f, 87.f),
			FVector2D(500.f, 700.f),
			FVector2D(5000.f, 12000.f)
		};
		TimeWindow = 0.02f;
		HistorySize = 30.f;
	}

	void ResetStruct()
	{
		NumBandChannels = 3;
		BandLimitsThreshold = {2.1f, 2.1f, 2.1f};
		BandLimits = {
			FVector2D(0.f, 87.f),
			FVector2D(500.f, 700.f),
			FVector2D(5000.f, 12000.f)
		};
		TimeWindow = 0.02f;
		HistorySize = 30.f;
	}
};

UCLASS()
class BEATSHOT_API USaveGameAASettings : public USaveGame
{
	GENERATED_BODY()

public:
	USaveGameAASettings();

	UPROPERTY(BlueprintReadOnly)
	FAASettingsStruct AASettings;
};
