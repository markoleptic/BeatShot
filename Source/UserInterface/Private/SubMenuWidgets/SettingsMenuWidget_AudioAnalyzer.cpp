// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SubMenuWidgets/SettingsMenuWidget_AudioAnalyzer.h"
#include "Components/Button.h"
#include "Components/ComboBoxString.h"
#include "Components/EditableTextBox.h"
#include "Components/HorizontalBox.h"
#include "Components/Slider.h"
#include "Components/VerticalBox.h"
#include "OverlayWidgets/AudioSelectWidget.h"
#include "OverlayWidgets/PopupMessageWidget.h"
#include "WidgetComponents/SavedTextWidget.h"

void USettingsMenuWidget_AudioAnalyzer::InitMainMenuChild()
{
	SaveAndRestartButton->SetVisibility(ESlateVisibility::Collapsed);
}

void USettingsMenuWidget_AudioAnalyzer::NativeConstruct()
{
	Super::NativeConstruct();

	ResetButton_AASettings->OnClicked.AddDynamic(this, &USettingsMenuWidget_AudioAnalyzer::ResetAASettings);
	SaveButton_AASettings->OnClicked.AddDynamic(this, &USettingsMenuWidget_AudioAnalyzer::SortAndCheckOverlap);
	SaveAndRestartButton->OnClicked.AddDynamic(this, &USettingsMenuWidget_AudioAnalyzer::SortAndCheckOverlap);
	NumBandChannels->OnSelectionChanged.AddDynamic(this, &USettingsMenuWidget_AudioAnalyzer::OnNumBandChannelsSelectionChanged);

	SavedTextWidget->SetSavedText(FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "SM_Saved_AudioAnalyzer"));

	AASettings = ISaveLoadInterface::LoadPlayerSettings().AudioAnalyzer;
	NewAASettings = AASettings;
	const FPlayerSettings_AudioAnalyzer DefaultAASettings = FPlayerSettings_AudioAnalyzer();

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

void USettingsMenuWidget_AudioAnalyzer::OnChannelValueCommitted(const UBandChannelWidget* BandChannel, const int32 Index, const float NewValue, const bool bIsMinValue)
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

void USettingsMenuWidget_AudioAnalyzer::OnBandThresholdChanged(const UBandThresholdWidget* BandThreshold, const int32 Index, const float NewValue)
{
	NewAASettings.BandLimitsThreshold[Index] = NewValue;
}

void USettingsMenuWidget_AudioAnalyzer::OnNumBandChannelsSelectionChanged(FString NewNum, ESelectInfo::Type SelectType)
{
	NewAASettings.NumBandChannels = NumBandChannels->GetSelectedIndex() + 1;
	FPlayerSettings_AudioAnalyzer DefaultAASettings = FPlayerSettings_AudioAnalyzer();
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

void USettingsMenuWidget_AudioAnalyzer::PopulateAASettings()
{
	NumBandChannels->SetSelectedIndex(NewAASettings.NumBandChannels - 1);
	BandChannelWidget->SetDefaultValues(NewAASettings.BandLimits[0], 0);
	BandThresholdWidget->SetDefaultValue(NewAASettings.BandLimitsThreshold[0], 0);

	UBandChannelWidget* PreviousBandChannel = nullptr;
	UBandThresholdWidget* PreviousBandThreshold = nullptr;
	FPlayerSettings_AudioAnalyzer DefaultAASettings = FPlayerSettings_AudioAnalyzer();
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

void USettingsMenuWidget_AudioAnalyzer::SaveAASettingsToSlot()
{
	SavePlayerSettings(NewAASettings);
	SavedTextWidget->PlayFadeInFadeOut();
}

void USettingsMenuWidget_AudioAnalyzer::ResetAASettings()
{
	NewAASettings.ResetToDefault();
	UBandChannelWidget* CurrentBandChannel = BandChannelWidget;
	UBandThresholdWidget* CurrentBandThreshold = BandThresholdWidget;
	CurrentBandChannel->SetDefaultValues(NewAASettings.BandLimits[0], 0);
	CurrentBandThreshold->SetDefaultValue(NewAASettings.BandLimitsThreshold[0], 0);
	FPlayerSettings_AudioAnalyzer DefaultAASettings = FPlayerSettings_AudioAnalyzer();
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

void USettingsMenuWidget_AudioAnalyzer::SortAndCheckOverlap()
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

void USettingsMenuWidget_AudioAnalyzer::ShowBandLimitErrorMessage()
{
	PopupMessageWidget = CreateWidget<UPopupMessageWidget>(GetWorld(), PopupMessageClass);
	PopupMessageWidget->InitPopup(FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "ASW_SongPathErrorTitle"),
	                              FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "AA_BandLimitThresholdError"),
	                              FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "ASW_SongPathErrorButton"));
	PopupMessageWidget->Button1->OnClicked.AddDynamic(this, &USettingsMenuWidget_AudioAnalyzer::HideSongPathErrorMessage);
	PopupMessageWidget->AddToViewport();
	PopupMessageWidget->FadeIn();
}

void USettingsMenuWidget_AudioAnalyzer::HideSongPathErrorMessage()
{
	PopupMessageWidget->FadeOut();
}
