// Fill out your copyright notice in the Description page of Project Settings.

#include "SubMenuWidgets/AASettingsWidget.h"
#include "Components/Button.h"
#include "Components/ComboBoxString.h"
#include "Components/EditableTextBox.h"
#include "Components/HorizontalBox.h"
#include "Components/Slider.h"
#include "Components/VerticalBox.h"
#include "WidgetComponents/SavedTextWidget.h"

void UAASettingsWidget::InitMainMenuChild()
{
	SaveAndRestartButton->SetVisibility(ESlateVisibility::Collapsed);
}

void UAASettingsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ResetAASettingsButton->OnClicked.AddDynamic(this, &UAASettingsWidget::ResetAASettings);
	SaveAASettingsButton->OnClicked.AddDynamic(this, &UAASettingsWidget::SaveAASettingsToSlot);
	SaveAndRestartButton->OnClicked.AddDynamic(this, &UAASettingsWidget::OnSaveAndRestartButtonClicked);
	NumBandChannels->OnSelectionChanged.AddDynamic(this, &UAASettingsWidget::OnNumBandChannelsSelectionChanged);
	
	for (int i = 0; i < MaxNumBandChannels; i++)
	{
		NumBandChannels->AddOption(FString::FromInt(i+1));
	}
	
	AASettings = ISaveLoadInterface::LoadAASettings();
	NewAASettings = AASettings;
	
	BandChannelWidget = CreateWidget<UBandChannelWidget>(this, BandChannelWidgetClass);
	BandChannelWidget->SetDefaultValues(NewAASettings.BandLimits[0], 0);
	BandChannelWidget->OnChannelValueCommitted.BindUFunction(this, "OnChannelValueCommitted");
	BandChannelBounds->AddChild(Cast<UBandChannelWidget>(BandChannelWidget));
	
	BandThresholdWidget = CreateWidget<UBandThresholdWidget>(this, BandThresholdWidgetClass);
	BandThresholdWidget->SetDefaultValue(NewAASettings.BandLimitsThreshold[0], 0);
	BandThresholdWidget->OnThresholdValueCommitted.BindUFunction(this, "OnBandThresholdChanged");
	BandThresholdBounds->AddChild(Cast<UBandThresholdWidget>(BandThresholdWidget));
	
	UBandChannelWidget* CurrentBandChannel = BandChannelWidget;
	UBandThresholdWidget* CurrentBandThreshold = BandThresholdWidget;
	FAASettingsStruct DefaultAASettings = FAASettingsStruct();
	for (int i = 1; i < MaxNumBandChannels; i++)
	{
		CurrentBandChannel->Next = CreateWidget<UBandChannelWidget>(this, BandChannelWidgetClass);
		CurrentBandChannel->Next->OnChannelValueCommitted.BindUFunction(this, "OnChannelValueCommitted");
		BandChannelBounds->AddChild(Cast<UBandChannelWidget>(CurrentBandChannel->Next));
		CurrentBandThreshold->Next = CreateWidget<UBandThresholdWidget>(this, BandThresholdWidgetClass);
		CurrentBandThreshold->Next->OnThresholdValueCommitted.BindUFunction(this, "OnBandThresholdChanged");
		BandThresholdBounds->AddChild(Cast<UBandThresholdWidget>(CurrentBandThreshold->Next));
		
		if (NewAASettings.NumBandChannels > i)
		{
			CurrentBandChannel->Next->SetDefaultValues(NewAASettings.BandLimits[i], i);
			CurrentBandThreshold->Next->SetDefaultValue(NewAASettings.BandLimitsThreshold[i], i);
		}
		else
		{
			CurrentBandChannel->Next->SetDefaultValues(DefaultAASettings.BandLimits[i], i);
			CurrentBandThreshold->Next->SetDefaultValue(DefaultAASettings.BandLimitsThreshold[i], i);
		}
		CurrentBandChannel = CurrentBandChannel->Next;
		CurrentBandThreshold = CurrentBandThreshold->Next;
	}
	PopulateAASettings();
}

void UAASettingsWidget::OnChannelValueCommitted(const UBandChannelWidget* BandChannel, const int32 Index, const float NewValue,
	const bool bIsMinValue)
{
	if (bIsMinValue)
	{
		NewAASettings.BandLimits[Index].X = NewValue;
	}
	else
	{
		NewAASettings.BandLimits[Index].Y = NewValue;
	}
}

void UAASettingsWidget::OnBandThresholdChanged(const UBandThresholdWidget* BandThreshold, const int32 Index,
	const float NewValue)
{
	NewAASettings.BandLimitsThreshold[Index] = NewValue;
}

void UAASettingsWidget::OnSaveAndRestartButtonClicked()
{
	SaveAASettingsToSlot();
	if (OnRestartButtonClicked.IsBound()) {
		OnRestartButtonClicked.Execute();
	}
}

void UAASettingsWidget::OnNumBandChannelsSelectionChanged(FString NewNum, ESelectInfo::Type SelectType)
{
	NewAASettings.NumBandChannels = NumBandChannels->GetSelectedIndex() + 1;
	FAASettingsStruct DefaultAASettings = FAASettingsStruct();
	if (const int ElementsToAdd = NewAASettings.NumBandChannels - NewAASettings.BandLimits.Num(); ElementsToAdd > 0)
	{
		for (int i = 0; i < ElementsToAdd; i ++)
		{
			NewAASettings.BandLimitsThreshold.Emplace(2.1);
		}
		NewAASettings.BandLimits.SetNum(NewAASettings.NumBandChannels, true);
	}
	else
	{
		NewAASettings.BandLimits.SetNum(NewAASettings.NumBandChannels, true);
		NewAASettings.BandLimitsThreshold.SetNum(NewAASettings.NumBandChannels, true);
	}
	PopulateAASettings();
}

void UAASettingsWidget::PopulateAASettings()
{
	NumBandChannels->SetSelectedIndex(NewAASettings.NumBandChannels - 1);

	UBandChannelWidget* CurrentBandChannelWidget = BandChannelWidget;
	UBandThresholdWidget* CurrentBandThresholdWidget = BandThresholdWidget;
	int Count = 0;
	while (Count < MaxNumBandChannels)
	{
		if (Count <= NewAASettings.NumBandChannels - 1)
		{
			CurrentBandChannelWidget->SetVisibility(ESlateVisibility::Visible);
			CurrentBandThresholdWidget->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			CurrentBandChannelWidget->SetVisibility(ESlateVisibility::Collapsed);
			CurrentBandThresholdWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
		if (CurrentBandChannelWidget->Next == nullptr || CurrentBandThresholdWidget->Next == nullptr)
		{
			break;
		}
		CurrentBandChannelWidget = CurrentBandChannelWidget->Next;
		CurrentBandThresholdWidget = CurrentBandThresholdWidget->Next;
		Count++;
	}
	TimeWindowSlider->SetValue(NewAASettings.TimeWindow);
	TimeWindowValue->SetText(FText::AsNumber(NewAASettings.TimeWindow, &FNumberFormattingOptions::DefaultNoGrouping()));
}

void UAASettingsWidget::SaveAASettingsToSlot()
{
	SaveAASettings(NewAASettings);
	OnAASettingsChange.Broadcast(AASettings);
	SavedTextWidget->PlayFadeInFadeOut();
}

void UAASettingsWidget::ResetAASettings()
{
	NewAASettings.ResetStruct();
	UBandChannelWidget* CurrentBandChannel = BandChannelWidget;
	UBandThresholdWidget* CurrentBandThreshold = BandThresholdWidget;
	CurrentBandChannel->SetDefaultValues(NewAASettings.BandLimits[0], 0);
	CurrentBandThreshold->SetDefaultValue(NewAASettings.BandLimitsThreshold[0], 0);
	for (int i = 1; i < MaxNumBandChannels; i++)
	{
		CurrentBandChannel->Next->SetDefaultValues(NewAASettings.BandLimits[i], i);
		CurrentBandThreshold->Next->SetDefaultValue(NewAASettings.BandLimitsThreshold[i], i);
		CurrentBandChannel = CurrentBandChannel->Next;
		CurrentBandThreshold = CurrentBandThreshold->Next;
	}
	PopulateAASettings();
}
