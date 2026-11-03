// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Utilities/SongMetadata.h"
#include "AudioConfig.generated.h"

/** The player chosen audio format for the current game mode. */
UENUM(BlueprintType)
enum class EAudioFormat : uint8
{
	None UMETA(DisplayName="None"),
	File UMETA(DisplayName="File"),
	Capture UMETA(DisplayName="Capture"),
	Loopback UMETA(DisplayName="Loopback")
};

ENUM_RANGE_BY_FIRST_AND_LAST(EAudioFormat, EAudioFormat::File, EAudioFormat::Loopback);

USTRUCT(BlueprintType)
struct BEATSHOTGLOBAL_API FBS_AudioConfig
{
	GENERATED_BODY()

	/** The path to the song file. */
	UPROPERTY(BlueprintReadOnly, Transient)
	FString SongFilePath;

	UPROPERTY(BlueprintReadOnly, Transient)
	FSongMetadata SongMetadata;

	/** Whether to playback streamed audio. */
	UPROPERTY(BlueprintReadOnly, Transient)
	bool bPlaybackAudio = true;

	/** The audio format type used for the AudioAnalyzer. */
	UPROPERTY(BlueprintReadOnly, Transient)
	EAudioFormat AudioFormat = EAudioFormat::None;

	/** The input audio device. */
	UPROPERTY(BlueprintReadOnly, Transient)
	FString InAudioDevice;

	FBS_AudioConfig() = default;
};
