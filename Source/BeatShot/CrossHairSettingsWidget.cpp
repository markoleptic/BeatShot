// Fill out your copyright notice in the Description page of Project Settings.


#include "CrossHairSettingsWidget.h"

#include "CrossHairWidget.h"
#include "DefaultGameInstance.h"
#include "SavedTextWidget.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/Image.h"
#include "Math/Color.h"
#include "Components/Slider.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetStringLibrary.h"

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
	const float SnappedValue = OnEditableTextBoxChanged(NewValue, ColorAValue, ColorASlider, 1, 0, 100);
	NewCrossHairSettings.CrossHairColor.A = FMath::GridSnap(SnappedValue / 100, 0.01);
	ColorPreview->SetColorAndOpacity(NewCrossHairSettings.CrossHairColor);
	HexValue->SetText(FText::FromString(NewCrossHairSettings.CrossHairColor.ToFColor(false).ToHex()));
	CrossHairWidget->SetImageColor(NewCrossHairSettings.CrossHairColor);
}

void UCrossHairSettingsWidget::OnColorRValueChange(const FText& NewValue, ETextCommit::Type CommitType)
{
	const float SnappedValue = OnEditableTextBoxChanged(NewValue, ColorRValue, ColorRSlider, 1, 0, 255);
	NewCrossHairSettings.CrossHairColor.R = SnappedValue / 255;
	ColorPreview->SetColorAndOpacity(NewCrossHairSettings.CrossHairColor);
	HexValue->SetText(FText::FromString(NewCrossHairSettings.CrossHairColor.ToFColor(false).ToHex()));
	CrossHairWidget->SetImageColor(NewCrossHairSettings.CrossHairColor);
}

void UCrossHairSettingsWidget::OnColorGValueChange(const FText& NewValue, ETextCommit::Type CommitType)
{
	const float SnappedValue = OnEditableTextBoxChanged(NewValue, ColorGValue, ColorGSlider, 1, 0, 255);
	NewCrossHairSettings.CrossHairColor.G = SnappedValue / 255;
	ColorPreview->SetColorAndOpacity(NewCrossHairSettings.CrossHairColor);
	HexValue->SetText(FText::FromString(NewCrossHairSettings.CrossHairColor.ToFColor(false).ToHex()));
	CrossHairWidget->SetImageColor(NewCrossHairSettings.CrossHairColor);
}

void UCrossHairSettingsWidget::OnColorBValueChange(const FText& NewValue, ETextCommit::Type CommitType)
{
	const float SnappedValue = OnEditableTextBoxChanged(NewValue, ColorBValue, ColorBSlider, 1, 0, 255);
	NewCrossHairSettings.CrossHairColor.B = SnappedValue / 255;
	ColorPreview->SetColorAndOpacity(NewCrossHairSettings.CrossHairColor);
	HexValue->SetText(FText::FromString(NewCrossHairSettings.CrossHairColor.ToFColor(false).ToHex()));
	CrossHairWidget->SetImageColor(NewCrossHairSettings.CrossHairColor);
}

void UCrossHairSettingsWidget::OnHexValueChange(const FText& NewValue, ETextCommit::Type CommitType)
{
	const FString String = FColor::FromHex(NewValue.ToString()).ToString();
	NewCrossHairSettings.CrossHairColor.InitFromString(String);
	NewCrossHairSettings.CrossHairColor.R = NewCrossHairSettings.CrossHairColor.R / 255;
	NewCrossHairSettings.CrossHairColor.G = NewCrossHairSettings.CrossHairColor.G / 255;
	NewCrossHairSettings.CrossHairColor.B = NewCrossHairSettings.CrossHairColor.B / 255;
	NewCrossHairSettings.CrossHairColor.A = NewCrossHairSettings.CrossHairColor.A / 255;
	ColorAValue->SetText(FText::AsNumber(roundf(NewCrossHairSettings.CrossHairColor.A * 100)));
	ColorRValue->SetText(FText::AsNumber(NewCrossHairSettings.CrossHairColor.R * 255));
	ColorGValue->SetText(FText::AsNumber(NewCrossHairSettings.CrossHairColor.G * 255));
	ColorBValue->SetText(FText::AsNumber(NewCrossHairSettings.CrossHairColor.B * 255));
	ColorASlider->SetValue(roundf(NewCrossHairSettings.CrossHairColor.A * 100));
	ColorRSlider->SetValue(NewCrossHairSettings.CrossHairColor.R * 255);
	ColorGSlider->SetValue(NewCrossHairSettings.CrossHairColor.G * 255);
	ColorBSlider->SetValue(NewCrossHairSettings.CrossHairColor.B * 255);
	ColorPreview->SetColorAndOpacity(NewCrossHairSettings.CrossHairColor);
	CrossHairWidget->SetImageColor(NewCrossHairSettings.CrossHairColor);
	UE_LOG(LogTemp, Display, TEXT("NewCrossHairSettings.CrossHairColor %s"), *NewCrossHairSettings.CrossHairColor.ToString());
}

void UCrossHairSettingsWidget::OnInnerOffsetValueChange(const FText& NewValue, ETextCommit::Type CommitType)
{
	const float SnappedValue = OnEditableTextBoxChanged(NewValue, InnerOffsetValue, InnerOffsetSlider, 1, 0, 30);
	NewCrossHairSettings.InnerOffset = SnappedValue;
	CrossHairWidget->SetInnerOffset(SnappedValue);
}

void UCrossHairSettingsWidget::OnLineLengthValueChange(const FText& NewValue, ETextCommit::Type CommitType)
{
	const float SnappedValue = OnEditableTextBoxChanged(NewValue, LineLengthValue, LineLengthSlider, 1, 0, 100);
	NewCrossHairSettings.LineLength = SnappedValue;
	CrossHairWidget->SetLineLength(SnappedValue);
}

void UCrossHairSettingsWidget::OnLineWidthValueChange(const FText& NewValue, ETextCommit::Type CommitType)
{
	const float SnappedValue = OnEditableTextBoxChanged(NewValue, LineWidthValue, LineWidthSlider, 1, 0, 100);
	NewCrossHairSettings.LineWidth = SnappedValue;
	CrossHairWidget->SetLineWidth(SnappedValue);
}

void UCrossHairSettingsWidget::OnOutlineWidthValueChange(const FText& NewValue, ETextCommit::Type CommitType)
{
	const float SnappedValue = OnEditableTextBoxChanged(NewValue, OutlineWidthValue, OutlineWidthSlider, 1, 0, 50);
	NewCrossHairSettings.OutlineWidth = SnappedValue;
	CrossHairWidget->SetOutlineWidth(SnappedValue);
}

void UCrossHairSettingsWidget::OnOutlineOpacityValueChange(const FText& NewValue, ETextCommit::Type CommitType)
{
	const float SnappedValue = OnEditableTextBoxChanged(NewValue, OutlineOpacityValue, OutlineOpacitySlider, 1, 0, 100);
	NewCrossHairSettings.OutlineOpacity = SnappedValue / 100;
	CrossHairWidget->SetOutlineOpacity(SnappedValue / 100);
}

void UCrossHairSettingsWidget::OnColorASliderChange(const float NewValue)
{
	const float SnappedValue = OnSliderChanged(NewValue, ColorAValue, 1);
	NewCrossHairSettings.CrossHairColor.A = SnappedValue / 100;
	ColorPreview->SetColorAndOpacity(NewCrossHairSettings.CrossHairColor);
	HexValue->SetText(FText::FromString(NewCrossHairSettings.CrossHairColor.ToFColor(false).ToHex()));
	CrossHairWidget->SetImageColor(NewCrossHairSettings.CrossHairColor);
}

void UCrossHairSettingsWidget::OnColorRSliderChange(const float NewValue)
{
	const float SnappedValue = OnSliderChanged(NewValue, ColorRValue, 1);
	NewCrossHairSettings.CrossHairColor.R = SnappedValue / 255;
	ColorPreview->SetColorAndOpacity(NewCrossHairSettings.CrossHairColor);
	HexValue->SetText(FText::FromString(NewCrossHairSettings.CrossHairColor.ToFColor(false).ToHex()));
	CrossHairWidget->SetImageColor(NewCrossHairSettings.CrossHairColor);
}

void UCrossHairSettingsWidget::OnColorGSliderChange(const float NewValue)
{
	const float SnappedValue = OnSliderChanged(NewValue, ColorGValue, 1);
	NewCrossHairSettings.CrossHairColor.G = SnappedValue / 255;
	ColorPreview->SetColorAndOpacity(NewCrossHairSettings.CrossHairColor);
	HexValue->SetText(FText::FromString(NewCrossHairSettings.CrossHairColor.ToFColor(false).ToHex()));
	CrossHairWidget->SetImageColor(NewCrossHairSettings.CrossHairColor);
}

void UCrossHairSettingsWidget::OnColorBSliderChange(const float NewValue)
{
	const float SnappedValue = OnSliderChanged(NewValue, ColorBValue, 1);
	NewCrossHairSettings.CrossHairColor.B = SnappedValue / 255;
	ColorPreview->SetColorAndOpacity(NewCrossHairSettings.CrossHairColor);
	HexValue->SetText(FText::FromString(NewCrossHairSettings.CrossHairColor.ToFColor(false).ToHex()));
	CrossHairWidget->SetImageColor(NewCrossHairSettings.CrossHairColor);
}

void UCrossHairSettingsWidget::OnInnerOffsetSliderChange(const float NewValue)
{
	const float SnappedValue = OnSliderChanged(NewValue, InnerOffsetValue, 1);
	NewCrossHairSettings.InnerOffset = SnappedValue;
	CrossHairWidget->SetInnerOffset(SnappedValue);
}

void UCrossHairSettingsWidget::OnLineLengthSliderChange(const float NewValue)
{
	const float SnappedValue = OnSliderChanged(NewValue, LineLengthValue, 1);
	NewCrossHairSettings.LineLength = SnappedValue;
	CrossHairWidget->SetLineLength(SnappedValue);
}

void UCrossHairSettingsWidget::OnLineWidthSliderChange(const float NewValue)
{
	const float SnappedValue = OnSliderChanged(NewValue, LineWidthValue, 1);
	NewCrossHairSettings.LineWidth = SnappedValue;
	CrossHairWidget->SetLineWidth(SnappedValue);
}

void UCrossHairSettingsWidget::OnOutlineOpacitySliderChange(const float NewValue)
{
	const float SnappedValue = OnSliderChanged(NewValue, OutlineOpacityValue, 1);
	NewCrossHairSettings.OutlineOpacity = SnappedValue / 100;
	CrossHairWidget->SetOutlineOpacity(SnappedValue / 100);
}

void UCrossHairSettingsWidget::OnOutlineWidthSliderChange(const float NewValue)
{
	const float SnappedValue = OnSliderChanged(NewValue, OutlineWidthValue, 1);
	NewCrossHairSettings.OutlineWidth = SnappedValue;
	CrossHairWidget->SetOutlineWidth(SnappedValue);
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
	SavedTextWidget->PlayFadeInFadeOut();
	InitialCrossHairSettings = NewCrossHairSettings;
}

float  UCrossHairSettingsWidget::OnEditableTextBoxChanged(const FText& NewTextValue, UEditableTextBox* TextBoxToChange,
												USlider* SliderToChange,
												const float GridSnapSize, const float Min, const float Max)
{
	const FString StringTextValue = UKismetStringLibrary::Replace(NewTextValue.ToString(), ",", "");
	const float ClampedValue = FMath::Clamp(FCString::Atof(*StringTextValue), Min, Max);
	const float SnappedValue = FMath::GridSnap(ClampedValue, GridSnapSize);
	UE_LOG(LogTemp, Display, TEXT("Snapped: %f"),SnappedValue);
	TextBoxToChange->SetText(FText::AsNumber(SnappedValue));
	SliderToChange->SetValue(SnappedValue);
	return SnappedValue;
}

float UCrossHairSettingsWidget::OnSliderChanged(const float NewValue, UEditableTextBox* TextBoxToChange,
									   const float GridSnapSize)
{
	const float SnappedValue = FMath::GridSnap(NewValue, GridSnapSize);
	TextBoxToChange->SetText(FText::AsNumber(SnappedValue));
	return SnappedValue;
}