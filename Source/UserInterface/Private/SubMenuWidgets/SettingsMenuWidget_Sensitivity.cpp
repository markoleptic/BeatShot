// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SubMenuWidgets/SettingsMenuWidget_Sensitivity.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "SaveGamePlayerSettings.h"
#include "Kismet/KismetMathLibrary.h"
#include "WidgetComponents/SavedTextWidget.h"

void USettingsMenuWidget_Sensitivity::NativeConstruct()
{
	Super::NativeConstruct();

	NewSensitivityValue->OnTextCommitted.AddDynamic(this, &USettingsMenuWidget_Sensitivity::OnNewSensitivityValue);
	NewSensitivityCsgoValue->OnTextCommitted.AddDynamic(this, &USettingsMenuWidget_Sensitivity::OnNewSensitivityCsgoValue);
	SensSlider->OnValueChanged.AddDynamic(this, &USettingsMenuWidget_Sensitivity::OnSensitivitySliderChanged);
	SaveButton_Sensitivity->OnClicked.AddDynamic(this, &USettingsMenuWidget_Sensitivity::OnSaveButtonClicked_Sensitivity);

	Sensitivity = LoadPlayerSettings().User.Sensitivity;
	SensSlider->SetValue(Sensitivity);
	CurrentSensitivityValue->SetText(FText::AsNumber(Sensitivity));
}

void USettingsMenuWidget_Sensitivity::OnSaveButtonClicked_Sensitivity()
{
	CurrentSensitivityValue->SetText(FText::AsNumber(SensSlider->GetValue()));
	FPlayerSettings_User Settings = LoadPlayerSettings().User;
	Settings.Sensitivity = SensSlider->GetValue();
	SavePlayerSettings(Settings);
	SavedTextWidget->SetSavedText(FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "SM_Saved_Sensitivity"));
	SavedTextWidget->PlayFadeInFadeOut();
}

void USettingsMenuWidget_Sensitivity::OnNewSensitivityValue(const FText& NewValue, ETextCommit::Type CommitType)
{
	const float NewSensValue = FCString::Atof(*NewValue.ToString());
	SensSlider->SetValue(NewSensValue);
	NewSensitivityCsgoValue->SetText(FText::FromString(FString::SanitizeFloat(NewSensValue * Constants::CsgoMultiplier)));
	Sensitivity = NewSensValue;
}

void USettingsMenuWidget_Sensitivity::OnNewSensitivityCsgoValue(const FText& NewValue, ETextCommit::Type CommitType)
{
	const float NewCsgoSensValue = FCString::Atof(*NewValue.ToString());
	SensSlider->SetValue(NewCsgoSensValue / Constants::CsgoMultiplier);
	NewSensitivityValue->SetText(FText::FromString(FString::SanitizeFloat(NewCsgoSensValue / Constants::CsgoMultiplier)));
	Sensitivity = NewCsgoSensValue / Constants::CsgoMultiplier;
}

void USettingsMenuWidget_Sensitivity::OnSensitivitySliderChanged(const float NewValue)
{
	const float NewSensValue = UKismetMathLibrary::GridSnap_Float(NewValue, 0.1);
	SensSlider->SetValue(NewSensValue);
	NewSensitivityValue->SetText(FText::FromString(FString::SanitizeFloat(NewSensValue)));
	NewSensitivityCsgoValue->SetText(FText::FromString(FString::SanitizeFloat(NewSensValue * Constants::CsgoMultiplier)));
	Sensitivity = NewSensValue;
}
