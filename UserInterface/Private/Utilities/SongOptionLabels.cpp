// Copyright 2022-2026 Markoleptic Games, SP. All Rights Reserved.


#include "Utilities/SongOptionLabels.h"
#include "Utilities/BSWidgetInterface.h"

namespace
{
FString FormatSongLength(const float LengthSeconds)
{
	const int32 TotalSeconds = FMath::FloorToInt(LengthSeconds);
	return FString::Printf(TEXT("%d:%02d"), TotalSeconds / 60, TotalSeconds % 60);
}
}

FString MakeSongOptionLabel(const FSongMetadata& SongMetadata, const int32 MaxFieldCount, const bool bUseFieldPrefixes)
{
	FString Label = SongMetadata.Title;
	if (MaxFieldCount >= 2 && !SongMetadata.Artist.IsEmpty())
	{
		Label += TEXT(" — ") + SongMetadata.Artist;
	}
	TArray<FString> ParentheticalParts;
	if (MaxFieldCount >= 3 && !SongMetadata.Album.IsEmpty())
	{
		if (bUseFieldPrefixes)
		{
			static const FString AlbumText = IBSWidgetInterface::GetWidgetTextFromKey("AF_SongAlbum").ToString();
			ParentheticalParts.Add(AlbumText + TEXT(" ") + SongMetadata.Album);
		}
		else
		{
			ParentheticalParts.Add(SongMetadata.Album);
		}
	}
	if (MaxFieldCount >= 4 && !SongMetadata.Year.IsEmpty())
	{
		if (bUseFieldPrefixes)
		{
			static const FString YearText = IBSWidgetInterface::GetWidgetTextFromKey("AF_SongYear").ToString();
			ParentheticalParts.Add(YearText + TEXT(" ") + SongMetadata.Year);
		}
		else
		{
			ParentheticalParts.Add(SongMetadata.Year);
		}
	}
	if (MaxFieldCount >= 5 && !SongMetadata.Genre.IsEmpty())
	{
		if (bUseFieldPrefixes)
		{
			static const FString GenreText = IBSWidgetInterface::GetWidgetTextFromKey("AF_SongGenre").ToString();
			ParentheticalParts.Add(GenreText + TEXT(" ") + SongMetadata.Genre);
		}
		else
		{
			ParentheticalParts.Add(SongMetadata.Genre);
		}
	}
	if (MaxFieldCount >= 6)
	{
		if (bUseFieldPrefixes)
		{
			static const FString LengthText = IBSWidgetInterface::GetWidgetTextFromKey("AF_SongLength").ToString();
			ParentheticalParts.Add(LengthText + TEXT(" ") + FormatSongLength(SongMetadata.Length));
		}
		else
		{
			ParentheticalParts.Add(FormatSongLength(SongMetadata.Length));
		}
	}
	if (ParentheticalParts.Num() > 0)
	{
		Label += TEXT(" (") + FString::Join(ParentheticalParts, TEXT(", ")) + TEXT(")");
	}
	return Label;
}

FSongOptionLabelSpec ComputeSongOptionLabelSpec(const TArray<FSongMetadata>& TitleGroup)
{
	for (const bool bUseFieldPrefixes : {false, true})
	{
		for (int32 FieldCount = 1; FieldCount <= MaxSongMetadataFields; FieldCount++)
		{
			TSet<FString> DistinctLabels;
			for (const FSongMetadata& Current : TitleGroup)
			{
				DistinctLabels.Add(MakeSongOptionLabel(Current, FieldCount, bUseFieldPrefixes));
			}
			if (DistinctLabels.Num() == TitleGroup.Num())
			{
				return FSongOptionLabelSpec(FieldCount, bUseFieldPrefixes);
			}
		}
	}
	// Unreachable: max fields with prefixes is an injective function of the full metadata.
	return FSongOptionLabelSpec(MaxSongMetadataFields, true);
}

TArray<FString> MakeSongOptionLabels(const TArray<FSongMetadata>& Entries)
{
	TArray<FString> OptionLabels;
	OptionLabels.SetNum(Entries.Num());
	TMap<FString, TArray<int32>> TitleToIndices;
	for (int32 Index = 0; Index < Entries.Num(); Index++)
	{
		TitleToIndices.FindOrAdd(Entries[Index].Title).Add(Index);
	}
	for (const TPair<FString, TArray<int32>>& TitlePair : TitleToIndices)
	{
		TArray<FSongMetadata> TitleGroup;
		for (const int32 Index : TitlePair.Value)
		{
			TitleGroup.Add(Entries[Index]);
		}
		const FSongOptionLabelSpec LabelSpec = ComputeSongOptionLabelSpec(TitleGroup);
		for (const int32 Index : TitlePair.Value)
		{
			OptionLabels[Index] =
				MakeSongOptionLabel(Entries[Index], LabelSpec.FieldCount, LabelSpec.bUseFieldPrefixes);
		}
	}
	return OptionLabels;
}

const FSongMetadata* FindSongByOptionLabel(const TArray<FSongMetadata>& Entries, const FString& OptionLabel)
{
	const TArray<FString> OptionLabels = MakeSongOptionLabels(Entries);
	for (int32 Index = 0; Index < Entries.Num(); Index++)
	{
		if (OptionLabels[Index] == OptionLabel)
		{
			return &Entries[Index];
		}
	}
	return nullptr;
}

namespace
{
TArray<TPair<FString, FSongMetadata>> ToEntries(const TMap<FString, FSongMetadata>& SongsByPath)
{
	TArray<TPair<FString, FSongMetadata>> Entries;
	Entries.Reserve(SongsByPath.Num());
	for (const TPair<FString, FSongMetadata>& Current : SongsByPath)
	{
		Entries.Emplace(Current);
	}
	return Entries;
}

TArray<FString> PathsOf(const TMap<FString, FSongMetadata>& SongsByPath)
{
	TArray<FString> Paths;
	Paths.Reserve(SongsByPath.Num());
	for (const TPair<FString, FSongMetadata>& Current : SongsByPath)
	{
		Paths.Add(Current.Key);
	}
	return Paths;
}

bool AreLabelsDistinct(const TArray<FString>& Labels)
{
	TSet<FString> DistinctLabels;
	for (const FString& Label : Labels)
	{
		DistinctLabels.Add(Label);
	}
	return DistinctLabels.Num() == Labels.Num();
}
}

TArray<FString> MakeSongPathOptionLabels(const TMap<FString, FSongMetadata>& SongsByPath)
{
	const auto Entries = ToEntries(SongsByPath);
	TArray<FString> OptionLabels;
	OptionLabels.SetNum(Entries.Num());

	TMap<FString, TArray<int32>> TitleToIndices;
	for (int32 Index = 0; Index < Entries.Num(); Index++)
	{
		TitleToIndices.FindOrAdd(Entries[Index].Value.Title).Add(Index);
	}

	for (const TPair<FString, TArray<int32>>& TitleToIndicesPair : TitleToIndices)
	{
		const TArray<int32>& GroupIndices = TitleToIndicesPair.Value;
		TArray<FSongMetadata> TitleGroup;
		for (const int32 Index : GroupIndices)
		{
			TitleGroup.Add(Entries[Index].Value);
		}
		const FSongOptionLabelSpec LabelSpec = ComputeSongOptionLabelSpec(TitleGroup);

		TArray<FString> GroupLabels;
		GroupLabels.Reserve(GroupIndices.Num());
		for (const int32 Index : GroupIndices)
		{
			GroupLabels.Add(
				MakeSongOptionLabel(Entries[Index].Value, LabelSpec.FieldCount, LabelSpec.bUseFieldPrefixes));
		}
		if (!AreLabelsDistinct(GroupLabels))
		{
			// Fully duplicated metadata under different paths: disambiguate by the shorter identifier first.
			for (int32 GroupIndex = 0; GroupIndex < GroupIndices.Num(); GroupIndex++)
			{
				GroupLabels[GroupIndex] += TEXT(" (") + FPaths::GetBaseFilename(Entries[GroupIndices[GroupIndex]].Key) +
					TEXT(")");
			}
		}
		if (!AreLabelsDistinct(GroupLabels))
		{
			// Same base filename too: the full path is the only remaining disambiguator, and it is unique.
			for (int32 GroupIndex = 0; GroupIndex < GroupIndices.Num(); GroupIndex++)
			{
				GroupLabels[GroupIndex] += TEXT(" (") + Entries[GroupIndices[GroupIndex]].Key + TEXT(")");
			}
		}
		for (int32 GroupIndex = 0; GroupIndex < GroupIndices.Num(); GroupIndex++)
		{
			OptionLabels[GroupIndices[GroupIndex]] = GroupLabels[GroupIndex];
		}
	}
	return OptionLabels;
}

FString GetSongPathOptionLabel(const TMap<FString, FSongMetadata>& SongsByPath, const FString& SongFilePath)
{
	const TArray<FString> OptionLabels = MakeSongPathOptionLabels(SongsByPath);
	const TArray<FString> Paths = PathsOf(SongsByPath);
	const int32 Index = Paths.Find(SongFilePath);
	return Index == INDEX_NONE ? FString() : OptionLabels[Index];
}

FString FindSongPathByOptionLabel(const TMap<FString, FSongMetadata>& SongsByPath, const FString& OptionLabel)
{
	const TArray<FString> OptionLabels = MakeSongPathOptionLabels(SongsByPath);
	const TArray<FString> Paths = PathsOf(SongsByPath);
	const int32 Index = OptionLabels.Find(OptionLabel);
	return Index == INDEX_NONE ? FString() : Paths[Index];
}
