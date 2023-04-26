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

	StartTargetColor->OnColorChanged.BindUFunction(this, "OnStartTargetColorChanged");
	PeakTargetColor->OnColorChanged.BindUFunction(this, "OnPeakTargetColorChanged");
	EndTargetColor->OnColorChanged.BindUFunction(this, "OnEndTargetColorChanged");
	BeatGridInactiveColor->OnColorChanged.BindUFunction(this, "OnBeatGridInactiveColorChanged");
	UseSeparateOutlineColorCheckbox->OnCheckStateChanged.AddDynamic(this, &USettingsMenuWidget_Game::UseSeparateOutlineColorCheckStateChanged);
	TargetOutlineColor->OnColorChanged.BindUFunction(this, "OnTargetOutlineColorChanged");
	CombatTextFrequency->OnTextCommitted.AddDynamic(this, &USettingsMenuWidget_Game::OnCombatTextFrequencyValueChanged);
	ShowStreakCombatTextCheckBox->OnCheckStateChanged.AddDynamic(this, &USettingsMenuWidget_Game::OnShowCombatTextCheckStateChanged);

	ResetButton_Game->OnClicked.AddDynamic(this, &USettingsMenuWidget_Game::OnResetButtonClicked_Game);
	SaveButton_Game->OnClicked.AddDynamic(this, &USettingsMenuWidget_Game::OnSaveButtonClicked_Game);

	NewSettings = LoadPlayerSettings().Game;

	StartTargetColor->SetColorText(FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "SM_TargetColor_Start"));
	StartTargetColor->SetBorderColors(false, false);

	PeakTargetColor->SetColorText(FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "SM_TargetColor_Peak"));
	PeakTargetColor->SetBorderColors(true, false);

	EndTargetColor->SetColorText(FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "SM_TargetColor_End"));
	EndTargetColor->SetBorderColors(false, false);

	BeatGridInactiveColor->SetColorText(FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "SM_TargetColor_BeatGridInactive"));
	BeatGridInactiveColor->SetBorderColors(true, false);

	TargetOutlineColor->SetColorText(FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "SM_TargetColor_Outline"));
	TargetOutlineColor->SetBorderColors(true, false);

	PopulateSettings();
}

void USettingsMenuWidget_Game::PopulateSettings()
{
	StartTargetColor->InitializeColor(NewSettings.StartTargetColor);
	PeakTargetColor->InitializeColor(NewSettings.PeakTargetColor);
	EndTargetColor->InitializeColor(NewSettings.EndTargetColor);
	BeatGridInactiveColor->InitializeColor(NewSettings.BeatGridInactiveTargetColor);
	TargetOutlineColor->InitializeColor(NewSettings.TargetOutlineColor);

	UseSeparateOutlineColorCheckbox->SetIsChecked(NewSettings.bUseSeparateOutlineColor);
	UseSeparateOutlineColorCheckStateChanged(NewSettings.bUseSeparateOutlineColor);

	ShowStreakCombatTextCheckBox->SetIsChecked(NewSettings.bShowStreakCombatText);
	CombatTextFrequency->SetText(FText::AsNumber(NewSettings.CombatTextFrequency));
}

void USettingsMenuWidget_Game::OnStartTargetColorChanged(const FLinearColor& NewColor)
{
	NewSettings.StartTargetColor = NewColor;
}

void USettingsMenuWidget_Game::OnPeakTargetColorChanged(const FLinearColor& NewColor)
{
	NewSettings.PeakTargetColor = NewColor;
}

void USettingsMenuWidget_Game::OnEndTargetColorChanged(const FLinearColor& NewColor)
{
	NewSettings.EndTargetColor = NewColor;
}

void USettingsMenuWidget_Game::UseSeparateOutlineColorCheckStateChanged(const bool bIsChecked)
{
	NewSettings.bUseSeparateOutlineColor = bIsChecked;
	if (bIsChecked)
	{
		TargetOutlineColor->SetVisibility(ESlateVisibility::Visible);
		return;
	}
	TargetOutlineColor->SetVisibility(ESlateVisibility::Collapsed);
}

void USettingsMenuWidget_Game::OnTargetOutlineColorChanged(const FLinearColor& NewColor)
{
	NewSettings.TargetOutlineColor = NewColor;
}

void USettingsMenuWidget_Game::OnBeatGridInactiveColorChanged(const FLinearColor& NewColor)
{
	NewSettings.BeatGridInactiveTargetColor = NewColor;
}

void USettingsMenuWidget_Game::OnCombatTextFrequencyValueChanged(const FText& NewValue, ETextCommit::Type CommitType)
{
	NewSettings.CombatTextFrequency = UKismetMathLibrary::GridSnap_Float(FCString::Atof(*NewValue.ToString()), 1);
}

void USettingsMenuWidget_Game::OnShowCombatTextCheckStateChanged(const bool bIsChecked)
{
	CombatTextFrequency->SetIsReadOnly(!bIsChecked);
	NewSettings.bShowStreakCombatText = bIsChecked;
	if (!bIsChecked)
	{
		CombatTextFrequency->SetText(FText());
		return;
	}
	NewSettings.CombatTextFrequency = UKismetMathLibrary::GridSnap_Float(FCString::Atof(*CombatTextFrequency->GetText().ToString()), 1);
}

void USettingsMenuWidget_Game::OnSaveButtonClicked_Game()
{
	SavePlayerSettings(NewSettings);
	SavedTextWidget->SetSavedText(FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "SM_Saved_Game"));
	SavedTextWidget->PlayFadeInFadeOut();
}

void USettingsMenuWidget_Game::OnResetButtonClicked_Game()
{
	NewSettings.ResetToDefault();
	PopulateSettings();
}
