// Copyright 2022-2026 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Fonts/SlateFontInfo.h"
#include "Styling/SlateBrush.h"
#include "PatchNotesStyle.generated.h"

/** Style shared by all patch-notes widget instances: fonts, icon brushes and paddings. */
USTRUCT(BlueprintType)
struct USERINTERFACE_API FPatchNotesStyle
{
	GENERATED_USTRUCT_BODY()

	/** Font used for the version text at the left of the header. */
	UPROPERTY(EditAnywhere, Category = "Fonts")
	FSlateFontInfo Font_Version;

	/** Font used for the date text at the right of the header. */
	UPROPERTY(EditAnywhere, Category = "Fonts")
	FSlateFontInfo Font_Date;

	/** Font used for each section title. */
	UPROPERTY(EditAnywhere, Category = "Fonts")
	FSlateFontInfo Font_SectionTitle;

	/** Font used for the bullet and sub-bullet text. */
	UPROPERTY(EditAnywhere, Category = "Fonts")
	FSlateFontInfo Font_Bullet;

	/** Brush shown to the left of each section title; its Image Size controls how large it renders. */
	UPROPERTY(EditAnywhere, Category = "Icons")
	FSlateBrush SectionTitleIcon;

	/** Brush shown to the left of each bullet and sub-bullet; its Image Size controls how large it renders. */
	UPROPERTY(EditAnywhere, Category = "Icons")
	FSlateBrush BulletIcon;

	/** Padding applied above and below each section. */
	UPROPERTY(EditAnywhere, Category = "Layout", meta = (ClampMin = "0"))
	float SectionPadding = 10.f;

	/** Padding applied above each bullet row. */
	UPROPERTY(EditAnywhere, Category = "Layout", meta = (ClampMin = "0"))
	float BulletPaddingTop = 10.f;

	/** Horizontal indentation per bullet depth (bullets at 1x, sub-bullets at 2x). */
	UPROPERTY(EditAnywhere, Category = "Layout", meta = (ClampMin = "0"))
	float IndentPerLevel = 50.f;

	/** Gap between an icon and its text. */
	UPROPERTY(EditAnywhere, Category = "Layout", meta = (ClampMin = "0"))
	float IconTextSpacing = 10.f;

	/** Padding between the right side of the rows and the edge of the widget. */
	UPROPERTY(EditAnywhere, Category = "Layout", meta = (ClampMin = "0"))
	float RightEdgePadding = 10.f;
};

/** Data asset carrying the shared patch-notes style; point widget instances at one shared copy. */
UCLASS(Blueprintable, BlueprintType)
class USERINTERFACE_API UPatchNotesWidgetStyle : public UDataAsset
{
	GENERATED_BODY()

public:
	/** The style values this asset carries. */
	UPROPERTY(EditAnywhere, Category = "Patch Notes", meta = (ShowOnlyInnerProperties))
	FPatchNotesStyle Values;
};
