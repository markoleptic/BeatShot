// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

// ReSharper disable CppMemberFunctionMayBeConst
#include "SubMenuWidgets/GameModesWidget_DefiningConfig.h"
#include "Components/ComboBoxString.h"
#include "Components/EditableTextBox.h"
#include "WidgetComponents/BSHorizontalBox.h"

void UGameModesWidget_DefiningConfig::NativeConstruct()
{
	Super::NativeConstruct();

	AddToTooltipData(QMark_GameModeTemplate, GetTooltipTextFromKey("GameModeTemplate"));
	AddToTooltipData(QMark_BaseGameMode, GetTooltipTextFromKey("BaseGameMode"));
	AddToTooltipData(QMark_CustomGameModeName, GetTooltipTextFromKey("CustomGameModeName"));
	AddToTooltipData(QMark_GameModeDifficulty, GetTooltipTextFromKey("GameModeDifficulty"));

	ComboBox_GameModeName->OnSelectionChanged.AddDynamic(this, &UGameModesWidget_DefiningConfig::OnSelectionChanged_GameModeName);
	TextBox_CustomGameModeName->OnTextChanged.AddDynamic(this, &UGameModesWidget_DefiningConfig::OnTextChanged_CustomGameMode);
	ComboBox_BaseGameMode->OnSelectionChanged.AddDynamic(this, &UGameModesWidget_DefiningConfig::OnSelectionChanged_BaseGameMode);
	ComboBox_GameModeDifficulty->OnSelectionChanged.AddDynamic(this, &UGameModesWidget_DefiningConfig::OnSelectionChanged_GameModeDifficulty);
}

void UGameModesWidget_DefiningConfig::InitSettingCategoryWidget()
{
	Super::InitSettingCategoryWidget();
}

void UGameModesWidget_DefiningConfig::InitializeDefiningConfig(const FBS_DefiningConfig& InDefiningConfig, const EBaseGameMode& BaseGameMode)
{
	switch(BaseGameMode)
	{
	case EBaseGameMode::SingleBeat:
		ComboBox_BaseGameMode->SetSelectedOption(UEnum::GetDisplayValueAsText(BaseGameMode).ToString());
		break;
	case EBaseGameMode::MultiBeat:
		ComboBox_BaseGameMode->SetSelectedOption(UEnum::GetDisplayValueAsText(BaseGameMode).ToString());
		break;
	case EBaseGameMode::BeatGrid:
		ComboBox_BaseGameMode->SetSelectedOption(UEnum::GetDisplayValueAsText(BaseGameMode).ToString());
		break;
	case EBaseGameMode::BeatTrack:
		ComboBox_BaseGameMode->SetSelectedOption(UEnum::GetDisplayValueAsText(BaseGameMode).ToString());
		break;
	case EBaseGameMode::ChargedBeatTrack:
		ComboBox_BaseGameMode->SetSelectedOption(UEnum::GetDisplayValueAsText(BaseGameMode).ToString());
		break;
	case EBaseGameMode::None:
		break;
	}
	ComboBox_GameModeDifficulty->SetSelectedOption(UEnum::GetDisplayValueAsText(InDefiningConfig.Difficulty).ToString());
}

FBS_DefiningConfig UGameModesWidget_DefiningConfig::GetDefiningConfig() const
{
	FBS_DefiningConfig ReturnConfig;
	ReturnConfig.GameModeType = EGameModeType::Custom;
	ReturnConfig.Difficulty = EGameModeDifficulty::None;
	if (IsPresetGameMode(ComboBox_GameModeName->GetSelectedOption()) && TextBox_CustomGameModeName->GetText().IsEmptyOrWhitespace())
	{
		ReturnConfig.CustomGameModeName = "";
	}
	else if (!TextBox_CustomGameModeName->GetText().IsEmptyOrWhitespace())
	{
		ReturnConfig.CustomGameModeName = TextBox_CustomGameModeName->GetText().ToString();
	}
	else
	{
		ReturnConfig.CustomGameModeName = ComboBox_GameModeName->GetSelectedOption();
	}

	for (const EBaseGameMode& Mode : TEnumRange<EBaseGameMode>())
	{
		if (ComboBox_BaseGameMode->GetSelectedOption().Equals(UEnum::GetDisplayValueAsText(Mode).ToString()))
		{
			ReturnConfig.BaseGameMode = Mode;
			break;
		}
	}
	return ReturnConfig;
}

void UGameModesWidget_DefiningConfig::PopulateGameModeNameComboBox(const FString& GameModeOptionToSelect)
{
	ComboBox_GameModeName->ClearOptions();

	for (const FBSConfig& GameMode : FBSConfig::GetPresetGameModes())
	{
		ComboBox_GameModeName->AddOption(UEnum::GetDisplayValueAsText(GameMode.DefiningConfig.BaseGameMode).ToString());
	}
	
	for (const FBSConfig& CustomGameMode : LoadCustomGameModes())
	{
		ComboBox_GameModeName->AddOption(CustomGameMode.DefiningConfig.CustomGameModeName);
	}
	
	if (GameModeOptionToSelect.IsEmpty())
	{
		ComboBox_GameModeName->ClearSelection();
		return;
	}
	ComboBox_GameModeName->SetSelectedOption(GameModeOptionToSelect);
}

void UGameModesWidget_DefiningConfig::PopulateGameModeNameComboBoxAfterSave()
{
	const FString GameModeNameSelectedOption = ComboBox_GameModeName->GetSelectedOption();
	const FString CustomGameModeName = TextBox_CustomGameModeName->GetText().ToString();

	if (CustomGameModeName.IsEmpty())
	{
		PopulateGameModeNameComboBox(GameModeNameSelectedOption);
	}
	else
	{
		PopulateGameModeNameComboBox(CustomGameModeName);
	}
	TextBox_CustomGameModeName->SetText(FText::GetEmpty());
}

void UGameModesWidget_DefiningConfig::OnTextChanged_CustomGameMode(const FText& NewCustomGameModeText)
{
	OnDefiningConfigUpdate_SaveStartButtonStates.Broadcast();
}

void UGameModesWidget_DefiningConfig::OnSelectionChanged_GameModeName(const FString SelectedGameModeName, const ESelectInfo::Type SelectionType)
{
	if (IsPresetGameMode(SelectedGameModeName))
	{
		BSBox_BaseGameMode->SetVisibility(ESlateVisibility::Collapsed);
		OnRepopulateGameModeOptions.Broadcast(FindPresetGameMode(SelectedGameModeName));
	}
	if (IsCustomGameMode(SelectedGameModeName))
	{
		BSBox_BaseGameMode->SetVisibility(ESlateVisibility::Visible);
		OnRepopulateGameModeOptions.Broadcast(FindCustomGameMode(SelectedGameModeName));
	}
	OnDefiningConfigUpdate_SaveStartButtonStates.Broadcast();
}

void UGameModesWidget_DefiningConfig::OnSelectionChanged_BaseGameMode(const FString SelectedBaseGameMode, const ESelectInfo::Type SelectionType)
{
	if (SelectionType != ESelectInfo::Type::Direct)
	{
		OnRepopulateGameModeOptions.Broadcast(FindPresetGameMode(SelectedBaseGameMode));
	}
}

void UGameModesWidget_DefiningConfig::OnSelectionChanged_GameModeDifficulty(const FString SelectedDifficulty, const ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Type::Direct)
	{
		return;
	}
	EGameModeDifficulty EnumDifficulty = EGameModeDifficulty::None;
	for (const EGameModeDifficulty Difficulty : TEnumRange<EGameModeDifficulty>())
	{
		if (UEnum::GetDisplayValueAsText(Difficulty).ToString().Equals(SelectedDifficulty))
		{
			EnumDifficulty = Difficulty;
			break;
		}
	}
	if (EnumDifficulty != EGameModeDifficulty::None)
	{
		const FBSConfig BSConfig = FBSConfig::MakePresetConfig(FindPresetGameMode(ComboBox_BaseGameMode->GetSelectedOption()).DefiningConfig.BaseGameMode, EnumDifficulty);
		OnRepopulateGameModeOptions.Broadcast(BSConfig);
	}
}
