// Copyright 2022-2026 Markoleptic Games, SP. All Rights Reserved.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "BSTestAssertions.h"
#include "Utilities/BSWidgetInterface.h"
#include "Utilities/SongMetadata.h"
#include "Utilities/SongOptionLabels.h"

namespace
{
FSongMetadata MakeSong(const TCHAR* InTitle)
{
	FSongMetadata Out;
	Out.Title = InTitle;
	Out.Length = 100.f;
	return Out;
}

/** Fully populated song; Length 150.5s -> "2:30". */
FSongMetadata MakeFullSong()
{
	FSongMetadata Out;
	Out.Title = TEXT("T");
	Out.Artist = TEXT("A");
	Out.Album = TEXT("Al");
	Out.Year = TEXT("2024");
	Out.Genre = TEXT("G");
	Out.Length = 150.5f;
	return Out;
}

FString GetPrefixText(const TCHAR* InKey)
{
	return IBSWidgetInterface::GetWidgetTextFromKey(InKey).ToString();
}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSongOptionLabelsMakeSongOptionLabelTest,
                                 TEXT("BeatShot.SongOptionLabels.MakeSongOptionLabel"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)

bool FSongOptionLabelsMakeSongOptionLabelTest::RunTest(const FString& Parameters)
{
	const FSongMetadata Full = MakeFullSong();

	// Clean format grows fields in the fixed order: Artist, Album, Year, Genre, Length.
	BS_TEST_EQUAL("1: title only", MakeSongOptionLabel(Full, 1, false), TEXT("T"));
	BS_TEST_EQUAL("2: + artist", MakeSongOptionLabel(Full, 2, false), TEXT("T — A"));
	BS_TEST_EQUAL("3: + album", MakeSongOptionLabel(Full, 3, false), TEXT("T — A (Al)"));
	BS_TEST_EQUAL("4: + year", MakeSongOptionLabel(Full, 4, false), TEXT("T — A (Al, 2024)"));
	BS_TEST_EQUAL("5: + genre", MakeSongOptionLabel(Full, 5, false), TEXT("T — A (Al, 2024, G)"));
	BS_TEST_EQUAL("6: + length", MakeSongOptionLabel(Full, 6, false), TEXT("T — A (Al, 2024, G, 2:30)"));

	// Empty fields are skipped without leaving a gap.
	FSongMetadata Gaps = Full;
	Gaps.Album.Empty();
	Gaps.Genre.Empty();
	BS_TEST_EQUAL("empty album+genre skipped", MakeSongOptionLabel(Gaps, 5, false), TEXT("T — A (2024)"));
	BS_TEST_EQUAL("length still last", MakeSongOptionLabel(Gaps, 6, false), TEXT("T — A (2024, 2:30)"));

	// Length floors to whole seconds; 0s -> "0:00".
	BS_TEST_EQUAL("fraction truncated", MakeSongOptionLabel(MakeSong(TEXT("S")), 6, false), TEXT("S (1:40)"));
	FSongMetadata UnderAMinute = MakeSong(TEXT("U"));
	UnderAMinute.Length = 59.9f;
	BS_TEST_EQUAL("59.9s -> 0:59", MakeSongOptionLabel(UnderAMinute, 6, false), TEXT("U (0:59)"));
	FSongMetadata Zero = MakeSong(TEXT("Z"));
	Zero.Length = 0.f;
	BS_TEST_EQUAL("0s -> 0:00", MakeSongOptionLabel(Zero, 6, false), TEXT("Z (0:00)"));

	// Prefixed format: localized prefixes come from the shared string table.
	const FString AlbumText = GetPrefixText(TEXT("AF_SongAlbum"));
	const FString YearText = GetPrefixText(TEXT("AF_SongYear"));
	BS_TEST_TRUE("album prefix resolves", !AlbumText.IsEmpty());
	BS_TEST_TRUE("year prefix resolves", !YearText.IsEmpty());
	BS_TEST_TRUE("prefixes are distinct", AlbumText != YearText);

	FSongMetadata Prefixed = MakeSong(TEXT("T"));
	Prefixed.Artist = TEXT("A");
	Prefixed.Album = TEXT("Al");
	Prefixed.Year = TEXT("2024");
	const FString ExpectedPrefixed = FString::Printf(TEXT("T — A (%s Al, %s 2024)"), *AlbumText, *YearText);
	BS_TEST_EQUAL("prefixed album + year", MakeSongOptionLabel(Prefixed, 4, true), ExpectedPrefixed);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSongOptionLabelsComputeSongOptionLabelSpecTest,
                                 TEXT("BeatShot.SongOptionLabels.ComputeSongOptionLabelSpec"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)

bool FSongOptionLabelsComputeSongOptionLabelSpecTest::RunTest(const FString& Parameters)
{
	{
		TArray<FSongMetadata> Group;
		Group.Add(MakeSong(TEXT("T")));
		const FSongOptionLabelSpec Spec = ComputeSongOptionLabelSpec(Group);
		BS_TEST_EQUAL("singleton: field count", Spec.FieldCount, 1);
		BS_TEST_TRUE("singleton: no prefixes", !Spec.bUseFieldPrefixes);
	}
	{
		TArray<FSongMetadata> Group = {MakeSong(TEXT("T")), MakeSong(TEXT("T"))};
		Group[0].Artist = TEXT("A2");
		Group[1].Artist = TEXT("A1");
		const FSongOptionLabelSpec Spec = ComputeSongOptionLabelSpec(Group);
		BS_TEST_EQUAL("distinct artist: field count", Spec.FieldCount, 2);
		BS_TEST_TRUE("distinct artist: no prefixes", !Spec.bUseFieldPrefixes);
	}
	{
		TArray<FSongMetadata> Group = {MakeSong(TEXT("T")), MakeSong(TEXT("T"))};
		for (FSongMetadata& Song : Group)
		{
			Song.Artist = TEXT("A");
		}
		Group[0].Album = TEXT("Al1");
		Group[1].Album = TEXT("Al2");
		const FSongOptionLabelSpec Spec = ComputeSongOptionLabelSpec(Group);
		BS_TEST_EQUAL("distinct album: field count", Spec.FieldCount, 3);
		BS_TEST_TRUE("distinct album: no prefixes", !Spec.bUseFieldPrefixes);
	}
	{
		TArray<FSongMetadata> Group = {MakeSong(TEXT("T")), MakeSong(TEXT("T"))};
		for (FSongMetadata& Song : Group)
		{
			Song.Artist = TEXT("A");
			Song.Album = TEXT("Al");
		}
		Group[0].Year = TEXT("2021");
		Group[1].Year = TEXT("2022");
		const FSongOptionLabelSpec Spec = ComputeSongOptionLabelSpec(Group);
		BS_TEST_EQUAL("distinct year: field count", Spec.FieldCount, 4);
		BS_TEST_TRUE("distinct year: no prefixes", !Spec.bUseFieldPrefixes);
	}
	{
		TArray<FSongMetadata> Group = {MakeSong(TEXT("T")), MakeSong(TEXT("T"))};
		for (FSongMetadata& Song : Group)
		{
			Song.Artist = TEXT("A");
			Song.Album = TEXT("Al");
			Song.Year = TEXT("2024");
		}
		Group[0].Genre = TEXT("G1");
		Group[1].Genre = TEXT("G2");
		const FSongOptionLabelSpec Spec = ComputeSongOptionLabelSpec(Group);
		BS_TEST_EQUAL("distinct genre: field count", Spec.FieldCount, 5);
		BS_TEST_TRUE("distinct genre: no prefixes", !Spec.bUseFieldPrefixes);
	}
	{
		// Same 5-tuple, only Length differs: clean format already distinguishes at max fields.
		TArray<FSongMetadata> Group;
		Group.Add(MakeFullSong());
		FSongMetadata Other = MakeFullSong();
		Other.Length = 200.f;
		Group.Add(Other);
		const FSongOptionLabelSpec Spec = ComputeSongOptionLabelSpec(Group);
		BS_TEST_EQUAL("distinct length: field count", Spec.FieldCount, 6);
		BS_TEST_TRUE("distinct length: no prefixes", !Spec.bUseFieldPrefixes);
	}
	{
		// Prefixes are only reachable when no clean field count can separate the group: each of the three songs
		// puts the same value in a different field, so the clean labels always collide; the prefixed ones come
		// apart as soon as both Album and Year are in scope.
		TArray<FSongMetadata> Group;
		FSongMetadata Song1 = MakeSong(TEXT("T"));
		Song1.Artist = TEXT("A");
		Song1.Album = TEXT("Al");
		FSongMetadata Song2 = MakeSong(TEXT("T"));
		Song2.Artist = TEXT("A");
		Song2.Year = TEXT("Al");
		FSongMetadata Song3 = MakeSong(TEXT("T"));
		Song3.Artist = TEXT("A");
		Song3.Genre = TEXT("Al");
		Group.Add(Song1);
		Group.Add(Song2);
		Group.Add(Song3);
		const FSongOptionLabelSpec Spec = ComputeSongOptionLabelSpec(Group);
		BS_TEST_EQUAL("prefixes needed: field count", Spec.FieldCount, 4);
		BS_TEST_TRUE("prefixes needed: uses prefixes", Spec.bUseFieldPrefixes);
	}
	{
		// Fully identical metadata cannot be distinguished at any level; the documented fallback is (6, true).
		TArray<FSongMetadata> Group;
		Group.Add(MakeFullSong());
		Group.Add(MakeFullSong());
		const FSongOptionLabelSpec Spec = ComputeSongOptionLabelSpec(Group);
		BS_TEST_EQUAL("fully duplicated: field count", Spec.FieldCount, 6);
		BS_TEST_TRUE("fully duplicated: uses prefixes", Spec.bUseFieldPrefixes);
	}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSongOptionLabelsMakeSongOptionLabelsTest,
                                 TEXT("BeatShot.SongOptionLabels.MakeSongOptionLabels"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)

bool FSongOptionLabelsMakeSongOptionLabelsTest::RunTest(const FString& Parameters)
{
	TArray<FSongMetadata> Entries = {MakeSong(TEXT("Solo")),};
	FSongMetadata DuoA = MakeSong(TEXT("Duo"));
	DuoA.Artist = TEXT("A1");
	FSongMetadata DuoB = MakeSong(TEXT("Duo"));
	DuoB.Artist = TEXT("A2");
	Entries.Add(DuoA);
	Entries.Add(DuoB);
	// Shared title group the clean format cannot separate (each song puts the same value in a different field):
	// labels fall back to the prefixed form.
	FSongMetadata PrefixedA = MakeSong(TEXT("Tri"));
	PrefixedA.Artist = TEXT("A");
	PrefixedA.Album = TEXT("Al");
	FSongMetadata PrefixedB = MakeSong(TEXT("Tri"));
	PrefixedB.Artist = TEXT("A");
	PrefixedB.Year = TEXT("Al");
	FSongMetadata PrefixedC = MakeSong(TEXT("Tri"));
	PrefixedC.Artist = TEXT("A");
	PrefixedC.Genre = TEXT("Al");
	Entries.Add(PrefixedA);
	Entries.Add(PrefixedB);
	Entries.Add(PrefixedC);

	const TArray<FString> Labels = MakeSongOptionLabels(Entries);
	BS_TEST_REQUIRE("one label per entry", Labels.Num() == Entries.Num());
	BS_TEST_EQUAL("singleton is bare title", Labels[0], TEXT("Solo"));
	BS_TEST_EQUAL("duo entry A", Labels[1], TEXT("Duo — A1"));
	BS_TEST_EQUAL("duo entry B", Labels[2], TEXT("Duo — A2"));
	BS_TEST_EQUAL("prefixed entry: album", Labels[3],
	            FString::Printf(TEXT("Tri — A (%s Al)"), *GetPrefixText(TEXT("AF_SongAlbum"))));
	BS_TEST_EQUAL("prefixed entry: year", Labels[4],
	            FString::Printf(TEXT("Tri — A (%s Al)"), *GetPrefixText(TEXT("AF_SongYear"))));
	// Genre sits beyond the prefix spec's field cap (4), so this entry stays plain.
	BS_TEST_EQUAL("prefixed entry: genre beyond field cap stays plain", Labels[5], TEXT("Tri — A"));

	TSet<FString> Distinct;
	for (const FString& Label : Labels)
	{
		Distinct.Add(Label);
	}
	BS_TEST_EQUAL("all labels unique", Distinct.Num(), Labels.Num());

	BS_TEST_EQUAL("empty list -> no labels", MakeSongOptionLabels(TArray<FSongMetadata>()).Num(), 0);

	{
		// Entry lists can contain fully duplicated songs (dedup is a caller concern); then labels simply collide.
		TArray<FSongMetadata> Duplicates;
		Duplicates.Add(MakeFullSong());
		Duplicates.Add(MakeFullSong());
		const TArray<FString> DuplicateLabels = MakeSongOptionLabels(Duplicates);
		BS_TEST_TRUE("duplicates share a label", DuplicateLabels.Num() == 2 && DuplicateLabels[0] == DuplicateLabels[1]);
	}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSongOptionLabelsFindSongByOptionLabelTest,
                                 TEXT("BeatShot.SongOptionLabels.FindSongByOptionLabel"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)

bool FSongOptionLabelsFindSongByOptionLabelTest::RunTest(const FString& Parameters)
{
	TArray<FSongMetadata> Entries = {MakeSong(TEXT("Solo")), MakeFullSong(), MakeFullSong()};
	const TArray<FString> Labels = MakeSongOptionLabels(Entries);

	BS_TEST_TRUE("solo resolves", FindSongByOptionLabel(Entries, Labels[0]) == &Entries[0]);
	// The two full songs are identical, so their labels collide; the first entry wins.
	BS_TEST_TRUE("duplicate label -> first entry", FindSongByOptionLabel(Entries, Labels[1]) == &Entries[1]);
	BS_TEST_TRUE("duplicate label resolves to first, not second",
	           FindSongByOptionLabel(Entries, Labels[2]) == &Entries[1]);
	BS_TEST_TRUE("no match -> null", FindSongByOptionLabel(Entries, TEXT("Nope")) == nullptr);
	BS_TEST_TRUE("empty list -> null", FindSongByOptionLabel(TArray<FSongMetadata>(), TEXT("Nope")) == nullptr);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSongOptionLabelsMakeSongPathOptionLabelsTest,
                                 TEXT("BeatShot.SongOptionLabels.MakeSongPathOptionLabels"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)

bool FSongOptionLabelsMakeSongPathOptionLabelsTest::RunTest(const FString& Parameters)
{
	TMap<FString, FSongMetadata> Map;
	Map.Add(TEXT("C:\\a\\one.mp3"), MakeSong(TEXT("Alpha")));
	FSongMetadata Shared1 = MakeSong(TEXT("Shared"));
	Shared1.Artist = TEXT("A1");
	Map.Add(TEXT("C:\\b\\shared1.mp3"), Shared1);
	FSongMetadata Shared2 = MakeSong(TEXT("Shared"));
	Shared2.Artist = TEXT("A2");
	Map.Add(TEXT("C:\\c\\shared2.mp3"), Shared2);

	const TArray<FString> Labels = MakeSongPathOptionLabels(Map);
	BS_TEST_EQUAL("one label per path", Labels.Num(), Map.Num());
	TSet<FString> Distinct;
	for (const FString& Label : Labels)
	{
		Distinct.Add(Label);
	}
	BS_TEST_EQUAL("labels unique", Distinct.Num(), 3);
	BS_TEST_TRUE("labels as expected",
	           Distinct.Contains(FString(TEXT("Alpha"))) && Distinct.Contains(FString(TEXT("Shared — A1"))) && Distinct.
	           Contains(FString(TEXT("Shared — A2"))));

	// Fully duplicated metadata under different paths: base filename (without extension) breaks the tie.
	const FSongMetadata Dup = MakeFullSong();
	TMap<FString, FSongMetadata> DupMap;
	DupMap.Add(TEXT("C:\\a\\one.mp3"), Dup);
	DupMap.Add(TEXT("C:\\b\\two.mp3"), Dup);
	const FString FullLabel = FString::Printf(
		TEXT("T — A (%s Al, %s 2024, %s G, %s 2:30)"), *GetPrefixText(TEXT("AF_SongAlbum")),
		*GetPrefixText(TEXT("AF_SongYear")), *GetPrefixText(TEXT("AF_SongGenre")),
		*GetPrefixText(TEXT("AF_SongLength")));
	BS_TEST_EQUAL("base filename disambiguates", GetSongPathOptionLabel(DupMap, TEXT("C:\\a\\one.mp3")),
	            FullLabel + TEXT(" (one)"));
	BS_TEST_TRUE("dup label resolves back to path",
	           FindSongPathByOptionLabel(DupMap, FullLabel + TEXT(" (two)")) == TEXT("C:\\b\\two.mp3"));

	// Same base filename too: full path is the final disambiguator.
	TMap<FString, FSongMetadata> SameBaseMap;
	SameBaseMap.Add(TEXT("C:\\dir1\\one.mp3"), Dup);
	SameBaseMap.Add(TEXT("C:\\dir2\\one.mp3"), Dup);
	const FString Dir1Label = FullLabel + TEXT(" (one) (C:\\dir1\\one.mp3)");
	const FString Dir2Label = FullLabel + TEXT(" (one) (C:\\dir2\\one.mp3)");
	BS_TEST_EQUAL("full path disambiguates", GetSongPathOptionLabel(SameBaseMap, TEXT("C:\\dir1\\one.mp3")), Dir1Label);
	BS_TEST_TRUE("same-base labels differ", Dir1Label != Dir2Label);
	BS_TEST_EQUAL("same-base uniqueness",
	            [&] { TSet<FString> Set; for (const FString& Label : MakeSongPathOptionLabels(SameBaseMap)) { Set.Add(
		            Label); } return Set.Num(); }(), 2);

	BS_TEST_EQUAL("empty map -> no labels", MakeSongPathOptionLabels(TMap<FString, FSongMetadata>()).Num(), 0);
	{
		const TMap<FString, FSongMetadata> Empty;
		BS_TEST_TRUE("empty map lookups are empty",
		           GetSongPathOptionLabel(Empty, TEXT("C:\\a\\one.mp3")).IsEmpty() && FindSongPathByOptionLabel(Empty,
			           TEXT("Alpha")).IsEmpty());
	}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSongOptionLabelsGetSongPathOptionLabelTest,
                                 TEXT("BeatShot.SongOptionLabels.GetSongPathOptionLabel"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)

bool FSongOptionLabelsGetSongPathOptionLabelTest::RunTest(const FString& Parameters)
{
	TMap<FString, FSongMetadata> Map;
	Map.Add(TEXT("C:\\a\\one.mp3"), MakeSong(TEXT("Alpha")));
	Map.Add(TEXT("C:\\b\\two.mp3"), MakeSong(TEXT("Beta")));
	FSongMetadata Shared1 = MakeSong(TEXT("Shared"));
	Shared1.Artist = TEXT("A1");
	Map.Add(TEXT("C:\\c\\shared1.mp3"), Shared1);
	FSongMetadata Shared2 = MakeSong(TEXT("Shared"));
	Shared2.Artist = TEXT("A2");
	Map.Add(TEXT("C:\\d\\shared2.mp3"), Shared2);

	BS_TEST_EQUAL("unique title label", GetSongPathOptionLabel(Map, TEXT("C:\\a\\one.mp3")), TEXT("Alpha"));
	BS_TEST_EQUAL("shared title label", GetSongPathOptionLabel(Map, TEXT("C:\\c\\shared1.mp3")), TEXT("Shared — A1"));
	BS_TEST_TRUE("unknown path -> empty", GetSongPathOptionLabel(Map, TEXT("C:\\nope\\none.mp3")).IsEmpty());

	// Round trip: every path's label maps back to exactly that path.
	for (const TPair<FString, FSongMetadata>& Entry : Map)
	{
		const FString Label = GetSongPathOptionLabel(Map, Entry.Key);
		BS_TEST_TRUE("round trip", FindSongPathByOptionLabel(Map, Label) == Entry.Key);
	}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSongOptionLabelsFindSongPathByOptionLabelTest,
                                 TEXT("BeatShot.SongOptionLabels.FindSongPathByOptionLabel"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)

bool FSongOptionLabelsFindSongPathByOptionLabelTest::RunTest(const FString& Parameters)
{
	TMap<FString, FSongMetadata> Map;
	Map.Add(TEXT("C:\\a\\one.mp3"), MakeSong(TEXT("Alpha")));
	Map.Add(TEXT("C:\\b\\two.mp3"), MakeSong(TEXT("Beta")));

	BS_TEST_EQUAL("label -> path", FindSongPathByOptionLabel(Map, TEXT("Alpha")), TEXT("C:\\a\\one.mp3"));
	BS_TEST_TRUE("unknown label -> empty", FindSongPathByOptionLabel(Map, TEXT("Nope")).IsEmpty());

	FSongMetadata Dup = MakeFullSong();
	TMap<FString, FSongMetadata> DupMap;
	DupMap.Add(TEXT("C:\\a\\one.mp3"), Dup);
	DupMap.Add(TEXT("C:\\b\\one.mp3"), Dup); // same base name
	const FString FullLabel = FString::Printf(
		TEXT("T — A (%s Al, %s 2024, %s G, %s 2:30)"), *GetPrefixText(TEXT("AF_SongAlbum")),
		*GetPrefixText(TEXT("AF_SongYear")), *GetPrefixText(TEXT("AF_SongGenre")),
		*GetPrefixText(TEXT("AF_SongLength")));
	BS_TEST_EQUAL("full-path label -> path",
	            FindSongPathByOptionLabel(DupMap, FullLabel + TEXT(" (one) (C:\\b\\one.mp3)")), TEXT("C:\\b\\one.mp3"));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSongOptionLabelsFindNeighborSongIndexTest,
                                 TEXT("BeatShot.SongOptionLabels.FindNeighborSongIndex"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)

bool FSongOptionLabelsFindNeighborSongIndexTest::RunTest(const FString& Parameters)
{
	TArray<FSongMetadata> Songs = {MakeSong(TEXT("A")), MakeSong(TEXT("B")), MakeSong(TEXT("C"))};
	BS_TEST_EQUAL("middle -> below", FindNeighborSongIndex(Songs, MakeSong(TEXT("B"))), 2);
	BS_TEST_EQUAL("last -> above", FindNeighborSongIndex(Songs, MakeSong(TEXT("C"))), 1);
	BS_TEST_EQUAL("first -> below", FindNeighborSongIndex(Songs, MakeSong(TEXT("A"))), 1);
	BS_TEST_EQUAL("not in list -> none", FindNeighborSongIndex(Songs, MakeSong(TEXT("Z"))), INDEX_NONE);

	TArray<FSongMetadata> Only = {MakeSong(TEXT("Only"))};
	BS_TEST_EQUAL("only entry -> none", FindNeighborSongIndex(Only, MakeSong(TEXT("Only"))), INDEX_NONE);
	BS_TEST_EQUAL("only other entry -> none", FindNeighborSongIndex(Only, MakeSong(TEXT("Nope"))), INDEX_NONE);

	// Neighbor is by label order, not array order.
	TArray<FSongMetadata> Shuffled = {MakeSong(TEXT("C")), MakeSong(TEXT("A")), MakeSong(TEXT("B"))};
	BS_TEST_EQUAL("shuffled: neighbor of A is B at index 2", FindNeighborSongIndex(Shuffled, MakeSong(TEXT("A"))), 2);
	TArray<FSongMetadata> Shuffled2 = {MakeSong(TEXT("A")), MakeSong(TEXT("C")), MakeSong(TEXT("B"))};
	BS_TEST_EQUAL("shuffled: neighbor of B (last) is C at index 1", FindNeighborSongIndex(Shuffled2, MakeSong(TEXT("B"))),
	            1);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSongOptionLabelsLabeledPathsOfTest, TEXT("BeatShot.SongOptionLabels.LabeledPathsOf"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)

bool FSongOptionLabelsLabeledPathsOfTest::RunTest(const FString& Parameters)
{
	TMap<FString, FSongMetadata> Map;
	Map.Add(TEXT("C:\\a\\one.mp3"), MakeSong(TEXT("Alpha")));
	Map.Add(TEXT("C:\\b\\two.mp3"), MakeSong(TEXT("Bravo")));
	Map.Add(TEXT("C:\\c\\three.mp3"), MakeSong(TEXT("Charlie")));

	const TArray<TPair<FString, FString>> LabeledPaths = LabeledPathsOf(Map);
	BS_TEST_EQUAL("one pair per path", LabeledPaths.Num(), Map.Num());
	TSet<FString> DistinctLabels;
	for (const TPair<FString, FString>& Pair : LabeledPaths)
	{
		DistinctLabels.Add(Pair.Key);
	}
	BS_TEST_EQUAL("labels unique", DistinctLabels.Num(), LabeledPaths.Num());

	// Every path appears paired with its own computed label (map order is unspecified).
	for (const TPair<FString, FSongMetadata>& Entry : Map)
	{
		const FString ExpectedLabel = GetSongPathOptionLabel(Map, Entry.Key);
		BS_TEST_TRUE("pair matches path+label",
		           LabeledPaths.ContainsByPredicate( [&Entry, &ExpectedLabel](const TPair<FString, FString>& Pair) {
			           return Pair.Value == Entry.Key && Pair.Key == ExpectedLabel; }));
	}

	BS_TEST_EQUAL("empty map -> empty", LabeledPathsOf(TMap<FString, FSongMetadata>()).Num(), 0);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSongOptionLabelsFindNeighborSongFilePathTest,
                                 TEXT("BeatShot.SongOptionLabels.FindNeighborSongFilePath"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)

bool FSongOptionLabelsFindNeighborSongFilePathTest::RunTest(const FString& Parameters)
{
	{
		TMap<FString, FSongMetadata> Remaining;
		const FString APath = TEXT("C:\\a\\a.mp3");
		FSongMetadata A = MakeSong(TEXT("X"));
		A.Artist = TEXT("A");
		Remaining.Add(APath, A);
		FSongMetadata Removed = MakeSong(TEXT("X"));
		Removed.Artist = TEXT("Z"); // sorts above "X — A"
		BS_TEST_EQUAL("single remaining is the neighbor",
		            FindNeighborSongFilePath(Remaining, TEXT("C:\\z\\z.mp3"), Removed), APath);
	}

	{
		// Context case: labels must be computed with the removed entry present, where it is the middle option.
		const FString APath = TEXT("C:\\a\\a.mp3");
		const FString BPath = TEXT("C:\\b\\b.mp3");
		const FString CPath = TEXT("C:\\c\\c.mp3");
		FSongMetadata A = MakeSong(TEXT("X"));
		A.Artist = TEXT("A");
		FSongMetadata B = MakeSong(TEXT("X"));
		B.Artist = TEXT("B");
		FSongMetadata C = MakeSong(TEXT("X"));
		C.Artist = TEXT("A");
		C.Album = TEXT("C-a"); // C labels "X — A (C-a)", sorting between A's "X — A" and B's "X — B"

		TMap<FString, FSongMetadata> Remaining;
		Remaining.Add(APath, A);
		Remaining.Add(BPath, B);
		BS_TEST_EQUAL("middle removed -> below (B)", FindNeighborSongFilePath(Remaining, CPath, C), BPath);

		TMap<FString, FSongMetadata> Remaining2;
		Remaining2.Add(APath, A);
		Remaining2.Add(CPath, C);
		BS_TEST_EQUAL("last removed -> above (C)", FindNeighborSongFilePath(Remaining2, BPath, B), CPath);
	}

	{
		TMap<FString, FSongMetadata> Empty;
		BS_TEST_TRUE("no remaining -> empty path",
		           FindNeighborSongFilePath(Empty, TEXT("C:\\z\\z.mp3"), MakeSong(TEXT("Z"))).IsEmpty());
	}

	{
		const FString PPath = TEXT("C:\\p\\p.mp3");
		const FString QPath = TEXT("C:\\q\\q.mp3");
		TMap<FString, FSongMetadata> Remaining;
		Remaining.Add(PPath, MakeSong(TEXT("Alpha")));
		Remaining.Add(QPath, MakeSong(TEXT("Beta")));
		BS_TEST_EQUAL("removed above both -> below (Beta)",
		            FindNeighborSongFilePath(Remaining, TEXT("C:\\r\\r.mp3"), MakeSong(TEXT("Gamma"))), QPath);
		BS_TEST_EQUAL("removed below both (title '0' sorts first) -> below (Alpha)",
		            FindNeighborSongFilePath(Remaining, TEXT("C:\\r\\r.mp3"), MakeSong(TEXT("0"))), PPath);
	}

	return true;
}
