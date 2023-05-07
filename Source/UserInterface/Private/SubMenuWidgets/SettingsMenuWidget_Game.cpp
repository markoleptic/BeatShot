// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SubMenuWidgets/SettingsMenuWidget_Game.h"

#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "Components/EditableTextBox.h"
#include "Kismet/KismetMathLibrary.h"
#include "WidgetComponents/ColorSelectWidget.h"
#include "WidgetComponents/SavedTextWidget.h"

void USettingsMenuWidget_Game::NativeConstruct()
{
	Super::NativeConstruct();

	ColorSelect_StartTargetColor->OnColorChanged.BindUFunction(this, "OnColorChanged_StartTarget");
	ColorSelect_PeakTargetColor->OnColorChanged.BindUFunction(this, "OnOnColorChanged_PeakTarget");
	ColorSelect_EndTargetColor->OnColorChanged.BindUFunction(this, "OnOnColorChanged_EndTarget");
	ColorSelect_BeatGridInactiveColor->OnColorChanged.BindUFunction(this, "OnColorChanged_BeatGridInactive");
	Checkbox_UseSeparateOutlineColor->OnCheckStateChanged.AddDynamic(this, &USettingsMenuWidget_Game::CheckStateChanged_UseSeparateOutlineColor);
	ColorSelect_TargetOutlineColor->OnColorChanged.BindUFunction(this, "OnColorChanged_TargetOutline");
	Value_CombatTextFrequency->OnTextCommitted.AddDynamic(this, &USettingsMenuWidget_Game::OnValueChanged_CombatTextFrequency);
	CheckBox_ShowStreakCombatText->OnCheckStateChanged.AddDynamic(this, &USettingsMenuWidget_Game::OnCheckStateChanged_ShowCombatText);

	Button_Reset->OnClicked.AddDynamic(this, &USettingsMenuWidget_Game::OnButtonClicked_Reset);
	Button_Save->OnClicked.AddDynamic(this, &USettingsMenuWidget_Game::OnButtonClicked_Save);

	NewSettings = LoadPlayerSettings().Game;

	ColorSelect_StartTargetColor->SetColorText(FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "SM_TargetColor_Start"));
	ColorSelect_StartTargetColor->SetBorderColors(false, false);

	ColorSelect_PeakTargetColor->SetColorText(FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "SM_TargetColor_Peak"));
	ColorSelect_PeakTargetColor->SetBorderColors(true, false);

	ColorSelect_EndTargetColor->SetColorText(FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "SM_TargetColor_End"));
	ColorSelect_EndTargetColor->SetBorderColors(false, false);

	ColorSelect_BeatGridInactiveColor->SetColorText(FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "SM_TargetColor_BeatGridInactive"));
	ColorSelect_BeatGridInactiveColor->SetBorderColors(true, false);

	ColorSelect_TargetOutlineColor->SetColorText(FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "SM_TargetColor_Outline"));
	ColorSelect_TargetOutlineColor->SetBorderColors(true, false);

	PopulateSettings();
}

void USettingsMenuWidget_Game::PopulateSettings()
{
	ColorSelect_StartTargetColor->InitializeColor(NewSettings.StartTargetColor);
	ColorSelect_PeakTargetColor->InitializeColor(NewSettings.PeakTargetColor);
	ColorSelect_EndTargetColor->InitializeColor(NewSettings.EndTargetColor);
	ColorSelect_BeatGridInactiveColor->InitializeColor(NewSettings.BeatGridInactiveTargetColor);
	ColorSelect_TargetOutlineColor->InitializeColor(NewSettings.TargetOutlineColor);

	Checkbox_UseSeparateOutlineColor->SetIsChecked(NewSettings.bUseSeparateOutlineColor);
	CheckStateChanged_UseSeparateOutlineColor(NewSettings.bUseSeparateOutlineColor);

	CheckBox_ShowStreakCombatText->SetIsChecked(NewSettings.bShowStreakCombatText);
	Value_CombatTextFrequency->SetText(FText::AsNumber(NewSettings.CombatTextFrequency));
}

void USettingsMenuWidget_Game::OnColorChanged_StartTarget(const FLinearColor& NewColor)
{
	NewSettings.StartTargetColor = NewColor;
}

void USettingsMenuWidget_Game::OnOnColorChanged_PeakTarget(const FLinearColor& NewColor)
{
	NewSettings.PeakTargetColor = NewColor;
}

void USettingsMenuWidget_Game::OnOnColorChanged_EndTarget(const FLinearColor& NewColor)
{
	NewSettings.EndTargetColor = NewColor;
}

void USettingsMenuWidget_Game::CheckStateChanged_UseSeparateOutlineColor(const bool bIsChecked)
{
	NewSettings.bUseSeparateOutlineColor = bIsChecked;
	if (bIsChecked)
	{
		ColorSelect_TargetOutlineColor->SetVisibility(ESlateVisibility::Visible);
		return;
	}
	ColorSelect_TargetOutlineColor->SetVisibility(ESlateVisibility::Collapsed);
}

void USettingsMenuWidget_Game::OnColorChanged_TargetOutline(const FLinearColor& NewColor)
{
	NewSettings.TargetOutlineColor = NewColor;
}

void USettingsMenuWidget_Game::OnColorChanged_BeatGridInactive(const FLinearColor& NewColor)
{
	NewSettings.BeatGridInactiveTargetColor = NewColor;
}

void USettingsMenuWidget_Game::OnValueChanged_CombatTextFrequency(const FText& NewValue, ETextCommit::Type CommitType)
{
	NewSettings.CombatTextFrequency = UKismetMathLibrary::GridSnap_Float(FCString::Atof(*NewValue.ToString()), 1);
}

void USettingsMenuWidget_Game::OnCheckStateChanged_ShowCombatText(const bool bIsChecked)
{
	Value_CombatTextFrequency->SetIsReadOnly(!bIsChecked);
	NewSettings.bShowStreakCombatText = bIsChecked;
	if (!bIsChecked)
	{
		Value_CombatTextFrequency->SetText(FText());
		return;
	}
	NewSettings.CombatTextFrequency = UKismetMathLibrary::GridSnap_Float(FCString::Atof(*Value_CombatTextFrequency->GetText().ToString()), 1);
}

void USettingsMenuWidget_Game::OnButtonClicked_Save()
{
	SavePlayerSettings(NewSettings);
	SavedTextWidget->SetSavedText(FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "SM_Saved_Game"));
	SavedTextWidget->PlayFadeInFadeOut();
}

void USettingsMenuWidget_Game::OnButtonClicked_Reset()
{
	NewSettings.ResetToDefault();
	PopulateSettings();
}
