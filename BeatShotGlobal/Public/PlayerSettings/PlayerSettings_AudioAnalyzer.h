// Copyright 2022-2025 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BSConstants.h"
#include "PlayerSettings_AudioAnalyzer.generated.h"

/** Audio Analyzer specific settings. */
USTRUCT(BlueprintType)
struct BEATSHOTGLOBAL_API FPlayerSettings_AudioAnalyzer
{
	GENERATED_BODY()

	/** Number of channels to break Tracker Sound frequencies into */
	UPROPERTY(BlueprintReadOnly)
	int NumBandChannels;

	/** Array to store Threshold values for each active band channel */
	UPROPERTY(BlueprintReadOnly)
	TArray<float> BandLimitsThreshold;

	/** Array to store band frequency channels */
	UPROPERTY(BlueprintReadOnly)
	TArray<FVector2D> BandLimits;

	/** Time window to take frequency sample */
	UPROPERTY(BlueprintReadOnly)
	float TimeWindow;

	/** History size of frequency sample */
	UPROPERTY(BlueprintReadOnly)
	int HistorySize;

	/** Max number of band channels allowed */
	int32 MaxNumBandChannels;

	UPROPERTY(BlueprintReadOnly)
	FString LastSelectedInputAudioDevice;

	FPlayerSettings_AudioAnalyzer()
	{
		BandLimits = Constants::DefaultBandLimits;
		BandLimitsThreshold = TArray<float>();
		BandLimitsThreshold.Init(Constants::DefaultBandLimitThreshold, Constants::DefaultNumBandChannels);
		NumBandChannels = Constants::DefaultNumBandChannels;
		TimeWindow = Constants::DefaultTimeWindow;
		HistorySize = Constants::DefaultHistorySize;
		MaxNumBandChannels = Constants::DefaultMaxNumBandChannels;
		LastSelectedInputAudioDevice = "";
	}

	/** Resets all settings to default, but keeps audio device information */
	void ResetToDefault()
	{
		BandLimits = Constants::DefaultBandLimits;
		BandLimitsThreshold = TArray<float>();
		BandLimitsThreshold.Init(Constants::DefaultBandLimitThreshold, Constants::DefaultNumBandChannels);
		NumBandChannels = Constants::DefaultNumBandChannels;
		TimeWindow = Constants::DefaultTimeWindow;
		HistorySize = Constants::DefaultHistorySize;
		MaxNumBandChannels = Constants::DefaultMaxNumBandChannels;
	}
};
