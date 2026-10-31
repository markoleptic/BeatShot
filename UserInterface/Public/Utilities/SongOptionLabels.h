// Copyright 2022-2026 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Utilities/SongMetadata.h"

/** Max number of metadata fields a disambiguating label may use. The 6th field (Length) is only reachable for lists
 *  that keep songs with an identical 5-tuple but different Length, which only happens for score data. */
constexpr int32 MaxSongMetadataFields = 6;

/** Smallest-distinguishing label config for one shared-title group; the clean format is preferred, and field
 *  prefixes (Album:, Year:, Genre:, Length:) are used only when it cannot disambiguate. */
struct FSongOptionLabelSpec
{
	int32 FieldCount = 1;
	bool bUseFieldPrefixes = false;

	FSongOptionLabelSpec(const int32 InFieldCount, const bool bInUseFieldPrefixes)
		: FieldCount(InFieldCount), bUseFieldPrefixes(bInUseFieldPrefixes)
	{
	}
};

/** Labels one song with up to MaxFieldCount fields; parenthetical parts get a localized field prefix when
 *  bUseFieldPrefixes. */
USERINTERFACE_API FString MakeSongOptionLabel(const FSongMetadata& SongMetadata, const int32 MaxFieldCount, const bool bUseFieldPrefixes);

/** Computes the label spec for a group of songs sharing a title. */
USERINTERFACE_API FSongOptionLabelSpec ComputeSongOptionLabelSpec(const TArray<FSongMetadata>& TitleGroup);

/** One unique option label per entry: bare Title for single-song title groups, extended per shared-title group with
 *  the smallest distinguishing format found by ComputeSongOptionLabelSpec. */
USERINTERFACE_API TArray<FString> MakeSongOptionLabels(const TArray<FSongMetadata>& Entries);

/** Finds the entry whose option label matches OptionLabel (see MakeSongOptionLabels), or nullptr if none does. */
USERINTERFACE_API const FSongMetadata* FindSongByOptionLabel(const TArray<FSongMetadata>& Entries, const FString& OptionLabel);

/** Option labels for a file song list: smallest distinguishing label within shared-title groups (see
 *  ComputeSongOptionLabelSpec); entries with fully duplicated metadata are disambiguated by base filename (without
 *  extension), then full path, so labels stay unique and the box may key on them. Aligned with SongsByPath
 *  iteration order. */
USERINTERFACE_API TArray<FString> MakeSongPathOptionLabels(const TMap<FString, FSongMetadata>& SongsByPath);

/** The MakeSongPathOptionLabels label for the given path, or empty if the path isn't among the songs. */
USERINTERFACE_API FString GetSongPathOptionLabel(const TMap<FString, FSongMetadata>& SongsByPath, const FString& SongFilePath);

/** The path whose file list option label matches OptionLabel, or empty if none does. */
USERINTERFACE_API FString FindSongPathByOptionLabel(const TMap<FString, FSongMetadata>& SongsByPath, const FString& OptionLabel);

/** Given Songs (which includes the removed entry), returns the index of the option that sorts directly below the
 *  removed option by option-label order, or the one directly above it if it is the last. INDEX_NONE if there is no
 *  neighbor (the removed option was the only one). */
USERINTERFACE_API int32 FindNeighborSongIndex(const TArray<FSongMetadata>& Songs, const FSongMetadata& RemovedSong);

/** Maps each option label of SongsByPath to its path (see MakeSongPathOptionLabels; labels stay unique). */
USERINTERFACE_API TArray<TPair<FString, FString>> LabeledPathsOf(const TMap<FString, FSongMetadata>& SongsByPath);

/** Given the remaining Songs, returns the path of the option that sorts directly below the removed option by
 *  option-label order, or the one directly above it if it is the last. Empty path if no entry remains. */
USERINTERFACE_API FString FindNeighborSongFilePath(const TMap<FString, FSongMetadata>& Songs,
                                                   const FString& RemovedFilePath,
                                                   const FSongMetadata& RemovedMetadata);
