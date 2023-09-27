// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "WidgetComponents/Tooltips/TooltipImage.h"
#include "MenuOptionStyle.generated.h"

/** Defines styles used in MenuOptionWidgets */
UCLASS(Abstract, Blueprintable, BlueprintType)
class USERINTERFACE_API UMenuOptionStyle : public UObject
{
	GENERATED_BODY()

public:
	/** The amount to indent the widget to the left */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MenuOptionStyle")
	float IndentAmount = 50.f;

	/** The padding applied to the left horizontal box inside the left border */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MenuOptionStyle")
	FMargin Padding_LeftHorizontalBox = FMargin(10.f, 5.f);

	/** The padding applied to the box containing the GameModeCategoryTags and TooltipImages */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MenuOptionStyle")
	FMargin Padding_TagsAndTooltips = FMargin(10.f, 5.f);

	/** The font applied to the DescriptionText. Also applied to ComboBoxes */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MenuOptionStyle|DescriptionText")
	FSlateFontInfo Font_DescriptionText;

	/** The padding applied to the DescriptionText */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MenuOptionStyle|DescriptionText")
	FMargin Padding_DescriptionText = FMargin(0.f, 4.f);

	/** The padding applied to the EditableTextBoxes */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MenuOptionStyle|EditableTextBox")
	FSlateFontInfo Font_EditableText;

	/** The font applied to the EditableTextBoxes beside a slider */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MenuOptionStyle|SliderTextBoxWidget")
	FSlateFontInfo Font_EditableTextBesideSlider;

	/** The class used for Tooltip Warnings */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MenuOptionStyle|TooltipWarning")
	TSubclassOf<UTooltipImage> TooltipWarningImageClass;

	/** The class used for Tooltip Cautions */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MenuOptionStyle|TooltipWarning")
	TSubclassOf<UTooltipImage> TooltipCautionImageClass;

	/** The padding used when placing a new Tooltip Warning/Caution */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MenuOptionStyle|TooltipWarning")
	FMargin Padding_TooltipWarning = FMargin(10.f, 0.f, 0.f, 0.f);

	/** The padding used when placing a new GameModeCategoryTagWidget */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MenuOptionStyle|GameModeCategoryTags")
	FMargin Padding_TagWidget = FMargin(-5.f, 0.f, 0.f, 0.f);

	/** The vertical alignment used when placing a new GameModeCategoryTagWidget */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MenuOptionStyle|GameModeCategoryTags")
	TEnumAsByte<EVerticalAlignment>  VerticalAlignment_TagWidget = VAlign_Center;

	/** The horizontal alignment used when placing a new GameModeCategoryTagWidget */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MenuOptionStyle|GameModeCategoryTags")
	TEnumAsByte<EHorizontalAlignment> HorizontalAlignment_TagWidget = HAlign_Right;
};
