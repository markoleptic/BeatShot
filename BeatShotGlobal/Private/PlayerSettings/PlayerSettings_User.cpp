// Copyright 2022-2025 Markoleptic Games, SP. All Rights Reserved.

#include "PlayerSettings/PlayerSettings_User.h"

TArray<FMapPlayerKeyArgs> FPlayerSettings_User::GetLegacyKeybindings()
{
	TArray<FMapPlayerKeyArgs> Out;
	for (const TPair<FName, FKey>& Keybinding : Keybindings)
	{
		FGameplayTagContainer Failure;
		FMapPlayerKeyArgs Args;
		Args.NewKey = Keybinding.Value;

		FString StringKey = Keybinding.Key.ToString();
		if (StringKey.Len() > 2 && StringKey.EndsWith("_2"))
		{
			StringKey = StringKey.LeftChop(2);
			FString Last;
			Last.AppendChar(StringKey[StringKey.Len() - 1]);
			if (!Last.IsNumeric())
			{
				Args.MappingName = FName(StringKey);
				Args.Slot = EPlayerMappableKeySlot::Second;
			}
		}
		else
		{
			Args.MappingName = Keybinding.Key;
			Args.Slot = EPlayerMappableKeySlot::First;
		}
		Out.Add(Args);
	}
	Keybindings.Empty();
	return Out;
}

void FPlayerSettings_User::AddSongMetadata(const FString& SongFilePath, const FSongMetadata& SongMetadata)
{
	// New songs enter the recency list as most recent. Reordering only happens on session start.
	FilePathsAndSongMetadata.Add(SongFilePath, SongMetadata);
	if (!RecentFilePaths.Contains(SongFilePath))
	{
		RecentFilePaths.Add(SongFilePath);
	}
}

void FPlayerSettings_User::RemoveSongMetadata(const FString& SongFilePath)
{
	RecentFilePaths.Remove(SongFilePath);
	FilePathsAndSongMetadata.Remove(SongFilePath);
	if (LastUsedSongFilePath == SongFilePath)
	{
		LastUsedSongFilePath = RecentFilePaths.IsEmpty() ? FString() : RecentFilePaths.Last();
	}
}

void FPlayerSettings_User::EditSongMetadata(const FString& SongFilePath, const FSongMetadata& SongMetadata)
{
	// The same identity may legitimately sit under different paths.
	if (FSongMetadata* Existing = FilePathsAndSongMetadata.Find(SongFilePath))
	{
		*Existing = SongMetadata;
	}
	else
	{
		// Unknown path: route through Add so the recency list stays consistent.
		AddSongMetadata(SongFilePath, SongMetadata);
	}
}

bool FPlayerSettings_User::AddAudioDeviceSongMetadata(const FSongMetadata& SongMetadata)
{
	// Reject only capture-list duplicates (the capture dropdown options are built from this list).
	// Duplicates of file-mode songs are allowed.
	const auto FilteredAudioDevice = RecentAudioDeviceSongMetadata.FilterByPredicate([&](const FSongMetadata& Current)
	{
		return SongMetadata.IsEqualExcludingLength(Current);
	});
	if (FilteredAudioDevice.IsEmpty())
	{
		RecentAudioDeviceSongMetadata.Add(SongMetadata);
		return true;
	}
	else
	{
		return false;
	}
}

void FPlayerSettings_User::RemoveAudioDeviceSongMetadata(const FSongMetadata& SongMetadata)
{
	// Length is user-editable, so it must not break the lookup.
	RecentAudioDeviceSongMetadata.RemoveAll([SongMetadata](const FSongMetadata& Current)
	{
		return SongMetadata.IsEqualExcludingLength(Current);
	});

	if (SongMetadata.IsEqualExcludingLength(LastUsedAudioDeviceSongMetadata))
	{
		LastUsedAudioDeviceSongMetadata = RecentAudioDeviceSongMetadata.IsEmpty()
		                                  ? FSongMetadata()
		                                  : RecentAudioDeviceSongMetadata.Last();
	}
}

bool FPlayerSettings_User::EditAudioDeviceSongMetadata(const FSongMetadata& ExistingSongMetadata,
                                                       const FSongMetadata& ProposedSongMetadata)
{
	// Length is user-editable, so identity lookups key on IsEqualExcludingLength.
	int32 ExistingIndex = INDEX_NONE;
	int32 NumMatchingExisting = 0;
	int32 NumMatchingProposed = 0;
	for (int32 SongIndex = 0; SongIndex < RecentAudioDeviceSongMetadata.Num(); SongIndex++)
	{
		const FSongMetadata& Current = RecentAudioDeviceSongMetadata[SongIndex];
		if (ExistingSongMetadata.IsEqualExcludingLength(Current))
		{
			if (ExistingIndex == INDEX_NONE)
			{
				ExistingIndex = SongIndex;
			}
			NumMatchingExisting++;
		}
		else if (ProposedSongMetadata.IsEqualExcludingLength(Current))
		{
			NumMatchingProposed++;
		}
	}
	check(NumMatchingExisting >= 1);
	if (NumMatchingExisting > 1 || NumMatchingProposed > 0)
	{
		return false;
	}

	RecentAudioDeviceSongMetadata[ExistingIndex] = ProposedSongMetadata;
	return true;
}

void FPlayerSettings_User::MarkFileSongUsed(const FBS_AudioConfig& AudioConfig)
{
	LastUsedAudioFormat = EAudioFormat::File;
	LastUsedSongFilePath = AudioConfig.SongFilePath;

	// Move the song to most recent; Remove keeps the list free of duplicates.
	RecentFilePaths.Remove(AudioConfig.SongFilePath);
	RecentFilePaths.Add(AudioConfig.SongFilePath);
}

void FPlayerSettings_User::MarkCaptureSongUsed(const FBS_AudioConfig& AudioConfig)
{
	LastUsedAudioFormat = EAudioFormat::Capture;
	LastUsedInputAudioDevice = AudioConfig.InAudioDevice;
	bPlaybackAudioDuringCapture = AudioConfig.bPlaybackAudio;
	LastUsedAudioDeviceSongMetadata = AudioConfig.SongMetadata;

	// Length is a session parameter, so identity lookups key on IsEqualExcludingLength.
	RecentAudioDeviceSongMetadata.RemoveAll([&](const FSongMetadata& Current)
	{
		return AudioConfig.SongMetadata.IsEqualExcludingLength(Current);
	});
	RecentAudioDeviceSongMetadata.Add(AudioConfig.SongMetadata);
}
