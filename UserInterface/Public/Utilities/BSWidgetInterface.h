// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/PanelWidget.h"
#include "UObject/Interface.h"
#include "BSWidgetInterface.generated.h"

class UBSComboBoxEntry;
class UBSComboBoxString;
class UButton;
class UTooltipIcon;
class UTooltipWidget;
class UEditableTextBox;
class USlider;

/** Enum representing a setting button. */
UENUM(BlueprintType)
enum class ESettingButtonType : uint8
{
	None UMETA(DisplayName="None"),
	Save UMETA(DisplayName="Save"),
	Reset UMETA(DisplayName="Reset"),
	Revert UMETA(DisplayName="Revert"),
	SaveAndRestart UMETA(DisplayName="Save And Restart"),
};

ENUM_RANGE_BY_FIRST_AND_LAST(ESettingButtonType, ESettingButtonType::Save, ESettingButtonType::SaveAndRestart);

UINTERFACE()
class UBSWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

/** Interface for commonly used objects and functions such as adding tooltips and syncing Sliders & TextBoxes. */
class USERINTERFACE_API IBSWidgetInterface
{
	GENERATED_BODY()

public:
	/** Clamps NewTextValue, updates associated Slider value while rounding to the GridSnapSize. */
	static float OnEditableTextBoxChanged(const FText& NewTextValue,
	                                      UEditableTextBox* TextBoxToChange,
	                                      USlider* SliderToChange,
	                                      float GridSnapSize,
	                                      float Min,
	                                      float Max);

	/** Updates associated TextBoxToChange with result of rounding to the GridSnapSize. */
	static float OnSliderChanged(float NewValue, UEditableTextBox* TextBoxToChange, float GridSnapSize);

	static void SetSliderAndEditableTextBoxValues(float NewValue,
	                                              UEditableTextBox* TextBoxToChange,
	                                              USlider* SliderToChange,
	                                              float GridSnapSize,
	                                              float Min,
	                                              float Max);

	/** Returns TooltipWidget. */
	static UTooltipWidget* GetTooltipWidget();

	/** Simple tooltip setup that bypasses any text formatting in FTooltipData. 
	 *  @param TooltipIcon icon to bind the OnTooltipIconHovered delegate to the static tooltip widget.
	 *  @param TooltipText the text to store in the tooltip icon, and eventually show on the tooltip widget.
	 *  @param bInAllowTextWrap whether to allow text wrapping in the tooltip widget.
	 */
	static void SetupTooltip(UTooltipIcon* TooltipIcon, const FText& TooltipText, bool bInAllowTextWrap = false);

	/** Override this function to use OnGenerateWidgetEvent and OnSelectionChanged_GenerateMultiSelectionItem. */
	virtual UBSComboBoxEntry* ConstructComboBoxEntryWidget() { return nullptr; }

	/** Returns the widget used for a ComboBox entry. Must override ConstructComboBoxEntryWidget. */
	UFUNCTION()
	virtual UWidget* OnGenerateWidgetEvent(const UBSComboBoxString* ComboBoxString, FString Method);

	/** Returns the widget used for a selected ComboBox entry. Must override ConstructComboBoxEntryWidget. */
	UFUNCTION()
	virtual UWidget* OnSelectionChanged_GenerateMultiSelectionItem(const UBSComboBoxString* ComboBoxString,
	                                                               const TArray<FString>& SelectedOptions);

	/** Returns the String Table key for a specific ComboBox, not the cleanest code, but it works. */
	virtual FString GetStringTableKeyFromComboBox(const UBSComboBoxString* ComboBoxString, const FString& EnumString);

	/** Simple template function to get a default class object from a subclass. */
	template <typename T>
	static const T* GetStyleCDO(const TSubclassOf<T> InSubclass)
	{
		if (InSubclass)
		{
			return Cast<T>(InSubclass->GetDefaultObject());
		}
		return nullptr;
	}

	/** Returns the String table key for an enum provided that the string table key format is EnumName_EnumValue. */
	template <typename T>
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

	/** Returns the String Table entry for a tooltip, provided a valid key. */
	static FText GetTooltipTextFromKey(const FString& InKey)
	{
		return FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", InKey);
	}

	/** Returns the String Table entry from the ST_Widgets string table, provided a valid key. */
	static FText GetWidgetTextFromKey(const FString& InKey)
	{
		return FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", InKey);;
	}

	/** Returns an array of pointers of type T, recursively searching for all instances of T inside the PanelWidget. */
	template <typename T>
	static TArray<T*> DescendWidget(UPanelWidget* PanelWidget)
	{
		TArray<T*> Boxes;
		// Check the PanelWidget first
		if (Cast<T>(PanelWidget))
		{
			Boxes.Add(Cast<T>(PanelWidget));
		}

		// Search through children
		for (UWidget* Child : PanelWidget->GetAllChildren())
		{
			// Child panel widget will get checked in first line of function
			if (UPanelWidget* ChildPanelWidget = Cast<UPanelWidget>(Child))
			{
				// Call function on any child's children
				Boxes.Append(DescendWidget<T>(ChildPanelWidget));
			}
		}
		return Boxes;
	}

	/** Returns a pointer of type T, recursively searching only until it finds the first instances of T inside the
	 *  PanelWidget. */
	template <typename T>
	static T* DescendWidgetReturnFirst(UPanelWidget* PanelWidget)
	{
		// Check the top level panel widget first
		if (Cast<T>(PanelWidget))
		{
			return Cast<T>(PanelWidget);
		}

		// Search through children
		for (UWidget* Child : PanelWidget->GetAllChildren())
		{
			// Child panel widget will get checked in first line of function
			if (UPanelWidget* ChildPanelWidget = Cast<UPanelWidget>(Child))
			{
				// Call function on any child's children
				if (T* FoundBorder = DescendWidgetReturnFirst<T>(ChildPanelWidget))
				{
					return FoundBorder;
				}
			}
		}
		return nullptr;
	}
};
