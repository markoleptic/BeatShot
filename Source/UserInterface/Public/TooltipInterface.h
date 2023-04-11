// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TooltipInterface.generated.h"

class UButton;
class UTooltipImage;
class UTooltipWidget;
class UEditableTextBox;
class USlider;

USTRUCT(BlueprintType)
struct FTooltipData
{
	GENERATED_BODY()

	TSoftObjectPtr<UTooltipImage> TooltipImage;
	FText TooltipText;
	bool bAllowTextWrap;

	FTooltipData()
	{
		TooltipImage = nullptr;
		TooltipText = FText();
		bAllowTextWrap = false;
	}

	FTooltipData(const UTooltipImage* InTooltipImage, const FText& InTooltipText, const bool bInbAllowTextWrap = false)
	{
		TooltipImage = InTooltipImage;
		TooltipText = InTooltipText;
		bAllowTextWrap = bInbAllowTextWrap;
	}

	FTooltipData(const UTooltipImage* InTooltipImage)
	{
		TooltipImage = InTooltipImage;
		TooltipText = FText();
		bAllowTextWrap = false;
	}

	bool operator==(const FTooltipData& Other) const
	{
		if (TooltipImage == Other.TooltipImage)
		{
			return true;
		}
		return false;
	}
};

UINTERFACE()
class UTooltipInterface : public UInterface
{
	GENERATED_BODY()
};

class USERINTERFACE_API ITooltipInterface
{
	GENERATED_BODY()
	
public:

	/** Override this function and then call SetTooltipWidget */
	virtual UTooltipWidget* ConstructTooltipWidget() = 0;

	/** Returns TooltipWidget */
	UTooltipWidget* GetTooltipWidget() const;

	/** Returns TooltipData */
	TArray<FTooltipData>& GetTooltipData();

	/** Call this to set the tooltip widget constructed from ConstructTooltipWidget */
	void SetTooltipWidget(const UTooltipWidget* InTooltipWidget);

	/** Add tooltip text to a given TooltipImage */
	void AddToTooltipData(UTooltipImage* TooltipImage, const FText& TooltipText, const bool bInAllowTextWrap = false);

	/** Add tooltip text to a given TooltipImage */
	void AddToTooltipData(const FTooltipData& InToolTipData);

	/** Edit the tooltip text for a given TooltipImage */
	void EditTooltipText(const UTooltipImage* TooltipImage, const FText& TooltipText);
	
	/** All Tooltip Images are bound to this function */
	UFUNCTION()
	virtual void OnTooltipImageHovered(UTooltipImage* HoveredTooltipImage);
	
	TArray<FTooltipData> TooltipData;
	
	TSoftObjectPtr<UTooltipWidget> TooltipWidget;

	/** Clamps NewTextValue, updates associated Slider value while rounding to the GridSnapSize */
	float OnEditableTextBoxChanged(const FText& NewTextValue, UEditableTextBox* TextBoxToChange, USlider* SliderToChange, const float GridSnapSize, const float Min, const float Max) const;

	/** Updates associated TextBoxToChange with result of rounding to the GridSnapSize */
	float OnSliderChanged(const float NewValue, UEditableTextBox* TextBoxToChange, const float GridSnapSize) const;
};
