// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

// ReSharper disable CppMemberFunctionMayBeConst
#include "SubMenuWidgets/GameModesWidget_DefiningConfig.h"
#include "Components/ComboBoxString.h"
#include "Components/EditableTextBox.h"
#include "WidgetComponents/BSHorizontalBox.h"

void UGameModesWidget_DefiningConfig::NativeConstruct()
{
	Super::NativeConstruct();

	SetupTooltip(QMark_GameModeTemplate, GetTooltipTextFromKey("GameModeTemplate"));
	SetupTooltip(QMark_BaseGameMode, GetTooltipTextFromKey("BaseGameMode"));
	SetupTooltip(QMark_CustomGameModeName, GetTooltipTextFromKey("CustomGameModeName"));
	SetupTooltip(QMark_GameModeDifficulty, GetTooltipTextFromKey("GameModeDifficulty"));

	ComboBox_GameModeName->OnSelectionChanged.AddDynamic(this, &UGameModesWidget_DefiningConfig::OnSelectionChanged_GameModeName);
	TextBox_CustomGameModeName->OnTextChanged.AddDynamic(this, &UGameModesWidget_DefiningConfig::OnTextChanged_CustomGameMode);
	ComboBox_BaseGameMode->OnSelectionChanged.AddDynamic(this, &UGameModesWidget_DefiningConfig::OnSelectionChanged_BaseGameMode);
	ComboBox_GameModeDifficulty->OnSelectionChanged.AddDynamic(this, &UGameModesWidget_DefiningConfig::OnSelectionChanged_GameModeDifficulty);

	ComboBox_GameModeName->ClearOptions();
	ComboBox_BaseGameMode->ClearOptions();

	/* Add DefaultModes to GameModeName ComboBox and BaseGameMode ComboBox */
	for (const EBaseGameMode& GameMode : TEnumRange<EBaseGameMode>())
	{
		if (GameMode != EBaseGameMode::None)
		{
			const FString GameModeName = UEnum::GetDisplayValueAsText(GameMode).ToString();
			ComboBox_GameModeName->AddOption(GameModeName);
			ComboBox_BaseGameMode->AddOption(GameModeName);
		}
	}

	/* Add Custom Game Modes to GameModeName ComboBox */
	if (!LoadCustomGameModes().IsEmpty())
	{
		const FBSConfig GameMode = LoadCustomGameModes()[0];
		const FString Result = ExportCustomGameMode(GameMode);
	}
	
	for (const FBSConfig& GameMode : LoadCustomGameModes())
	{
		ComboBox_GameModeName->AddOption(GameMode.DefiningConfig.CustomGameModeName);
	}

	/* Add difficulties to GameModeDifficulty ComboBox */
	for (const EGameModeDifficulty& Mode : TEnumRange<EGameModeDifficulty>())
	{
		ComboBox_GameModeDifficulty->AddOption(UEnum::GetDisplayValueAsText(Mode).ToString());
	}
}

void UGameModesWidget_DefiningConfig::InitSettingCategoryWidget()
{
	Super::InitSettingCategoryWidget();
}

void UGameModesWidget_DefiningConfig::InitializeDefiningConfig(const FBS_DefiningConfig& InDefiningConfig, const EBaseGameMode& BaseGameMode)
{
	ComboBox_BaseGameMode->SetSelectedOption(UEnum::GetDisplayValueAsText(BaseGameMode).ToString());
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

	for (const EBaseGameMode& GameMode : TEnumRange<EBaseGameMode>())
	{
		if (GameMode != EBaseGameMode::None)
		{
			ComboBox_GameModeName->AddOption(UEnum::GetDisplayValueAsText(GameMode).ToString());
		}
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
		OnRepopulateGameModeOptions.Broadcast(FindPresetGameMode(SelectedGameModeName, EGameModeDifficulty::Normal));
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
		OnRepopulateGameModeOptions.Broadcast(FindPresetGameMode(SelectedBaseGameMode, EGameModeDifficulty::Normal));
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
		const FBSConfig BSConfig = FindPresetGameMode(ComboBox_BaseGameMode->GetSelectedOption(), EnumDifficulty);
		OnRepopulateGameModeOptions.Broadcast(BSConfig);
	}
}
