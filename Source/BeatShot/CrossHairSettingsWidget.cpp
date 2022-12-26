// Fill out your copyright notice in the Description page of Project Settings.


#include "CrossHairSettingsWidget.h"

#include "Crosshair.h"
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
	ColorAValue->SetText(FText::AsNumber(roundf(CrossHairSettings.CrosshairColor.A * 100)));
	ColorRValue->SetText(FText::AsNumber(roundf(CrossHairSettings.CrosshairColor.R * 255)));
	ColorGValue->SetText(FText::AsNumber(roundf(CrossHairSettings.CrosshairColor.G * 255)));
	ColorBValue->SetText(FText::AsNumber(roundf(CrossHairSettings.CrosshairColor.B * 255)));
	HexValue->SetText(FText::FromString(CrossHairSettings.CrosshairColor.ToFColor(false).ToHex()));
	InnerOffsetValue->SetText(FText::AsNumber(CrossHairSettings.InnerOffset));
	LineLengthValue->SetText(FText::AsNumber(CrossHairSettings.LineLength));
	LineWidthValue->SetText(FText::AsNumber(CrossHairSettings.LineWidth));
	OutlineOpacityValue->SetText(FText::AsNumber(roundf(CrossHairSettings.OutlineOpacity * 100)));
	OutlineWidthValue->SetText(FText::AsNumber(CrossHairSettings.OutlineWidth));

	ColorASlider->SetValue(roundf(CrossHairSettings.CrosshairColor.A * 100));
	ColorRSlider->SetValue(roundf(CrossHairSettings.CrosshairColor.R * 255));
	ColorGSlider->SetValue(roundf(CrossHairSettings.CrosshairColor.G * 255));
	ColorBSlider->SetValue(roundf(CrossHairSettings.CrosshairColor.B * 255));
	InnerOffsetSlider->SetValue(CrossHairSettings.InnerOffset);
	LineLengthSlider->SetValue(CrossHairSettings.LineLength);
	LineWidthSlider->SetValue(CrossHairSettings.LineWidth);
	OutlineOpacitySlider->SetValue(roundf(CrossHairSettings.OutlineOpacity * 100));
	OutlineWidthSlider->SetValue(CrossHairSettings.OutlineWidth);

	ColorPreview->SetColorAndOpacity(CrossHairSettings.CrosshairColor);
	CrossHairWidget->InitializeCrossHair(CrossHairSettings);
}

void UCrossHairSettingsWidget::OnColorAValueChange(const FText& NewValue, ETextCommit::Type CommitType)
{
	const float RoundedValue = roundf(FCString::Atof(*NewValue.ToString()));
	NewCrossHairSettings.CrosshairColor.A = RoundedValue / 100;
	ColorASlider->SetValue(RoundedValue);

	ColorPreview->SetColorAndOpacity(NewCrossHairSettings.CrosshairColor);
	HexValue->SetText(FText::FromString(NewCrossHairSettings.CrosshairColor.ToFColor(false).ToHex()));
	CrossHairWidget->SetImageColor(NewCrossHairSettings.CrosshairColor);
}

void UCrossHairSettingsWidget::OnColorRValueChange(const FText& NewValue, ETextCommit::Type CommitType)
{
	const float RoundedValue = roundf(FCString::Atof(*NewValue.ToString()));
	NewCrossHairSettings.CrosshairColor.R = RoundedValue / 255;
	ColorRSlider->SetValue(RoundedValue);

	ColorPreview->SetColorAndOpacity(NewCrossHairSettings.CrosshairColor);
	HexValue->SetText(FText::FromString(NewCrossHairSettings.CrosshairColor.ToFColor(false).ToHex()));
	CrossHairWidget->SetImageColor(NewCrossHairSettings.CrosshairColor);
}

void UCrossHairSettingsWidget::OnColorGValueChange(const FText& NewValue, ETextCommit::Type CommitType)
{
	const float RoundedValue = roundf(FCString::Atof(*NewValue.ToString()));
	NewCrossHairSettings.CrosshairColor.G = RoundedValue / 255;
	ColorGSlider->SetValue(RoundedValue);

	ColorPreview->SetColorAndOpacity(NewCrossHairSettings.CrosshairColor);
	HexValue->SetText(FText::FromString(NewCrossHairSettings.CrosshairColor.ToFColor(false).ToHex()));
	CrossHairWidget->SetImageColor(NewCrossHairSettings.CrosshairColor);
}

void UCrossHairSettingsWidget::OnColorBValueChange(const FText& NewValue, ETextCommit::Type CommitType)
{
	const float RoundedValue = roundf(FCString::Atof(*NewValue.ToString()));
	NewCrossHairSettings.CrosshairColor.B = RoundedValue / 255;
	ColorBSlider->SetValue(RoundedValue);

	ColorPreview->SetColorAndOpacity(NewCrossHairSettings.CrosshairColor);
	HexValue->SetText(FText::FromString(NewCrossHairSettings.CrosshairColor.ToFColor(false).ToHex()));
	CrossHairWidget->SetImageColor(NewCrossHairSettings.CrosshairColor);
}

void UCrossHairSettingsWidget::OnHexValueChange(const FText& NewValue, ETextCommit::Type CommitType)
{
	NewCrossHairSettings.CrosshairColor = FLinearColor(FColor::FromHex(NewValue.ToString()));
	ColorAValue->SetText(FText::AsNumber(NewCrossHairSettings.CrosshairColor.A));
	ColorRValue->SetText(FText::AsNumber(NewCrossHairSettings.CrosshairColor.R));
	ColorGValue->SetText(FText::AsNumber(NewCrossHairSettings.CrosshairColor.G));
	ColorBValue->SetText(FText::AsNumber(NewCrossHairSettings.CrosshairColor.B));
	ColorASlider->SetValue(NewCrossHairSettings.CrosshairColor.A);
	ColorRSlider->SetValue(NewCrossHairSettings.CrosshairColor.R);
	ColorGSlider->SetValue(NewCrossHairSettings.CrosshairColor.G);
	ColorBSlider->SetValue(NewCrossHairSettings.CrosshairColor.B);
	ColorPreview->SetColorAndOpacity(NewCrossHairSettings.CrosshairColor);
	CrossHairWidget->SetImageColor(NewCrossHairSettings.CrosshairColor);
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
	NewCrossHairSettings.CrosshairColor.A = RoundedValue / 100;

	ColorPreview->SetColorAndOpacity(NewCrossHairSettings.CrosshairColor);
	HexValue->SetText(FText::FromString(NewCrossHairSettings.CrosshairColor.ToFColor(false).ToHex()));
	CrossHairWidget->SetImageColor(NewCrossHairSettings.CrosshairColor);
}

void UCrossHairSettingsWidget::OnColorRSliderChange(const float NewValue)
{
	const float RoundedValue = roundf(NewValue);
	ColorRValue->SetText(FText::AsNumber(RoundedValue));
	NewCrossHairSettings.CrosshairColor.R = RoundedValue / 255;

	ColorPreview->SetColorAndOpacity(NewCrossHairSettings.CrosshairColor);
	HexValue->SetText(FText::FromString(NewCrossHairSettings.CrosshairColor.ToFColor(false).ToHex()));
	CrossHairWidget->SetImageColor(NewCrossHairSettings.CrosshairColor);
}

void UCrossHairSettingsWidget::OnColorGSliderChange(const float NewValue)
{
	const float RoundedValue = roundf(NewValue);
	ColorGValue->SetText(FText::AsNumber(RoundedValue));
	NewCrossHairSettings.CrosshairColor.G = RoundedValue / 255;

	ColorPreview->SetColorAndOpacity(NewCrossHairSettings.CrosshairColor);
	HexValue->SetText(FText::FromString(NewCrossHairSettings.CrosshairColor.ToFColor(false).ToHex()));
	CrossHairWidget->SetImageColor(NewCrossHairSettings.CrosshairColor);
}

void UCrossHairSettingsWidget::OnColorBSliderChange(const float NewValue)
{
	const float RoundedValue = roundf(NewValue);
	ColorBValue->SetText(FText::AsNumber(RoundedValue));
	NewCrossHairSettings.CrosshairColor.B = RoundedValue / 255;

	ColorPreview->SetColorAndOpacity(NewCrossHairSettings.CrosshairColor);
	HexValue->SetText(FText::FromString(NewCrossHairSettings.CrosshairColor.ToFColor(false).ToHex()));
	CrossHairWidget->SetImageColor(NewCrossHairSettings.CrosshairColor);
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
	Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()))->
		SavePlayerSettings(NewCrossHairSettings);
}
