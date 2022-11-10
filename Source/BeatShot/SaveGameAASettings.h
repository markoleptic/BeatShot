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

UCLASS()
class BEATSHOT_API USaveGameAASettings : public USaveGame
{
	GENERATED_BODY()

public:
	USaveGameAASettings();

	UPROPERTY(VisibleAnywhere, Category = "AA")
	FAASettingsStruct AASettings;
};
