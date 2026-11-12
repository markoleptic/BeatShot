// Copyright 2022-2026 Markoleptic Games, SP. All Rights Reserved.

#include "CoreMinimal.h"
#include "BSGameModeConfig/AudioConfig.h"
#include "Misc/AutomationTest.h"
#include "BSTestAssertions.h"
#include "PlayerSettings/PlayerSettings_User.h"
#include "Utilities/SongMetadata.h"

namespace
{
FSongMetadata MakeSong(const TCHAR* InTitle, float InLength = 100.f)
{
	FSongMetadata Out;
	Out.Title = InTitle;
	Out.Length = InLength;
	return Out;
}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPlayerSettingsUserAddSongMetadataTest,
                                 TEXT("BeatShot.PlayerSettingsUser.AddSongMetadata"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)

bool FPlayerSettingsUserAddSongMetadataTest::RunTest(const FString& Parameters)
{
	FPlayerSettings_User Settings;
	Settings.AddSongMetadata(TEXT("/a.mp3"), MakeSong(TEXT("Alpha"), 100.f));

	BS_TEST_REQUIRE("one map entry", Settings.FilePathsAndSongMetadata.Num() == 1);
	BS_TEST_REQUIRE("one recent entry", Settings.RecentFilePaths.Num() == 1);
	BS_TEST_TRUE("recent path set", Settings.RecentFilePaths[0] == TEXT("/a.mp3"));
	BS_TEST_TRUE("metadata stored", Settings.FilePathsAndSongMetadata[TEXT("/a.mp3")] == MakeSong(TEXT("Alpha"), 100.f));

	// Re-adding the same path must not duplicate the recency entry.
	Settings.AddSongMetadata(TEXT("/a.mp3"), MakeSong(TEXT("Alpha"), 300.f));
	BS_TEST_EQUAL("re-add: recency stays single", Settings.RecentFilePaths.Num(), 1);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPlayerSettingsUserRemoveSongMetadataTest,
                                 TEXT("BeatShot.PlayerSettingsUser.RemoveSongMetadata"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)

bool FPlayerSettingsUserRemoveSongMetadataTest::RunTest(const FString& Parameters)
{
	{
		// Remove the last-used path: recency bumps to the *remaining* entry (most recent = list .Last()).
		FPlayerSettings_User Settings;
		Settings.AddSongMetadata(TEXT("/a.mp3"), MakeSong(TEXT("Alpha")));
		Settings.AddSongMetadata(TEXT("/b.mp3"), MakeSong(TEXT("Bravo")));
		Settings.LastUsedSongFilePath = TEXT("/a.mp3");
		Settings.RemoveSongMetadata(TEXT("/a.mp3"));

		BS_TEST_TRUE("gone from map", !Settings.FilePathsAndSongMetadata.Contains(TEXT("/a.mp3")));
		BS_TEST_TRUE("gone from recency", !Settings.RecentFilePaths.Contains(TEXT("/a.mp3")));
		BS_TEST_EQUAL("one entry left", Settings.RecentFilePaths.Num(), 1);
		BS_TEST_TRUE("last-used bumps", Settings.LastUsedSongFilePath == TEXT("/b.mp3"));
	}
	{
		// Removing the only entry clears the last-used path.
		FPlayerSettings_User Settings;
		Settings.AddSongMetadata(TEXT("/a.mp3"), MakeSong(TEXT("Alpha")));
		Settings.LastUsedSongFilePath = TEXT("/a.mp3");
		Settings.RemoveSongMetadata(TEXT("/a.mp3"));
		BS_TEST_TRUE("last-used clears", Settings.LastUsedSongFilePath.IsEmpty());
	}
	{
		// Removing a non-last-used path leaves LastUsedSongFilePath alone.
		FPlayerSettings_User Settings;
		Settings.AddSongMetadata(TEXT("/a.mp3"), MakeSong(TEXT("Alpha")));
		Settings.AddSongMetadata(TEXT("/b.mp3"), MakeSong(TEXT("Bravo")));
		Settings.LastUsedSongFilePath = TEXT("/a.mp3");
		Settings.RemoveSongMetadata(TEXT("/b.mp3"));
		BS_TEST_TRUE("last-used unchanged", Settings.LastUsedSongFilePath == TEXT("/a.mp3"));
	}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPlayerSettingsUserEditSongMetadataTest,
                                 TEXT("BeatShot.PlayerSettingsUser.EditSongMetadata"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)

bool FPlayerSettingsUserEditSongMetadataTest::RunTest(const FString& Parameters)
{
	FPlayerSettings_User Settings;
	Settings.AddSongMetadata(TEXT("/a.mp3"), MakeSong(TEXT("Alpha"), 100.f));

	// Existing path: replace in place, no recency-list change.
	Settings.EditSongMetadata(TEXT("/a.mp3"), MakeSong(TEXT("Alpha2"), 200.f));
	BS_TEST_EQUAL("recency unchanged", Settings.RecentFilePaths.Num(), 1);
	BS_TEST_TRUE("title updated", Settings.FilePathsAndSongMetadata[TEXT("/a.mp3")].Title == TEXT("Alpha2"));

	// Unknown path: goes through Add -> appears in recency.
	Settings.EditSongMetadata(TEXT("/c.mp3"), MakeSong(TEXT("Charlie")));
	BS_TEST_TRUE("new path in recency", Settings.RecentFilePaths.Contains(TEXT("/c.mp3")));
	BS_TEST_TRUE("new path in map", Settings.FilePathsAndSongMetadata.Contains(TEXT("/c.mp3")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPlayerSettingsUserAddAudioDeviceSongMetadataTest,
                                 TEXT("BeatShot.PlayerSettingsUser.AddAudioDeviceSongMetadata"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)

bool FPlayerSettingsUserAddAudioDeviceSongMetadataTest::RunTest(const FString& Parameters)
{
	FPlayerSettings_User Settings;
	BS_TEST_TRUE("first add succeeds", Settings.AddAudioDeviceSongMetadata(MakeSong(TEXT("Xray"), 100.f)));
	BS_TEST_EQUAL("one entry", Settings.RecentAudioDeviceSongMetadata.Num(), 1);

	// Identity is IsEqualExcludingLength — same identity with a different Length is a duplicate.
	BS_TEST_FALSE("identity-dup rejected (len differs)",
	            Settings.AddAudioDeviceSongMetadata(MakeSong(TEXT("Xray"), 999.f)));
	BS_TEST_EQUAL("still one entry", Settings.RecentAudioDeviceSongMetadata.Num(), 1);

	BS_TEST_TRUE("distinct identity allowed", Settings.AddAudioDeviceSongMetadata(MakeSong(TEXT("Yankee"))));
	BS_TEST_EQUAL("two entries", Settings.RecentAudioDeviceSongMetadata.Num(), 2);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPlayerSettingsUserRemoveAudioDeviceSongMetadataTest,
                                 TEXT("BeatShot.PlayerSettingsUser.RemoveAudioDeviceSongMetadata"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)

bool FPlayerSettingsUserRemoveAudioDeviceSongMetadataTest::RunTest(const FString& Parameters)
{
	FPlayerSettings_User Settings;
	Settings.RecentAudioDeviceSongMetadata.Add(MakeSong(TEXT("Xray"), 100.f));
	Settings.RecentAudioDeviceSongMetadata.Add(MakeSong(TEXT("Yankee"), 100.f));
	Settings.LastUsedAudioDeviceSongMetadata = MakeSong(TEXT("Xray"), 100.f);

	// Length is user-editable, so the key is IsEqualExcludingLength, not full equality.
	Settings.RemoveAudioDeviceSongMetadata(MakeSong(TEXT("Xray"), 777.f));

	BS_TEST_REQUIRE("removed one", Settings.RecentAudioDeviceSongMetadata.Num() == 1);
	BS_TEST_TRUE("X gone, Y left", Settings.RecentAudioDeviceSongMetadata[0].Title == TEXT("Yankee"));
	BS_TEST_TRUE("last-used bumps", Settings.LastUsedAudioDeviceSongMetadata.Title == TEXT("Yankee"));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPlayerSettingsUserEditAudioDeviceSongMetadataTest,
                                 TEXT("BeatShot.PlayerSettingsUser.EditAudioDeviceSongMetadata"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)

bool FPlayerSettingsUserEditAudioDeviceSongMetadataTest::RunTest(const FString& Parameters)
{
	// Success: re-identify an existing entry to a distinct identity (new genre); other entry untouched.
	{
		FPlayerSettings_User Settings;
		Settings.RecentAudioDeviceSongMetadata.Add(MakeSong(TEXT("Alpha"), 100.f));
		Settings.RecentAudioDeviceSongMetadata.Add(MakeSong(TEXT("Bravo"), 100.f));

		const FSongMetadata Original = MakeSong(TEXT("Alpha"), 100.f);
		FSongMetadata Edited = MakeSong(TEXT("Alpha"), 200.f);
		Edited.Genre = TEXT("Rock");

		BS_TEST_TRUE("success", Settings.EditAudioDeviceSongMetadata(Original, Edited));
		BS_TEST_REQUIRE("count unchanged", Settings.RecentAudioDeviceSongMetadata.Num() == 2);
		BS_TEST_TRUE("first entry replaced", Settings.RecentAudioDeviceSongMetadata[0] == Edited);
		BS_TEST_TRUE("second entry unchanged", Settings.RecentAudioDeviceSongMetadata[1].Title == TEXT("Bravo"));
	}
	// Reject: proposed identity collides with a different entry in the list.
	{
		FPlayerSettings_User Settings;
		Settings.RecentAudioDeviceSongMetadata.Add(MakeSong(TEXT("Alpha"), 100.f));
		Settings.RecentAudioDeviceSongMetadata.Add(MakeSong(TEXT("Bravo"), 100.f));
		BS_TEST_FALSE("reject when proposed collides with existing",
		            Settings.EditAudioDeviceSongMetadata(MakeSong(TEXT("Alpha")), MakeSong(TEXT("Bravo"))));
		BS_TEST_EQUAL("list unchanged (collide)", Settings.RecentAudioDeviceSongMetadata.Num(), 2);
	}
	// Reject: two existing entries share the same identity as `Existing` (ambiguous target).
	{
		FPlayerSettings_User Settings;
		Settings.RecentAudioDeviceSongMetadata.Add(MakeSong(TEXT("Alpha"), 100.f));
		Settings.RecentAudioDeviceSongMetadata.Add(MakeSong(TEXT("Alpha"), 200.f));
		BS_TEST_FALSE("reject when 2+ existing match",
		            Settings.EditAudioDeviceSongMetadata(MakeSong(TEXT("Alpha")), MakeSong(TEXT("Charlie"))));
		BS_TEST_EQUAL("list unchanged (2-match)", Settings.RecentAudioDeviceSongMetadata.Num(), 2);
	}
	// NOTE: the "Existing matches 0 entries" branch runs `check(NumMatchingExisting >= 1)` —
	// a contract violation, not a testable path. Not exercised here.

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPlayerSettingsUserMarkFileSongUsedTest,
                                 TEXT("BeatShot.PlayerSettingsUser.MarkFileSongUsed"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)

bool FPlayerSettingsUserMarkFileSongUsedTest::RunTest(const FString& Parameters)
{
	FPlayerSettings_User Settings;
	Settings.AddSongMetadata(TEXT("/a.mp3"), MakeSong(TEXT("Alpha")));
	Settings.AddSongMetadata(TEXT("/b.mp3"), MakeSong(TEXT("Bravo")));

	FBS_AudioConfig Cfg;
	Cfg.SongFilePath = TEXT("/a.mp3");
	Settings.MarkFileSongUsed(Cfg);

	BS_TEST_REQUIRE("recency num preserved", Settings.RecentFilePaths.Num() == 2);
	BS_TEST_TRUE("path moves to most-recent", Settings.RecentFilePaths.Last() == TEXT("/a.mp3"));
	BS_TEST_TRUE("last-used path", Settings.LastUsedSongFilePath == TEXT("/a.mp3"));
	BS_TEST_TRUE("audio format set to File", Settings.LastUsedAudioFormat == EAudioFormat::File);

	// A new path lands at the most-recent position.
	FBS_AudioConfig Cfg2;
	Cfg2.SongFilePath = TEXT("/c.mp3");
	Settings.MarkFileSongUsed(Cfg2);
	BS_TEST_TRUE("new path most-recent", Settings.RecentFilePaths.Last() == TEXT("/c.mp3"));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPlayerSettingsUserMarkCaptureSongUsedTest,
                                 TEXT("BeatShot.PlayerSettingsUser.MarkCaptureSongUsed"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)

bool FPlayerSettingsUserMarkCaptureSongUsedTest::RunTest(const FString& Parameters)
{
	FPlayerSettings_User Settings;
	Settings.RecentAudioDeviceSongMetadata.Add(MakeSong(TEXT("Xray"), 100.f));

	FBS_AudioConfig Cfg;
	Cfg.InAudioDevice = TEXT("device0");
	Cfg.bPlaybackAudio = false;
	Cfg.SongMetadata = MakeSong(TEXT("Xray"), 200.f); // same identity as existing, new length
	Settings.MarkCaptureSongUsed(Cfg);

	BS_TEST_REQUIRE("identity-dup collapsed to one", Settings.RecentAudioDeviceSongMetadata.Num() == 1);
	BS_TEST_TRUE("length updated to proposed",
	           FMath::IsNearlyEqual(Settings.RecentAudioDeviceSongMetadata[0].Length, 200.f));
	BS_TEST_TRUE("last-used matches new", Settings.LastUsedAudioDeviceSongMetadata == MakeSong(TEXT("Xray"), 200.f));
	BS_TEST_TRUE("device recorded", Settings.LastUsedInputAudioDevice == TEXT("device0"));
	BS_TEST_FALSE("playback flag is off", Settings.bPlaybackAudioDuringCapture);
	BS_TEST_TRUE("audio format set to Capture", Settings.LastUsedAudioFormat == EAudioFormat::Capture);

	return true;
}
