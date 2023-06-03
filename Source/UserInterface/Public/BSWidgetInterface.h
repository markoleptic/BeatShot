// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WidgetComponents/TooltipImage.h"
#include "BSWidgetInterface.generated.h"

class UButton;
class UTooltipImage;
class UTooltipWidget;
class UEditableTextBox;
class USlider;

/** Contains data for the tooltip of a widget */
USTRUCT(BlueprintType)
struct FTooltipData
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<UTooltipImage> TooltipImage;
	FText TooltipText;
	bool bAllowTextWrap;
	FGuid Guid;
	
	FTooltipData()
	{
		TooltipImage = nullptr;
		TooltipText = FText();
		bAllowTextWrap = false;
		Guid = FGuid();
	}

	FTooltipData(UTooltipImage* InTooltipImage, const FText& InTooltipText, const bool bInbAllowTextWrap = false)
	{
		TooltipImage = InTooltipImage;
		Guid = InTooltipImage->Guid;
		TooltipText = InTooltipText;
		bAllowTextWrap = bInbAllowTextWrap;
	}

	explicit FTooltipData(const FGuid InGuid)
	{
		TooltipImage = nullptr;
		TooltipText = FText();
		bAllowTextWrap = false;
		Guid = InGuid;
	}

	bool operator==(const FTooltipData& Other) const
	{
		if (Guid == Other.Guid)
		{
			return true;
		}
		return false;
	}
};

/** Enum representing a setting button */
UENUM(BlueprintType)
enum class ESettingButtonType : uint8
{
	None UMETA(DisplayName="None"),
	Save UMETA(DisplayName="Save"),
	Reset UMETA(DisplayName="Reset"),
	Revert UMETA(DisplayName="Reset"),
	SaveAndRestart UMETA(DisplayName="Save And Restart"),
};
ENUM_RANGE_BY_FIRST_AND_LAST(ESettingButtonType, ESettingButtonType::Save, ESettingButtonType::SaveAndRestart);

/** Interface for commonly used objects and functions such as adding tooltips and syncing Sliders & TextBoxes */
UINTERFACE()
class UBSWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

/** Interface for commonly used objects and functions such as adding tooltips and syncing Sliders & TextBoxes */
class USERINTERFACE_API IBSWidgetInterface
{
	GENERATED_BODY()
	
public:

	/** Clamps NewTextValue, updates associated Slider value while rounding to the GridSnapSize */
	static float OnEditableTextBoxChanged(const FText& NewTextValue, UEditableTextBox* TextBoxToChange, USlider* SliderToChange, const float GridSnapSize, const float Min, const float Max);

	/** Updates associated TextBoxToChange with result of rounding to the GridSnapSize */
	static float OnSliderChanged(const float NewValue, UEditableTextBox* TextBoxToChange, const float GridSnapSize);

	/** Override this function and then call SetTooltipWidget */
	virtual UTooltipWidget* ConstructTooltipWidget() = 0;

	/** Returns TooltipWidget */
	UTooltipWidget* GetTooltipWidget() const;

	/** Call this to set the tooltip widget constructed from ConstructTooltipWidget */
	void SetTooltipWidget(UTooltipWidget* InTooltipWidget);

	/** Add tooltip text to a given TooltipImage */
	void AddToTooltipData(UTooltipImage* TooltipImage, const FText& TooltipText, const bool bInAllowTextWrap = false);

	/** Add tooltip text to a given TooltipImage */
	void AddToTooltipData(const FTooltipData& InToolTipData);

	/** Edit the tooltip text for a given TooltipImage */
	void EditTooltipText(const UTooltipImage* TooltipImage, const FText& TooltipText);
	
	/** All Tooltip Images are bound to this function */
	UFUNCTION()
	virtual void OnTooltipImageHovered(UTooltipImage* HoveredTooltipImage);

	/** Returns the enum value corresponding to the string, or the specified DefaultNotFound if no matches were found */
	template<typename T, typename Enum_Value>
	static T GetEnumFromString(const FString& InString, const Enum_Value& DefaultNotFound)
	{
		for (const T& Method : TEnumRange<T>())
		{
			if (InString.Equals(UEnum::GetDisplayValueAsText(Method).ToString()))
			{
				return Method;
			}
		}
		return DefaultNotFound;
	}

	/** Returns the String table key for an enum provided that the string table key format is EnumName_EnumValue */
	template<typename T>
	static FString GetStringTableKeyNameFromEnum(const T& InEnum)
	{
		FString EnumName;
		FString EnumValue;
		const FString EnumString = UEnum::GetValueAsString(InEnum);
		if (EnumString.Contains("::"))
		{
			EnumString.Split("::", &EnumName, &EnumValue);
			return EnumName.RightChop(1) + "_" + EnumValue;
		}
		return EnumString;
	}

	/** Returns the String Table entry for a tooltip, provided a valid key */
	static FText GetTooltipTextFromKey(const FString& InKey)
	{
		const FText StringTableEntry = FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", InKey);
		if (InKey.IsEmpty() || StringTableEntry.IsEmpty())
		{
			UE_LOG(LogTemp, Warning, TEXT("Couldn't find String Table entry for key: %s"), *InKey);
		}
		return StringTableEntry;
	}

private:
	TArray<FTooltipData> TooltipData;
	TObjectPtr<UTooltipWidget> TooltipWidget;
};