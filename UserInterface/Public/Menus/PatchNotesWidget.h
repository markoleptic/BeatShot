// Copyright 2022-2026 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Internationalization/Text.h"
#include "PatchNotesWidget.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogPatchNotes, Log, All);


class UPatchNotesWidgetStyle;
class UStringTable;
class UVerticalBox;
class UWidgetTree;
struct FPatchNotesStyle;

/**
 * Renders one release's patch notes entirely from the string-table entries that share a key prefix.
 * Each instance supplies its own PatchKey / PatchVersionText and points StyleAsset at the shared
 * UPatchNotesWidgetStyle asset (fonts, icon brushes, paddings); a null StyleAsset falls back to
 * the built-in FPatchNotesStyle defaults.
 * Keys:
 * - <Prefix>_<Section> = section title
 * - <Prefix>_<Section>_<Bullet> = bullet
 * - <Prefix>_<Section>_<Bullet>_<Sub> = sub-bullet
 * - <Prefix>_Date = release date in the header
 */
UCLASS()
class USERINTERFACE_API UPatchNotesWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** One bullet row and its sub-bullets, in display order. */
	struct FPatchNotesBullet
	{
		FText Text;
		TArray<FText> SubBulletTexts;
	};

	/** One section title and all of its bullets, in display order. */
	struct FPatchNotesSection
	{
		FText Title;
		TArray<FPatchNotesBullet> Bullets;
	};

	/** Prefix identifying this release's entries in the string table, e.g. "080". */
	UPROPERTY(EditInstanceOnly, Category = "Patch Notes")
	FString PatchKey;

	/** Version string shown at the left of the header, e.g. "0.8.0". */
	UPROPERTY(EditInstanceOnly, Category = "Patch Notes")
	FString PatchVersionText;

	/** Path to the UStringTable that holds the patch note entries. */
	UPROPERTY(EditInstanceOnly, Category = "Patch Notes")
	FString StringTablePath = TEXT("/Game/StringTables/ST_PatchNotes.ST_PatchNotes");

	/** Shared style (fonts, icon brushes, paddings) every instance renders with. */
	UPROPERTY(EditInstanceOnly, Category = "Patch Notes|Style")
	TObjectPtr<UPatchNotesWidgetStyle> StyleAsset;

	/**
	 * Parses this release's entries out of the string table into sections with their bullets
	 * and sub-bullets, in display order. Keys that don't follow the numeric key convention are
	 * ignored.
	 */
	static void CollectPatchNotes(const UStringTable* StringTable,
	                              const FString& TableId,
	                              const FString& KeyPrefix,
	                              FText& OutDateText,
	                              TArray<FPatchNotesSection>& OutSections);

protected:
	/** Builds the widget tree from the string-table entries once the native (non-blueprint) data is initialized. */
	virtual void InitializeNativeClassData() override;

private:
	/** Adds the version/date header row to Root. */
	void AddHeader(UWidgetTree* Tree, UVerticalBox* Root, const FText& DateText, const FPatchNotesStyle& Style);

	/** Adds one section (title row plus its bullets and sub-bullets) to Root. */
	void AddSection(UWidgetTree* Tree,
	                UVerticalBox* Root,
	                const FPatchNotesSection& Section,
	                const FPatchNotesStyle& Style);

	/** Adds one bullet row at the given depth (1 = bullet, 2 = sub-bullet) to SectionBox. */
	void AddBulletRow(UWidgetTree* Tree,
	                  UVerticalBox* SectionBox,
	                  const FText& Text,
	                  int32 Depth,
	                  const FPatchNotesStyle& Style);
};
