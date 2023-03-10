// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "WidgetComponents/BandChannelWidget.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Kismet/KismetStringLibrary.h"

void UBandChannelWidget::NativeConstruct()
{
	Super::NativeConstruct();
	BandChannelMin->OnTextCommitted.AddDynamic(this, &UBandChannelWidget::OnMinValueCommitted);
	BandChannelMax->OnTextCommitted.AddDynamic(this, &UBandChannelWidget::OnMaxValueCommitted);
}

void UBandChannelWidget::SetDefaultValues(const FVector2d Values, const int32 ChannelIndex)
{
	// FNumberFormattingOptions NumberFormattingOptions;
	// NumberFormattingOptions.SetRoundingMode(ER)
	BandChannelMin->SetText(FText::AsNumber(Values.X));
	BandChannelMax->SetText(FText::AsNumber(Values.Y));
	const TArray ChannelNumber = {
		FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "AA_BandChannelText"), FText::FromString(FString::FromInt(ChannelIndex + 1)),
		FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "AA_BandChannelUnit")
	};
	ChannelText->SetText(FText::Join(FText::FromString(" "), ChannelNumber));
	Index = ChannelIndex;

	FSlateBrush LightBrush = FSlateBrush();
	LightBrush.TintColor = FLinearColor(0, 0, 0, 0.1);
	FSlateBrush DarkBrush = FSlateBrush();
	DarkBrush.TintColor = FLinearColor(0, 0, 0, 0.2);

	if (ChannelIndex == 0 || ChannelIndex % 2 == 0)
	{
		ChannelTextBorder->SetBrush(DarkBrush);
		LowerBorder->SetBrush(LightBrush);
		UpperBorder->SetBrush(LightBrush);
	}
	else
	{
		ChannelTextBorder->SetBrush(LightBrush);
		LowerBorder->SetBrush(DarkBrush);
		UpperBorder->SetBrush(DarkBrush);
	}
}

void UBandChannelWidget::OnMinValueCommitted(const FText& NewValue, ETextCommit::Type CommitType)
{
	const float NewFloatValue = FCString::Atof(*UKismetStringLibrary::Replace(NewValue.ToString(), ",", ""));
	if (NewFloatValue < AbsoluteMin)
	{
		BandChannelMin->SetText(FText::AsNumber(0));
		return;
	}
	if (NewFloatValue > AbsoluteMax)
	{
		BandChannelMin->SetText(FText::AsNumber(22720));
		return;
	}
	if (!OnChannelValueCommitted.ExecuteIfBound(this, Index, NewFloatValue, true))
	{
		UE_LOG(LogTemp, Display, TEXT("OnChannelValueCommitted not bound."));
	}
}

void UBandChannelWidget::OnMaxValueCommitted(const FText& NewValue, ETextCommit::Type CommitType)
{
	const float NewFloatValue = FCString::Atof(*UKismetStringLibrary::Replace(NewValue.ToString(), ",", ""));
	if (NewFloatValue < AbsoluteMin)
	{
		BandChannelMax->SetText(FText::AsNumber(0));
		return;
	}
	if (NewFloatValue > AbsoluteMax)
	{
		BandChannelMax->SetText(FText::AsNumber(22720));
		return;
	}
	if (!OnChannelValueCommitted.ExecuteIfBound(this, Index, NewFloatValue, false))
	{
		UE_LOG(LogTemp, Display, TEXT("OnChannelValueCommitted not bound."));
	}
}
