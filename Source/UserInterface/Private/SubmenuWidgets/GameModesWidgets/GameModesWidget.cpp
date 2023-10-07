// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.
// ReSharper disable CppMemberFunctionMayBeConst


#include "SubMenuWidgets/GameModesWidgets/GameModesWidget.h"
#include "GlobalConstants.h"
#include "Blueprint/WidgetTree.h"
#include "Components/WidgetSwitcher.h"
#include "Components/VerticalBox.h"
#include "Components/Border.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "OverlayWidgets/PopupWidgets/PopupMessageWidget.h"
#include "OverlayWidgets/PopupWidgets/AudioSelectWidget.h"
#include "OverlayWidgets/PopupWidgets/GameModeSharingWidget.h"
#include "WidgetComponents/SavedTextWidget.h"
#include "SubMenuWidgets/GameModesWidgets/CustomGameModesWidget_CreatorView.h"
#include "SubMenuWidgets/GameModesWidgets/CustomGameModesWidget_PropertyView.h"
#include "WidgetComponents/Buttons/MenuButton.h"
#include "Windows/WindowsPlatformApplicationMisc.h"
#include "SubMenuWidgets/GameModesWidgets/Components/CustomGameModesWidget_Preview.h"

using namespace Constants;

void UGameModesWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetupButtons();
	BindAllDelegates();

	GameModeConfig = FBSConfig();
	BSConfig = &GameModeConfig;

	// Start with CustomGameModesWidget_CreatorView as default
	Box_CreatorView->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	Box_PropertyView->SetVisibility(ESlateVisibility::Collapsed);
	CustomGameModesWidget_Current = CustomGameModesWidget_CreatorView;

	// Initialize CustomGameModesWidgets
	CustomGameModesWidget_CreatorView->Init(BSConfig, GetGameModeDataAsset());
	CustomGameModesWidget_PropertyView->Init(BSConfig, GetGameModeDataAsset());

	// Setup default custom game mode options to MultiBeat Normal
	InitCustomGameModesWidgetOptions(EBaseGameMode::MultiBeat, EGameModeDifficulty::Normal);

	Border_DifficultySelect->SetVisibility(ESlateVisibility::Collapsed);

	if (CustomGameModesWidget_CreatorView->Widget_Preview)
	{
		CustomGameModesWidget_CreatorView->Widget_Preview->ToggleGameModePreview(bIsMainMenuChild);
	}
}

void UGameModesWidget::NativeDestruct()
{
	Super::NativeDestruct();
	BSConfig = nullptr;
}

bool UGameModesWidget::GetCreatorViewVisible() const
{
	return CustomGameModesWidget_Current == CustomGameModesWidget_CreatorView;
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
	Button_ClearRLHistory->SetIsEnabled(false);
	Button_RemoveAllCustom->SetIsEnabled(!LoadCustomGameModes().IsEmpty());
	MenuButton_CreatorView->SetActive();
	MenuButton_PropertyView->SetInActive();
	MenuButton_DefaultGameModes->SetActive();

	CustomGameModesWidget_CreatorView->Widget_Preview->Button_Create->SetIsEnabled(false);
	CustomGameModesWidget_CreatorView->Widget_Preview->Button_RefreshPreview->SetIsEnabled(true);

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

void UGameModesWidget::InitCustomGameModesWidgetOptions(const EBaseGameMode& BaseGameMode, const EGameModeDifficulty& Difficulty)
{
	const FBSConfig DefaultConfig = FindPresetGameMode(BaseGameMode, Difficulty);
	const FStartWidgetProperties StartWidgetUpdate = FStartWidgetProperties(DefaultConfig.DefiningConfig, true);
	CustomGameModesWidget_CreatorView->SetStartWidgetProperties(StartWidgetUpdate);
	CustomGameModesWidget_PropertyView->SetStartWidgetProperties(StartWidgetUpdate);
	PopulateGameModeOptions(DefaultConfig);
}

void UGameModesWidget::BindAllDelegates()
{
	// Menu Buttons
	MenuButton_DefaultGameModes->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_MenuButton);
	MenuButton_CustomGameModes->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_MenuButton);
	MenuButton_CreatorView->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_MenuButton);
	MenuButton_PropertyView->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_MenuButton);

	// Custom Game Modes Buttons
	Button_SaveCustom->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_CustomGameModeButton);
	Button_SaveCustomAndStart->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_CustomGameModeButton);
	Button_StartWithoutSaving->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_CustomGameModeButton);
	Button_RemoveAllCustom->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_CustomGameModeButton);
	Button_RemoveSelectedCustom->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_CustomGameModeButton);
	Button_ImportCustom->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_CustomGameModeButton);
	Button_ExportCustom->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_CustomGameModeButton);
	Button_ClearRLHistory->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_CustomGameModeButton);

	// Default Game Modes Buttons
	Button_CustomizeFromStandard->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_DefaultGameMode);
	Button_PlayFromStandard->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_DefaultGameMode);

	// Difficulty Buttons
	Button_NormalDifficulty->OnBSButtonPressed.AddDynamic(this, &UGameModesWidget::OnButtonClicked_SelectedDifficulty);
	Button_HardDifficulty->OnBSButtonPressed.AddDynamic(this, &UGameModesWidget::OnButtonClicked_SelectedDifficulty);
	Button_DeathDifficulty->OnBSButtonPressed.AddDynamic(this, &UGameModesWidget::OnButtonClicked_SelectedDifficulty);

	// Default Game Modes Buttons
	Button_BeatGrid->OnBSButtonPressed.AddDynamic(this, &UGameModesWidget::OnButtonClicked_SelectedDefaultGameMode);
	Button_BeatTrack->OnBSButtonPressed.AddDynamic(this, &UGameModesWidget::OnButtonClicked_SelectedDefaultGameMode);
	Button_MultiBeat->OnBSButtonPressed.AddDynamic(this, &UGameModesWidget::OnButtonClicked_SelectedDefaultGameMode);
	Button_SingleBeat->OnBSButtonPressed.AddDynamic(this, &UGameModesWidget::OnButtonClicked_SelectedDefaultGameMode);
	Button_ClusterBeat->OnBSButtonPressed.AddDynamic(this, &UGameModesWidget::OnButtonClicked_SelectedDefaultGameMode);
	Button_ChargedBeatTrack->OnBSButtonPressed.AddDynamic(this, &UGameModesWidget::OnButtonClicked_SelectedDefaultGameMode);

	// Custom Game Modes Widgets
	CustomGameModesWidget_CreatorView->Widget_Preview->Button_Create->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_CustomGameModeButton);
	CustomGameModesWidget_CreatorView->Widget_Preview->Button_RefreshPreview->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_CustomGameModeButton);
	CustomGameModesWidget_CreatorView->RequestGameModeTemplateUpdate.AddUObject(this, &ThisClass::OnRequestGameModeTemplateUpdate);
	CustomGameModesWidget_PropertyView->RequestGameModeTemplateUpdate.AddUObject(this, &ThisClass::OnRequestGameModeTemplateUpdate);

	CustomGameModesWidget_CreatorView->RequestGameModePreviewUpdate.AddUObject(this, &ThisClass::RefreshGameModePreview);

	CustomGameModesWidget_PropertyView->RequestButtonStateUpdate.AddUObject(this, &ThisClass::UpdateSaveStartButtonStates);
	CustomGameModesWidget_CreatorView->RequestButtonStateUpdate.AddUObject(this, &ThisClass::UpdateSaveStartButtonStates);
	CustomGameModesWidget_PropertyView->OnGameModeBreakingChange.AddUObject(this, &ThisClass::OnGameModeBreakingOptionPresentStateChanged);
	CustomGameModesWidget_CreatorView->OnGameModeBreakingChange.AddUObject(this, &ThisClass::OnGameModeBreakingOptionPresentStateChanged);

	OnTransitionComplete_ToCreatorView.BindDynamic(this, &UGameModesWidget::OnTransitionCompleted_ToCreatorView);
	OnTransitionComplete_ToPropertyView.BindDynamic(this, &UGameModesWidget::OnTransitionCompleted_ToPropertyView);
}

void UGameModesWidget::OnButtonClicked_SelectedDefaultGameMode(const UBSButton* Button)
{
	PresetSelection_PresetGameMode = static_cast<EBaseGameMode>(Button->GetEnumValue());

	Button_NormalDifficulty->SetInActive();
	Button_HardDifficulty->SetInActive();
	Button_DeathDifficulty->SetInActive();
	PresetSelection_Difficulty = EGameModeDifficulty::Normal;

	Border_DifficultySelect->SetVisibility(ESlateVisibility::Visible);
	Button_PlayFromStandard->SetIsEnabled(false);
	Button_CustomizeFromStandard->SetIsEnabled(true);
}

void UGameModesWidget::OnButtonClicked_SelectedDifficulty(const UBSButton* Button)
{
	PresetSelection_Difficulty = static_cast<EGameModeDifficulty>(Button->GetEnumValue());
	Button_PlayFromStandard->SetIsEnabled(true);
}

void UGameModesWidget::OnButtonClicked_DefaultGameMode(const UBSButton* Button)
{
	if (Button == Button_CustomizeFromStandard)
	{
		InitCustomGameModesWidgetOptions(PresetSelection_PresetGameMode, PresetSelection_Difficulty);
		MenuButton_CustomGameModes->SetActive();
	}
	else if (Button == Button_PlayFromStandard)
	{
		ShowAudioFormatSelect(true);
	}
}

void UGameModesWidget::OnButtonClicked_CustomGameModeButton(const UBSButton* Button)
{
	if (Button == Button_SaveCustom)
	{
		SynchronizeStartWidgets();
		OnButtonClicked_SaveCustom();
	}
	else if (Button == CustomGameModesWidget_CreatorView->Widget_Preview->Button_Create)
	{
		SynchronizeStartWidgets();
		OnButtonClicked_SaveCustom();
	}
	else if (Button == Button_SaveCustomAndStart)
	{
		SynchronizeStartWidgets();
		OnButtonClicked_SaveCustomAndStart();
	}
	else if (Button == CustomGameModesWidget_CreatorView->Widget_Preview->Button_RefreshPreview)
	{
		RefreshGameModePreview();
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
	else if (Button == Button_ClearRLHistory)
	{
		OnButtonClicked_ClearRLHistory();
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
		const UMenuButton* MenuButton = Cast<UMenuButton>(Button);
		if (!MenuButton)
		{
			return;
		}

		UVerticalBox* Box = MenuButton->GetBox();
		if (!Box)
		{
			return;
		}

		// Execute OnCreatorViewVisibilityChanged if CreatorView is currently showing or will be showing
		if (Box == Box_DefaultGameModes)
		{
			RefreshGameModePreview();
		}
		else if (Box == Box_CustomGameModes)
		{
			RefreshGameModePreview();
		}

		MenuSwitcher->SetActiveWidget(Box);
	}
}

void UGameModesWidget::OnButtonClicked_ImportCustom()
{
	GameModeSharingWidget = CreateWidget<UGameModeSharingWidget>(this, GameModeSharingClass);
	GameModeSharingWidget->InitPopup(FText::FromString("Import Custom Game Mode"), FText::GetEmpty(), FText::FromString("Import"), FText::FromString("Cancel"));

	GameModeSharingWidget->OnButton1Pressed_NonDynamic.AddLambda([&]
	{
		const FString ImportString = GameModeSharingWidget->GetImportString();
		const FBSConfig ImportedConfig = ImportCustomGameMode(ImportString);

		// Can't import default game modes
		if (IsPresetGameMode(ImportedConfig.DefiningConfig.CustomGameModeName))
		{
			SetAndPlaySavedText(FText::FromString("Failed to import " + ImportedConfig.DefiningConfig.CustomGameModeName));
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
			SaveCustomAndReselect(FText::FromString("Successfully imported " + ImportedConfig.DefiningConfig.CustomGameModeName));
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
	const FStartWidgetProperties StartWidgetProperties = CustomGameModesWidget_Current->GetStartWidgetProperties();
	CustomGameModesWidget_Current->SetNewCustomGameModeName("");

	if (!IsCustomGameMode(StartWidgetProperties.DefiningConfig.CustomGameModeName))
	{
		SetAndPlaySavedText(FText::FromString("The selected mode is not a custom game mode."));
		return;
	}

	const FBSConfig SelectedConfig = GetCustomGameModeOptions();
	const FString ExportString = ExportCustomGameMode(SelectedConfig);
	FPlatformApplicationMisc::ClipboardCopy(*ExportString);

	SetAndPlaySavedText(FText::FromString("Export String copied to clipboard!"));
}

void UGameModesWidget::OnButtonClicked_ClearRLHistory()
{
	if (!IsCustomGameMode(BSConfig->DefiningConfig.CustomGameModeName))
	{
		return;
	}

	PopupMessageWidget = CreateWidget<UPopupMessageWidget>(this, PopupMessageClass);
	PopupMessageWidget->InitPopup(FText::FromString("Reset RL History"),
	                              FText::FromString("Are you sure you want reset the Reinforcement Learning history of "
		                              + BSConfig->DefiningConfig.CustomGameModeName + "? This will set the all QTable values to zero, "
		                              "deleting any learning that has taken place."),
	                              FText::FromString("Yes"), FText::FromString("No"));
	PopupMessageWidget->OnButton1Pressed_NonDynamic.AddLambda([this]
	{
		PopupMessageWidget->FadeOut();
		const int32 NumReset = ResetQTable(BSConfig->DefiningConfig);
		if (NumReset >= 1)
		{
			SetAndPlaySavedText(FText::FromString("Cleared RL History for " + BSConfig->DefiningConfig.CustomGameModeName));
		}
		UpdateSaveStartButtonStates();
	});
	PopupMessageWidget->OnButton2Pressed_NonDynamic.AddLambda([this]
	{
		PopupMessageWidget->FadeOut();
	});

	PopupMessageWidget->AddToViewport();
	PopupMessageWidget->FadeIn();
}

void UGameModesWidget::OnButtonClicked_SaveCustom()
{
	const FStartWidgetProperties StartWidgetProperties = CustomGameModesWidget_Current->GetStartWidgetProperties();
	const FString NewCustomGameModeName = StartWidgetProperties.NewCustomGameModeName;

	if (StartWidgetProperties.DefiningConfig.CustomGameModeName.Equals(NewCustomGameModeName) || StartWidgetProperties.NewCustomGameModeName.IsEmpty())
	{
		if (IsCurrentConfigIdenticalToSelectedCustom())
		{
			CustomGameModesWidget_PropertyView->SetNewCustomGameModeName("");
			CustomGameModesWidget_CreatorView->SetNewCustomGameModeName("");
			SetAndPlaySavedText(FText::FromString(StartWidgetProperties.DefiningConfig.CustomGameModeName + " already up to date"));
			return;
		}
	}

	if (CheckForExistingAndDisplayOverwriteMessage(false))
	{
		return;
	}

	SaveCustomAndReselect();
}

void UGameModesWidget::OnButtonClicked_SaveCustomAndStart()
{
	// Bypass saving if identical to existing
	if (IsCurrentConfigIdenticalToSelectedCustom())
	{
		CustomGameModesWidget_PropertyView->SetNewCustomGameModeName("");
		CustomGameModesWidget_CreatorView->SetNewCustomGameModeName("");
		ShowAudioFormatSelect(false);
		return;
	}

	if (CheckForExistingAndDisplayOverwriteMessage(true))
	{
		return;
	}

	if (SaveCustomAndReselect())
	{
		ShowAudioFormatSelect(false);
	}
}

void UGameModesWidget::OnButtonClicked_RemoveSelectedCustom()
{
	if (!IsCustomGameMode(BSConfig->DefiningConfig.CustomGameModeName))
	{
		return;
	}

	PopupMessageWidget = CreateWidget<UPopupMessageWidget>(this, PopupMessageClass);
	PopupMessageWidget->InitPopup(FText::FromString("Removal Confirmation"), FText::FromString("Are you sure you want remove " + BSConfig->DefiningConfig.CustomGameModeName + "?"),
	                              FText::FromString("Yes"), FText::FromString("No"));
	PopupMessageWidget->OnButton1Pressed_NonDynamic.AddLambda([this]
	{
		PopupMessageWidget->FadeOut();
		const FString RemovedGameModeName = BSConfig->DefiningConfig.CustomGameModeName;
		const int32 NumRemoved = RemoveCustomGameMode(FindCustomGameMode(RemovedGameModeName));
		if (NumRemoved >= 1)
		{
			SetAndPlaySavedText(FText::FromString(RemovedGameModeName + " removed"));
			CustomGameModesWidget_CreatorView->RefreshGameModeTemplateComboBoxOptions();
			CustomGameModesWidget_PropertyView->RefreshGameModeTemplateComboBoxOptions();
		}
		CustomGameModesWidget_CreatorView->SetNewCustomGameModeName("");
		CustomGameModesWidget_PropertyView->SetNewCustomGameModeName("");
		UpdateSaveStartButtonStates();
	});
	PopupMessageWidget->OnButton2Pressed_NonDynamic.AddLambda([this]
	{
		PopupMessageWidget->FadeOut();
	});

	PopupMessageWidget->AddToViewport();
	PopupMessageWidget->FadeIn();
}

void UGameModesWidget::OnButtonClicked_RemoveAllCustom()
{
	PopupMessageWidget = CreateWidget<UPopupMessageWidget>(this, PopupMessageClass);
	PopupMessageWidget->InitPopup(FText::FromString("Removal Confirmation"), FText::FromString("Are you sure you want remove all custom game modes?"), FText::FromString("Yes"),
	                              FText::FromString("No"));
	PopupMessageWidget->OnButton1Pressed_NonDynamic.AddLambda([this]
	{
		PopupMessageWidget->FadeOut();
		const int32 NumRemoved = RemoveAllCustomGameModes();
		if (NumRemoved >= 1)
		{
			SetAndPlaySavedText(FText::FromString(FString::FromInt(NumRemoved) + " game modes removed"));
			CustomGameModesWidget_CreatorView->RefreshGameModeTemplateComboBoxOptions();
			CustomGameModesWidget_PropertyView->RefreshGameModeTemplateComboBoxOptions();
		}

		CustomGameModesWidget_PropertyView->SetNewCustomGameModeName("");
		CustomGameModesWidget_CreatorView->SetNewCustomGameModeName("");
		UpdateSaveStartButtonStates();
	});
	PopupMessageWidget->OnButton2Pressed_NonDynamic.AddLambda([this]
	{
		PopupMessageWidget->FadeOut();
	});
	PopupMessageWidget->AddToViewport();
	PopupMessageWidget->FadeIn();
}

void UGameModesWidget::PopulateGameModeOptions(const FBSConfig& InBSConfig)
{
	GameModeConfig = InBSConfig;

	// Override Difficulty for Custom Modes to always be None
	if (IsCustomGameMode(GameModeConfig.DefiningConfig.CustomGameModeName))
	{
		GameModeConfig.DefiningConfig.Difficulty = EGameModeDifficulty::None;
		const FStartWidgetProperties StartWidgetUpdate = FStartWidgetProperties(GameModeConfig.DefiningConfig, true);
		CustomGameModesWidget_CreatorView->SetStartWidgetProperties(StartWidgetUpdate);
		CustomGameModesWidget_PropertyView->SetStartWidgetProperties(StartWidgetUpdate);
	}

	CustomGameModesWidget_CreatorView->UpdateOptionsFromConfig();
	CustomGameModesWidget_PropertyView->UpdateOptionsFromConfig();

	RefreshGameModePreview();
}

FBSConfig UGameModesWidget::GetCustomGameModeOptions() const
{
	FBSConfig ReturnStruct = GameModeConfig;

	if (!CustomGameModesWidget_Current->GetNewCustomGameModeName().IsEmpty())
	{
		ReturnStruct.DefiningConfig.CustomGameModeName = CustomGameModesWidget_Current->GetNewCustomGameModeName();
	}

	ReturnStruct.OnCreate_Custom();

	return ReturnStruct;
}

bool UGameModesWidget::SaveCustomAndReselect(const FText& SuccessMessage)
{
	const FBSConfig GameModeToSave = GetCustomGameModeOptions();

	// Last chance to fail
	if (IsPresetGameMode(GameModeToSave.DefiningConfig.CustomGameModeName) || GameModeToSave.DefiningConfig.GameModeType == EGameModeType::Preset)
	{
		CustomGameModesWidget_PropertyView->SetNewCustomGameModeName("");
		CustomGameModesWidget_CreatorView->SetNewCustomGameModeName("");
		const TArray SavedText = {FText::FromString("Error trying to save Custom Game Mode:"), FText::FromString(GameModeToSave.DefiningConfig.CustomGameModeName)};
		SetAndPlaySavedText(FText::Join(FText::FromString(" "), SavedText));
		return false;
	}

	SaveCustomGameMode(GameModeToSave);

	if (SuccessMessage.IsEmpty())
	{
		const TArray SavedText = {FText::FromString(GameModeToSave.DefiningConfig.CustomGameModeName), FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "GM_GameModeSavedText")};
		SetAndPlaySavedText(FText::Join(FText::FromString(" "), SavedText));
	}
	else
	{
		SetAndPlaySavedText(SuccessMessage);
	}

	CustomGameModesWidget_CreatorView->RefreshGameModeTemplateComboBoxOptions();
	CustomGameModesWidget_PropertyView->RefreshGameModeTemplateComboBoxOptions();

	PopulateGameModeOptions(GameModeToSave);

	return true;
}

void UGameModesWidget::UpdateSaveStartButtonStates()
{
	const FStartWidgetProperties StartWidgetProperties = CustomGameModesWidget_Current->GetStartWidgetProperties();
	const bool bAllCustomGameModeOptionsValid = CustomGameModesWidget_Current->GetAllNonStartChildWidgetOptionsValid();

	const bool bIsDefaultMode = StartWidgetProperties.DefiningConfig.BaseGameMode != EBaseGameMode::None;
	const bool bIsCustomMode = IsCustomGameMode(StartWidgetProperties.DefiningConfig.CustomGameModeName);

	const bool bNewCustomGameModeNameEmpty = StartWidgetProperties.NewCustomGameModeName.IsEmpty();
	const bool bInvalidCustomGameModeName = IsPresetGameMode(StartWidgetProperties.NewCustomGameModeName);

	// RemoveAll Button
	if (LoadCustomGameModes().IsEmpty())
	{
		Button_RemoveAllCustom->SetIsEnabled(false);
	}
	else
	{
		Button_RemoveAllCustom->SetIsEnabled(true);
	}

	// Remove Custom Button
	if (bIsCustomMode)
	{
		Button_RemoveSelectedCustom->SetIsEnabled(true);
	}
	else
	{
		Button_RemoveSelectedCustom->SetIsEnabled(false);
	}

    // Clear RL History Button
	if (bIsCustomMode && bAllCustomGameModeOptionsValid)
	{
		Button_ClearRLHistory->SetIsEnabled(true);
	}
	else
	{
		Button_ClearRLHistory->SetIsEnabled(false);
	}

	// Export Button
	if (bIsCustomMode && bNewCustomGameModeNameEmpty && bAllCustomGameModeOptionsValid)
	{
		Button_ExportCustom->SetIsEnabled(true);
	}
	else
	{
		Button_ExportCustom->SetIsEnabled(false);
	}

	if (!bAllCustomGameModeOptionsValid)
	{
		Button_SaveCustom->SetIsEnabled(false);
		Button_SaveCustomAndStart->SetIsEnabled(false);
		CustomGameModesWidget_CreatorView->Widget_Preview->Button_Create->SetIsEnabled(false);
		Button_StartWithoutSaving->SetIsEnabled(false);
		return;
	}

	// At this point, they can play the game mode but not save it
	Button_StartWithoutSaving->SetIsEnabled(true);

	// No name for default mode or invalid name
	if ((bIsDefaultMode && bNewCustomGameModeNameEmpty) || bInvalidCustomGameModeName)
	{
		Button_SaveCustom->SetIsEnabled(false);
		Button_SaveCustomAndStart->SetIsEnabled(false);
		CustomGameModesWidget_CreatorView->Widget_Preview->Button_Create->SetIsEnabled(false);
		return;
	}


	if (!bIsCustomMode && !bIsDefaultMode && bNewCustomGameModeNameEmpty)
	{
		Button_SaveCustom->SetIsEnabled(false);
		Button_SaveCustomAndStart->SetIsEnabled(false);
		CustomGameModesWidget_CreatorView->Widget_Preview->Button_Create->SetIsEnabled(false);
		return;
	}

	Button_SaveCustom->SetIsEnabled(true);
	Button_SaveCustomAndStart->SetIsEnabled(true);
	CustomGameModesWidget_CreatorView->Widget_Preview->Button_Create->SetIsEnabled(true);
}

void UGameModesWidget::ShowAudioFormatSelect(const bool bStartFromDefaultGameMode)
{
	AudioSelectWidget = CreateWidget<UAudioSelectWidget>(this, AudioSelectClass);

	AudioSelectWidget->OnStartButtonClickedDelegate.BindLambda([this, bStartFromDefaultGameMode](const FBS_AudioConfig& AudioConfig)
	{
		FGameModeTransitionState GameModeTransitionState;

		GameModeTransitionState.bSaveCurrentScores = false;
		GameModeTransitionState.TransitionState = bIsMainMenuChild ? ETransitionState::StartFromMainMenu : ETransitionState::StartFromPostGameMenu;
		GameModeTransitionState.BSConfig = bStartFromDefaultGameMode ? FindPresetGameMode(PresetSelection_PresetGameMode, PresetSelection_Difficulty) : GetCustomGameModeOptions();

		GameModeTransitionState.BSConfig.AudioConfig.SongTitle = AudioConfig.SongTitle;
		GameModeTransitionState.BSConfig.AudioConfig.SongLength = AudioConfig.SongLength;
		GameModeTransitionState.BSConfig.AudioConfig.InAudioDevice = AudioConfig.InAudioDevice;
		GameModeTransitionState.BSConfig.AudioConfig.OutAudioDevice = AudioConfig.OutAudioDevice;
		GameModeTransitionState.BSConfig.AudioConfig.SongPath = AudioConfig.SongPath;
		GameModeTransitionState.BSConfig.AudioConfig.bPlaybackAudio = AudioConfig.bPlaybackAudio;
		GameModeTransitionState.BSConfig.AudioConfig.AudioFormat = AudioConfig.AudioFormat;

		GameModeTransitionState.BSConfig.OnCreate();
		if (!bStartFromDefaultGameMode)
		{
			GameModeTransitionState.BSConfig.OnCreate_Custom();
		}

		OnGameModeStateChanged.Broadcast(GameModeTransitionState);
		AudioSelectWidget->FadeOut();
	});

	AudioSelectWidget->AddToViewport();
	AudioSelectWidget->FadeIn();
}

bool UGameModesWidget::IsCurrentConfigIdenticalToSelectedCustom()
{
	const FStartWidgetProperties StartWidgetProperties = CustomGameModesWidget_Current->GetStartWidgetProperties();

	// Bypass saving if identical to existing
	if (IsCustomGameMode(StartWidgetProperties.DefiningConfig.CustomGameModeName))
	{
		if (DoesCustomGameModeMatchConfig(StartWidgetProperties.DefiningConfig.CustomGameModeName, *BSConfig))
		{
			return true;
		}
	}

	return false;
}

bool UGameModesWidget::CheckForExistingAndDisplayOverwriteMessage(const bool bStartGameAfter)
{
	const FStartWidgetProperties StartWidgetProperties = CustomGameModesWidget_Current->GetStartWidgetProperties();
	const FString NewCustomGameModeName = StartWidgetProperties.NewCustomGameModeName;

	// If NewCustomGameModeName is blank, ask to override
	if (IsCustomGameMode(StartWidgetProperties.DefiningConfig.CustomGameModeName) && NewCustomGameModeName.IsEmpty())
	{
		ShowConfirmOverwriteMessage(bStartGameAfter);
		return true;
	}

	// If NewCustomGameModeName already exists as a saved custom game mode, ask to override
	if (IsCustomGameMode(NewCustomGameModeName))
	{
		ShowConfirmOverwriteMessage(bStartGameAfter);
		return true;
	}

	return false;
}

void UGameModesWidget::ShowConfirmOverwriteMessage(const bool bStartGameAfter)
{
	PopupMessageWidget = CreateWidget<UPopupMessageWidget>(this, PopupMessageClass);
	PopupMessageWidget->InitPopup(FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "GM_OverwritePopupTitle"), FText::GetEmpty(),
	                              FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "GM_OverwriteConfirm"),
	                              FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "GM_OverwriteCancel"));

	PopupMessageWidget->OnButton1Pressed_NonDynamic.AddLambda([this, &bStartGameAfter]
	{
		PopupMessageWidget->FadeOut();
		if (SaveCustomAndReselect() && bStartGameAfter)
		{
			ShowAudioFormatSelect(false);
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
	PopupMessageWidget->OnButton1Pressed_NonDynamic.AddLambda([this, &ImportedConfig]
	{
		PopupMessageWidget->FadeOut();
		SaveCustomAndReselect(FText::FromString("Successfully imported " + ImportedConfig.DefiningConfig.CustomGameModeName));
	});
	PopupMessageWidget->OnButton2Pressed_NonDynamic.AddLambda([this]
	{
		PopupMessageWidget->FadeOut();
	});

	PopupMessageWidget->AddToViewport();
	PopupMessageWidget->FadeIn();
}

void UGameModesWidget::TransitionGameModeViewToCreator()
{
	if (IsAnimationPlaying(TransitionCustomGameModeView) || GetCreatorViewVisible())
	{
		return;
	}
	SynchronizeStartWidgets();
	BindToAnimationFinished(TransitionCustomGameModeView, OnTransitionComplete_ToCreatorView);
	Box_CreatorView->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	PlayAnimationReverse(TransitionCustomGameModeView);
}

void UGameModesWidget::TransitionGameModeViewToProperty()
{
	if (IsAnimationPlaying(TransitionCustomGameModeView) || !GetCreatorViewVisible())
	{
		return;
	}
	SynchronizeStartWidgets();
	BindToAnimationFinished(TransitionCustomGameModeView, OnTransitionComplete_ToPropertyView);
	Box_PropertyView->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	PlayAnimationForward(TransitionCustomGameModeView);
}

void UGameModesWidget::OnTransitionCompleted_ToPropertyView()
{
	Box_CreatorView->SetVisibility(ESlateVisibility::Collapsed);
	UnbindFromAnimationFinished(TransitionCustomGameModeView, OnTransitionComplete_ToPropertyView);
	CustomGameModesWidget_Current = CustomGameModesWidget_PropertyView;
	StopGameModePreview();
}

void UGameModesWidget::OnTransitionCompleted_ToCreatorView()
{
	Box_PropertyView->SetVisibility(ESlateVisibility::Collapsed);
	UnbindFromAnimationFinished(TransitionCustomGameModeView, OnTransitionComplete_ToCreatorView);
	CustomGameModesWidget_Current = CustomGameModesWidget_CreatorView;
	RefreshGameModePreview();
}

void UGameModesWidget::OnRequestGameModeTemplateUpdate(const FString& InGameMode, const EGameModeDifficulty& Difficulty)
{
	if (IsPresetGameMode(InGameMode))
	{
		if (Difficulty == EGameModeDifficulty::None)
		{
			PopulateGameModeOptions(FindPresetGameMode(InGameMode, EGameModeDifficulty::Normal));
		}
		else
		{
			PopulateGameModeOptions(FindPresetGameMode(InGameMode, Difficulty));
		}
	}
	else if (IsCustomGameMode(InGameMode))
	{
		PopulateGameModeOptions(FindCustomGameMode(InGameMode));
	}
	UpdateSaveStartButtonStates();
}

void UGameModesWidget::SynchronizeStartWidgets()
{
	UCustomGameModesWidgetBase* From;
	UCustomGameModesWidgetBase* To;
	if (CustomGameModesWidget_Current == CustomGameModesWidget_CreatorView)
	{
		From = Cast<UCustomGameModesWidgetBase>(CustomGameModesWidget_CreatorView.Get());
		To = Cast<UCustomGameModesWidgetBase>(CustomGameModesWidget_PropertyView.Get());
	}
	else
	{
		From = Cast<UCustomGameModesWidgetBase>(CustomGameModesWidget_PropertyView.Get());
		To = Cast<UCustomGameModesWidgetBase>(CustomGameModesWidget_CreatorView.Get());
	}

	const FStartWidgetProperties FromProperties = From->GetStartWidgetProperties();
	const FStartWidgetProperties ToProperties = To->GetStartWidgetProperties();

	if (FromProperties == ToProperties)
	{
		To->UpdateOptionsFromConfig();
		return;
	}

	To->SetStartWidgetProperties(FromProperties);
	To->UpdateOptionsFromConfig();
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

void UGameModesWidget::SetAndPlaySavedText(const FText& InText)
{
	if (CustomGameModesWidget_Current == CustomGameModesWidget_PropertyView)
	{
		SavedTextWidget_PropertyView->SetSavedText(InText);
		SavedTextWidget_PropertyView->PlayFadeInFadeOut();
	}
	else
	{
		CustomGameModesWidget_CreatorView->SavedTextWidget_CreatorView->SetSavedText(InText);
		CustomGameModesWidget_CreatorView->SavedTextWidget_CreatorView->PlayFadeInFadeOut();
	}
}

void UGameModesWidget::OnGameModeBreakingOptionPresentStateChanged(const bool bIsPresent)
{
	if (bIsPresent == bGameModeBreakingOptionPresent)
	{
		return;
	}

	CustomGameModesWidget_CreatorView->Widget_Preview->Button_RefreshPreview->SetIsEnabled(!bIsPresent);
	const FString From = bGameModeBreakingOptionPresent ? "True" : "False";
	const FString To = bIsPresent ? "True" : "False";
	UE_LOG(LogTemp, Display, TEXT("OnGameModeBreakingOption GameModesWidget: %s -> %s"), *From, *To);
	bGameModeBreakingOptionPresent = bIsPresent;
	OnGameModeBreakingChange.Broadcast(bGameModeBreakingOptionPresent);
}

void UGameModesWidget::RefreshGameModePreview()
{
	if (GetCreatorViewVisible() && RequestSimulateTargetManagerStateChange.IsBound())
	{
		RequestSimulateTargetManagerStateChange.Broadcast(true);
	}
}

void UGameModesWidget::StopGameModePreview()
{
	if (RequestSimulateTargetManagerStateChange.IsBound())
	{
		RequestSimulateTargetManagerStateChange.Broadcast(false);
	}
}
