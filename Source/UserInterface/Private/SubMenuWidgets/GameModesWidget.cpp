// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.
// ReSharper disable CppMemberFunctionMayBeConst


#include "SubMenuWidgets/GameModesWidget.h"
#include "GlobalConstants.h"
#include "Blueprint/WidgetTree.h"
#include "Components/WidgetSwitcher.h"
#include "Components/VerticalBox.h"
#include "Components/Border.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "OverlayWidgets/PopupMessageWidget.h"
#include "OverlayWidgets/AudioSelectWidget.h"
#include "OverlayWidgets/GameModeSharingWidget.h"
#include "WidgetComponents/DoubleSyncedSliderAndTextBox.h"
#include "WidgetComponents/SavedTextWidget.h"
#include "SubMenuWidgets/GameModesWidget_DefiningConfig.h"
#include "SubMenuWidgets/CustomGameModesWidget/CustomGameModesWidget_CreatorView.h"
#include "SubMenuWidgets/CustomGameModesWidget/CustomGameModesWidget_PropertyView.h"
#include "WidgetComponents/Buttons/MenuButton.h"
#include "Windows/WindowsPlatformApplicationMisc.h"
#include "BSWidgetInterface.h"

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
	CustomGameModesWidget_CreatorView->Button_Create->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_CustomGameModeButton);
	CustomGameModesWidget_CreatorView->RequestGameModeTemplateUpdate.AddUObject(this, &ThisClass::OnRequestGameModeTemplateUpdate);
	CustomGameModesWidget_PropertyView->RequestGameModeTemplateUpdate.AddUObject(this, &ThisClass::OnRequestGameModeTemplateUpdate);
	
	// TODO: Might need to also bind to CreatorView
	CustomGameModesWidget_PropertyView->RequestButtonStateUpdate.AddUObject(this, &ThisClass::UpdateSaveStartButtonStates);
	
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
		OnButtonClicked_SaveCustom();
	}
	else if (Button == CustomGameModesWidget_CreatorView->Button_Create)
	{
		SynchronizeStartWidgets(CustomGameModesWidget_CreatorView, CustomGameModesWidget_PropertyView);
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
		if (Box == Box_DefaultGameModes && GetCreatorViewVisible())
		{
			if (OnCreatorViewVisibilityChanged.IsBound())
			{
				OnCreatorViewVisibilityChanged.Broadcast(false);
			}
		}
		else if (Box == Box_CustomGameModes && GetCreatorViewVisible())
		{
			if (OnCreatorViewVisibilityChanged.IsBound())
			{
				OnCreatorViewVisibilityChanged.Broadcast(true);
			}
		}
		
		MenuSwitcher->SetActiveWidget(Box);
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
	const FString SelectedCustomGameMode = CustomGameModesWidget_PropertyView->GetNewCustomGameModeName();
	
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
	if (CheckForExistingAndDisplayOverwriteMessage(false))
	{
		return;
	}
	SaveCustomAndReselect();
}

void UGameModesWidget::OnButtonClicked_SaveCustomAndStart()
{
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
	PopupMessageWidget->InitPopup(FText::FromString("Removal Confirmation"),
		FText::FromString("Are you sure you want remove " + BSConfig->DefiningConfig.CustomGameModeName + "?"),
							  FText::FromString("Yes"),
							  FText::FromString("No"));
	PopupMessageWidget->OnButton1Pressed_NonDynamic.AddLambda([&]
	{
		PopupMessageWidget->FadeOut();
		const FString RemovedGameModeName = BSConfig->DefiningConfig.CustomGameModeName;
		const int32 NumRemoved = RemoveCustomGameMode(FindCustomGameMode(RemovedGameModeName));
		if (NumRemoved >= 1)
		{
			SavedTextWidget->SetSavedText(FText::FromString(RemovedGameModeName + " removed"));
			SavedTextWidget->PlayFadeInFadeOut();

			CustomGameModesWidget_CreatorView->RefreshGameModeTemplateComboBoxOptions();
			CustomGameModesWidget_PropertyView->RefreshGameModeTemplateComboBoxOptions();
		}
		CustomGameModesWidget_CreatorView->SetNewCustomGameModeName("");
		CustomGameModesWidget_PropertyView->SetNewCustomGameModeName("");
		UpdateSaveStartButtonStates();
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
	PopupMessageWidget->InitPopup(FText::FromString("Removal Confirmation"),
		FText::FromString("Are you sure you want remove all custom game modes?"),
							  FText::FromString("Yes"),
							  FText::FromString("No"));
	PopupMessageWidget->OnButton1Pressed_NonDynamic.AddLambda([&]
	{
		RemoveAllCustomGameModes();
		PopupMessageWidget->FadeOut();
		CustomGameModesWidget_PropertyView->SetNewCustomGameModeName("");
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

void UGameModesWidget::PopulateGameModeOptions(const FBSConfig& InBSConfig)
{
	GameModeConfig = InBSConfig;
	//BSConfig = &GameModeConfig;

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
	
	if (GetCreatorViewVisible() && OnPopulateGameModeOptions.IsBound())
	{
		OnPopulateGameModeOptions.Broadcast();
	}
}

FBSConfig UGameModesWidget::GetCustomGameModeOptions() const
{
	FBSConfig ReturnStruct = GameModeConfig;
	
	if (!CustomGameModesWidget_PropertyView->GetNewCustomGameModeName().IsEmpty())
	{
		ReturnStruct.DefiningConfig.CustomGameModeName = CustomGameModesWidget_PropertyView->GetNewCustomGameModeName();
	}
	
	// Override GameModeType to always be Custom
	ReturnStruct.DefiningConfig.GameModeType = EGameModeType::Custom;
	// Override Difficulty to always be None
	ReturnStruct.DefiningConfig.Difficulty = EGameModeDifficulty::None;
	// Set PlayerDelay to same value as SpawnBeatDelay
	ReturnStruct.AudioConfig.PlayerDelay = ReturnStruct.TargetConfig.SpawnBeatDelay;
	
	return ReturnStruct;
}

bool UGameModesWidget::SaveCustomAndReselect(const FText& SuccessMessage)
{
	const FBSConfig GameModeToSave = GetCustomGameModeOptions();
	
	// Last chance to fail
	if (IsPresetGameMode(GameModeToSave.DefiningConfig.CustomGameModeName) || GameModeToSave.DefiningConfig.GameModeType == EGameModeType::Preset)
	{
		CustomGameModesWidget_PropertyView->SetNewCustomGameModeName("");
		const TArray SavedText = {FText::FromString("Error trying to save Custom Game Mode:"), FText::FromString(GameModeToSave.DefiningConfig.CustomGameModeName)};
		SavedTextWidget->SetSavedText(FText::Join(FText::FromString(" "), SavedText));
		SavedTextWidget->PlayFadeInFadeOut();
		return false;
	}
	
	SaveCustomGameMode(GameModeToSave);

	if (SuccessMessage.IsEmpty())
	{
		const TArray SavedText = {FText::FromString(GameModeToSave.DefiningConfig.CustomGameModeName), FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "GM_GameModeSavedText")};
		SavedTextWidget->SetSavedText(FText::Join(FText::FromString(" "), SavedText));
	}
	else
	{
		SavedTextWidget->SetSavedText(SuccessMessage);
	}
	
	SavedTextWidget->PlayFadeInFadeOut();

	CustomGameModesWidget_CreatorView->RefreshGameModeTemplateComboBoxOptions();
	CustomGameModesWidget_PropertyView->RefreshGameModeTemplateComboBoxOptions();

	PopulateGameModeOptions(GameModeToSave);

	return true;
}

void UGameModesWidget::UpdateSaveStartButtonStates()
{
	const FStartWidgetProperties StartWidgetProperties = CustomGameModesWidget_PropertyView->GetStartWidgetProperties();
	
	const bool bIsDefaultMode = StartWidgetProperties.DefiningConfig.BaseGameMode != EBaseGameMode::None;
	const bool bIsCustomMode = IsCustomGameMode(StartWidgetProperties.DefiningConfig.CustomGameModeName);

	const bool bNewCustomGameModeNameEmpty = StartWidgetProperties.NewCustomGameModeName.IsEmpty();
	const bool bInvalidCustomGameModeName = IsPresetGameMode(StartWidgetProperties.NewCustomGameModeName);
	const bool bAllCustomGameModeOptionsValid = CustomGameModesWidget_PropertyView->GetAllChildWidgetOptionsValid();
	
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
		CustomGameModesWidget_CreatorView->Button_Create->SetIsEnabled(false);
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
		CustomGameModesWidget_CreatorView->Button_Create->SetIsEnabled(false);
		return;
	}

	
	if (!bIsCustomMode && !bIsDefaultMode && bNewCustomGameModeNameEmpty)
	{
		Button_SaveCustom->SetIsEnabled(false);
		Button_SaveCustomAndStart->SetIsEnabled(false);
		CustomGameModesWidget_CreatorView->Button_Create->SetIsEnabled(false);
		return;
	}
	
	Button_SaveCustom->SetIsEnabled(true);
	Button_SaveCustomAndStart->SetIsEnabled(true);
	CustomGameModesWidget_CreatorView->Button_Create->SetIsEnabled(true);
}

void UGameModesWidget::ShowAudioFormatSelect(const bool bStartFromDefaultGameMode)
{
	AudioSelectWidget = CreateWidget<UAudioSelectWidget>(this, AudioSelectClass);
	
	AudioSelectWidget->OnStartButtonClickedDelegate.BindLambda([this, bStartFromDefaultGameMode] (const FBS_AudioConfig& AudioConfig)
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

		// Override the player delay to zero if using Capture
		if (GameModeTransitionState.BSConfig.AudioConfig.AudioFormat == EAudioFormat::Capture ||
			GameModeTransitionState.BSConfig.AudioConfig.AudioFormat == EAudioFormat::Loopback)
		{
			GameModeTransitionState.BSConfig.AudioConfig.PlayerDelay = 0.f;
			GameModeTransitionState.BSConfig.TargetConfig.SpawnBeatDelay = 0.f;
		}
		
		OnGameModeStateChanged.Broadcast(GameModeTransitionState);
		AudioSelectWidget->FadeOut();
	});
	
	AudioSelectWidget->AddToViewport();
	AudioSelectWidget->FadeIn();
}

bool UGameModesWidget::CheckForExistingAndDisplayOverwriteMessage(const bool bStartGameAfter)
{
	const FStartWidgetProperties StartWidgetUpdate = CustomGameModesWidget_PropertyView->GetStartWidgetProperties();
	const FString NewCustomGameModeName = StartWidgetUpdate.NewCustomGameModeName;

	// If NewCustomGameModeName is blank, ask to override
	if (IsCustomGameMode(StartWidgetUpdate.DefiningConfig.CustomGameModeName) && NewCustomGameModeName.IsEmpty())
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

	PopupMessageWidget->OnButton1Pressed_NonDynamic.AddLambda([this, bStartGameAfter]
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
	PopupMessageWidget->OnButton1Pressed_NonDynamic.AddLambda([this, ImportedConfig]
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
	SynchronizeStartWidgets(CustomGameModesWidget_PropertyView, CustomGameModesWidget_CreatorView);
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
	SynchronizeStartWidgets(CustomGameModesWidget_CreatorView, CustomGameModesWidget_PropertyView);
	BindToAnimationFinished(TransitionCustomGameModeView, OnTransitionComplete_ToPropertyView);
	Box_PropertyView->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	PlayAnimationForward(TransitionCustomGameModeView);
}

void UGameModesWidget::OnTransitionCompleted_ToPropertyView()
{
	Box_CreatorView->SetVisibility(ESlateVisibility::Collapsed);
	UnbindFromAnimationFinished(TransitionCustomGameModeView, OnTransitionComplete_ToPropertyView);
	CustomGameModesWidget_Current = CustomGameModesWidget_PropertyView;
	if (OnCreatorViewVisibilityChanged.IsBound())
	{
		OnCreatorViewVisibilityChanged.Broadcast(false);
	}
}

void UGameModesWidget::OnTransitionCompleted_ToCreatorView()
{
	Box_PropertyView->SetVisibility(ESlateVisibility::Collapsed);
	UnbindFromAnimationFinished(TransitionCustomGameModeView, OnTransitionComplete_ToCreatorView);
	CustomGameModesWidget_Current = CustomGameModesWidget_CreatorView;
	if (OnCreatorViewVisibilityChanged.IsBound())
	{
		OnCreatorViewVisibilityChanged.Broadcast(true);
	}
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

void UGameModesWidget::SynchronizeStartWidgets(const TObjectPtr<UCustomGameModesWidgetBase> From, const TObjectPtr<UCustomGameModesWidgetBase> To)
{
	const FStartWidgetProperties FromProperties = From->GetStartWidgetProperties();
	const FStartWidgetProperties ToProperties = To->GetStartWidgetProperties();
	
	const FBSConfig* FromConfig = From->GetBSConfig();
	const FBSConfig* ToConfig = To->GetBSConfig();

	const FString FromString = ExportCustomGameMode(*FromConfig);
	const FString ToString = ExportCustomGameMode(*ToConfig);
	const FString ThisString = ExportCustomGameMode(GameModeConfig);

	if (FromString != ToString)
	{
		UE_LOG(LogTemp, Display, TEXT("FromConfig != ToConfig"));
	}

	if (FromString != ThisString)
	{
		UE_LOG(LogTemp, Display, TEXT("FromConfig != GameModeConfig"));
	}
	
	if (FromProperties == ToProperties)
	{
		UE_LOG(LogTemp, Display, TEXT("FromProperties == ToProperties"));
		To->UpdateOptionsFromConfig();
		return;
	}

	UE_LOG(LogTemp, Display, TEXT("FromProperties != ToProperties"));

	const FString FromUseTemplate = FromProperties.bUseTemplateChecked ? "True" : "False";
	const FString ToUseTemplate = ToProperties.bUseTemplateChecked ? "True" : "False";
	
	UE_LOG(LogTemp, Display, TEXT("bUseTemplateChecked: From: %s To: %s"), *FromUseTemplate, *ToUseTemplate);
	UE_LOG(LogTemp, Display, TEXT("NewCustomGameModeName: From: %s To: %s"), *FromProperties.NewCustomGameModeName, *ToProperties.NewCustomGameModeName);
	UE_LOG(LogTemp, Display, TEXT("CustomGameModeName: From: %s To: %s"), *FromProperties.DefiningConfig.CustomGameModeName, *ToProperties.DefiningConfig.CustomGameModeName);
	UE_LOG(LogTemp, Display, TEXT("GameModeType: From: %s To: %s"), *IBSWidgetInterface::GetStringFromEnum(FromProperties.DefiningConfig.GameModeType),
		*IBSWidgetInterface::GetStringFromEnum(ToProperties.DefiningConfig.GameModeType));
	UE_LOG(LogTemp, Display, TEXT("BaseGameMode: From: %s To: %s"), *IBSWidgetInterface::GetStringFromEnum(FromProperties.DefiningConfig.BaseGameMode),
		*IBSWidgetInterface::GetStringFromEnum(ToProperties.DefiningConfig.BaseGameMode));
	UE_LOG(LogTemp, Display, TEXT("Difficulty: From: %s To: %s"), *IBSWidgetInterface::GetStringFromEnum(FromProperties.DefiningConfig.Difficulty),
		*IBSWidgetInterface::GetStringFromEnum(ToProperties.DefiningConfig.Difficulty));
	
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
