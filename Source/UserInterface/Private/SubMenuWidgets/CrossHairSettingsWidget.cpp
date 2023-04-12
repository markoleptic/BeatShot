// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SubMenuWidgets/CrossHairSettingsWidget.h"
#include "Components/Button.h"
#include "UserInterface.h"
#include "Components/EditableTextBox.h"
#include "Math/Color.h"
#include "Components/Slider.h"
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
	SavedTextWidget->SetSavedText(FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "SM_Save_CrossHair"));
	InitialCrossHairSettings = LoadPlayerSettings().CrossHair;
	NewCrossHairSettings = InitialCrossHairSettings;
	SetCrossHairOptions(InitialCrossHairSettings);
}

void UCrossHairSettingsWidget::SetCrossHairOptions(const FPlayerSettings_CrossHair& CrossHairSettings)
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
	const float SnappedValue = UUserInterface::OnEditableTextBoxChanged(NewValue, InnerOffsetValue, InnerOffsetSlider, 1, 0, 30);
	NewCrossHairSettings.InnerOffset = SnappedValue;
	CrossHairWidget->SetInnerOffset(SnappedValue);
}

void UCrossHairSettingsWidget::OnLineLengthValueChange(const FText& NewValue, ETextCommit::Type CommitType)
{
	const float SnappedValue = UUserInterface::OnEditableTextBoxChanged(NewValue, LineLengthValue, LineLengthSlider, 1, 0, 100);
	NewCrossHairSettings.LineLength = SnappedValue;
	CrossHairWidget->SetLineLength(SnappedValue);
}

void UCrossHairSettingsWidget::OnLineWidthValueChange(const FText& NewValue, ETextCommit::Type CommitType)
{
	const float SnappedValue = UUserInterface::OnEditableTextBoxChanged(NewValue, LineWidthValue, LineWidthSlider, 1, 0, 100);
	NewCrossHairSettings.LineWidth = SnappedValue;
	CrossHairWidget->SetLineWidth(SnappedValue);
}

void UCrossHairSettingsWidget::OnOutlineWidthValueChange(const FText& NewValue, ETextCommit::Type CommitType)
{
	const float SnappedValue = UUserInterface::OnEditableTextBoxChanged(NewValue, OutlineWidthValue, OutlineWidthSlider, 1, 0, 50);
	NewCrossHairSettings.OutlineWidth = SnappedValue;
	CrossHairWidget->SetOutlineWidth(SnappedValue);
}

void UCrossHairSettingsWidget::OnOutlineOpacityValueChange(const FText& NewValue, ETextCommit::Type CommitType)
{
	const float SnappedValue = UUserInterface::OnEditableTextBoxChanged(NewValue, OutlineOpacityValue, OutlineOpacitySlider, 1, 0, 100);
	NewCrossHairSettings.OutlineOpacity = SnappedValue / 100;
	CrossHairWidget->SetOutlineOpacity(SnappedValue / 100);
}

void UCrossHairSettingsWidget::OnInnerOffsetSliderChange(const float NewValue)
{
	const float SnappedValue = UUserInterface::OnSliderChanged(NewValue, InnerOffsetValue, 1);
	NewCrossHairSettings.InnerOffset = SnappedValue;
	CrossHairWidget->SetInnerOffset(SnappedValue);
}

void UCrossHairSettingsWidget::OnLineLengthSliderChange(const float NewValue)
{
	const float SnappedValue = UUserInterface::OnSliderChanged(NewValue, LineLengthValue, 1);
	NewCrossHairSettings.LineLength = SnappedValue;
	CrossHairWidget->SetLineLength(SnappedValue);
}

void UCrossHairSettingsWidget::OnLineWidthSliderChange(const float NewValue)
{
	const float SnappedValue = UUserInterface::OnSliderChanged(NewValue, LineWidthValue, 1);
	NewCrossHairSettings.LineWidth = SnappedValue;
	CrossHairWidget->SetLineWidth(SnappedValue);
}

void UCrossHairSettingsWidget::OnOutlineOpacitySliderChange(const float NewValue)
{
	const float SnappedValue = UUserInterface::OnSliderChanged(NewValue, OutlineOpacityValue, 1);
	NewCrossHairSettings.OutlineOpacity = SnappedValue / 100;
	CrossHairWidget->SetOutlineOpacity(SnappedValue / 100);
}

void UCrossHairSettingsWidget::OnOutlineWidthSliderChange(const float NewValue)
{
	const float SnappedValue = UUserInterface::OnSliderChanged(NewValue, OutlineWidthValue, 1);
	NewCrossHairSettings.OutlineWidth = SnappedValue;
	CrossHairWidget->SetOutlineWidth(SnappedValue);
}

void UCrossHairSettingsWidget::OnResetToDefaultButtonClicked()
{
	NewCrossHairSettings = FPlayerSettings_CrossHair();
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
	FPlayerSettings_CrossHair Settings;
	Settings.LineWidth = NewCrossHairSettings.LineWidth;
	Settings.LineLength = NewCrossHairSettings.LineLength;
	Settings.InnerOffset = NewCrossHairSettings.InnerOffset;
	Settings.CrossHairColor = NewCrossHairSettings.CrossHairColor;
	Settings.OutlineOpacity = NewCrossHairSettings.OutlineOpacity;
	Settings.OutlineWidth = NewCrossHairSettings.OutlineWidth;
	SavePlayerSettings(Settings);
	SavedTextWidget->PlayFadeInFadeOut();
	InitialCrossHairSettings = NewCrossHairSettings;
}
