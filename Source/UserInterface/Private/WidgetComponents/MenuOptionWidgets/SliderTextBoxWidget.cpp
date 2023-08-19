// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "WidgetComponents/MenuOptionWidgets/SliderTextBoxWidget.h"
#include "Components/EditableTextBox.h"
#include "Components/Slider.h"
#include "BSWidgetInterface.h"

void USliderTextBoxWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	Slider->OnValueChanged.AddUniqueDynamic(this, &ThisClass::OnSliderChanged_Slider);
	EditableTextBox->OnTextCommitted.AddUniqueDynamic(this, &ThisClass::OnTextCommitted_EditableTextBox);
}

void USliderTextBoxWidget::OnSliderChanged_Slider(const float Value)
{
	IBSWidgetInterface::OnSliderChanged(Value, EditableTextBox, GridSnapSize);
}

void USliderTextBoxWidget::OnTextCommitted_EditableTextBox(const FText& Text, ETextCommit::Type CommitType)
{
	IBSWidgetInterface::OnEditableTextBoxChanged(Text, EditableTextBox, Slider, GridSnapSize, Slider->GetMinValue(), Slider->GetMaxValue());
}

void USliderTextBoxWidget::SetValues(const float Min, const float Max, const float SnapSize)
{
	Slider->SetMinValue(Min);
	Slider->SetMaxValue(Max);
	Slider->SetStepSize(SnapSize);
	GridSnapSize = SnapSize;
}

void USliderTextBoxWidget::SetValue(const float Value)
{
	Slider->SetValue(Value);
	IBSWidgetInterface::OnSliderChanged(Value, EditableTextBox, GridSnapSize);
}


