// Copyright 2022-2025 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BSConstants.h"
#include "InputCoreTypes.h"
#include "SongMetadataSortingMethod.h"
#include "BSGameModeConfig/AudioConfig.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "Utilities/SongMetadata.h"
#include "PlayerSettings_User.generated.h"

/** User settings. */
USTRUCT(BlueprintType)
struct BEATSHOTGLOBAL_API FPlayerSettings_User
{
	GENERATED_BODY()

	/** Sensitivity of Character. */
	UPROPERTY(BlueprintReadOnly)
	float Sensitivity = Constants::DefaultSensitivity;

	UPROPERTY(BlueprintReadOnly)
	bool bNightModeUnlocked = false;

	UPROPERTY(BlueprintReadOnly)
	bool bHasRanBenchmark = false;

	UPROPERTY(BlueprintReadOnly)
	TMap<FName, FKey> Keybindings;

	UPROPERTY(BlueprintReadOnly)
	EAudioFormat LastUsedAudioFormat = EAudioFormat::File;

	UPROPERTY(BlueprintReadOnly)
	FString LastUsedSongFilePath;

	UPROPERTY(BlueprintReadOnly)
	TMap<FString, FSongMetadata> FilePathsAndSongMetadata;

	UPROPERTY(BlueprintReadOnly)
	TArray<FString> RecentFilePaths;

	UPROPERTY(BlueprintReadOnly)
	FString LastUsedInputAudioDevice;

	UPROPERTY(BlueprintReadOnly)
	FSongMetadata LastUsedAudioDeviceSongMetadata;

	/** Recent songs for capture mode. List identity is IsEqualExcludingLength — Length is a mutable session parameter.
	 */
	UPROPERTY(BlueprintReadOnly)
	TArray<FSongMetadata> RecentAudioDeviceSongMetadata;

	UPROPERTY(BlueprintReadOnly)
	bool bPlaybackAudioDuringCapture = false;

	UPROPERTY(BlueprintReadOnly)
	ESongMetadataSortingMethod SongMetadataSortingMethod = ESongMetadataSortingMethod::SongTitle;

	FPlayerSettings_User() = default;

	/** @return An array of Keybindings for use with UEnhancedInputUserSettings. Empties the Keybindings map */
	TArray<FMapPlayerKeyArgs> GetLegacyKeybindings();

	/** Adds (or updates) a file song; new songs enter the recency list as most recent. */
	void AddSongMetadata(const FString& SongFilePath, const FSongMetadata& SongMetadata);

	void RemoveSongMetadata(const FString& SongFilePath);

	/** Updates (or adds) the song at this path. Always succeeds — file-mode songs are keyed by path. */
	void EditSongMetadata(const FString& SongFilePath, const FSongMetadata& SongMetadata);

	bool AddAudioDeviceSongMetadata(const FSongMetadata& SongMetadata);

	void RemoveAudioDeviceSongMetadata(const FSongMetadata& SongMetadata);

	bool EditAudioDeviceSongMetadata(const FSongMetadata& ExistingSongMetadata,
	                                 const FSongMetadata& ProposedSongMetadata);

	/** Moves the file song to most recent and records it as the last-used file song. */
	void MarkFileSongUsed(const FBS_AudioConfig& AudioConfig);

	/** Moves the capture song to most recent and sets the last-used device, playback, and song. */
	void MarkCaptureSongUsed(const FBS_AudioConfig& AudioConfig);
};
