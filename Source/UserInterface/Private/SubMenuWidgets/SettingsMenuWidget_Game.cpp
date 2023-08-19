// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SubMenuWidgets/SettingsMenuWidget_Game.h"
#include "BSWidgetInterface.h"
#include "Components/CheckBox.h"
#include "Components/EditableTextBox.h"
#include "Kismet/KismetMathLibrary.h"
#include "WidgetComponents/Buttons/BSButton.h"
#include "WidgetComponents/ColorSelectWidget.h"
#include "WidgetComponents/SavedTextWidget.h"

void USettingsMenuWidget_Game::NativeConstruct()
{
	Super::NativeConstruct();

	ColorSelect_StartTargetColor->OnColorChanged.BindUFunction(this, "OnColorChanged_StartTarget");
	ColorSelect_PeakTargetColor->OnColorChanged.BindUFunction(this, "OnOnColorChanged_PeakTarget");
	ColorSelect_EndTargetColor->OnColorChanged.BindUFunction(this, "OnOnColorChanged_EndTarget");
	ColorSelect_BeatGridInactiveColor->OnColorChanged.BindUFunction(this, "OnColorChanged_BeatGridInactive");
	Checkbox_UseSeparateOutlineColor->OnCheckStateChanged.AddDynamic(this, &ThisClass::OnCheckStateChanged_UseSeparateOutlineColor);
	ColorSelect_TargetOutlineColor->OnColorChanged.BindUFunction(this, "OnColorChanged_TargetOutline");
	Value_CombatTextFrequency->OnTextCommitted.AddDynamic(this, &ThisClass::OnValueChanged_CombatTextFrequency);
	CheckBox_ShowStreakCombatText->OnCheckStateChanged.AddDynamic(this, &ThisClass::OnCheckStateChanged_ShowCombatText);

	CheckBox_Recoil->OnCheckStateChanged.AddDynamic(this, &ThisClass::OnCheckStateChanged_Recoil);
	CheckBox_AutomaticFire->OnCheckStateChanged.AddDynamic(this, &ThisClass::OnCheckStateChanged_AutomaticFire);
	CheckBox_ShowBulletDecals->OnCheckStateChanged.AddDynamic(this, &ThisClass::OnCheckStateChanged_ShowBulletDecals);
	CheckBox_ShowBulletTracers->OnCheckStateChanged.AddDynamic(this, &ThisClass::OnCheckStateChanged_ShowBulletTracers);
	CheckBox_ShowMuzzleFlash->OnCheckStateChanged.AddDynamic(this, &ThisClass::OnCheckStateChanged_ShowMuzzleFlash);
	CheckBox_ShowMesh->OnCheckStateChanged.AddDynamic(this, &ThisClass::OnCheckStateChanged_ShowMesh);
	CheckBox_ShowHitTimingWidget->OnCheckStateChanged.AddDynamic(this, &ThisClass::OnCheckStateChanged_ShowHitTimingWidget);

	Button_Reset->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_BSButton);
	Button_Revert->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_BSButton);
	Button_Save->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_BSButton);

	Button_Reset->SetDefaults(static_cast<uint8>(ESettingButtonType::Reset));
	Button_Revert->SetDefaults(static_cast<uint8>(ESettingButtonType::Revert));
	Button_Save->SetDefaults(static_cast<uint8>(ESettingButtonType::Save));

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

	InitialGameSettings = LoadPlayerSettings().Game;
	NewGameSettings = InitialGameSettings;
	InitializeGameSettings(NewGameSettings);
}

void USettingsMenuWidget_Game::InitializeGameSettings(const FPlayerSettings_Game& PlayerSettings_Game)
{
	ColorSelect_StartTargetColor->InitializeColor(PlayerSettings_Game.StartTargetColor);
	ColorSelect_PeakTargetColor->InitializeColor(PlayerSettings_Game.PeakTargetColor);
	ColorSelect_EndTargetColor->InitializeColor(PlayerSettings_Game.EndTargetColor);
	ColorSelect_BeatGridInactiveColor->InitializeColor(PlayerSettings_Game.InactiveTargetColor);
	ColorSelect_TargetOutlineColor->InitializeColor(PlayerSettings_Game.TargetOutlineColor);

	Checkbox_UseSeparateOutlineColor->SetIsChecked(PlayerSettings_Game.bUseSeparateOutlineColor);
	OnCheckStateChanged_UseSeparateOutlineColor(PlayerSettings_Game.bUseSeparateOutlineColor);

	CheckBox_ShowStreakCombatText->SetIsChecked(PlayerSettings_Game.bShowStreakCombatText);
	Value_CombatTextFrequency->SetText(FText::AsNumber(PlayerSettings_Game.CombatTextFrequency));

	CheckBox_Recoil->SetIsChecked(PlayerSettings_Game.bShouldRecoil);
	CheckBox_AutomaticFire->SetIsChecked(PlayerSettings_Game.bAutomaticFire);
	CheckBox_ShowBulletDecals->SetIsChecked(PlayerSettings_Game.bShowBulletDecals);
	CheckBox_ShowBulletTracers->SetIsChecked(PlayerSettings_Game.bShowBulletTracers);
	CheckBox_ShowMuzzleFlash->SetIsChecked(PlayerSettings_Game.bShowMuzzleFlash);
	CheckBox_ShowMesh->SetIsChecked(PlayerSettings_Game.bShowCharacterMesh && PlayerSettings_Game.bShowWeaponMesh);
	CheckBox_ShowHitTimingWidget->SetIsChecked(PlayerSettings_Game.bShowHitTimingWidget);
}

void USettingsMenuWidget_Game::OnColorChanged_StartTarget(const FLinearColor& NewColor)
{
	NewGameSettings.StartTargetColor = NewColor;
}

void USettingsMenuWidget_Game::OnOnColorChanged_PeakTarget(const FLinearColor& NewColor)
{
	NewGameSettings.PeakTargetColor = NewColor;
}

void USettingsMenuWidget_Game::OnOnColorChanged_EndTarget(const FLinearColor& NewColor)
{
	NewGameSettings.EndTargetColor = NewColor;
}

void USettingsMenuWidget_Game::OnCheckStateChanged_UseSeparateOutlineColor(const bool bIsChecked)
{
	NewGameSettings.bUseSeparateOutlineColor = bIsChecked;
	if (bIsChecked)
	{
		ColorSelect_TargetOutlineColor->SetVisibility(ESlateVisibility::Visible);
		return;
	}
	ColorSelect_TargetOutlineColor->SetVisibility(ESlateVisibility::Collapsed);
}

void USettingsMenuWidget_Game::OnColorChanged_TargetOutline(const FLinearColor& NewColor)
{
	NewGameSettings.TargetOutlineColor = NewColor;
}

void USettingsMenuWidget_Game::OnColorChanged_BeatGridInactive(const FLinearColor& NewColor)
{
	NewGameSettings.InactiveTargetColor = NewColor;
}

void USettingsMenuWidget_Game::OnValueChanged_CombatTextFrequency(const FText& NewValue, ETextCommit::Type CommitType)
{
	NewGameSettings.CombatTextFrequency = UKismetMathLibrary::GridSnap_Float(FCString::Atof(*NewValue.ToString()), 1);
}

void USettingsMenuWidget_Game::OnCheckStateChanged_ShowCombatText(const bool bIsChecked)
{
	Value_CombatTextFrequency->SetIsReadOnly(!bIsChecked);
	NewGameSettings.bShowStreakCombatText = bIsChecked;
	if (!bIsChecked)
	{
		Value_CombatTextFrequency->SetText(FText());
		return;
	}
	NewGameSettings.CombatTextFrequency = UKismetMathLibrary::GridSnap_Float(FCString::Atof(*Value_CombatTextFrequency->GetText().ToString()), 1);
}

void USettingsMenuWidget_Game::OnCheckStateChanged_Recoil(const bool bIsChecked)
{
	NewGameSettings.bShouldRecoil = bIsChecked;
}

void USettingsMenuWidget_Game::OnCheckStateChanged_AutomaticFire(const bool bIsChecked)
{
	NewGameSettings.bAutomaticFire = bIsChecked;
}

void USettingsMenuWidget_Game::OnCheckStateChanged_ShowBulletDecals(const bool bIsChecked)
{
	NewGameSettings.bShowBulletDecals = bIsChecked;
}

void USettingsMenuWidget_Game::OnCheckStateChanged_ShowBulletTracers(const bool bIsChecked)
{
	NewGameSettings.bShowBulletTracers = bIsChecked;
}

void USettingsMenuWidget_Game::OnCheckStateChanged_ShowMuzzleFlash(const bool bIsChecked)
{
	NewGameSettings.bShowMuzzleFlash = bIsChecked;
}

void USettingsMenuWidget_Game::OnCheckStateChanged_ShowMesh(const bool bIsChecked)
{
	NewGameSettings.bShowWeaponMesh = bIsChecked;
	NewGameSettings.bShowCharacterMesh = bIsChecked;
}

void USettingsMenuWidget_Game::OnCheckStateChanged_ShowHitTimingWidget(const bool bIsChecked)
{
	NewGameSettings.bShowHitTimingWidget = bIsChecked;
}

void USettingsMenuWidget_Game::OnButtonClicked_BSButton(const UBSButton* Button)
{
	switch (static_cast<ESettingButtonType>(Button->GetEnumValue()))
	{
	case ESettingButtonType::Save:
		OnButtonClicked_Save();
		break;
	case ESettingButtonType::Revert:
		OnButtonClicked_Revert();
		break;
	case ESettingButtonType::Reset:
		OnButtonClicked_Reset();
		break;
	default:
		break;
	}
}

void USettingsMenuWidget_Game::OnButtonClicked_Save()
{
	SavePlayerSettings(NewGameSettings);
	SavedTextWidget->SetSavedText(FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "SM_Saved_Game"));
	SavedTextWidget->PlayFadeInFadeOut();
	InitialGameSettings = NewGameSettings;
}

void USettingsMenuWidget_Game::OnButtonClicked_Reset()
{
	NewGameSettings.ResetToDefault();
	InitializeGameSettings(NewGameSettings);
}

void USettingsMenuWidget_Game::OnButtonClicked_Revert()
{
	NewGameSettings = InitialGameSettings;
	InitializeGameSettings(NewGameSettings);
}
