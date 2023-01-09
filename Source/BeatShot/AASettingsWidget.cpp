// Fill out your copyright notice in the Description page of Project Settings.


#include "AASettingsWidget.h"
#include "DefaultGameInstance.h"
#include "Components/Button.h"
#include "Components/ComboBoxString.h"
#include "Components/EditableTextBox.h"
#include "Components/HorizontalBox.h"
#include "Components/Slider.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

void UAASettingsWidget::InitMainMenuChild()
{
	SaveAndRestartButton->SetVisibility(ESlateVisibility::Collapsed);
}

void UAASettingsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ResetAASettingsButton->OnClicked.AddDynamic(this, &UAASettingsWidget::ResetAASettings);
	SaveAASettingsButton->OnClicked.AddDynamic(this, &UAASettingsWidget::SaveAASettings);
	SaveAndRestartButton->OnClicked.AddDynamic(this, &UAASettingsWidget::OnSaveAndRestartButtonClicked);
	NumBandChannels->OnSelectionChanged.AddDynamic(this, &UAASettingsWidget::ShowBandChannelsAndThresholds);

	ThresholdSliderOne->OnValueChanged.AddDynamic(this, &UAASettingsWidget::OnThresholdSliderOneChanged);
	ThresholdSliderTwo->OnValueChanged.AddDynamic(this, &UAASettingsWidget::OnThresholdSliderTwoChanged);
	ThresholdSliderThree->OnValueChanged.AddDynamic(this, &UAASettingsWidget::OnThresholdSliderThreeChanged);
	ThresholdSliderFour->OnValueChanged.AddDynamic(this, &UAASettingsWidget::OnThresholdSliderFourChanged);

	ThresholdValueOne->OnTextChanged.AddDynamic(this,  &UAASettingsWidget::OnThresholdValueOneChanged);
	ThresholdValueTwo->OnTextChanged.AddDynamic(this,  &UAASettingsWidget::OnThresholdValueTwoChanged);
	ThresholdValueThree->OnTextChanged.AddDynamic(this,  &UAASettingsWidget::OnThresholdValueThreeChanged);
	ThresholdValueFour->OnTextChanged.AddDynamic(this,  &UAASettingsWidget::OnThresholdValueFourChanged);
	
	LoadAASettings();
	PopulateAASettings();
}

void UAASettingsWidget::OnSaveAndRestartButtonClicked()
{
	SaveAASettings();
	if (OnRestartButtonClicked.IsBound()) {
		OnRestartButtonClicked.Execute();
	}
}

void UAASettingsWidget::OnThresholdSliderChanged(const float NewValue, UEditableTextBox* TextBoxToChange,
	const float SnapSize)
{
	TextBoxToChange->SetText(FText::FromString(FString::SanitizeFloat(UKismetMathLibrary::GridSnap_Float(NewValue, SnapSize))));
}

void UAASettingsWidget::OnThresholdValueChanged(const FText& NewText, USlider* SliderToChange, const float SnapSize)
{
	SliderToChange->SetValue(UKismetMathLibrary::GridSnap_Float(FCString::Atof(*NewText.ToString()), SnapSize));
}

void UAASettingsWidget::ShowBandChannelsAndThresholds(const FString SelectedOption, ESelectInfo::Type SelectionType)
{
	if (NumBandChannels->GetSelectedIndex() == 3)
	{
		BandChannelOne->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		BandChannelTwo->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		BandChannelThree->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		BandChannelFour->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

		ThresholdBoxOne->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		ThresholdBoxTwo->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		ThresholdBoxThree->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		ThresholdBoxFour->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

		AASettings.NumBandChannels = 4;
	}
	else if (NumBandChannels->GetSelectedIndex() == 2)
	{
		BandChannelOne->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		BandChannelTwo->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		BandChannelThree->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		BandChannelFour->SetVisibility(ESlateVisibility::Collapsed);

		ThresholdBoxOne->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		ThresholdBoxTwo->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		ThresholdBoxThree->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		ThresholdBoxFour->SetVisibility(ESlateVisibility::Collapsed);

		AASettings.NumBandChannels = 3;
	}
	else if (NumBandChannels->GetSelectedIndex() == 1)
	{
		BandChannelOne->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		BandChannelTwo->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		BandChannelThree->SetVisibility(ESlateVisibility::Collapsed);
		BandChannelFour->SetVisibility(ESlateVisibility::Collapsed);

		ThresholdBoxOne->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		ThresholdBoxTwo->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		ThresholdBoxThree->SetVisibility(ESlateVisibility::Collapsed);
		ThresholdBoxFour->SetVisibility(ESlateVisibility::Collapsed);

		AASettings.NumBandChannels = 2;
	}
	else if (NumBandChannels->GetSelectedIndex() == 0)
	{
		BandChannelOne->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		BandChannelTwo->SetVisibility(ESlateVisibility::Collapsed);
		BandChannelThree->SetVisibility(ESlateVisibility::Collapsed);
		BandChannelFour->SetVisibility(ESlateVisibility::Collapsed);

		ThresholdBoxOne->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		ThresholdBoxTwo->SetVisibility(ESlateVisibility::Collapsed);
		ThresholdBoxThree->SetVisibility(ESlateVisibility::Collapsed);
		ThresholdBoxFour->SetVisibility(ESlateVisibility::Collapsed);

		AASettings.NumBandChannels = 1;
	}
}

void UAASettingsWidget::LoadAASettings()
{
	AASettings = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()))->LoadAASettings();
}

void UAASettingsWidget::PopulateAASettings()
{
	NumBandChannels->SetSelectedIndex(AASettings.NumBandChannels - 1);

	if (AASettings.NumBandChannels == 4)
	{
		BandChannelOneMin->SetText(FText::AsNumber(AASettings.BandLimits[0].X, &FNumberFormattingOptions::DefaultNoGrouping()));
		BandChannelOneMax->SetText(FText::AsNumber(AASettings.BandLimits[0].Y, &FNumberFormattingOptions::DefaultNoGrouping()));
		BandChannelTwoMin->SetText(FText::AsNumber(AASettings.BandLimits[1].X, &FNumberFormattingOptions::DefaultNoGrouping()));
		BandChannelTwoMax->SetText(FText::AsNumber(AASettings.BandLimits[1].Y, &FNumberFormattingOptions::DefaultNoGrouping()));
		BandChannelThreeMin->SetText(FText::AsNumber(AASettings.BandLimits[2].X, &FNumberFormattingOptions::DefaultNoGrouping()));
		BandChannelThreeMax->SetText(FText::AsNumber(AASettings.BandLimits[2].Y, &FNumberFormattingOptions::DefaultNoGrouping()));
		BandChannelFourMin->SetText(FText::AsNumber(AASettings.BandLimits[3].X, &FNumberFormattingOptions::DefaultNoGrouping()));
		BandChannelFourMax->SetText(FText::AsNumber(AASettings.BandLimits[3].Y, &FNumberFormattingOptions::DefaultNoGrouping()));

		ThresholdSliderOne->SetValue(AASettings.BandLimitsThreshold[0]);
		ThresholdSliderTwo->SetValue(AASettings.BandLimitsThreshold[1]);
		ThresholdSliderThree->SetValue(AASettings.BandLimitsThreshold[2]);
		ThresholdSliderFour->SetValue(AASettings.BandLimitsThreshold[3]);

		ThresholdValueOne->SetText(FText::AsNumber(AASettings.BandLimitsThreshold[0], &FNumberFormattingOptions::DefaultNoGrouping()));
		ThresholdValueTwo->SetText(FText::AsNumber(AASettings.BandLimitsThreshold[1], &FNumberFormattingOptions::DefaultNoGrouping()));
		ThresholdValueThree->SetText(FText::AsNumber(AASettings.BandLimitsThreshold[2], &FNumberFormattingOptions::DefaultNoGrouping()));
		ThresholdValueFour->SetText(FText::AsNumber(AASettings.BandLimitsThreshold[3], &FNumberFormattingOptions::DefaultNoGrouping()));

	}

	if (AASettings.NumBandChannels == 3)
	{
		BandChannelOneMin->SetText(FText::AsNumber(AASettings.BandLimits[0].X, &FNumberFormattingOptions::DefaultNoGrouping()));
		BandChannelOneMax->SetText(FText::AsNumber(AASettings.BandLimits[0].Y, &FNumberFormattingOptions::DefaultNoGrouping()));
		BandChannelTwoMin->SetText(FText::AsNumber(AASettings.BandLimits[1].X, &FNumberFormattingOptions::DefaultNoGrouping()));
		BandChannelTwoMax->SetText(FText::AsNumber(AASettings.BandLimits[1].Y, &FNumberFormattingOptions::DefaultNoGrouping()));
		BandChannelThreeMin->SetText(FText::AsNumber(AASettings.BandLimits[2].X, &FNumberFormattingOptions::DefaultNoGrouping()));
		BandChannelThreeMax->SetText(FText::AsNumber(AASettings.BandLimits[2].Y, &FNumberFormattingOptions::DefaultNoGrouping()));

		ThresholdSliderOne->SetValue(AASettings.BandLimitsThreshold[0]);
		ThresholdSliderTwo->SetValue(AASettings.BandLimitsThreshold[1]);
		ThresholdSliderThree->SetValue(AASettings.BandLimitsThreshold[2]);

		ThresholdValueOne->SetText(FText::AsNumber(AASettings.BandLimitsThreshold[0], &FNumberFormattingOptions::DefaultNoGrouping()));
		ThresholdValueTwo->SetText(FText::AsNumber(AASettings.BandLimitsThreshold[1], &FNumberFormattingOptions::DefaultNoGrouping()));
		ThresholdValueThree->SetText(FText::AsNumber(AASettings.BandLimitsThreshold[2], &FNumberFormattingOptions::DefaultNoGrouping()));
	}

	if (AASettings.NumBandChannels == 2)
	{
		BandChannelOneMin->SetText(FText::AsNumber(AASettings.BandLimits[0].X, &FNumberFormattingOptions::DefaultNoGrouping()));
		BandChannelOneMax->SetText(FText::AsNumber(AASettings.BandLimits[0].Y, &FNumberFormattingOptions::DefaultNoGrouping()));
		BandChannelTwoMin->SetText(FText::AsNumber(AASettings.BandLimits[1].X, &FNumberFormattingOptions::DefaultNoGrouping()));
		BandChannelTwoMax->SetText(FText::AsNumber(AASettings.BandLimits[1].Y, &FNumberFormattingOptions::DefaultNoGrouping()));

		ThresholdSliderOne->SetValue(AASettings.BandLimitsThreshold[0]);
		ThresholdSliderTwo->SetValue(AASettings.BandLimitsThreshold[1]);

		ThresholdValueOne->SetText(FText::AsNumber(AASettings.BandLimitsThreshold[0], &FNumberFormattingOptions::DefaultNoGrouping()));
		ThresholdValueTwo->SetText(FText::AsNumber(AASettings.BandLimitsThreshold[1], &FNumberFormattingOptions::DefaultNoGrouping()));
	}

	if (AASettings.NumBandChannels == 1)
	{
		BandChannelOneMin->SetText(FText::AsNumber(AASettings.BandLimits[0].X, &FNumberFormattingOptions::DefaultNoGrouping()));
		BandChannelOneMax->SetText(FText::AsNumber(AASettings.BandLimits[0].Y, &FNumberFormattingOptions::DefaultNoGrouping()));

		ThresholdSliderOne->SetValue(AASettings.BandLimitsThreshold[0]);

		ThresholdValueOne->SetText(FText::AsNumber(AASettings.BandLimitsThreshold[0], &FNumberFormattingOptions::DefaultNoGrouping()));
	}

	TimeWindowSlider->SetValue(AASettings.TimeWindow);
	TimeWindowValue->SetText(FText::AsNumber(AASettings.TimeWindow, &FNumberFormattingOptions::DefaultNoGrouping()));
}

void UAASettingsWidget::SaveAASettings()
{
	AASettings.NumBandChannels = NumBandChannels->GetSelectedIndex() + 1.f;

	if (AASettings.NumBandChannels == 4)
	{
		AASettings.BandLimits.SetNum(4, true);
		AASettings.BandLimits[0].X = FCString::Atof(*BandChannelOneMin->GetText().ToString());
		AASettings.BandLimits[0].Y = FCString::Atof(*BandChannelOneMax->GetText().ToString());
		AASettings.BandLimits[1].X = FCString::Atof(*BandChannelTwoMin->GetText().ToString());
		AASettings.BandLimits[1].Y = FCString::Atof(*BandChannelTwoMax->GetText().ToString());
		AASettings.BandLimits[2].X = FCString::Atof(*BandChannelThreeMin->GetText().ToString());
		AASettings.BandLimits[2].Y = FCString::Atof(*BandChannelThreeMax->GetText().ToString());
		AASettings.BandLimits[3].X = FCString::Atof(*BandChannelFourMin->GetText().ToString());
		AASettings.BandLimits[3].Y = FCString::Atof(*BandChannelFourMax->GetText().ToString());

		AASettings.BandLimitsThreshold.SetNum(4, true);
		AASettings.BandLimitsThreshold[0] = ThresholdSliderOne->GetValue();
		AASettings.BandLimitsThreshold[1] = ThresholdSliderTwo->GetValue();
		AASettings.BandLimitsThreshold[2] = ThresholdSliderThree->GetValue();
		AASettings.BandLimitsThreshold[3] = ThresholdSliderFour->GetValue();
	}
	else if (AASettings.NumBandChannels == 3)
	{
		AASettings.BandLimits.SetNum(3, true);
		AASettings.BandLimits[0].X = FCString::Atof(*BandChannelOneMin->GetText().ToString());
		AASettings.BandLimits[0].Y = FCString::Atof(*BandChannelOneMax->GetText().ToString());
		AASettings.BandLimits[1].X = FCString::Atof(*BandChannelTwoMin->GetText().ToString());
		AASettings.BandLimits[1].Y = FCString::Atof(*BandChannelTwoMax->GetText().ToString());
		AASettings.BandLimits[2].X = FCString::Atof(*BandChannelThreeMin->GetText().ToString());
		AASettings.BandLimits[2].Y = FCString::Atof(*BandChannelThreeMax->GetText().ToString());

		AASettings.BandLimitsThreshold.SetNum(3, true);
		AASettings.BandLimitsThreshold[0] = ThresholdSliderOne->GetValue();
		AASettings.BandLimitsThreshold[1] = ThresholdSliderTwo->GetValue();
		AASettings.BandLimitsThreshold[2] = ThresholdSliderThree->GetValue();
	}
	else if (AASettings.NumBandChannels == 2)
	{
		AASettings.BandLimits.SetNum(2, true);
		AASettings.BandLimits[0].X = FCString::Atof(*BandChannelOneMin->GetText().ToString());
		AASettings.BandLimits[0].Y = FCString::Atof(*BandChannelOneMax->GetText().ToString());
		AASettings.BandLimits[1].X = FCString::Atof(*BandChannelTwoMin->GetText().ToString());
		AASettings.BandLimits[1].Y = FCString::Atof(*BandChannelTwoMax->GetText().ToString());

		AASettings.BandLimitsThreshold.SetNum(2, true);
		AASettings.BandLimitsThreshold[0] = ThresholdSliderOne->GetValue();
		AASettings.BandLimitsThreshold[1] = ThresholdSliderTwo->GetValue();
	}
	else if (AASettings.NumBandChannels == 1)
	{
		AASettings.BandLimits.SetNum(1, true);
		AASettings.BandLimits[0].X = FCString::Atof(*BandChannelOneMin->GetText().ToString());
		AASettings.BandLimits[0].Y = FCString::Atof(*BandChannelOneMax->GetText().ToString());

		AASettings.BandLimitsThreshold.SetNum(1, true);
		AASettings.BandLimitsThreshold[0] = ThresholdSliderOne->GetValue();
	}

	AASettings.TimeWindow = TimeWindowSlider->GetValue();
	Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()))->SaveAASettings(AASettings);
}

void UAASettingsWidget::ResetAASettings()
{
	AASettings.ResetStruct();
	PopulateAASettings();
}
