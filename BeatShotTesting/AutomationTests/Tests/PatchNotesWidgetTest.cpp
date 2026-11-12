// Copyright 2022-2026 Markoleptic Games, SP. All Rights Reserved.

#include "CoreMinimal.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Engine/Texture2D.h"
#include "Internationalization/StringTable.h"
#include "Menus/PatchNotesWidget.h"
#include "Misc/AutomationTest.h"
#include "BSTestAssertions.h"
#include "Styles/PatchNotesStyle.h"

namespace
{
const TCHAR* PatchNotesTablePath = TEXT("/Game/StringTables/ST_PatchNotes.ST_PatchNotes");
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPatchNotesWidgetCollectPatchNotesTest,
                                 TEXT("BeatShot.PatchNotesWidget.CollectPatchNotes"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)

bool FPatchNotesWidgetCollectPatchNotesTest::RunTest(const FString& Parameters)
{
	const UStringTable* StringTable = LoadObject<UStringTable>(nullptr, PatchNotesTablePath);
	BS_TEST_REQUIRE("string table loads", StringTable != nullptr);

	FText DateText;
	TArray<UPatchNotesWidget::FPatchNotesSection> Sections;
	UPatchNotesWidget::CollectPatchNotes(StringTable, PatchNotesTablePath, TEXT("080_"), DateText, Sections);

	BS_TEST_REQUIRE("three sections", Sections.Num() == 3);
	BS_TEST_TRUE("release date parsed", DateText.ToString() == TEXT("September 20, 2026"));

	BS_TEST_TRUE("section 1 title", Sections[0].Title.ToString() == TEXT("In Game Performance Statistics"));
	BS_TEST_TRUE("section 2 title", Sections[1].Title.ToString() == TEXT("General"));
	BS_TEST_TRUE("section 3 title", Sections[2].Title.ToString() == TEXT("Bug Fixes"));

	BS_TEST_EQUAL("section 1 bullets", Sections[0].Bullets.Num(), 5);
	BS_TEST_REQUIRE("section 2 bullets", Sections[1].Bullets.Num() == 3);
	BS_TEST_EQUAL("section 3 bullets", Sections[2].Bullets.Num(), 2);

	// Only section 2's third bullet has a (single) sub-bullet.
	BS_TEST_REQUIRE("section 2 bullet 3 sub-bullets", Sections[1].Bullets[2].SubBulletTexts.Num() == 1);
	BS_TEST_TRUE("section 2 bullet 3 sub-bullet text",
	           Sections[1].Bullets[2].SubBulletTexts[0].ToString().StartsWith(TEXT("Existing custom game mode names")));
	for (int32 BulletIndex = 0; BulletIndex < 3; ++BulletIndex)
	{
		if (BulletIndex != 2)
		{
			BS_TEST_EQUAL(FString::Printf(TEXT("section 2 bullet %d sub-bullets"), BulletIndex + 1),
			            Sections[1].Bullets[BulletIndex].SubBulletTexts.Num(), 0);
		}
	}

	// Section 1 and 3 bullets have no sub-bullets.
	for (int32 BulletIndex = 0; BulletIndex < Sections[0].Bullets.Num(); ++BulletIndex)
	{
		BS_TEST_EQUAL(FString::Printf(TEXT("section 1 bullet %d sub-bullets"), BulletIndex + 1),
		            Sections[0].Bullets[BulletIndex].SubBulletTexts.Num(), 0);
	}
	for (int32 BulletIndex = 0; BulletIndex < Sections[2].Bullets.Num(); ++BulletIndex)
	{
		BS_TEST_EQUAL(FString::Printf(TEXT("section 3 bullet %d sub-bullets"), BulletIndex + 1),
		            Sections[2].Bullets[BulletIndex].SubBulletTexts.Num(), 0);
	}

	return true;
}


IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPatchNotesWidgetBuildTreeTest, TEXT("BeatShot.PatchNotesWidget.BuildTree"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)

bool FPatchNotesWidgetBuildTreeTest::RunTest(const FString& Parameters)
{
	UPatchNotesWidgetStyle* Style = NewObject<UPatchNotesWidgetStyle>();
	Style->Values.SectionTitleIcon.SetResourceObject(LoadObject<UTexture2D>(
		nullptr, TEXT("/Game/Assets/UserInterface/Textures/Icons/T_Crosshairs-Solid.T_Crosshairs-Solid")));
	Style->Values.BulletIcon.SetResourceObject(
		LoadObject<UTexture2D>(nullptr, TEXT("/Game/Assets/UserInterface/Textures/Icons/T_Play_Solid.T_Play_Solid")));

	UPatchNotesWidget* Widget = NewObject<UPatchNotesWidget>();
	Widget->StringTablePath = PatchNotesTablePath;
	Widget->PatchKey = TEXT("080");
	Widget->PatchVersionText = TEXT("0.8.0");
	Widget->StyleAsset = Style;
	Widget->Initialize();

	BS_TEST_REQUIRE("root widget exists", (Widget->GetRootWidget()) != nullptr);

	int32 TextBlockCount = 0;
	int32 IconCount = 0;
	bool FoundVersionText = false;
	bool FoundDateText = false;
	Widget->WidgetTree->ForEachWidgetAndDescendants([&](UWidget* InWidget)
	{
		if (const UTextBlock* TextBlock = Cast<UTextBlock>(InWidget))
		{
			++TextBlockCount;
			const FString Text = TextBlock->GetText().ToString();
			FoundVersionText = FoundVersionText || (Text == TEXT("0.8.0"));
			FoundDateText = FoundDateText || (Text == TEXT("September 20, 2026"));
		}
		else if (Cast<UImage>(InWidget))
		{
			++IconCount;
		}
	});

	// 2 (header) + 3 (section titles) + 11 (bullets + sub-bullets) = 16 text blocks,
	// 3 (section title icons) + 11 (bullet icons) = 14 icons.
	BS_TEST_EQUAL("16 text blocks", TextBlockCount, 16);
	BS_TEST_EQUAL("14 icons", IconCount, 14);
	BS_TEST_TRUE("version text present", FoundVersionText);
	BS_TEST_TRUE("date text present", FoundDateText);

	return true;
}
