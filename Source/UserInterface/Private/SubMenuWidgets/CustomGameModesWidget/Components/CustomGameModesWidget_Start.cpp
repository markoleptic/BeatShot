// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SubMenuWidgets/CustomGameModesWidget/Components/CustomGameModesWidget_Start.h"
#include "Components/CheckBox.h"
#include "Components/EditableTextBox.h"
#include "WidgetComponents/BSComboBoxString.h"
#include "WidgetComponents/MenuOptionWidgets/CheckBoxOptionWidget.h"
#include "WidgetComponents/MenuOptionWidgets/ComboBoxOptionWidget.h"
#include "WidgetComponents/MenuOptionWidgets/EditableTextBoxOptionWidget.h"

void UCustomGameModesWidget_Start::NativeConstruct()
{
	Super::NativeConstruct();

	SetupTooltip(CheckBoxOption_UseTemplate->GetTooltipImage(), CheckBoxOption_UseTemplate->GetTooltipRegularText());
	SetupTooltip(ComboBoxOption_GameModeTemplates->GetTooltipImage(), ComboBoxOption_GameModeTemplates->GetTooltipRegularText());
	SetupTooltip(ComboBoxOption_GameModeDifficulty->GetTooltipImage(), ComboBoxOption_GameModeDifficulty->GetTooltipRegularText());
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

void UCustomGameModesWidget_Start::InitComponent(FBSConfig* InConfigPtr, TObjectPtr<UCustomGameModesWidgetComponent> InNext)
{
	Super::InitComponent(InConfigPtr, InNext);
}

FString UCustomGameModesWidget_Start::GetNewCustomGameModeName() const
{
	return EditableTextBoxOption_CustomGameModeName->EditableTextBox->GetText().ToString();
}

void UCustomGameModesWidget_Start::SetNewCustomGameModeName(const FString& InCustomGameModeName) const
{
	EditableTextBoxOption_CustomGameModeName->EditableTextBox->SetText(FText::FromString(InCustomGameModeName));
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
	const FString GameModeString = ComboBoxOption_GameModeTemplates->ComboBox->GetSelectedOption();
	const FString DifficultyString = ComboBoxOption_GameModeDifficulty->ComboBox->GetSelectedOption();
	
	if (ConfigPtr->DefiningConfig.GameModeType == EGameModeType::Preset)
	{
		CheckBoxOption_UseTemplate->CheckBox->SetIsChecked(true);
		ComboBoxOption_GameModeTemplates->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		ComboBoxOption_GameModeTemplates->ComboBox->SetSelectedOption(UEnum::GetDisplayValueAsText(ConfigPtr->DefiningConfig.BaseGameMode).ToString());
		ComboBoxOption_GameModeDifficulty->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		ComboBoxOption_GameModeDifficulty->ComboBox->SetSelectedOption(UEnum::GetDisplayValueAsText(ConfigPtr->DefiningConfig.Difficulty).ToString());
	}
	else if (ConfigPtr->DefiningConfig.GameModeType == EGameModeType::Custom)
	{
		CheckBoxOption_UseTemplate->CheckBox->SetIsChecked(true);
		ComboBoxOption_GameModeTemplates->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		ComboBoxOption_GameModeTemplates->ComboBox->SetSelectedOption(ConfigPtr->DefiningConfig.CustomGameModeName);
		ComboBoxOption_GameModeDifficulty->SetVisibility(ESlateVisibility::Collapsed);
		ComboBoxOption_GameModeDifficulty->ComboBox->ClearSelection();
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
	if (!Text.IsEmptyOrWhitespace())
	{
		ConfigPtr->DefiningConfig.CustomGameModeName = Text.ToString();
	}
	SetCanTransitionForward(UpdateCanTransitionForward());
}

void UCustomGameModesWidget_Start::OnSelectionChanged_GameModeTemplates(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType)
{
	if (Selected.Num() != 1)
	{
		SetCanTransitionForward(UpdateCanTransitionForward());
		return;
	}

	const FString GameModeString = Selected[0];
	FString DifficultyString = ComboBoxOption_GameModeDifficulty->ComboBox->GetSelectedOption();
	const EGameModeDifficulty Difficulty = GetEnumFromString<EGameModeDifficulty>(DifficultyString, EGameModeDifficulty::None);
	
	if (IsPresetGameMode(GameModeString))
	{
		ComboBoxOption_GameModeDifficulty->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		
		// Set default difficulty to Normal if switching from Custom (no difficulty) to preset
		if (Difficulty == EGameModeDifficulty::None)
		{
			DifficultyString = UEnum::GetDisplayValueAsText(EGameModeDifficulty::Normal).ToString();
		}
	}
	else
	{
		ComboBoxOption_GameModeDifficulty->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (SelectionType != ESelectInfo::Type::Direct)
	{
		RequestGameModeTemplateUpdate.Broadcast(GameModeString, GetEnumFromString<EGameModeDifficulty>(DifficultyString, EGameModeDifficulty::None));
	}

	SetCanTransitionForward(UpdateCanTransitionForward());
	UpdateBrushColors();
}

void UCustomGameModesWidget_Start::OnSelectionChanged_GameModeDifficulty(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType)
{
	if (Selected.Num() != 1)
	{
		SetCanTransitionForward(UpdateCanTransitionForward());
		return;
	}

	const FString GameModeString = ComboBoxOption_GameModeTemplates->ComboBox->GetSelectedOption();
	
	// Only request update for difficulty if Preset GameMode
	if (SelectionType != ESelectInfo::Type::Direct && IsPresetGameMode(GameModeString))
	{
		RequestGameModeTemplateUpdate.Broadcast(GameModeString, GetEnumFromString<EGameModeDifficulty>(Selected[0], EGameModeDifficulty::None));
	}

	SetCanTransitionForward(UpdateCanTransitionForward());
}
