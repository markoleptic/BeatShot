// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "WidgetComponents/BandThresholdWidget.h"

#include "Components/EditableTextBox.h"
#include "Components/Slider.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"

void UBandThresholdWidget::NativeConstruct()
{
	Super::NativeConstruct();
	ThresholdValue->OnTextCommitted.AddDynamic(this, &UBandThresholdWidget::OnValueCommitted);
	ThresholdSlider->OnValueChanged.AddDynamic(this, &UBandThresholdWidget::OnSliderChanged);
}

void UBandThresholdWidget::SetDefaultValue(const float Value, const int32 ChannelIndex)
{
	ThresholdValue->SetText(FText::AsNumber(Value));
	ThresholdSlider->SetValue(Value);
	const TArray ChannelNumber = { FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "AA_BandChannelText"), FText::FromString(FString::FromInt(ChannelIndex + 1))};
	ChannelText->SetText(FText::Join(FText::FromString(" "), ChannelNumber));
	Index = ChannelIndex;
	FSlateBrush LightBrush = FSlateBrush();
	LightBrush.TintColor = FLinearColor(0,0,0,0.1);
	FSlateBrush DarkBrush = FSlateBrush();
	DarkBrush.TintColor = FLinearColor(0,0,0,0.2);
	
	if (ChannelIndex == 0 || ChannelIndex % 2 == 0)
	{
		LeftBorder->SetBrush(LightBrush);
		RightBorder->SetBrush(DarkBrush);
	}
	else
	{
		LeftBorder->SetBrush(DarkBrush);
		RightBorder->SetBrush(LightBrush);
	}
}

void UBandThresholdWidget::OnValueCommitted(const FText& NewValue, ETextCommit::Type CommitType)
{
	const float Value = FMath::GridSnap(FMath::Clamp(FCString::Atof(*NewValue.ToString()), ThresholdSlider->MinValue, ThresholdSlider->MaxValue), 0.1);
	ThresholdValue->SetText(FText::AsNumber(Value));
	ThresholdSlider->SetValue(Value);
	if (!OnThresholdValueCommitted.ExecuteIfBound(this, Index, Value))
	{
		UE_LOG(LogTemp, Display, TEXT("OnThresholdValueCommitted not bound."));
	}
}

void UBandThresholdWidget::OnSliderChanged(const float NewValue)
{
	const float Value = FMath::GridSnap(NewValue, 0.1);
	ThresholdValue->SetText(FText::AsNumber(Value));
	if (!OnThresholdValueCommitted.ExecuteIfBound(this, Index, Value))
	{
		UE_LOG(LogTemp, Display, TEXT("OnThresholdValueCommitted not bound."));
	}
}
