// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SubMenuWidgets/GameSettingsWidget.h"

#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "Components/EditableTextBox.h"
#include "Kismet/KismetMathLibrary.h"
#include "WidgetComponents/ColorSelectWidget.h"
#include "WidgetComponents/SavedTextWidget.h"

void UGameSettingsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	StartTargetColor->OnColorChanged.BindUFunction(this, "OnStartTargetColorChanged");
	PeakTargetColor->OnColorChanged.BindUFunction(this, "OnPeakTargetColorChanged");
	EndTargetColor->OnColorChanged.BindUFunction(this, "OnEndTargetColorChanged");
	BeatGridInactiveColor->OnColorChanged.BindUFunction(this, "OnBeatGridInactiveColorChanged");
	UseSeparateOutlineColorCheckbox->OnCheckStateChanged.AddDynamic(this, &UGameSettingsWidget::UseSeparateOutlineColorCheckStateChanged);
	TargetOutlineColor->OnColorChanged.BindUFunction(this, "OnTargetOutlineColorChanged");
	CombatTextFrequency->OnTextCommitted.AddDynamic(this, &UGameSettingsWidget::OnCombatTextFrequencyValueChanged);
	ShowStreakCombatTextCheckBox->OnCheckStateChanged.AddDynamic(this, &UGameSettingsWidget::OnShowCombatTextCheckStateChanged);

	ResetButton_Game->OnClicked.AddDynamic(this, &UGameSettingsWidget::OnResetButtonClicked_Game);
	SaveButton_Game->OnClicked.AddDynamic(this, &UGameSettingsWidget::OnSaveButtonClicked_Game);

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

void UGameSettingsWidget::PopulateSettings()
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

void UGameSettingsWidget::OnStartTargetColorChanged(const FLinearColor& NewColor)
{
	NewSettings.StartTargetColor = NewColor;
}

void UGameSettingsWidget::OnPeakTargetColorChanged(const FLinearColor& NewColor)
{
	NewSettings.PeakTargetColor = NewColor;
}

void UGameSettingsWidget::OnEndTargetColorChanged(const FLinearColor& NewColor)
{
	NewSettings.EndTargetColor = NewColor;
}

void UGameSettingsWidget::UseSeparateOutlineColorCheckStateChanged(const bool bIsChecked)
{
	NewSettings.bUseSeparateOutlineColor = bIsChecked;
	if (bIsChecked)
	{
		TargetOutlineColor->SetVisibility(ESlateVisibility::Visible);
		return;
	}
	TargetOutlineColor->SetVisibility(ESlateVisibility::Collapsed);
}

void UGameSettingsWidget::OnTargetOutlineColorChanged(const FLinearColor& NewColor)
{
	NewSettings.TargetOutlineColor = NewColor;
}

void UGameSettingsWidget::OnBeatGridInactiveColorChanged(const FLinearColor& NewColor)
{
	NewSettings.BeatGridInactiveTargetColor = NewColor;
}

void UGameSettingsWidget::OnCombatTextFrequencyValueChanged(const FText& NewValue, ETextCommit::Type CommitType)
{
	NewSettings.CombatTextFrequency = UKismetMathLibrary::GridSnap_Float(FCString::Atof(*NewValue.ToString()), 1);
}

void UGameSettingsWidget::OnShowCombatTextCheckStateChanged(const bool bIsChecked)
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

void UGameSettingsWidget::OnSaveButtonClicked_Game()
{
	SavePlayerSettings(NewSettings);
	SavedTextWidget->SetSavedText(FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "SM_Saved_Game"));
	SavedTextWidget->PlayFadeInFadeOut();
}

void UGameSettingsWidget::OnResetButtonClicked_Game()
{
	NewSettings.ResetToDefault();
	PopulateSettings();
}
