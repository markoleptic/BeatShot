// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SubMenuWidgets/AASettingsWidget.h"
#include "Components/Button.h"
#include "Components/ComboBoxString.h"
#include "Components/EditableTextBox.h"
#include "Components/HorizontalBox.h"
#include "Components/Slider.h"
#include "Components/VerticalBox.h"
#include "OverlayWidgets/AudioSelectWidget.h"
#include "OverlayWidgets/PopupMessageWidget.h"
#include "WidgetComponents/SavedTextWidget.h"

void UAASettingsWidget::InitMainMenuChild()
{
	SaveAndRestartButton->SetVisibility(ESlateVisibility::Collapsed);
}

void UAASettingsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ResetButton_AASettings->OnClicked.AddDynamic(this, &UAASettingsWidget::ResetAASettings);
	SaveButton_AASettings->OnClicked.AddDynamic(this, &UAASettingsWidget::SortAndCheckOverlap);
	SaveAndRestartButton->OnClicked.AddDynamic(this, &UAASettingsWidget::SortAndCheckOverlap);
	NumBandChannels->OnSelectionChanged.AddDynamic(this, &UAASettingsWidget::OnNumBandChannelsSelectionChanged);

	SavedTextWidget->SetSavedText(FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "SM_Saved_AudioAnalyzer"));

	AASettings = ISaveLoadInterface::LoadAASettings();
	NewAASettings = AASettings;
	const FAASettingsStruct DefaultAASettings = FAASettingsStruct();

	for (int i = 0; i < DefaultAASettings.MaxNumBandChannels; i++)
	{
		NumBandChannels->AddOption(FString::FromInt(i + 1));
	}

	UBandChannelWidget* PreviousBandChannel = nullptr;
	UBandThresholdWidget* PreviousBandThreshold = nullptr;

	for (int i = 0; i < DefaultAASettings.MaxNumBandChannels; i++)
	{
		UBandChannelWidget* CurrentBandChannel = CreateWidget<UBandChannelWidget>(this, BandChannelWidgetClass);
		UBandThresholdWidget* CurrentBandThreshold = CreateWidget<UBandThresholdWidget>(this, BandThresholdWidgetClass);
		CurrentBandChannel->OnChannelValueCommitted.BindUFunction(this, "OnChannelValueCommitted");
		BandChannelBounds->AddChild(Cast<UBandChannelWidget>(CurrentBandChannel));
		CurrentBandThreshold->OnThresholdValueCommitted.BindUFunction(this, "OnBandThresholdChanged");
		BandThresholdBounds->AddChild(Cast<UBandThresholdWidget>(CurrentBandThreshold));

		if (i == 0)
		{
			BandChannelWidget = CurrentBandChannel;
			BandThresholdWidget = CurrentBandThreshold;
			PreviousBandChannel = BandChannelWidget;
			PreviousBandThreshold = BandThresholdWidget;
		}
		else
		{
			PreviousBandChannel->Next = CurrentBandChannel;
			PreviousBandThreshold->Next = CurrentBandThreshold;
			PreviousBandChannel = CurrentBandChannel;
			PreviousBandThreshold = CurrentBandThreshold;
		}
	}
	PopulateAASettings();
}

void UAASettingsWidget::OnChannelValueCommitted(const UBandChannelWidget* BandChannel, const int32 Index, const float NewValue, const bool bIsMinValue)
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

void UAASettingsWidget::OnBandThresholdChanged(const UBandThresholdWidget* BandThreshold, const int32 Index, const float NewValue)
{
	NewAASettings.BandLimitsThreshold[Index] = NewValue;
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
			NewAASettings.BandLimits.Emplace(FVector2d::ZeroVector);
		}
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
	BandChannelWidget->SetDefaultValues(NewAASettings.BandLimits[0], 0);
	BandThresholdWidget->SetDefaultValue(NewAASettings.BandLimitsThreshold[0], 0);

	UBandChannelWidget* PreviousBandChannel = nullptr;
	UBandThresholdWidget* PreviousBandThreshold = nullptr;
	FAASettingsStruct DefaultAASettings = FAASettingsStruct();
	for (int i = 0; i < DefaultAASettings.MaxNumBandChannels; i++)
	{
		if (i == 0)
		{
			PreviousBandChannel = BandChannelWidget;
			PreviousBandThreshold = BandThresholdWidget;
		}

		if (NewAASettings.NumBandChannels > i)
		{
			PreviousBandChannel->SetDefaultValues(NewAASettings.BandLimits[i], i);
			PreviousBandThreshold->SetDefaultValue(NewAASettings.BandLimitsThreshold[i], i);
			PreviousBandChannel->SetVisibility(ESlateVisibility::Visible);
			PreviousBandThreshold->SetVisibility(ESlateVisibility::Visible);
		}
		else if (DefaultAASettings.NumBandChannels > i)
		{
			PreviousBandChannel->SetDefaultValues(DefaultAASettings.BandLimits[i], i);
			PreviousBandThreshold->SetDefaultValue(DefaultAASettings.BandLimitsThreshold[i], i);
			PreviousBandChannel->SetVisibility(ESlateVisibility::Collapsed);
			PreviousBandThreshold->SetVisibility(ESlateVisibility::Collapsed);
		}
		else
		{
			PreviousBandChannel->SetDefaultValues(FVector2d(0, 0), i);
			PreviousBandThreshold->SetDefaultValue(DefaultAASettings.BandLimitsThreshold[0], i);
			PreviousBandChannel->SetVisibility(ESlateVisibility::Collapsed);
			PreviousBandThreshold->SetVisibility(ESlateVisibility::Collapsed);
		}
		PreviousBandChannel = PreviousBandChannel->Next;
		PreviousBandThreshold = PreviousBandThreshold->Next;
	}

	TimeWindowSlider->SetValue(NewAASettings.TimeWindow);
	TimeWindowValue->SetText(FText::AsNumber(NewAASettings.TimeWindow, &FNumberFormattingOptions::DefaultNoGrouping()));
}

void UAASettingsWidget::SaveAASettingsToSlot()
{
	SaveAASettings(NewAASettings);
	if (!OnSettingsSaved_AudioAnalyzer.ExecuteIfBound())
	{
		UE_LOG(LogTemp, Display, TEXT("OnSettingsSaved_AudioAnalyzer not bound."));
	}
	SavedTextWidget->PlayFadeInFadeOut();
}

void UAASettingsWidget::ResetAASettings()
{
	NewAASettings.ResetStruct();
	UBandChannelWidget* CurrentBandChannel = BandChannelWidget;
	UBandThresholdWidget* CurrentBandThreshold = BandThresholdWidget;
	CurrentBandChannel->SetDefaultValues(NewAASettings.BandLimits[0], 0);
	CurrentBandThreshold->SetDefaultValue(NewAASettings.BandLimitsThreshold[0], 0);
	FAASettingsStruct DefaultAASettings = FAASettingsStruct();
	for (int i = 1; i < DefaultAASettings.MaxNumBandChannels; i++)
	{
		if (DefaultAASettings.NumBandChannels > i)
		{
			CurrentBandChannel->Next->SetDefaultValues(DefaultAASettings.BandLimits[i], i);
			CurrentBandThreshold->Next->SetDefaultValue(DefaultAASettings.BandLimitsThreshold[i], i);
		}
		else
		{
			CurrentBandChannel->Next->SetDefaultValues(FVector2d(0, 0), i);
			CurrentBandThreshold->Next->SetDefaultValue(DefaultAASettings.BandLimitsThreshold[0], i);
		}
		CurrentBandChannel = CurrentBandChannel->Next;
		CurrentBandThreshold = CurrentBandThreshold->Next;
	}
	PopulateAASettings();
}

void UAASettingsWidget::SortAndCheckOverlap()
{
	NewAASettings.BandLimits.RemoveAll([this](const FVector2d& BandLimit)
	{
		if (BandLimit == FVector2d(0, 0))
		{
			return true;
		}
		return false;
	});
	NewAASettings.BandLimits.Sort([](const FVector2d& A, const FVector2d& B)
	{
		if (A.X < B.X)
		{
			return true;
		}
		if (A.X == B.X && A.Y < B.Y)
		{
			return true;
		}
		return false;
	});
	NewAASettings.NumBandChannels = NewAASettings.BandLimits.Num();
	NewAASettings.BandLimitsThreshold.SetNum(NewAASettings.BandLimits.Num());
	PopulateAASettings();
	double LastEndThreshold = NewAASettings.BandLimits[0].Y;
	for (int i = 0; i < NewAASettings.BandLimits.Num(); i++)
	{
		if (NewAASettings.BandLimits[i].X >= NewAASettings.BandLimits[i].Y)
		{
			ShowBandLimitErrorMessage();
			return;
		}
		if (i == 0)
		{
			continue;
		}
		if (NewAASettings.BandLimits[i].X <= LastEndThreshold)
		{
			ShowBandLimitErrorMessage();
			return;
		}
		LastEndThreshold = NewAASettings.BandLimits[i].Y;
	}
	SaveAASettingsToSlot();
	if (!OnRestartButtonClicked.ExecuteIfBound())
	{
		UE_LOG(LogTemp, Display, TEXT("OnRestartButtonClicked not bound."));
	}
}

void UAASettingsWidget::ShowBandLimitErrorMessage()
{
	PopupMessageWidget = CreateWidget<UPopupMessageWidget>(GetWorld(), PopupMessageClass);
	PopupMessageWidget->InitPopup(FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "ASW_SongPathErrorTitle"),
	                              FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "AA_BandLimitThresholdError"),
	                              FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "ASW_SongPathErrorButton"));
	PopupMessageWidget->Button1->OnClicked.AddDynamic(this, &UAASettingsWidget::HideSongPathErrorMessage);
	PopupMessageWidget->AddToViewport();
	PopupMessageWidget->FadeIn();
}

void UAASettingsWidget::HideSongPathErrorMessage()
{
	PopupMessageWidget->FadeOut();
}
