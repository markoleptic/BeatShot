// Fill out your copyright notice in the Description page of Project Settings.


#include "SubMenuWidgets/CrossHairSettingsWidget.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/Image.h"
#include "Math/Color.h"
#include "Components/Slider.h"
#include "Kismet/KismetStringLibrary.h"
#include "OverlayWidgets/CrossHairWidget.h"
#include "WidgetComponents/ColorSelectWidget.h"
#include "WidgetComponents/SavedTextWidget.h"

void UCrossHairSettingsWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	InnerOffsetValue->OnTextCommitted.AddDynamic(this, &UCrossHairSettingsWidget::OnInnerOffsetValueChange);
	LineLengthValue->OnTextCommitted.AddDynamic(this, &UCrossHairSettingsWidget::OnLineLengthValueChange);
	LineWidthValue->OnTextCommitted.AddDynamic(this, &UCrossHairSettingsWidget::OnLineWidthValueChange);
	OutlineOpacityValue->OnTextCommitted.AddDynamic(this, &UCrossHairSettingsWidget::OnOutlineOpacityValueChange);
	OutlineWidthValue->OnTextCommitted.AddDynamic(this, &UCrossHairSettingsWidget::OnOutlineWidthValueChange);
	
	InnerOffsetSlider->OnValueChanged.AddDynamic(this, &UCrossHairSettingsWidget::OnInnerOffsetSliderChange);
	LineLengthSlider->OnValueChanged.AddDynamic(this, &UCrossHairSettingsWidget::OnLineLengthSliderChange);
	LineWidthSlider->OnValueChanged.AddDynamic(this, &UCrossHairSettingsWidget::OnLineWidthSliderChange);
	OutlineOpacitySlider->OnValueChanged.AddDynamic(this, &UCrossHairSettingsWidget::OnOutlineOpacitySliderChange);
	OutlineWidthSlider->OnValueChanged.AddDynamic(this, &UCrossHairSettingsWidget::OnOutlineWidthSliderChange);

	ResetToDefaultButton->OnClicked.AddDynamic(this, &UCrossHairSettingsWidget::OnResetToDefaultButtonClicked);
	RevertCrossHairButton->OnClicked.AddDynamic(this, &UCrossHairSettingsWidget::OnRevertCrossHairButtonClicked);
	SaveCrossHairButton->OnClicked.AddDynamic(this, &UCrossHairSettingsWidget::OnSaveCrossHairButtonClicked);

	ColorSelectWidget->OnColorChanged.BindUFunction(this, "OnColorChanged");

	InitialCrossHairSettings = LoadPlayerSettings();
	NewCrossHairSettings = InitialCrossHairSettings;
	SetCrossHairOptions(InitialCrossHairSettings);
}

void UCrossHairSettingsWidget::SetCrossHairOptions(const FPlayerSettings& CrossHairSettings)
{
	ColorSelectWidget->InitializeColor(CrossHairSettings.CrossHairColor);
	ColorSelectWidget->SetBorderColors(true, true);
	InnerOffsetValue->SetText(FText::AsNumber(CrossHairSettings.InnerOffset));
	LineLengthValue->SetText(FText::AsNumber(CrossHairSettings.LineLength));
	LineWidthValue->SetText(FText::AsNumber(CrossHairSettings.LineWidth));
	OutlineOpacityValue->SetText(FText::AsNumber(roundf(CrossHairSettings.OutlineOpacity * 100)));
	OutlineWidthValue->SetText(FText::AsNumber(CrossHairSettings.OutlineWidth));
	
	InnerOffsetSlider->SetValue(CrossHairSettings.InnerOffset);
	LineLengthSlider->SetValue(CrossHairSettings.LineLength);
	LineWidthSlider->SetValue(CrossHairSettings.LineWidth);
	OutlineOpacitySlider->SetValue(roundf(CrossHairSettings.OutlineOpacity * 100));
	OutlineWidthSlider->SetValue(CrossHairSettings.OutlineWidth);
	
	CrossHairWidget->InitializeCrossHair(CrossHairSettings);
}

void UCrossHairSettingsWidget::OnColorChanged(const FLinearColor& NewColor)
{
	NewCrossHairSettings.CrossHairColor = NewColor;
	CrossHairWidget->SetImageColor(NewCrossHairSettings.CrossHairColor);
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
	FPlayerSettings MostRecentSettings = LoadPlayerSettings();
	MostRecentSettings.LineWidth = NewCrossHairSettings.LineWidth;
	MostRecentSettings.LineLength = NewCrossHairSettings.LineLength;
	MostRecentSettings.InnerOffset = NewCrossHairSettings.InnerOffset;
	MostRecentSettings.CrossHairColor = NewCrossHairSettings.CrossHairColor;
	MostRecentSettings.OutlineOpacity = NewCrossHairSettings.OutlineOpacity;
	MostRecentSettings.OutlineWidth = NewCrossHairSettings.OutlineWidth;
	SavePlayerSettings(MostRecentSettings);
	if (!OnCrossHairSettingsChanged.ExecuteIfBound(MostRecentSettings))
	{
		UE_LOG(LogTemp, Display, TEXT("OnCrossHairSettingsChanged not bound."));
	}
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