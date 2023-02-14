// Fill out your copyright notice in the Description page of Project Settings.


#include "SubMenuWidgets/SensitivitySettingsWidget.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "SaveGamePlayerSettings.h"
#include "Kismet/KismetMathLibrary.h"
#include "WidgetComponents/SavedTextWidget.h"

void USensitivitySettingsWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	NewSensitivityValue->OnTextCommitted.AddDynamic(this, &USensitivitySettingsWidget::OnNewSensitivityValue);
	NewSensitivityCsgoValue->OnTextCommitted.AddDynamic(this, &USensitivitySettingsWidget::OnNewSensitivityCsgoValue);
	SensSlider->OnValueChanged.AddDynamic(this, &USensitivitySettingsWidget::OnSensitivitySliderChanged);
	SaveButton_Sensitivity->OnClicked.AddDynamic(this, &USensitivitySettingsWidget::OnSaveButtonClicked_Sensitivity);

	Sensitivity = LoadPlayerSettings().Sensitivity;
	SensSlider->SetValue(Sensitivity);
	CurrentSensitivityValue->SetText(FText::AsNumber(Sensitivity));
}

void USensitivitySettingsWidget::OnSaveButtonClicked_Sensitivity()
{
	CurrentSensitivityValue->SetText(FText::AsNumber(SensSlider->GetValue()));
	FPlayerSettings Settings = LoadPlayerSettings();
	Settings.Sensitivity = SensSlider->GetValue();
	SavePlayerSettings(Settings);
	SavedTextWidget->SetSavedText(FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "SM_Saved_Sensitivity"));
	SavedTextWidget->PlayFadeInFadeOut();
	OnSettingsSaved_Sensitivity.Broadcast();
}

void USensitivitySettingsWidget::OnNewSensitivityValue(const FText& NewValue, ETextCommit::Type CommitType)
{
	const float NewSensValue = FCString::Atof(*NewValue.ToString());
	SensSlider->SetValue(NewSensValue);
	NewSensitivityCsgoValue->SetText(FText::FromString(FString::SanitizeFloat(NewSensValue * CsgoMultiplier)));
	Sensitivity = NewSensValue;
}

void USensitivitySettingsWidget::OnNewSensitivityCsgoValue(const FText& NewValue, ETextCommit::Type CommitType)
{
	const float NewCsgoSensValue = FCString::Atof(*NewValue.ToString());
	SensSlider->SetValue(NewCsgoSensValue / CsgoMultiplier);
	NewSensitivityValue->SetText(FText::FromString(FString::SanitizeFloat(NewCsgoSensValue / CsgoMultiplier)));
	Sensitivity = NewCsgoSensValue / CsgoMultiplier;
}

void USensitivitySettingsWidget::OnSensitivitySliderChanged(const float NewValue)
{
	const float NewSensValue = UKismetMathLibrary::GridSnap_Float(NewValue, 0.1);
	SensSlider->SetValue(NewSensValue);
	NewSensitivityValue->SetText(FText::FromString(FString::SanitizeFloat(NewSensValue)));
	NewSensitivityCsgoValue->SetText(FText::FromString(FString::SanitizeFloat(NewSensValue * CsgoMultiplier)));
	Sensitivity = NewSensValue;
}