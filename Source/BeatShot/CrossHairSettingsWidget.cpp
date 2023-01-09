// Fill out your copyright notice in the Description page of Project Settings.


#include "CrossHairSettingsWidget.h"

#include "CrossHairWidget.h"
#include "DefaultGameInstance.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/Image.h"
#include "Components/Slider.h"
#include "Kismet/GameplayStatics.h"

void UCrossHairSettingsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ColorAValue->OnTextCommitted.AddDynamic(this, &UCrossHairSettingsWidget::OnColorAValueChange);
	ColorRValue->OnTextCommitted.AddDynamic(this, &UCrossHairSettingsWidget::OnColorRValueChange);
	ColorGValue->OnTextCommitted.AddDynamic(this, &UCrossHairSettingsWidget::OnColorGValueChange);
	ColorBValue->OnTextCommitted.AddDynamic(this, &UCrossHairSettingsWidget::OnColorBValueChange);
	HexValue->OnTextCommitted.AddDynamic(this, &UCrossHairSettingsWidget::OnHexValueChange);
	InnerOffsetValue->OnTextCommitted.AddDynamic(this, &UCrossHairSettingsWidget::OnInnerOffsetValueChange);
	LineLengthValue->OnTextCommitted.AddDynamic(this, &UCrossHairSettingsWidget::OnLineLengthValueChange);
	LineWidthValue->OnTextCommitted.AddDynamic(this, &UCrossHairSettingsWidget::OnLineWidthValueChange);
	OutlineOpacityValue->OnTextCommitted.AddDynamic(this, &UCrossHairSettingsWidget::OnOutlineOpacityValueChange);
	OutlineWidthValue->OnTextCommitted.AddDynamic(this, &UCrossHairSettingsWidget::OnOutlineWidthValueChange);

	ColorASlider->OnValueChanged.AddDynamic(this, &UCrossHairSettingsWidget::OnColorASliderChange);
	ColorRSlider->OnValueChanged.AddDynamic(this, &UCrossHairSettingsWidget::OnColorRSliderChange);
	ColorGSlider->OnValueChanged.AddDynamic(this, &UCrossHairSettingsWidget::OnColorGSliderChange);
	ColorBSlider->OnValueChanged.AddDynamic(this, &UCrossHairSettingsWidget::OnColorBSliderChange);
	InnerOffsetSlider->OnValueChanged.AddDynamic(this, &UCrossHairSettingsWidget::OnInnerOffsetSliderChange);
	LineLengthSlider->OnValueChanged.AddDynamic(this, &UCrossHairSettingsWidget::OnLineLengthSliderChange);
	LineWidthSlider->OnValueChanged.AddDynamic(this, &UCrossHairSettingsWidget::OnLineWidthSliderChange);
	OutlineOpacitySlider->OnValueChanged.AddDynamic(this, &UCrossHairSettingsWidget::OnOutlineOpacitySliderChange);
	OutlineWidthSlider->OnValueChanged.AddDynamic(this, &UCrossHairSettingsWidget::OnOutlineWidthSliderChange);

	ResetToDefaultButton->OnClicked.AddDynamic(this, &UCrossHairSettingsWidget::OnResetToDefaultButtonClicked);
	RevertCrossHairButton->OnClicked.AddDynamic(this, &UCrossHairSettingsWidget::OnRevertCrossHairButtonClicked);
	SaveCrossHairButton->OnClicked.AddDynamic(this, &UCrossHairSettingsWidget::OnSaveCrossHairButtonClicked);

	InitialCrossHairSettings = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()))->
		LoadPlayerSettings();
	NewCrossHairSettings = InitialCrossHairSettings;
	SetCrossHairOptions(InitialCrossHairSettings);
}

void UCrossHairSettingsWidget::SetCrossHairOptions(const FPlayerSettings& CrossHairSettings)
{
	ColorAValue->SetText(FText::AsNumber(roundf(CrossHairSettings.CrossHairColor.A * 100)));
	ColorRValue->SetText(FText::AsNumber(roundf(CrossHairSettings.CrossHairColor.R * 255)));
	ColorGValue->SetText(FText::AsNumber(roundf(CrossHairSettings.CrossHairColor.G * 255)));
	ColorBValue->SetText(FText::AsNumber(roundf(CrossHairSettings.CrossHairColor.B * 255)));
	HexValue->SetText(FText::FromString(CrossHairSettings.CrossHairColor.ToFColor(false).ToHex()));
	InnerOffsetValue->SetText(FText::AsNumber(CrossHairSettings.InnerOffset));
	LineLengthValue->SetText(FText::AsNumber(CrossHairSettings.LineLength));
	LineWidthValue->SetText(FText::AsNumber(CrossHairSettings.LineWidth));
	OutlineOpacityValue->SetText(FText::AsNumber(roundf(CrossHairSettings.OutlineOpacity * 100)));
	OutlineWidthValue->SetText(FText::AsNumber(CrossHairSettings.OutlineWidth));

	ColorASlider->SetValue(roundf(CrossHairSettings.CrossHairColor.A * 100));
	ColorRSlider->SetValue(roundf(CrossHairSettings.CrossHairColor.R * 255));
	ColorGSlider->SetValue(roundf(CrossHairSettings.CrossHairColor.G * 255));
	ColorBSlider->SetValue(roundf(CrossHairSettings.CrossHairColor.B * 255));
	InnerOffsetSlider->SetValue(CrossHairSettings.InnerOffset);
	LineLengthSlider->SetValue(CrossHairSettings.LineLength);
	LineWidthSlider->SetValue(CrossHairSettings.LineWidth);
	OutlineOpacitySlider->SetValue(roundf(CrossHairSettings.OutlineOpacity * 100));
	OutlineWidthSlider->SetValue(CrossHairSettings.OutlineWidth);

	ColorPreview->SetColorAndOpacity(CrossHairSettings.CrossHairColor);
	CrossHairWidget->InitializeCrossHair(CrossHairSettings);
}

void UCrossHairSettingsWidget::OnColorAValueChange(const FText& NewValue, ETextCommit::Type CommitType)
{
	const float RoundedValue = roundf(FCString::Atof(*NewValue.ToString()));
	NewCrossHairSettings.CrossHairColor.A = RoundedValue / 100;
	ColorASlider->SetValue(RoundedValue);

	ColorPreview->SetColorAndOpacity(NewCrossHairSettings.CrossHairColor);
	HexValue->SetText(FText::FromString(NewCrossHairSettings.CrossHairColor.ToFColor(false).ToHex()));
	CrossHairWidget->SetImageColor(NewCrossHairSettings.CrossHairColor);
}

void UCrossHairSettingsWidget::OnColorRValueChange(const FText& NewValue, ETextCommit::Type CommitType)
{
	const float RoundedValue = roundf(FCString::Atof(*NewValue.ToString()));
	NewCrossHairSettings.CrossHairColor.R = RoundedValue / 255;
	ColorRSlider->SetValue(RoundedValue);

	ColorPreview->SetColorAndOpacity(NewCrossHairSettings.CrossHairColor);
	HexValue->SetText(FText::FromString(NewCrossHairSettings.CrossHairColor.ToFColor(false).ToHex()));
	CrossHairWidget->SetImageColor(NewCrossHairSettings.CrossHairColor);
}

void UCrossHairSettingsWidget::OnColorGValueChange(const FText& NewValue, ETextCommit::Type CommitType)
{
	const float RoundedValue = roundf(FCString::Atof(*NewValue.ToString()));
	NewCrossHairSettings.CrossHairColor.G = RoundedValue / 255;
	ColorGSlider->SetValue(RoundedValue);

	ColorPreview->SetColorAndOpacity(NewCrossHairSettings.CrossHairColor);
	HexValue->SetText(FText::FromString(NewCrossHairSettings.CrossHairColor.ToFColor(false).ToHex()));
	CrossHairWidget->SetImageColor(NewCrossHairSettings.CrossHairColor);
}

void UCrossHairSettingsWidget::OnColorBValueChange(const FText& NewValue, ETextCommit::Type CommitType)
{
	const float RoundedValue = roundf(FCString::Atof(*NewValue.ToString()));
	NewCrossHairSettings.CrossHairColor.B = RoundedValue / 255;
	ColorBSlider->SetValue(RoundedValue);

	ColorPreview->SetColorAndOpacity(NewCrossHairSettings.CrossHairColor);
	HexValue->SetText(FText::FromString(NewCrossHairSettings.CrossHairColor.ToFColor(false).ToHex()));
	CrossHairWidget->SetImageColor(NewCrossHairSettings.CrossHairColor);
}

void UCrossHairSettingsWidget::OnHexValueChange(const FText& NewValue, ETextCommit::Type CommitType)
{
	NewCrossHairSettings.CrossHairColor = FLinearColor(FColor::FromHex(NewValue.ToString()));
	ColorAValue->SetText(FText::AsNumber(NewCrossHairSettings.CrossHairColor.A));
	ColorRValue->SetText(FText::AsNumber(NewCrossHairSettings.CrossHairColor.R));
	ColorGValue->SetText(FText::AsNumber(NewCrossHairSettings.CrossHairColor.G));
	ColorBValue->SetText(FText::AsNumber(NewCrossHairSettings.CrossHairColor.B));
	ColorASlider->SetValue(NewCrossHairSettings.CrossHairColor.A);
	ColorRSlider->SetValue(NewCrossHairSettings.CrossHairColor.R);
	ColorGSlider->SetValue(NewCrossHairSettings.CrossHairColor.G);
	ColorBSlider->SetValue(NewCrossHairSettings.CrossHairColor.B);
	ColorPreview->SetColorAndOpacity(NewCrossHairSettings.CrossHairColor);
	CrossHairWidget->SetImageColor(NewCrossHairSettings.CrossHairColor);
}

void UCrossHairSettingsWidget::OnInnerOffsetValueChange(const FText& NewValue, ETextCommit::Type CommitType)
{
	const int32 Value = roundf(FCString::Atof(*NewValue.ToString()));
	NewCrossHairSettings.InnerOffset = Value;
	InnerOffsetSlider->SetValue(Value);
	CrossHairWidget->SetInnerOffset(Value);
}

void UCrossHairSettingsWidget::OnLineLengthValueChange(const FText& NewValue, ETextCommit::Type CommitType)
{
	const int32 Value = roundf(FCString::Atof(*NewValue.ToString()));
	NewCrossHairSettings.LineLength = Value;
	LineLengthSlider->SetValue(Value);
	CrossHairWidget->SetLineLength(Value);
}

void UCrossHairSettingsWidget::OnLineWidthValueChange(const FText& NewValue, ETextCommit::Type CommitType)
{
	const int32 Value = roundf(FCString::Atof(*NewValue.ToString()));
	NewCrossHairSettings.LineWidth = Value;
	LineWidthSlider->SetValue(Value);
	CrossHairWidget->SetLineWidth(Value);
}

void UCrossHairSettingsWidget::OnOutlineWidthValueChange(const FText& NewValue, ETextCommit::Type CommitType)
{
	const int32 Value = roundf(FCString::Atof(*NewValue.ToString()));
	NewCrossHairSettings.OutlineWidth = Value;
	OutlineWidthSlider->SetValue(Value);
	CrossHairWidget->SetOutlineWidth(Value);
}

void UCrossHairSettingsWidget::OnOutlineOpacityValueChange(const FText& NewValue, ETextCommit::Type CommitType)
{
	const float RoundedValue = roundf(FCString::Atof(*NewValue.ToString()));
	NewCrossHairSettings.OutlineOpacity = RoundedValue / 100;
	OutlineOpacitySlider->SetValue(RoundedValue);
	CrossHairWidget->SetOutlineOpacity(RoundedValue / 100);
}

void UCrossHairSettingsWidget::OnColorASliderChange(const float NewValue)
{
	const float RoundedValue = roundf(NewValue);
	ColorAValue->SetText(FText::AsNumber(RoundedValue));
	NewCrossHairSettings.CrossHairColor.A = RoundedValue / 100;

	ColorPreview->SetColorAndOpacity(NewCrossHairSettings.CrossHairColor);
	HexValue->SetText(FText::FromString(NewCrossHairSettings.CrossHairColor.ToFColor(false).ToHex()));
	CrossHairWidget->SetImageColor(NewCrossHairSettings.CrossHairColor);
}

void UCrossHairSettingsWidget::OnColorRSliderChange(const float NewValue)
{
	const float RoundedValue = roundf(NewValue);
	ColorRValue->SetText(FText::AsNumber(RoundedValue));
	NewCrossHairSettings.CrossHairColor.R = RoundedValue / 255;

	ColorPreview->SetColorAndOpacity(NewCrossHairSettings.CrossHairColor);
	HexValue->SetText(FText::FromString(NewCrossHairSettings.CrossHairColor.ToFColor(false).ToHex()));
	CrossHairWidget->SetImageColor(NewCrossHairSettings.CrossHairColor);
}

void UCrossHairSettingsWidget::OnColorGSliderChange(const float NewValue)
{
	const float RoundedValue = roundf(NewValue);
	ColorGValue->SetText(FText::AsNumber(RoundedValue));
	NewCrossHairSettings.CrossHairColor.G = RoundedValue / 255;

	ColorPreview->SetColorAndOpacity(NewCrossHairSettings.CrossHairColor);
	HexValue->SetText(FText::FromString(NewCrossHairSettings.CrossHairColor.ToFColor(false).ToHex()));
	CrossHairWidget->SetImageColor(NewCrossHairSettings.CrossHairColor);
}

void UCrossHairSettingsWidget::OnColorBSliderChange(const float NewValue)
{
	const float RoundedValue = roundf(NewValue);
	ColorBValue->SetText(FText::AsNumber(RoundedValue));
	NewCrossHairSettings.CrossHairColor.B = RoundedValue / 255;

	ColorPreview->SetColorAndOpacity(NewCrossHairSettings.CrossHairColor);
	HexValue->SetText(FText::FromString(NewCrossHairSettings.CrossHairColor.ToFColor(false).ToHex()));
	CrossHairWidget->SetImageColor(NewCrossHairSettings.CrossHairColor);
}

void UCrossHairSettingsWidget::OnInnerOffsetSliderChange(const float NewValue)
{
	const float RoundedValue = roundf(NewValue);
	NewCrossHairSettings.InnerOffset = RoundedValue;
	InnerOffsetValue->SetText(FText::AsNumber(RoundedValue));
	CrossHairWidget->SetInnerOffset(RoundedValue);
}

void UCrossHairSettingsWidget::OnLineLengthSliderChange(const float NewValue)
{
	const float RoundedValue = roundf(NewValue);
	NewCrossHairSettings.LineLength = RoundedValue;
	LineLengthValue->SetText(FText::AsNumber(RoundedValue));
	CrossHairWidget->SetLineLength(RoundedValue);
}

void UCrossHairSettingsWidget::OnLineWidthSliderChange(const float NewValue)
{
	const float RoundedValue = roundf(NewValue);
	NewCrossHairSettings.LineWidth = RoundedValue;
	LineWidthValue->SetText(FText::AsNumber(RoundedValue));
	CrossHairWidget->SetLineWidth(RoundedValue);
}

void UCrossHairSettingsWidget::OnOutlineOpacitySliderChange(const float NewValue)
{
	const float RoundedValue = roundf(NewValue);
	NewCrossHairSettings.OutlineOpacity = RoundedValue / 100;
	OutlineOpacityValue->SetText(FText::AsNumber(RoundedValue));
	CrossHairWidget->SetOutlineOpacity(RoundedValue / 100);
}

void UCrossHairSettingsWidget::OnOutlineWidthSliderChange(const float NewValue)
{
	const float RoundedValue = roundf(NewValue);
	NewCrossHairSettings.OutlineWidth = RoundedValue;
	OutlineWidthValue->SetText(FText::AsNumber(RoundedValue));
	CrossHairWidget->SetOutlineWidth(RoundedValue);
}

void UCrossHairSettingsWidget::OnResetToDefaultButtonClicked()
{
	NewCrossHairSettings.ResetCrossHair();
	SetCrossHairOptions(NewCrossHairSettings);
}

void UCrossHairSettingsWidget::OnRevertCrossHairButtonClicked()
{
	NewCrossHairSettings = InitialCrossHairSettings;
	SetCrossHairOptions(NewCrossHairSettings);
}

void UCrossHairSettingsWidget::OnSaveCrossHairButtonClicked()
{
	/** Load settings again in case the user changed other settings before navigating to crosshair settings */
	const UDefaultGameInstance* GI = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	FPlayerSettings MostRecentSettings = GI->LoadPlayerSettings();
	MostRecentSettings.LineWidth = NewCrossHairSettings.LineWidth;
	MostRecentSettings.LineLength = NewCrossHairSettings.LineLength;
	MostRecentSettings.InnerOffset = NewCrossHairSettings.InnerOffset;
	MostRecentSettings.CrossHairColor = NewCrossHairSettings.CrossHairColor;
	MostRecentSettings.OutlineOpacity = NewCrossHairSettings.OutlineOpacity;
	MostRecentSettings.OutlineWidth = NewCrossHairSettings.OutlineWidth;
	Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()))->
		SavePlayerSettings(MostRecentSettings);
}
