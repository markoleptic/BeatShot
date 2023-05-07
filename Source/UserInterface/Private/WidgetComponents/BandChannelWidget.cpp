// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "WidgetComponents/BandChannelWidget.h"

#include "GlobalConstants.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Kismet/KismetStringLibrary.h"

void UBandChannelWidget::NativeConstruct()
{
	Super::NativeConstruct();
	Value_BandChannelMin->OnTextCommitted.AddDynamic(this, &UBandChannelWidget::OnValueCommitted_Min);
	Value_BandChannelMax->OnTextCommitted.AddDynamic(this, &UBandChannelWidget::OnValueCommitted_Max);
}

void UBandChannelWidget::SetDefaultValues(const FVector2d Values, const int32 ChannelIndex)
{
	// FNumberFormattingOptions NumberFormattingOptions;
	// NumberFormattingOptions.SetRoundingMode(ER)
	Value_BandChannelMin->SetText(FText::AsNumber(Values.X));
	Value_BandChannelMax->SetText(FText::AsNumber(Values.Y));
	const TArray ChannelNumber = {
		FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "AA_BandChannelText"), FText::FromString(FString::FromInt(ChannelIndex + 1)),
		FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "AA_BandChannelUnit")
	};
	TextBlock_Channel->SetText(FText::Join(FText::FromString(" "), ChannelNumber));
	Index = ChannelIndex;

	FSlateBrush LightBrush = FSlateBrush();
	LightBrush.TintColor = FLinearColor(0, 0, 0, 0.1);
	FSlateBrush DarkBrush = FSlateBrush();
	DarkBrush.TintColor = FLinearColor(0, 0, 0, 0.2);

	if (ChannelIndex == 0 || ChannelIndex % 2 == 0)
	{
		Border_ChannelText->SetBrush(DarkBrush);
		Border_Lower->SetBrush(LightBrush);
		Border_Upper->SetBrush(LightBrush);
	}
	else
	{
		Border_ChannelText->SetBrush(LightBrush);
		Border_Lower->SetBrush(DarkBrush);
		Border_Upper->SetBrush(DarkBrush);
	}
}

void UBandChannelWidget::OnValueCommitted_Min(const FText& NewValue, ETextCommit::Type CommitType)
{
	const float NewFloatValue = FCString::Atof(*UKismetStringLibrary::Replace(NewValue.ToString(), ",", ""));
	if (NewFloatValue < Constants::MinValue_BandFrequency)
	{
		Value_BandChannelMin->SetText(FText::AsNumber(Constants::MinValue_BandFrequency));
		return;
	}
	if (NewFloatValue > Constants::MaxValue_BandFrequency)
	{
		Value_BandChannelMin->SetText(FText::AsNumber(Constants::MaxValue_BandFrequency));
		return;
	}
	if (!OnChannelValueCommitted.ExecuteIfBound(this, Index, NewFloatValue, true))
	{
		UE_LOG(LogTemp, Display, TEXT("OnChannelValueCommitted not bound."));
	}
}

void UBandChannelWidget::OnValueCommitted_Max(const FText& NewValue, ETextCommit::Type CommitType)
{
	const float NewFloatValue = FCString::Atof(*UKismetStringLibrary::Replace(NewValue.ToString(), ",", ""));
	if (NewFloatValue < Constants::MinValue_BandFrequency)
	{
		Value_BandChannelMax->SetText(FText::AsNumber(0));
		return;
	}
	if (NewFloatValue > Constants::MaxValue_BandFrequency)
	{
		Value_BandChannelMax->SetText(FText::AsNumber(Constants::MaxValue_BandFrequency));
		return;
	}
	if (!OnChannelValueCommitted.ExecuteIfBound(this, Index, NewFloatValue, false))
	{
		UE_LOG(LogTemp, Display, TEXT("OnChannelValueCommitted not bound."));
	}
}
