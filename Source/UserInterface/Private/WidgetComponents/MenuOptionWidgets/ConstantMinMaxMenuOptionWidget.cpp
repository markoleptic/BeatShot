// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "WidgetComponents/MenuOptionWidgets/ConstantMinMaxMenuOptionWidget.h"
#include "BSWidgetInterface.h"
#include "CommonTextBlock.h"
#include "Components/BorderSlot.h"
#include "Components/CheckBox.h"
#include "Components/EditableTextBox.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Slider.h"
#include "Components/Spacer.h"
#include "Styles/MenuOptionStyle.h"
#include "WidgetComponents/Boxes/BSHorizontalBox.h"

void UConstantMinMaxMenuOptionWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	TextBlock_Max->SetText(MaxText);

	Slider_ConstantOrMin->OnValueChanged.AddUniqueDynamic(this, &ThisClass::OnSliderChanged_ConstantOrMin);
	Slider_Max->OnValueChanged.AddUniqueDynamic(this, &ThisClass::OnSliderChanged_Max);
	EditableTextBox_ConstantOrMin->OnTextCommitted.AddUniqueDynamic(this, &ThisClass::OnTextCommitted_ConstantOrMin);
	EditableTextBox_Max->OnTextCommitted.AddUniqueDynamic(this, &ThisClass::OnTextCommitted_Max);
	CheckBox->OnCheckStateChanged.AddUniqueDynamic(this, &ThisClass::OnCheckStateChanged_CheckBox);
}

void UConstantMinMaxMenuOptionWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	TextBlock_Max->SetText(MaxText);
}

void UConstantMinMaxMenuOptionWidget::SetStyling()
{
	Super::SetStyling();
	if (MenuOptionStyle)
	{
		if (EditableTextBox_ConstantOrMin)
		{
			EditableTextBox_ConstantOrMin->WidgetStyle.SetFont(MenuOptionStyle->Font_EditableTextBesideSlider);
		}
		if (EditableTextBox_Max)
		{
			EditableTextBox_Max->WidgetStyle.SetFont(MenuOptionStyle->Font_EditableTextBesideSlider);
		}
		if (TextBlock_ConstantOrMin)
		{
			TextBlock_ConstantOrMin->SetFont(MenuOptionStyle->Font_DescriptionText);
		}
		if (TextBlock_Max)
		{
			TextBlock_Max->SetFont(MenuOptionStyle->Font_DescriptionText);
		}
		if (TextBlock_Description_ConstantOrMin)
		{
			TextBlock_Description_ConstantOrMin->SetFont(MenuOptionStyle->Font_DescriptionText);
			UHorizontalBoxSlot* HorizontalBoxSlot = Cast<UHorizontalBoxSlot>(TextBlock_Description_ConstantOrMin->Slot);
			if (HorizontalBoxSlot)
			{
				HorizontalBoxSlot->SetPadding(MenuOptionStyle->Padding_DescriptionText);
			}
		}
		if (TextBlock_Description_Max)
		{
			TextBlock_Description_Max->SetFont(MenuOptionStyle->Font_DescriptionText);
			UHorizontalBoxSlot* HorizontalBoxSlot = Cast<UHorizontalBoxSlot>(TextBlock_Description_Max->Slot);
			if (HorizontalBoxSlot)
			{
				HorizontalBoxSlot->SetPadding(MenuOptionStyle->Padding_DescriptionText);
			}
		}
		if (Indent_Left_ConstantOrMin)
		{
			Indent_Left_ConstantOrMin->SetSize(FVector2d(IndentLevel * MenuOptionStyle->IndentAmount, 0.f));
		}
		if (Indent_Left_Max)
		{
			Indent_Left_Max->SetSize(FVector2d(IndentLevel * MenuOptionStyle->IndentAmount, 0.f));
		}
		if (Box_Left_Max)
		{
			if (UBorderSlot* BorderSlot = Cast<UBorderSlot>(Box_Left_Max->Slot))
			{
				BorderSlot->SetPadding(MenuOptionStyle->Padding_LeftBox);
			}
		}
		if (Box_Left_ConstantOrMin)
		{
			if (UBorderSlot* BorderSlot = Cast<UBorderSlot>(Box_Left_ConstantOrMin->Slot))
			{
				BorderSlot->SetPadding(MenuOptionStyle->Padding_LeftBox);
			}
		}
		if (Box_Right_ConstantOrMin)
		{
			if (UBorderSlot* BorderSlot = Cast<UBorderSlot>(Box_Right_ConstantOrMin->Slot))
			{
				BorderSlot->SetPadding(MenuOptionStyle->Padding_RightBox);
			}
		}
		if (Box_Right_Max)
		{
			if (UBorderSlot* BorderSlot = Cast<UBorderSlot>(Box_Right_Max->Slot))
			{
				BorderSlot->SetPadding(MenuOptionStyle->Padding_RightBox);
			}
		}
	}
}

void UConstantMinMaxMenuOptionWidget::SetValues(const float Min, const float Max, const float SnapSize)
{
	Slider_ConstantOrMin->SetMinValue(Min);
	Slider_Max->SetMinValue(Min);
	Slider_ConstantOrMin->SetMaxValue(Max);
	Slider_Max->SetMaxValue(Max);
	Slider_ConstantOrMin->SetStepSize(SnapSize);
	Slider_Max->SetStepSize(SnapSize);
	GridSnapSize = SnapSize;
}

void UConstantMinMaxMenuOptionWidget::SetValue_ConstantOrMin(const float Value) const
{
	IBSWidgetInterface::SetSliderAndEditableTextBoxValues(Value, EditableTextBox_ConstantOrMin, Slider_ConstantOrMin,
		GridSnapSize, Slider_ConstantOrMin->GetMinValue(), Slider_ConstantOrMin->GetMaxValue());
}

void UConstantMinMaxMenuOptionWidget::SetValue_Max(const float Value) const
{
	IBSWidgetInterface::SetSliderAndEditableTextBoxValues(Value, EditableTextBox_Max, Slider_Max, GridSnapSize,
		Slider_Max->GetMinValue(), Slider_Max->GetMaxValue());
}

void UConstantMinMaxMenuOptionWidget::SetIsChecked(const bool bIsChecked) const
{
	CheckBox->SetIsChecked(bIsChecked);
	UpdateMinMaxDependencies(bIsChecked);
}

float UConstantMinMaxMenuOptionWidget::GetMinOrConstantSliderValue() const
{
	return Slider_ConstantOrMin->GetValue();
}

float UConstantMinMaxMenuOptionWidget::GetMaxSliderValue() const
{
	return Slider_Max->GetValue();
}

float UConstantMinMaxMenuOptionWidget::GetMinOrConstantEditableTextBoxValue() const
{
	const FString StringTextValue = EditableTextBox_ConstantOrMin->GetText().ToString().Replace(*FString(","), *FString(),
	ESearchCase::IgnoreCase);
	const float ClampedValue = FMath::Clamp(FCString::Atof(*StringTextValue), Slider_ConstantOrMin->GetMinValue(),
		Slider_ConstantOrMin->GetMaxValue());
	const float SnappedValue = FMath::GridSnap(ClampedValue, GridSnapSize);
	return SnappedValue;
}

float UConstantMinMaxMenuOptionWidget::GetMaxEditableTextBoxValue() const
{
	const FString StringTextValue = EditableTextBox_Max->GetText().ToString().Replace(*FString(","), *FString(),
	ESearchCase::IgnoreCase);
	const float ClampedValue = FMath::Clamp(FCString::Atof(*StringTextValue), Slider_Max->GetMinValue(),
		Slider_Max->GetMaxValue());
	const float SnappedValue = FMath::GridSnap(ClampedValue, GridSnapSize);
	return SnappedValue;
}

bool UConstantMinMaxMenuOptionWidget::GetIsChecked() const
{
	return CheckBox->IsChecked();
}

void UConstantMinMaxMenuOptionWidget::SetSliderAndTextBoxEnabledStates(const bool bEnabled) const
{
	Slider_ConstantOrMin->SetLocked(!bEnabled);
	Slider_Max->SetLocked(!bEnabled);
	EditableTextBox_ConstantOrMin->SetIsReadOnly(!bEnabled);
	EditableTextBox_Max->SetIsReadOnly(!bEnabled);
}

void UConstantMinMaxMenuOptionWidget::UpdateMinMaxDependencies(const bool bConstant) const
{
	if (bConstant)
	{
		TextBlock_ConstantOrMin->SetVisibility(ESlateVisibility::Collapsed);
		BSBox_Max->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		TextBlock_ConstantOrMin->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		TextBlock_ConstantOrMin->SetText(MinText);
		BSBox_Max->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void UConstantMinMaxMenuOptionWidget::OnSliderChanged_ConstantOrMin(const float Value)
{
	const float ClampedValue = IBSWidgetInterface::OnSliderChanged(Value, EditableTextBox_ConstantOrMin, GridSnapSize);
	OnMinMaxMenuOptionChanged.Broadcast(this, CheckBox->IsChecked(), ClampedValue, GetMaxSliderValue());
}

void UConstantMinMaxMenuOptionWidget::OnSliderChanged_Max(const float Value)
{
	const float ClampedValue = IBSWidgetInterface::OnSliderChanged(Value, EditableTextBox_Max, GridSnapSize);
	OnMinMaxMenuOptionChanged.Broadcast(this, CheckBox->IsChecked(), GetMinOrConstantSliderValue(), ClampedValue);
}

void UConstantMinMaxMenuOptionWidget::OnTextCommitted_ConstantOrMin(const FText& Text, ETextCommit::Type CommitType)
{
	const float ClampedValue = IBSWidgetInterface::OnEditableTextBoxChanged(Text, EditableTextBox_ConstantOrMin,
		Slider_ConstantOrMin, GridSnapSize, Slider_ConstantOrMin->GetMinValue(), Slider_ConstantOrMin->GetMaxValue());
	OnMinMaxMenuOptionChanged.Broadcast(this, CheckBox->IsChecked(), ClampedValue, GetMaxSliderValue());
}

void UConstantMinMaxMenuOptionWidget::OnTextCommitted_Max(const FText& Text, ETextCommit::Type CommitType)
{
	const float ClampedValue = IBSWidgetInterface::OnEditableTextBoxChanged(Text, EditableTextBox_Max, Slider_Max,
		GridSnapSize, Slider_Max->GetMinValue(), Slider_Max->GetMaxValue());
	OnMinMaxMenuOptionChanged.Broadcast(this, CheckBox->IsChecked(), GetMinOrConstantSliderValue(), ClampedValue);
}

void UConstantMinMaxMenuOptionWidget::OnCheckStateChanged_CheckBox(const bool bChecked)
{
	UpdateMinMaxDependencies(bChecked);
	OnMinMaxMenuOptionChanged.Broadcast(this, bChecked, GetMinOrConstantSliderValue(), GetMaxSliderValue());
}
