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

	Value_NewSensitivity->OnTextCommitted.AddDynamic(this, &USettingsMenuWidget_Sensitivity::OnValueChanged_NewSensitivity);
	Value_NewSensitivityCsgo->OnTextCommitted.AddDynamic(this, &USettingsMenuWidget_Sensitivity::OnValueChanged_NewSensitivityCsgo);
	Slider_Sensitivity->OnValueChanged.AddDynamic(this, &USettingsMenuWidget_Sensitivity::OnSliderChanged_Sensitivity);
	Button_Save->OnClicked.AddDynamic(this, &USettingsMenuWidget_Sensitivity::OnButtonClicked_Save);

	Sensitivity = LoadPlayerSettings().User.Sensitivity;
	Slider_Sensitivity->SetValue(Sensitivity);
	Value_CurrentSensitivity->SetText(FText::AsNumber(Sensitivity));
}

void USettingsMenuWidget_Sensitivity::OnButtonClicked_Save()
{
	Value_CurrentSensitivity->SetText(FText::AsNumber(Slider_Sensitivity->GetValue()));
	FPlayerSettings_User Settings = LoadPlayerSettings().User;
	Settings.Sensitivity = Slider_Sensitivity->GetValue();
	SavePlayerSettings(Settings);
	SavedTextWidget->SetSavedText(FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "SM_Saved_Sensitivity"));
	SavedTextWidget->PlayFadeInFadeOut();
}

void USettingsMenuWidget_Sensitivity::OnValueChanged_NewSensitivity(const FText& NewValue, ETextCommit::Type CommitType)
{
	const float NewSensValue = FCString::Atof(*NewValue.ToString());
	Slider_Sensitivity->SetValue(NewSensValue);
	Value_NewSensitivityCsgo->SetText(FText::FromString(FString::SanitizeFloat(NewSensValue * Constants::CsgoMultiplier)));
	Sensitivity = NewSensValue;
}

void USettingsMenuWidget_Sensitivity::OnValueChanged_NewSensitivityCsgo(const FText& NewValue, ETextCommit::Type CommitType)
{
	const float NewCsgoSensValue = FCString::Atof(*NewValue.ToString());
	Slider_Sensitivity->SetValue(NewCsgoSensValue / Constants::CsgoMultiplier);
	Value_NewSensitivity->SetText(FText::FromString(FString::SanitizeFloat(NewCsgoSensValue / Constants::CsgoMultiplier)));
	Sensitivity = NewCsgoSensValue / Constants::CsgoMultiplier;
}

void USettingsMenuWidget_Sensitivity::OnSliderChanged_Sensitivity(const float NewValue)
{
	const float NewSensValue = UKismetMathLibrary::GridSnap_Float(NewValue, 0.1);
	Slider_Sensitivity->SetValue(NewSensValue);
	Value_NewSensitivity->SetText(FText::FromString(FString::SanitizeFloat(NewSensValue)));
	Value_NewSensitivityCsgo->SetText(FText::FromString(FString::SanitizeFloat(NewSensValue * Constants::CsgoMultiplier)));
	Sensitivity = NewSensValue;
}
