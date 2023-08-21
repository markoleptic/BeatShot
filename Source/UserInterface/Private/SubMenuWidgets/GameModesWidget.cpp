// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.
// ReSharper disable CppMemberFunctionMayBeConst


#include "SubMenuWidgets/GameModesWidget.h"
#include "GlobalConstants.h"
#include "Blueprint/WidgetTree.h"
#include "Components/WidgetSwitcher.h"
#include "Components/VerticalBox.h"
#include "Components/Border.h"
#include "Components/CheckBox.h"
#include "Components/ComboBoxString.h"
#include "Components/EditableTextBox.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "OverlayWidgets/PopupMessageWidget.h"
#include "OverlayWidgets/AudioSelectWidget.h"
#include "OverlayWidgets/GameModeSharingWidget.h"
#include "WidgetComponents/DoubleSyncedSliderAndTextBox.h"
#include "WidgetComponents/SavedTextWidget.h"
#include "SubMenuWidgets/GameModesWidget_DefiningConfig.h"
#include "SubMenuWidgets/GameModesWidget_AIConfig.h"
#include "SubMenuWidgets/GameModesWidget_GridConfig.h"
#include "SubMenuWidgets/GameModesWidget_TargetConfig.h"
#include "SubMenuWidgets/CustomGameModesWidget/CustomGameModesWidget_CreatorView.h"
#include "SubMenuWidgets/CustomGameModesWidget/CustomGameModesWidget_PropertyView.h"
#include "WidgetComponents/BSComboBoxString.h"
#include "WidgetComponents/Buttons/MenuButton.h"
#include "Windows/WindowsPlatformApplicationMisc.h"

using namespace Constants;

void UGameModesWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	SetupButtons();
	BindAllDelegates();
	
	GameModeConfig = FindPresetGameMode("MultiBeat", EGameModeDifficulty::Normal);
	GameModeConfigPtr = &GameModeConfig;

	// Initialize CustomGameModesWidgets
	CustomGameModesWidget_CreatorView->Init(GameModeConfigPtr, GetGameModeDataAsset());
	CustomGameModesWidget_PropertyView->Init(GameModeConfigPtr, GetGameModeDataAsset());
	
	Border_DifficultySelect->SetVisibility(ESlateVisibility::Collapsed);
	Box_PropertyView->SetVisibility(ESlateVisibility::Collapsed);
	
	// Setup default custom game mode options to MultiBeat
	PopulateGameModeOptions(GameModeConfig);
}

void UGameModesWidget::SetupButtons()
{
	// Default Button Enabled States
	Button_PlayFromStandard->SetIsEnabled(false);
	Button_CustomizeFromStandard->SetIsEnabled(false);
	Button_SaveCustom->SetIsEnabled(false);
	Button_SaveCustomAndStart->SetIsEnabled(false);
	Button_RemoveSelectedCustom->SetIsEnabled(false);
	Button_ExportCustom->SetIsEnabled(false);
	Button_ImportCustom->SetIsEnabled(true);
	Button_RemoveAllCustom->SetIsEnabled(!LoadCustomGameModes().IsEmpty());
	MenuButton_CreatorView->SetActive();
	MenuButton_PropertyView->SetInActive();
	MenuButton_DefaultGameModes->SetActive();

	// Difficulty buttons
	Button_NormalDifficulty->SetDefaults(static_cast<uint8>(EGameModeDifficulty::Normal), Button_HardDifficulty);
	Button_HardDifficulty->SetDefaults(static_cast<uint8>(EGameModeDifficulty::Hard), Button_DeathDifficulty);
	Button_DeathDifficulty->SetDefaults(static_cast<uint8>(EGameModeDifficulty::Death), Button_NormalDifficulty);
	
	// Preset Game Mode Buttons
	Button_BeatGrid->SetDefaults(static_cast<uint8>(EBaseGameMode::BeatGrid), Button_BeatTrack);
	Button_BeatTrack->SetDefaults(static_cast<uint8>(EBaseGameMode::BeatTrack), Button_MultiBeat);
	Button_MultiBeat->SetDefaults(static_cast<uint8>(EBaseGameMode::MultiBeat), Button_SingleBeat);
	Button_SingleBeat->SetDefaults(static_cast<uint8>(EBaseGameMode::SingleBeat), Button_ClusterBeat);
	Button_ClusterBeat->SetDefaults(static_cast<uint8>(EBaseGameMode::ClusterBeat), Button_ChargedBeatTrack);
	Button_ChargedBeatTrack->SetDefaults(static_cast<uint8>(EBaseGameMode::ChargedBeatTrack), Button_BeatGrid);

	// Menu Buttons
	MenuButton_DefaultGameModes->SetDefaults(Box_DefaultGameModes, MenuButton_CustomGameModes);
	MenuButton_CustomGameModes->SetDefaults(Box_CustomGameModes, MenuButton_DefaultGameModes);
	MenuButton_PropertyView->SetDefaults(nullptr, MenuButton_CreatorView);
	MenuButton_CreatorView->SetDefaults(nullptr, MenuButton_PropertyView);
}

void UGameModesWidget::BindAllDelegates()
{
	// Menu Buttons
	MenuButton_DefaultGameModes->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_MenuButton);
	MenuButton_CustomGameModes->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_MenuButton);
	MenuButton_CreatorView->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_MenuButton);
	MenuButton_PropertyView->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_MenuButton);

	// Custom Game Modes Buttons
	Button_CustomizeFromStandard->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_CustomGameModeButton);
	Button_PlayFromStandard->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_CustomGameModeButton);
	Button_SaveCustom->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_CustomGameModeButton);
	Button_SaveCustomAndStart->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_CustomGameModeButton);
	Button_StartWithoutSaving->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_CustomGameModeButton);
	Button_RemoveAllCustom->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_CustomGameModeButton);
	Button_RemoveSelectedCustom->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_CustomGameModeButton);
	Button_ImportCustom->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_CustomGameModeButton);
	Button_ExportCustom->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_CustomGameModeButton);
	
	// Difficulty Buttons
	Button_NormalDifficulty->OnBSButtonPressed.AddDynamic(this, &UGameModesWidget::OnButtonClicked_Difficulty);
	Button_HardDifficulty->OnBSButtonPressed.AddDynamic(this, &UGameModesWidget::OnButtonClicked_Difficulty);
	Button_DeathDifficulty->OnBSButtonPressed.AddDynamic(this, &UGameModesWidget::OnButtonClicked_Difficulty);
	
	// Default Game Modes Buttons
	Button_BeatGrid->OnBSButtonPressed.AddDynamic(this, &UGameModesWidget::OnButtonClicked_DefaultGameMode);
	Button_BeatTrack->OnBSButtonPressed.AddDynamic(this, &UGameModesWidget::OnButtonClicked_DefaultGameMode);
	Button_MultiBeat->OnBSButtonPressed.AddDynamic(this, &UGameModesWidget::OnButtonClicked_DefaultGameMode);
	Button_SingleBeat->OnBSButtonPressed.AddDynamic(this, &UGameModesWidget::OnButtonClicked_DefaultGameMode);
	Button_ClusterBeat->OnBSButtonPressed.AddDynamic(this, &UGameModesWidget::OnButtonClicked_DefaultGameMode);
	Button_ChargedBeatTrack->OnBSButtonPressed.AddDynamic(this, &UGameModesWidget::OnButtonClicked_DefaultGameMode);
	
	//AIConfig->OnAIEnabled.BindUObject(this, &UGameModesWidget::UpdateSaveStartButtonStates);
	//TargetConfig->OnTargetUpdate_SaveStartButtonStates.AddUObject(this, &UGameModesWidget::OnTargetDamageTypeChanged);
	//DefiningConfig->OnRepopulateGameModeOptions.AddUObject(this, &UGameModesWidget::PopulateGameModeOptions);
	//DefiningConfig->OnDefiningConfigUpdate_SaveStartButtonStates.AddUObject(this, &UGameModesWidget::UpdateSaveStartButtonStates);
	//TargetConfig->GridConfig->OnBeatGridUpdate_SaveStartButtonStates.AddUObject(this, &UGameModesWidget::UpdateSaveStartButtonStates);

	// Custom Game Modes Widget Animation
	CustomGameModesWidget_CreatorView->RequestGameModeTemplateUpdate.AddUObject(this, &ThisClass::OnRequestGameModeTemplateUpdate);
	CustomGameModesWidget_PropertyView->RequestGameModeTemplateUpdate.AddUObject(this, &ThisClass::OnRequestGameModeTemplateUpdate);
	CustomGameModesWidget_PropertyView->RequestButtonStateUpdate.AddUObject(this, &ThisClass::UpdateSaveStartButtonStates);
	
	OnTransitionInCreatorViewFinish.BindDynamic(this, &UGameModesWidget::SetCollapsed_PropertyView);
	OnTransitionInPropertyViewFinish.BindDynamic(this, &UGameModesWidget::SetCollapsed_CreatorView);
}

void UGameModesWidget::OnButtonClicked_DefaultGameMode(const UBSButton* Button)
{
	PresetSelection_PresetGameMode = static_cast<EBaseGameMode>(Button->GetEnumValue());
	DefiningConfig->ComboBox_GameModeName->SetSelectedOption(UEnum::GetDisplayValueAsText(PresetSelection_PresetGameMode).ToString());
	PopulateGameModeOptions(FindPresetGameMode(PresetSelection_PresetGameMode, EGameModeDifficulty::Normal));
	
	Button_NormalDifficulty->SetInActive();
	Button_HardDifficulty->SetInActive();
	Button_DeathDifficulty->SetInActive();
	PresetSelection_Difficulty = EGameModeDifficulty::Normal;
	
	Border_DifficultySelect->SetVisibility(ESlateVisibility::Visible);
	Button_PlayFromStandard->SetIsEnabled(false);
	Button_CustomizeFromStandard->SetIsEnabled(true);
}

void UGameModesWidget::OnButtonClicked_Difficulty(const UBSButton* Button)
{
	PresetSelection_Difficulty = static_cast<EGameModeDifficulty>(Button->GetEnumValue());
	PopulateGameModeOptions(FindPresetGameMode(PresetSelection_PresetGameMode, PresetSelection_Difficulty));
	Button_PlayFromStandard->SetIsEnabled(true);
}

void UGameModesWidget::OnButtonClicked_CustomGameModeButton(const UBSButton* Button)
{
	if (Button == Button_CustomizeFromStandard)
	{
		MenuButton_CustomGameModes->SetActive();
	}
	else if (Button == Button_PlayFromStandard)
	{
		ShowAudioFormatSelect(true);
	}
	else if (Button == Button_SaveCustom)
	{
		OnButtonClicked_SaveCustom();
	}
	else if (Button == Button_SaveCustomAndStart)
	{
		OnButtonClicked_SaveCustomAndStart();
	}
	else if (Button == Button_StartWithoutSaving)
	{
		ShowAudioFormatSelect(false);
	}
	else if (Button == Button_RemoveAllCustom)
	{
		OnButtonClicked_RemoveAllCustom();
	}
	else if (Button == Button_RemoveSelectedCustom)
	{
		OnButtonClicked_RemoveSelectedCustom();
	}
	else if (Button == Button_ImportCustom)
	{
		OnButtonClicked_ImportCustom();
	}
	else if (Button == Button_ExportCustom)
	{
		OnButtonClicked_ExportCustom();
	}
}

void UGameModesWidget::OnButtonClicked_MenuButton(const UBSButton* Button)
{
	if (Button == MenuButton_CreatorView)
	{
		TransitionGameModeViewToCreator();
	}
	else if (Button == MenuButton_PropertyView)
	{
		TransitionGameModeViewToProperty();
	}
	else
	{
		if (const UMenuButton* MenuButton = Cast<UMenuButton>(Button))
		{
			if (UVerticalBox* Box = MenuButton->GetBox())
			{
				MenuSwitcher->SetActiveWidget(Box);
			}
		}
	}

}

void UGameModesWidget::OnButtonClicked_ImportCustom()
{
	GameModeSharingWidget = CreateWidget<UGameModeSharingWidget>(this, GameModeSharingClass);
	GameModeSharingWidget->InitPopup(FText::FromString("Import Custom Game Mode"), FText::GetEmpty(),
							  FText::FromString("Import"),
							  FText::FromString("Cancel"));
	
	GameModeSharingWidget->OnButton1Pressed_NonDynamic.AddLambda([&]
	{
		const FString ImportString = GameModeSharingWidget->GetImportString();
		const FBSConfig ImportedConfig = ImportCustomGameMode(ImportString);

		// Can't import default game modes
		if (IsPresetGameMode(ImportedConfig.DefiningConfig.CustomGameModeName))
		{
			SavedTextWidget->SetSavedText(FText::FromString("Failed to import " + ImportedConfig.DefiningConfig.CustomGameModeName));
			SavedTextWidget->PlayFadeInFadeOut();
			GameModeSharingWidget->FadeOut();
			return;
		}
		
		GameModeSharingWidget->FadeOut();

		// Check if overriding existing Custom Game Mode
		if (IsCustomGameMode(ImportedConfig.DefiningConfig.CustomGameModeName))
		{
			ShowConfirmOverwriteMessage_Import(ImportedConfig);
		}
		else
		{
			SaveCustomGameMode(ImportedConfig);
			SavedTextWidget->SetSavedText(FText::FromString("Successfully imported " + ImportedConfig.DefiningConfig.CustomGameModeName));
			SavedTextWidget->PlayFadeInFadeOut();
			PopulateGameModeOptions(ImportedConfig);
			DefiningConfig->PopulateGameModeNameComboBox(ImportedConfig.DefiningConfig.CustomGameModeName);
		}
	});
	GameModeSharingWidget->OnButton2Pressed_NonDynamic.AddLambda([&]
	{
		GameModeSharingWidget->FadeOut();
	});
	GameModeSharingWidget->AddToViewport();
	GameModeSharingWidget->FadeIn();
}

void UGameModesWidget::OnButtonClicked_ExportCustom()
{
	const FString SelectedCustomGameMode = DefiningConfig->ComboBox_GameModeName->GetSelectedOption();
	
	if (!IsCustomGameMode(SelectedCustomGameMode))
	{
		SavedTextWidget->SetSavedText(FText::FromString("Can't export Default Game Modes"));
		SavedTextWidget->PlayFadeInFadeOut();
		return;
	}

	const FBSConfig SelectedConfig = GetCustomGameModeOptions();
	const FString ExportString = ExportCustomGameMode(SelectedConfig);
	FPlatformApplicationMisc::ClipboardCopy(*ExportString);

	SavedTextWidget->SetSavedText(FText::FromString("Export String copied to clipboard!"));
	SavedTextWidget->PlayFadeInFadeOut();
}

void UGameModesWidget::OnButtonClicked_SaveCustom()
{
	if (IsPresetGameMode(DefiningConfig->TextBox_CustomGameModeName->GetText().ToString()))
	{
		DefiningConfig->TextBox_CustomGameModeName->SetText(FText());
		return;
	}
	
	if (CheckForExistingAndDisplayOverwriteMessage(false))
	{
		return;
	}
	
	SaveCustomGameModeAndShowSavedText(GetCustomGameModeOptions());
	DefiningConfig->PopulateGameModeNameComboBoxAfterSave();
}

void UGameModesWidget::OnButtonClicked_SaveCustomAndStart()
{
	if (IsPresetGameMode(DefiningConfig->TextBox_CustomGameModeName->GetText().ToString()))
	{
		DefiningConfig->TextBox_CustomGameModeName->SetText(FText());
		return;
	}
	
	if (CheckForExistingAndDisplayOverwriteMessage(true))
	{
		return;
	}

	SaveCustomGameModeAndShowSavedText(GetCustomGameModeOptions());
	DefiningConfig->PopulateGameModeNameComboBoxAfterSave();
	ShowAudioFormatSelect(false);
}

void UGameModesWidget::OnButtonClicked_RemoveSelectedCustom()
{
	if (!IsCustomGameMode(DefiningConfig->ComboBox_GameModeName->GetSelectedOption()))
	{
		return;
	}
	
	PopupMessageWidget = CreateWidget<UPopupMessageWidget>(this, PopupMessageClass);
	PopupMessageWidget->InitPopup(FText::FromString("Removal Confirmation"), FText::FromString("Are you sure you want remove " + DefiningConfig->ComboBox_GameModeName->GetSelectedOption() + "?"),
							  FText::FromString("Yes"),
							  FText::FromString("No"));
	PopupMessageWidget->OnButton1Pressed_NonDynamic.AddLambda([&]
	{
		const FString RemovedGameModeName = DefiningConfig->ComboBox_GameModeName->GetSelectedOption();
		const int32 NumRemoved = RemoveCustomGameMode(FindCustomGameMode(RemovedGameModeName));
		if (NumRemoved >= 1)
		{
			SavedTextWidget->SetSavedText(FText::FromString(RemovedGameModeName + " removed"));
			SavedTextWidget->PlayFadeInFadeOut();
		}
		DefiningConfig->PopulateGameModeNameComboBox("");
		PopupMessageWidget->FadeOut();
	});
	PopupMessageWidget->OnButton2Pressed_NonDynamic.AddLambda([&]
	{
		PopupMessageWidget->FadeOut();
	});
	
	PopupMessageWidget->AddToViewport();
	PopupMessageWidget->FadeIn();
}

void UGameModesWidget::OnButtonClicked_RemoveAllCustom()
{
	PopupMessageWidget = CreateWidget<UPopupMessageWidget>(this, PopupMessageClass);
	PopupMessageWidget->InitPopup(FText::FromString("Removal Confirmation"), FText::FromString("Are you sure you want remove all custom game modes?"),
							  FText::FromString("Yes"),
							  FText::FromString("No"));
	PopupMessageWidget->OnButton1Pressed_NonDynamic.AddLambda([&]
	{
		RemoveAllCustomGameModes();
		PopupMessageWidget->FadeOut();
		DefiningConfig->PopulateGameModeNameComboBox("");
		SavedTextWidget->SetSavedText(FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "GM_AllGameModesRemovedText"));
		SavedTextWidget->PlayFadeInFadeOut();
	});
	PopupMessageWidget->OnButton2Pressed_NonDynamic.AddLambda([&]
	{
		PopupMessageWidget->FadeOut();
	});
	PopupMessageWidget->AddToViewport();
	PopupMessageWidget->FadeIn();
}

// Replaced with RequestUpdateAfterConfigChange
void UGameModesWidget::OnTargetDamageTypeChanged()
{
	switch (TargetConfig->GetTargetDamageType()) {
	case ETargetDamageType::Tracking:
		AIConfig->CheckBox_EnableAI->SetIsChecked(false);
		AIConfig->CheckBox_EnableAI->SetIsEnabled(false);
		break;
	case ETargetDamageType::Hit:
		AIConfig->CheckBox_EnableAI->SetIsEnabled(true);
		break;
	case ETargetDamageType::Combined:
	case ETargetDamageType::None:
		break;
	}
}

void UGameModesWidget::PopulateGameModeOptions(const FBSConfig& InBSConfig)
{
	GameModeConfig = InBSConfig;
	GameModeConfigPtr = &GameModeConfig;
	DefiningConfig->InitializeDefiningConfig(InBSConfig.DefiningConfig, InBSConfig.DefiningConfig.BaseGameMode);
	TargetConfig->InitializeTargetConfig(InBSConfig.TargetConfig, InBSConfig.DefiningConfig.BaseGameMode);
	TargetConfig->GridConfig->InitGridConfig(InBSConfig.GridConfig, TargetConfig->TargetScaleConstrained->GetTextTooltipBox_Max());
	TargetConfig->GridConfig->OnBeatGridUpdate_MaxTargetScale(InBSConfig.TargetConfig.MaxTargetScale);
	AIConfig->InitializeAIConfig(InBSConfig.AIConfig, InBSConfig.DefiningConfig.BaseGameMode);
	CustomGameModesWidget_CreatorView->Update();
	CustomGameModesWidget_PropertyView->Update();
	OnTargetDamageTypeChanged();
}

FBSConfig UGameModesWidget::GetCustomGameModeOptions() const
{
	FBSConfig ReturnStruct;
	ReturnStruct.DefiningConfig = DefiningConfig->GetDefiningConfig();
	ReturnStruct.TargetConfig = TargetConfig->GetTargetConfig();
	ReturnStruct.AudioConfig.PlayerDelay = ReturnStruct.TargetConfig.SpawnBeatDelay;
	ReturnStruct.AIConfig = AIConfig->GetAIConfig();
	ReturnStruct.GridConfig = TargetConfig->GridConfig->GetGridConfig();
	return ReturnStruct;
}

void UGameModesWidget::SaveCustomGameModeAndShowSavedText(const FBSConfig& GameModeToSave)
{
	SaveCustomGameMode(GameModeToSave);
	const TArray SavedText = {FText::FromString(GameModeToSave.DefiningConfig.CustomGameModeName), FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "GM_GameModeSavedText")};
	SavedTextWidget->SetSavedText(FText::Join(FText::FromString(" "), SavedText));
	SavedTextWidget->PlayFadeInFadeOut();
}

void UGameModesWidget::UpdateSaveStartButtonStates()
{
	/*const bool bNoSavedCustomGameModes = LoadCustomGameModes().IsEmpty();
	const bool bGridIsConstrained = TargetConfig->GridConfig->IsAnyParameterConstrained();
	const bool bIsDefaultMode = IsPresetGameMode(DefiningConfig->ComboBox_GameModeName->GetSelectedOption());
	const bool bIsCustomMode = IsCustomGameMode(DefiningConfig->ComboBox_GameModeName->GetSelectedOption());
	const bool bGameModeNameComboBoxEmpty = DefiningConfig->ComboBox_GameModeName->GetSelectedOption().IsEmpty();
	const bool bCustomTextEmpty = DefiningConfig->TextBox_CustomGameModeName->GetText().IsEmptyOrWhitespace();
	const bool bInvalidCustomGameModeName = IsPresetGameMode(DefiningConfig->TextBox_CustomGameModeName->GetText().ToString());
	const bool bAIConfigIsInvalid = !IsAIValid();

	/* RemoveAll Button #1#
	if (bNoSavedCustomGameModes)
	{
		Button_RemoveAllCustom->SetIsEnabled(false);
	}
	else
	{
		Button_RemoveAllCustom->SetIsEnabled(true);
	}
	
	if (bGridIsConstrained)
	{
		Button_SaveCustom->SetIsEnabled(false);
		Button_SaveCustomAndStart->SetIsEnabled(false);
		Button_StartWithoutSaving->SetIsEnabled(false);
		return;
	}

	if (bAIConfigIsInvalid)
	{
		Button_SaveCustom->SetIsEnabled(false);
		Button_SaveCustomAndStart->SetIsEnabled(false);
		Button_StartWithoutSaving->SetIsEnabled(false);
		return;
	}

	// At this point, they can play the game mode but not save it
	Button_StartWithoutSaving->SetIsEnabled(true);
	
	if (bGameModeNameComboBoxEmpty || (bIsDefaultMode && bCustomTextEmpty) || bInvalidCustomGameModeName)
	{
		Button_SaveCustom->SetIsEnabled(false);
		Button_SaveCustomAndStart->SetIsEnabled(false);
		Button_RemoveSelectedCustom->SetIsEnabled(false);
		Button_ExportCustom->SetIsEnabled(false);
		return;
	}

	if (bIsCustomMode || (bIsDefaultMode && !bCustomTextEmpty))
	{
		Button_SaveCustom->SetIsEnabled(true);
		Button_SaveCustomAndStart->SetIsEnabled(true);
		Button_RemoveSelectedCustom->SetIsEnabled(true);
		Button_ExportCustom->SetIsEnabled(true);
	}*/

	// --------------------
	// New stuff start here
	// --------------------
	
	const FBS_DefiningConfig LocalDefiningConfig = CustomGameModesWidget_CreatorView->GetDefiningConfig();
	const FString NewCustomGameModeName = CustomGameModesWidget_CreatorView->GetNewCustomGameModeName();
	
	const bool bNoSavedCustomGameModes = LoadCustomGameModes().IsEmpty();
	// TODO: Grid constraints
	const bool bGridIsConstrained = false;
	const bool bIsDefaultMode = LocalDefiningConfig.BaseGameMode != EBaseGameMode::None;
	const bool bIsCustomMode = IsCustomGameMode(LocalDefiningConfig.CustomGameModeName);
	const bool bGameModeNameComboBoxEmpty = LocalDefiningConfig.BaseGameMode == EBaseGameMode::None;
	const bool bCustomTextEmpty = NewCustomGameModeName.IsEmpty();
	const bool bInvalidCustomGameModeName = IsPresetGameMode(NewCustomGameModeName);
	// TODO: AI constraints
	const bool bAIConfigIsInvalid = false;

	/* RemoveAll Button */
	if (bNoSavedCustomGameModes)
	{
		Button_RemoveAllCustom->SetIsEnabled(false);
	}
	else
	{
		Button_RemoveAllCustom->SetIsEnabled(true);
	}
	
	if (bGridIsConstrained)
	{
		Button_SaveCustom->SetIsEnabled(false);
		Button_SaveCustomAndStart->SetIsEnabled(false);
		Button_StartWithoutSaving->SetIsEnabled(false);
		return;
	}

	if (bAIConfigIsInvalid)
	{
		Button_SaveCustom->SetIsEnabled(false);
		Button_SaveCustomAndStart->SetIsEnabled(false);
		Button_StartWithoutSaving->SetIsEnabled(false);
		return;
	}

	// At this point, they can play the game mode but not save it
	Button_StartWithoutSaving->SetIsEnabled(true);
	
	if (bGameModeNameComboBoxEmpty || (bIsDefaultMode && bCustomTextEmpty) || bInvalidCustomGameModeName)
	{
		Button_SaveCustom->SetIsEnabled(false);
		Button_SaveCustomAndStart->SetIsEnabled(false);
		Button_RemoveSelectedCustom->SetIsEnabled(false);
		Button_ExportCustom->SetIsEnabled(false);
		return;
	}

	if (bIsCustomMode || (bIsDefaultMode && !bCustomTextEmpty))
	{
		Button_SaveCustom->SetIsEnabled(true);
		Button_SaveCustomAndStart->SetIsEnabled(true);
		Button_RemoveSelectedCustom->SetIsEnabled(true);
		Button_ExportCustom->SetIsEnabled(true);
	}
}

void UGameModesWidget::ShowConfirmOverwriteMessage(const bool bStartGameAfter)
{
	PopupMessageWidget = CreateWidget<UPopupMessageWidget>(this, PopupMessageClass);
	PopupMessageWidget->InitPopup(FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "GM_OverwritePopupTitle"), FText::GetEmpty(),
								  FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "GM_OverwriteConfirm"),
								  FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "GM_OverwriteCancel"));

	PopupMessageWidget->OnButton1Pressed_NonDynamic.AddLambda([this, bStartGameAfter]
	{
		if (bStartGameAfter)
		{
			PopupMessageWidget->FadeOut();
			SaveCustomGameModeAndShowSavedText(GetCustomGameModeOptions());
			ShowAudioFormatSelect(false);
		}
		else
		{
			PopupMessageWidget->FadeOut();
			SaveCustomGameModeAndShowSavedText(GetCustomGameModeOptions());
			DefiningConfig->PopulateGameModeNameComboBoxAfterSave();
		}
	});
	PopupMessageWidget->OnButton2Pressed_NonDynamic.AddLambda([this]
	{
		PopupMessageWidget->FadeOut();
	});

	PopupMessageWidget->AddToViewport();
	PopupMessageWidget->FadeIn();
}

void UGameModesWidget::ShowConfirmOverwriteMessage_Import(const FBSConfig& ImportedConfig)
{
	PopupMessageWidget = CreateWidget<UPopupMessageWidget>(this, PopupMessageClass);
	PopupMessageWidget->InitPopup(FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "GM_OverwritePopupTitle"), FText::GetEmpty(),
								  FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "GM_OverwriteConfirm"),
								  FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "GM_OverwriteCancel"));
	PopupMessageWidget->OnButton1Pressed_NonDynamic.AddLambda([this, ImportedConfig]
	{
		PopupMessageWidget->FadeOut();
		SaveCustomGameMode(ImportedConfig);
		SavedTextWidget->SetSavedText(FText::FromString("Successfully imported " + ImportedConfig.DefiningConfig.CustomGameModeName));
		SavedTextWidget->PlayFadeInFadeOut();
		PopulateGameModeOptions(ImportedConfig);
		DefiningConfig->PopulateGameModeNameComboBox(ImportedConfig.DefiningConfig.CustomGameModeName);
	});
	PopupMessageWidget->OnButton2Pressed_NonDynamic.AddLambda([this]
	{
		PopupMessageWidget->FadeOut();
	});

	PopupMessageWidget->AddToViewport();
	PopupMessageWidget->FadeIn();
}

void UGameModesWidget::ShowAudioFormatSelect(const bool bStartFromDefaultGameMode)
{
	AudioSelectWidget = CreateWidget<UAudioSelectWidget>(this, AudioSelectClass);
	AudioSelectWidget->OnStartButtonClickedDelegate.BindLambda([this, bStartFromDefaultGameMode](const FBS_AudioConfig& AudioConfig)
	{
		FGameModeTransitionState GameModeTransitionState;
		FBSConfig BSConfig;
		GameModeTransitionState.bSaveCurrentScores = false;
		if (bIsMainMenuChild)
		{
			GameModeTransitionState.TransitionState = ETransitionState::StartFromMainMenu;
		}
		else
		{
			GameModeTransitionState.TransitionState = ETransitionState::StartFromPostGameMenu;
		}
		if (bStartFromDefaultGameMode)
		{
			BSConfig = FindPresetGameMode(PresetSelection_PresetGameMode, PresetSelection_Difficulty);
		}
		else
		{
			BSConfig = GetCustomGameModeOptions();
		}
		BSConfig.AudioConfig.SongTitle = AudioConfig.SongTitle;
		BSConfig.AudioConfig.SongLength = AudioConfig.SongLength;
		BSConfig.AudioConfig.InAudioDevice = AudioConfig.InAudioDevice;
		BSConfig.AudioConfig.OutAudioDevice = AudioConfig.OutAudioDevice;
		BSConfig.AudioConfig.SongPath = AudioConfig.SongPath;
		BSConfig.AudioConfig.bPlaybackAudio = AudioConfig.bPlaybackAudio;
		BSConfig.AudioConfig.AudioFormat = AudioConfig.AudioFormat;

		/* Override the player delay to zero if using Capture */
		if (BSConfig.AudioConfig.AudioFormat == EAudioFormat::Capture || BSConfig.AudioConfig.AudioFormat == EAudioFormat::Loopback)
		{
			BSConfig.AudioConfig.PlayerDelay = 0.f;
			BSConfig.TargetConfig.SpawnBeatDelay = 0.f;
		}
		
		GameModeTransitionState.BSConfig = BSConfig;
		OnGameModeStateChanged.Broadcast(GameModeTransitionState);
		AudioSelectWidget->FadeOut();
	});
	AudioSelectWidget->AddToViewport();
	AudioSelectWidget->FadeIn();
}

bool UGameModesWidget::CheckForExistingAndDisplayOverwriteMessage(const bool bStartGameAfter)
{
	const FString SelectedGameModeName = DefiningConfig->ComboBox_GameModeName->GetSelectedOption();
	const FString CustomGameModeName = DefiningConfig->TextBox_CustomGameModeName->GetText().ToString();

	if ((IsCustomGameMode(SelectedGameModeName) && CustomGameModeName.IsEmpty()) ||
		IsCustomGameMode(CustomGameModeName))
	{
		ShowConfirmOverwriteMessage(bStartGameAfter);
		return true;
	}
	return false;
}

bool UGameModesWidget::IsAIValid()
{
	if (AIConfig->CheckBox_EnableAI->IsChecked())
	{
		if (TargetConfig->GetTargetDistributionPolicy() == ETargetDistributionPolicy::Grid)
		{
			if (const FIntPoint Spacing = TargetConfig->GridConfig->GetGridSpacing(); Spacing.X % 5 != 0 || Spacing.Y % 5 != 0)
			{
				if (!TooltipWarningImage_EnableAI)
				{
					TooltipWarningImage_EnableAI = ConstructWarningEMarkWidget(AIConfig->HorizontalBox_EnableAI);
					AIConfig->SetupTooltip(TooltipWarningImage_EnableAI.Get(), IBSWidgetInterface::GetTooltipTextFromKey("InvalidAI_GridSpacing"));
				}
				TooltipWarningImage_EnableAI->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				return false;
			}
		}
	}
	
	if (TooltipWarningImage_EnableAI)
	{
		TooltipWarningImage_EnableAI->SetVisibility(ESlateVisibility::Collapsed);
	}
	return true;
}

void UGameModesWidget::TransitionGameModeViewToCreator()
{
	CustomGameModesWidget_CreatorView->Update();
	SynchronizeCustomGameModeName(CustomGameModesWidget_PropertyView, CustomGameModesWidget_CreatorView);
	BindToAnimationFinished(TransitionCustomGameModeView, OnTransitionInCreatorViewFinish);
	Box_CreatorView->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	PlayAnimationReverse(TransitionCustomGameModeView);
}

void UGameModesWidget::TransitionGameModeViewToProperty()
{
	CustomGameModesWidget_PropertyView->Update();
	SynchronizeCustomGameModeName(CustomGameModesWidget_CreatorView, CustomGameModesWidget_PropertyView);
	BindToAnimationFinished(TransitionCustomGameModeView, OnTransitionInPropertyViewFinish);
	Box_PropertyView->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	PlayAnimationForward(TransitionCustomGameModeView);
}

void UGameModesWidget::SetCollapsed_CreatorView()
{
	Box_CreatorView->SetVisibility(ESlateVisibility::Collapsed);
	UnbindFromAnimationFinished(TransitionCustomGameModeView, OnTransitionInPropertyViewFinish);
}

void UGameModesWidget::SetCollapsed_PropertyView()
{
	Box_PropertyView->SetVisibility(ESlateVisibility::Collapsed);
	UnbindFromAnimationFinished(TransitionCustomGameModeView, OnTransitionInCreatorViewFinish);
}

void UGameModesWidget::OnRequestGameModeTemplateUpdate(const FString& InGameMode, const EGameModeDifficulty& Difficulty)
{
	if (IsPresetGameMode(InGameMode))
	{
		PopulateGameModeOptions(FindPresetGameMode(InGameMode, Difficulty));
	}
	else if (IsCustomGameMode(InGameMode))
	{
		PopulateGameModeOptions(FindCustomGameMode(InGameMode));
	}
}

void UGameModesWidget::SynchronizeCustomGameModeName(const TObjectPtr<UCustomGameModesWidgetBase> From, const TObjectPtr<UCustomGameModesWidgetBase> To)
{
	const FString NewGameModeName = From->GetNewCustomGameModeName();
	if (NewGameModeName.IsEmpty() || IsCustomGameMode(NewGameModeName) || IsPresetGameMode(NewGameModeName))
	{
		return;
	}
	To->SetNewCustomGameModeName(NewGameModeName);
}

UTooltipImage* UGameModesWidget::ConstructWarningEMarkWidget(UHorizontalBox* BoxToPlaceIn)
{
	UTooltipImage* TooltipImage = WidgetTree->ConstructWidget<UTooltipImage>(WarningEMarkClass);
	UHorizontalBoxSlot* HorizontalBoxSlot = BoxToPlaceIn->AddChildToHorizontalBox(TooltipImage);
	HorizontalBoxSlot->SetHorizontalAlignment(HAlign_Right);
	HorizontalBoxSlot->SetPadding(FMargin(10.f, 0.f, 0.f, 0.f));
	HorizontalBoxSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
	return TooltipImage;
}
