// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "WidgetComponents/BandThresholdWidget.h"

#include "Components/EditableTextBox.h"
#include "Components/Slider.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"

void UBandThresholdWidget::NativeConstruct()
{
	Super::NativeConstruct();
	Value_Threshold->OnTextCommitted.AddDynamic(this, &UBandThresholdWidget::OnValueChanged_Threshold);
	Slider_Threshold->OnValueChanged.AddDynamic(this, &UBandThresholdWidget::OnSliderChanged_Threshold);
}

void UBandThresholdWidget::SetDefaultValue(const float Value, const int32 ChannelIndex)
{
	Value_Threshold->SetText(FText::AsNumber(Value));
	Slider_Threshold->SetValue(Value);
	const TArray ChannelNumber = {FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "AA_BandChannelText"), FText::FromString(FString::FromInt(ChannelIndex + 1))};
	TextBlock_Channel->SetText(FText::Join(FText::FromString(" "), ChannelNumber));
	Index = ChannelIndex;
	FSlateBrush LightBrush = FSlateBrush();
	LightBrush.TintColor = FLinearColor(0, 0, 0, 0.1);
	FSlateBrush DarkBrush = FSlateBrush();
	DarkBrush.TintColor = FLinearColor(0, 0, 0, 0.2);

	if (ChannelIndex == 0 || ChannelIndex % 2 == 0)
	{
		Border_Left->SetBrush(LightBrush);
		Border_Right->SetBrush(DarkBrush);
	}
	else
	{
		Border_Left->SetBrush(DarkBrush);
		Border_Right->SetBrush(LightBrush);
	}
}

void UBandThresholdWidget::OnValueChanged_Threshold(const FText& NewValue, ETextCommit::Type CommitType)
{
	const float Value = FMath::GridSnap(FMath::Clamp(FCString::Atof(*NewValue.ToString()), Slider_Threshold->GetMinValue(), Slider_Threshold->GetMaxValue()), 0.1);
	Value_Threshold->SetText(FText::AsNumber(Value));
	Slider_Threshold->SetValue(Value);
	if (!OnThresholdValueCommitted.ExecuteIfBound(this, Index, Value))
	{
		UE_LOG(LogTemp, Display, TEXT("OnThresholdValueCommitted not bound."));
	}
}

void UBandThresholdWidget::OnSliderChanged_Threshold(const float NewValue)
{
	const float Value = FMath::GridSnap(NewValue, 0.1);
	Value_Threshold->SetText(FText::AsNumber(Value));
	if (!OnThresholdValueCommitted.ExecuteIfBound(this, Index, Value))
	{
		UE_LOG(LogTemp, Display, TEXT("OnThresholdValueCommitted not bound."));
	}
}
