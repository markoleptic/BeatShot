// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "WidgetComponents/MenuOptionWidgets/BandThresholdWidget.h"
#include "Components/EditableTextBox.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"

void UBandThresholdWidget::NativeConstruct()
{
	Super::NativeConstruct();
	OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged_Threshold);
	SetShowTooltipImage(false);
}

void UBandThresholdWidget::SetDefaultValue(const float Value, const int32 ChannelIndex)
{
	EditableTextBox->SetText(FText::AsNumber(Value));
	Slider->SetValue(Value);
	const TArray ChannelNumber = {
		FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "AA_BandChannelText"),
		FText::FromString(FString::FromInt(ChannelIndex + 1))
	};
	TextBlock_Description->SetText(FText::Join(FText::FromString(" "), ChannelNumber));
	Index = ChannelIndex;
}

void UBandThresholdWidget::OnSliderTextBoxValueChanged_Threshold(USliderTextBoxOptionWidget* SliderTextBoxOptionWidget,
	const float Value)
{
	if (!OnThresholdValueCommitted.ExecuteIfBound(this, Index, Value))
	{
		UE_LOG(LogTemp, Display, TEXT("OnThresholdValueCommitted not bound."));
	}
}
