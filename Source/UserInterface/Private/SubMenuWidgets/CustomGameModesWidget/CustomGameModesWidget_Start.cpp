// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SubMenuWidgets/CustomGameModesWidget/CustomGameModesWidget_Start.h"

#include "Components/CheckBox.h"
#include "Components/EditableTextBox.h"
#include "WidgetComponents/BSComboBoxString.h"
#include "WidgetComponents/MenuOptionWidgets/CheckBoxOptionWidget.h"
#include "WidgetComponents/MenuOptionWidgets/ComboBoxOptionWidget.h"
#include "WidgetComponents/MenuOptionWidgets/EditableTextBoxOptionWidget.h"

void UCustomGameModesWidget_Start::NativeConstruct()
{
	Super::NativeConstruct();
	
	CheckBoxOption_UseTemplate->SetIndentLevel(0);
	CheckBoxOption_UseTemplate->SetShowTooltipImage(false);
	CheckBoxOption_UseTemplate->SetShowTooltipWarningImage(false);
	CheckBoxOption_UseTemplate->SetShowCheckBoxLock(false);
	SetupTooltip(CheckBoxOption_UseTemplate->GetTooltipImage(), CheckBoxOption_UseTemplate->GetTooltipRegularText());
	
	ComboBoxOption_GameModeTemplates->SetIndentLevel(1);
	ComboBoxOption_GameModeTemplates->SetShowTooltipImage(true);
	ComboBoxOption_GameModeTemplates->SetShowTooltipWarningImage(false);
	ComboBoxOption_GameModeTemplates->SetShowCheckBoxLock(false);
	SetupTooltip(ComboBoxOption_GameModeTemplates->GetTooltipImage(), ComboBoxOption_GameModeTemplates->GetTooltipRegularText());

	ComboBoxOption_GameModeDifficulty->SetIndentLevel(1);
	ComboBoxOption_GameModeDifficulty->SetShowTooltipImage(true);
	ComboBoxOption_GameModeDifficulty->SetShowTooltipWarningImage(false);
	ComboBoxOption_GameModeDifficulty->SetShowCheckBoxLock(false);
	SetupTooltip(ComboBoxOption_GameModeDifficulty->GetTooltipImage(), ComboBoxOption_GameModeDifficulty->GetTooltipRegularText());

	EditableTextBoxOption_CustomGameModeName->SetIndentLevel(1);
	EditableTextBoxOption_CustomGameModeName->SetShowTooltipImage(true);
	EditableTextBoxOption_CustomGameModeName->SetShowTooltipWarningImage(false);
	EditableTextBoxOption_CustomGameModeName->SetShowCheckBoxLock(false);
	SetupTooltip(EditableTextBoxOption_CustomGameModeName->GetTooltipImage(), EditableTextBoxOption_CustomGameModeName->GetTooltipRegularText());

	CheckBoxOption_UseTemplate->CheckBox->OnCheckStateChanged.AddUniqueDynamic(this, &ThisClass::OnCheckStateChanged_UseTemplate);
	EditableTextBoxOption_CustomGameModeName->EditableTextBox->OnTextChanged.AddUniqueDynamic(this, &ThisClass::OnTextChanged_CustomGameModeName);
	ComboBoxOption_GameModeTemplates->ComboBox->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnSelectionChanged_GameModeTemplates);
	ComboBoxOption_GameModeDifficulty->ComboBox->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnSelectionChanged_GameModeDifficulty);

	ComboBoxOption_GameModeTemplates->ComboBox->ClearOptions();
	ComboBoxOption_GameModeDifficulty->ComboBox->ClearOptions();

	/* Add DefaultModes to GameModeTemplates */
	for (const EBaseGameMode& GameMode : TEnumRange<EBaseGameMode>())
	{
		if (GameMode != EBaseGameMode::None)
		{
			const FString GameModeName = UEnum::GetDisplayValueAsText(GameMode).ToString();
			ComboBoxOption_GameModeTemplates->ComboBox->AddOption(GameModeName);
		}
	}

	/* Add CustomGameModes to GameModeTemplates */
	for (const FBSConfig& GameMode : LoadCustomGameModes())
	{
		ComboBoxOption_GameModeTemplates->ComboBox->AddOption(GameMode.DefiningConfig.CustomGameModeName);
	}

	/* Add difficulties to GameModeDifficulty ComboBox */
	for (const EGameModeDifficulty& Difficulty : TEnumRange<EGameModeDifficulty>())
	{
		if (Difficulty != EGameModeDifficulty::None)
		{
			ComboBoxOption_GameModeDifficulty->ComboBox->AddOption(UEnum::GetDisplayValueAsText(Difficulty).ToString());
		}
	}

	ComboBoxOption_GameModeTemplates->SetVisibility(ESlateVisibility::Collapsed);
	ComboBoxOption_GameModeDifficulty->SetVisibility(ESlateVisibility::Collapsed);
	UpdateBrushColors();
}

void UCustomGameModesWidget_Start::Init(FBSConfig* InConfigPtr, TObjectPtr<UCustomGameModesWidgetComponent> InNext)
{
	Super::Init(InConfigPtr, InNext);
	UpdateOptions();
}

bool UCustomGameModesWidget_Start::UpdateCanTransitionForward()
{
	if (CheckBoxOption_UseTemplate->CheckBox->IsChecked())
	{
		if (ComboBoxOption_GameModeTemplates->ComboBox->GetSelectedOptionCount() != 1)
		{
			return false;
		}
		if (IsPresetGameMode(ComboBoxOption_GameModeTemplates->ComboBox->GetSelectedOption()))
		{
			if (ComboBoxOption_GameModeDifficulty->ComboBox->GetSelectedOptionCount() != 1)
			{
				return false;
			}
		}
	}
	const FText CustomGameModeNameText = EditableTextBoxOption_CustomGameModeName->EditableTextBox->GetText();
	if (CustomGameModeNameText.IsEmptyOrWhitespace())
	{
		return false;
	}
	if (IsPresetGameMode(CustomGameModeNameText.ToString()))
	{
		return false;
	}
	return true;
}

void UCustomGameModesWidget_Start::UpdateOptions()
{
	if (ConfigPtr->DefiningConfig.GameModeType == EGameModeType::Preset)
	{
		CheckBoxOption_UseTemplate->CheckBox->SetIsChecked(true);
		ComboBoxOption_GameModeTemplates->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		ComboBoxOption_GameModeTemplates->ComboBox->SetSelectedOption(UEnum::GetDisplayValueAsText(ConfigPtr->DefiningConfig.BaseGameMode).ToString());
		ComboBoxOption_GameModeDifficulty->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		ComboBoxOption_GameModeDifficulty->ComboBox->SetSelectedOption(UEnum::GetDisplayValueAsText(ConfigPtr->DefiningConfig.Difficulty).ToString());
	}
	else
	{
		CheckBoxOption_UseTemplate->CheckBox->SetIsChecked(false);
		ComboBoxOption_GameModeTemplates->SetVisibility(ESlateVisibility::Collapsed);
		ComboBoxOption_GameModeTemplates->ComboBox->ClearSelection();
		ComboBoxOption_GameModeDifficulty->SetVisibility(ESlateVisibility::Collapsed);
		ComboBoxOption_GameModeDifficulty->ComboBox->ClearSelection();
	}

	SetCanTransitionForward(UpdateCanTransitionForward());
	UpdateBrushColors();
}

void UCustomGameModesWidget_Start::OnCheckStateChanged_UseTemplate(const bool bChecked)
{
	if (bChecked)
	{
		ComboBoxOption_GameModeTemplates->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		ComboBoxOption_GameModeTemplates->SetVisibility(ESlateVisibility::Collapsed);
		ComboBoxOption_GameModeDifficulty->SetVisibility(ESlateVisibility::Collapsed);
	}
	UpdateBrushColors();
	SetCanTransitionForward(UpdateCanTransitionForward());
}

void UCustomGameModesWidget_Start::OnTextChanged_CustomGameModeName(const FText& Text)
{
	SetCanTransitionForward(UpdateCanTransitionForward());
}

void UCustomGameModesWidget_Start::OnSelectionChanged_GameModeTemplates(const TArray<FString>& Selected,
	const ESelectInfo::Type SelectionType)
{
	SetCanTransitionForward(UpdateCanTransitionForward());
	
	if (Selected.Num() != 1)
	{
		return;
	}

	const ESlateVisibility PreChangeVisibility = ComboBoxOption_GameModeDifficulty->GetVisibility();
	
	if (IsPresetGameMode(Selected[0]))
	{
		ComboBoxOption_GameModeDifficulty->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		if (PreChangeVisibility == ESlateVisibility::Collapsed)
		{
			UpdateBrushColors();
		}
	}
	else if (IsCustomGameMode(Selected[0]))
	{
		ComboBoxOption_GameModeDifficulty->SetVisibility(ESlateVisibility::Collapsed);
		if (PreChangeVisibility == ESlateVisibility::SelfHitTestInvisible)
		{
			UpdateBrushColors();
		}
	}
}

void UCustomGameModesWidget_Start::OnSelectionChanged_GameModeDifficulty(const TArray<FString>& Selected,
	const ESelectInfo::Type SelectionType)
{
	SetCanTransitionForward(UpdateCanTransitionForward());
}
