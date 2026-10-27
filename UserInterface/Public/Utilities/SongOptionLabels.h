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
FString MakeSongOptionLabel(const FSongMetadata& SongMetadata,
                            const int32 MaxFieldCount,
                            const bool bUseFieldPrefixes);

/** Computes the label spec for a group of songs sharing a title. */
FSongOptionLabelSpec ComputeSongOptionLabelSpec(const TArray<FSongMetadata>& TitleGroup);

/** One unique option label per entry: bare Title for single-song title groups, extended per shared-title group with
 *  the smallest distinguishing format found by ComputeSongOptionLabelSpec. */
TArray<FString> MakeSongOptionLabels(const TArray<FSongMetadata>& Entries);

/** Finds the entry whose option label matches OptionLabel (see MakeSongOptionLabels), or nullptr if none does. */
const FSongMetadata* FindSongByOptionLabel(const TArray<FSongMetadata>& Entries, const FString& OptionLabel);
