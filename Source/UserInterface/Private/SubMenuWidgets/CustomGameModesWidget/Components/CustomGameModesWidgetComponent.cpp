// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SubMenuWidgets/CustomGameModesWidget/Components/CustomGameModesWidgetComponent.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CheckBox.h"
#include "Components/EditableTextBox.h"
#include "WidgetComponents/BSComboBoxString.h"
#include "WidgetComponents/MenuOptionWidgets/CheckBoxOptionWidget.h"
#include "WidgetComponents/MenuOptionWidgets/ComboBoxOptionWidget.h"
#include "WidgetComponents/MenuOptionWidgets/EditableTextBoxOptionWidget.h"
#include "WidgetComponents/MenuOptionWidgets/SliderTextBoxWidget.h"

void UCustomGameModesWidgetComponent::NativeConstruct()
{
	Super::NativeConstruct();
	
	WidgetTree->ForEachWidget([this] (UWidget* Widget)
	{
		if (const UMenuOptionWidget* MenuOption = Cast<UMenuOptionWidget>(Widget))
		{
			if (MenuOption->ShouldShowTooltip() && !MenuOption->GetTooltipImageText().IsEmpty())
			SetupTooltip(MenuOption->GetTooltipImage(), MenuOption->GetTooltipImageText());
		}
	});
}

void UCustomGameModesWidgetComponent::UpdateAllOptionsValid()
{
}

void UCustomGameModesWidgetComponent::NativeDestruct()
{
	Super::NativeDestruct();
	BSConfig = nullptr;
}

void UCustomGameModesWidgetComponent::InitComponent(FBSConfig* InConfigPtr, const TObjectPtr<UCustomGameModesWidgetComponent> InNext)
{
	BSConfig = InConfigPtr;
	SetNext(InNext);
	UpdateOptionsFromConfig();
	bIsInitialized = true;
}

void UCustomGameModesWidgetComponent::UpdateOptionsFromConfig()
{
}

TObjectPtr<UCustomGameModesWidgetComponent> UCustomGameModesWidgetComponent::GetNext() const
{
	return Next;
}

void UCustomGameModesWidgetComponent::SetNext(const TObjectPtr<UCustomGameModesWidgetComponent> InNext)
{
	Next = InNext;
}

bool UCustomGameModesWidgetComponent::UpdateValueIfDifferent(const USliderTextBoxWidget* Widget, const float Value)
{
	if (!FMath::IsNearlyEqual(Widget->GetSliderValue(), Value) || !FMath::IsNearlyEqual(Widget->GetEditableTextBoxValue(), Value))
	{
		Widget->SetValue(Value);
		return true;
	}
	return false;
}

bool UCustomGameModesWidgetComponent::UpdateValueIfDifferent(const UComboBoxOptionWidget* Widget, const FString& NewOption)
{
	if (NewOption.IsEmpty())
	{
		if (Widget->ComboBox->GetSelectedOptionCount() == 0)
		{
			return false;
		}
		Widget->ComboBox->ClearSelection();
		return true;
	}
	if (Widget->ComboBox->GetSelectedOption() == NewOption)
	{
		return false;
	}
	Widget->ComboBox->SetSelectedOption(NewOption);
	return true;
}

bool UCustomGameModesWidgetComponent::UpdateValueIfDifferent(const UComboBoxOptionWidget* Widget, const TArray<FString>& NewOptions)
{
	const TArray<FString> SelectedOptions = Widget->ComboBox->GetSelectedOptions();
	
	if (NewOptions.IsEmpty())
	{
		if (SelectedOptions.IsEmpty())
		{
			return false;
		}
		Widget->ComboBox->ClearSelection();
		return true;
	}
	
	if (SelectedOptions == NewOptions && SelectedOptions.Num() == NewOptions.Num())
	{
		return false;
	}
	
	Widget->ComboBox->SetSelectedOptions(NewOptions);
	return true;
}

bool UCustomGameModesWidgetComponent::UpdateValueIfDifferent(const UCheckBoxOptionWidget* Widget, const bool bIsChecked)
{
	if (Widget->CheckBox->IsChecked() == bIsChecked)
	{
		return false;
	}
	Widget->CheckBox->SetIsChecked(bIsChecked);
	return true;
}

bool UCustomGameModesWidgetComponent::UpdateValueIfDifferent(const UEditableTextBoxOptionWidget* Widget, const FText& NewText)
{
	if (Widget->EditableTextBox->GetText().EqualTo(NewText))
	{
		return false;
	}
	Widget->EditableTextBox->SetText(NewText);
	return true;
}

bool UCustomGameModesWidgetComponent::UpdateWarningTooltips(UMenuOptionWidget* Widget, const TArray<FTooltipData>& NewValues)
{
	const TArray<FTooltipData> OldValues = Widget->GetAllTooltipData();
	
	if (NewValues.IsEmpty())
	{
		if (OldValues.IsEmpty())
		{
			return false;
		}
		if (!OldValues.IsEmpty())
		{
			Widget->RemoveAllTooltipWarningImages();
			return true;
		}
	}

	if (NewValues.Num() == OldValues.Num())
	{
		bool bAllSame = true;
		for (int i = 0; i < NewValues.Num(); i++ )
		{
			if (NewValues[i] == OldValues[i])
			{
				continue;
			}
			bAllSame = false;
		}
		if (bAllSame)
		{
			return false;
		}
	}
	
	// TooltipImages currently showing but shouldn't be
	TArray<FTooltipData> ValuesToRemove = OldValues.FilterByPredicate([&NewValues] (const FTooltipData& Value)
	{
		return !NewValues.Contains(Value);
	});
	
	// Remove TooltipImages currently showing but shouldn't be
	for (const FTooltipData& ValueToRemove : ValuesToRemove)
	{
		Widget->RemoveTooltipWarningImage(ValueToRemove.TooltipStringTableKey);
	}
	
	// Update the rest
	for (const FTooltipData& Value : NewValues)
	{
		FTooltipData TooltipWarningValue = Widget->FindOrAddTooltip(Value.TooltipStringTableKey, Value.TooltipType, Value.AdditionalTooltipText);
		if (TooltipWarningValue.TooltipImage.IsValid())
		{
			SetupTooltip(TooltipWarningValue.TooltipImage.Get(), TooltipWarningValue);
		}
	}
	
	return true;
}

