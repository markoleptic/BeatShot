// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SubMenuWidgets/CustomGameModesWidget/Components/CustomGameModesWidgetComponent.h"
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

	OnTransitionInLeftFinish.BindDynamic(this, &ThisClass::SetCollapsed);
	OnTransitionInRightFinish.BindDynamic(this, &ThisClass::SetCollapsed);
}

bool UCustomGameModesWidgetComponent::UpdateAllOptionsValid()
{
	return false;
}

void UCustomGameModesWidgetComponent::SetAllOptionsValid(const bool bUpdateAllOptionsValid)
{
	if (bUpdateAllOptionsValid == bAllOptionsValid)
	{
		return;
	}
	
	bAllOptionsValid = bUpdateAllOptionsValid;
	
	if (OnValidOptionsStateChanged.IsBound())
	{
		OnValidOptionsStateChanged.Broadcast(this, bAllOptionsValid);
	}
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

bool UCustomGameModesWidgetComponent::GetAllOptionsValid() const
{
	return bAllOptionsValid;
}

void UCustomGameModesWidgetComponent::PlayAnim_TransitionInLeft_Forward(const bool bCollapseOnFinish)
{
	if (bCollapseOnFinish)
	{
		BindToAnimationFinished(TransitionInLeft, OnTransitionInLeftFinish);
	}
	
	if (GetVisibility() == ESlateVisibility::Collapsed)
	{
		SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	
	PlayAnimationForward(TransitionInLeft);
}

void UCustomGameModesWidgetComponent::PlayAnim_TransitionInLeft_Reverse(const bool bCollapseOnFinish)
{
	if (bCollapseOnFinish)
	{
		BindToAnimationFinished(TransitionInLeft, OnTransitionInLeftFinish);
	}
	
	if (GetVisibility() == ESlateVisibility::Collapsed)
	{
		SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	
	PlayAnimationReverse(TransitionInLeft);
}

void UCustomGameModesWidgetComponent::PlayAnim_TransitionInRight_Forward(const bool bCollapseOnFinish)
{
	if (bCollapseOnFinish)
	{
		BindToAnimationFinished(TransitionInRight, OnTransitionInRightFinish);
	}
	
	if (GetVisibility() == ESlateVisibility::Collapsed)
	{
		SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	
	PlayAnimationForward(TransitionInRight);
}

void UCustomGameModesWidgetComponent::PlayAnim_TransitionInRight_Reverse(const bool bCollapseOnFinish)
{
	if (bCollapseOnFinish)
	{
		BindToAnimationFinished(TransitionInRight, OnTransitionInRightFinish);
	}
	
	if (GetVisibility() == ESlateVisibility::Collapsed)
	{
		SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	
	PlayAnimationReverse(TransitionInRight);
}

TObjectPtr<UCustomGameModesWidgetComponent> UCustomGameModesWidgetComponent::GetNext() const
{
	return Next;
}

void UCustomGameModesWidgetComponent::SetNext(const TObjectPtr<UCustomGameModesWidgetComponent> InNext)
{
	Next = InNext;
}

void UCustomGameModesWidgetComponent::SetCollapsed()
{
	SetVisibility(ESlateVisibility::Collapsed);
	UnbindFromAnimationFinished(TransitionInLeft, OnTransitionInLeftFinish);
	UnbindFromAnimationFinished(TransitionInRight, OnTransitionInRightFinish);
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

bool UCustomGameModesWidgetComponent::UpdateTooltipWarningImages(UMenuOptionWidget* Widget, const TArray<FTooltipWarningValue>& NewValues)
{
	const TArray<FTooltipWarningValue> OldValues = Widget->GetTooltipWarningImageValues();
	
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

	TArray<FString> NewKeys;
	for (const FTooltipWarningValue& Value : NewValues)
	{
		NewKeys.Add(Value.TooltipStringTableKey);
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
	TArray<FTooltipWarningValue> ValuesToRemove = OldValues.FilterByPredicate([&NewValues] (const FTooltipWarningValue& Value)
	{
		return !NewValues.Contains(Value);
	});
	
	// Remove TooltipImages currently showing but shouldn't be
	for (const FTooltipWarningValue& ValueToRemove : ValuesToRemove)
	{
		Widget->RemoveTooltipWarningImage(ValueToRemove.TooltipStringTableKey);
	}

	// Update the rest
	for (const FTooltipWarningValue& Value : NewValues)
	{
		const FTooltipWarningValue TooltipWarningValue = Widget->FindOrAddTooltipWarningValue(Value.TooltipStringTableKey);
		const FText TextFromStringTable = GetTooltipTextFromKey(Value.TooltipStringTableKey);
		if (TooltipWarningValue.TooltipWarningImage)
		{
			SetupTooltip(TooltipWarningValue.TooltipWarningImage, FText::Join(FText::FromString(" "), TextFromStringTable, Value.AdditionalTooltipText));
		}
	}
	
	return true;
}

