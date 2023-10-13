// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.
// ReSharper disable CppMemberFunctionMayBeConst


#include "SubMenuWidgets/GameModesWidgets/GameModesWidget.h"
#include "GlobalConstants.h"
#include "Blueprint/WidgetTree.h"
#include "Components/WidgetSwitcher.h"
#include "Components/VerticalBox.h"
#include "Components/Border.h"
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
#include "WidgetComponents/MenuOptionWidgets/DefaultGameModeOptionWidget.h"

using namespace Constants;

void UGameModesWidget::NativeConstruct()
{
	Super::NativeConstruct();

	TArray<UDefaultGameModeOptionWidget*> Temp;
	Box_DefaultGameModesOptions->ClearChildren();
	DefaultGameModesParams.ValueSort([&] (const FDefaultGameModeParams& Params, const FDefaultGameModeParams& Params2)
	{
		return Params < Params2;
	});
	for (TPair<EBaseGameMode, FDefaultGameModeParams>& Pair : DefaultGameModesParams)
	{
		UDefaultGameModeOptionWidget* Widget = CreateWidget<UDefaultGameModeOptionWidget>(this, DefaultGameModesWidgetClass);
		Widget->SetBaseGameMode(Pair.Key);
		Widget->SetDescriptionText(Pair.Value.GameModeName);
		Widget->SetAltDescriptionText(Pair.Value.AltDescriptionText);
		Widget->SetShowTooltipImage(false);
		Temp.Add(Widget);
		Box_DefaultGameModesOptions->AddChildToVerticalBox(Widget);
	}

	for (int i = 0; i < Temp.Num(); i++)
	{
		if (!Temp.IsValidIndex(i)) continue;
		const UDefaultGameModeOptionWidget* Widget = Temp[i];
		const int NextIndex = i == Temp.Num() - 1 ? 0 : i + 1;
		Widget->Button->SetDefaults(static_cast<uint8>(Widget->GetBaseGameMode()), Temp[NextIndex]->Button);
		Widget->Button->OnBSButtonPressed.AddUniqueDynamic(this, &UGameModesWidget::OnButtonClicked_SelectedDefaultGameMode);
	}
	Box_DefaultGameModesOptions->UpdateBrushColors();

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

void UGameModesWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	
	Box_DefaultGameModesOptions->ClearChildren();
	DefaultGameModesParams.ValueSort([&] (const FDefaultGameModeParams& Params, const FDefaultGameModeParams& Params2)
	{
		return Params < Params2;
	});
	for (TPair<EBaseGameMode, FDefaultGameModeParams>& Pair : DefaultGameModesParams)
	{
		if (!DefaultGameModesWidgetClass) return;
		UDefaultGameModeOptionWidget* Widget = CreateWidget<UDefaultGameModeOptionWidget>(this, DefaultGameModesWidgetClass);
		Widget->SetBaseGameMode(Pair.Key);
		Widget->SetDescriptionText(Pair.Value.GameModeName);
		Widget->SetAltDescriptionText(Pair.Value.AltDescriptionText);
		Widget->SetShowTooltipImage(false);
		Box_DefaultGameModesOptions->AddChildToVerticalBox(Widget);
	}
	Box_DefaultGameModesOptions->UpdateBrushColors();
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
	Button_StartFromPreset->SetIsEnabled(false);
	Button_CustomizeFromPreset->SetIsEnabled(false);

	Button_SaveCustom->SetIsEnabled(false);
	Button_StartFromCustom->SetIsEnabled(false);
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

	// Menu Buttons
	MenuButton_DefaultGameModes->SetDefaults(Box_DefaultGameModes, MenuButton_CustomGameModes);
	MenuButton_CustomGameModes->SetDefaults(Box_CustomGameModes, MenuButton_DefaultGameModes);
	MenuButton_PropertyView->SetDefaults(nullptr, MenuButton_CreatorView);
	MenuButton_CreatorView->SetDefaults(nullptr, MenuButton_PropertyView);
}

void UGameModesWidget::InitCustomGameModesWidgetOptions(const EBaseGameMode& BaseGameMode,
	const EGameModeDifficulty& Difficulty)
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
	Button_StartFromCustom->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_CustomGameModeButton);
	Button_RemoveAllCustom->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_CustomGameModeButton);
	Button_RemoveSelectedCustom->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_CustomGameModeButton);
	Button_ImportCustom->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_CustomGameModeButton);
	Button_ExportCustom->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_CustomGameModeButton);
	Button_ClearRLHistory->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_CustomGameModeButton);

	// Default Game Modes Buttons
	Button_CustomizeFromPreset->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_DefaultGameMode);
	Button_StartFromPreset->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_DefaultGameMode);

	// Difficulty Buttons
	Button_NormalDifficulty->OnBSButtonPressed.AddDynamic(this, &UGameModesWidget::OnButtonClicked_SelectedDifficulty);
	Button_HardDifficulty->OnBSButtonPressed.AddDynamic(this, &UGameModesWidget::OnButtonClicked_SelectedDifficulty);
	Button_DeathDifficulty->OnBSButtonPressed.AddDynamic(this, &UGameModesWidget::OnButtonClicked_SelectedDifficulty);

	// Custom Game Modes Widgets
	CustomGameModesWidget_CreatorView->Widget_Preview->Button_Create->OnBSButtonPressed.AddDynamic(this,
		&ThisClass::OnButtonClicked_CustomGameModeButton);
	CustomGameModesWidget_CreatorView->Widget_Preview->Button_RefreshPreview->OnBSButtonPressed.AddDynamic(this,
		&ThisClass::OnButtonClicked_CustomGameModeButton);
	CustomGameModesWidget_CreatorView->RequestGameModeTemplateUpdate.AddUObject(this,
		&ThisClass::OnRequestGameModeTemplateUpdate);
	CustomGameModesWidget_PropertyView->RequestGameModeTemplateUpdate.AddUObject(this,
		&ThisClass::OnRequestGameModeTemplateUpdate);

	CustomGameModesWidget_CreatorView->RequestGameModePreviewUpdate.
	                                   AddUObject(this, &ThisClass::RefreshGameModePreview);

	CustomGameModesWidget_PropertyView->RequestButtonStateUpdate.AddUObject(this,
		&ThisClass::UpdateSaveStartButtonStates);
	CustomGameModesWidget_CreatorView->RequestButtonStateUpdate.AddUObject(this,
		&ThisClass::UpdateSaveStartButtonStates);
	CustomGameModesWidget_PropertyView->OnGameModeBreakingChange.AddUObject(this,
		&ThisClass::OnGameModeBreakingOptionPresentStateChanged);
	CustomGameModesWidget_CreatorView->OnGameModeBreakingChange.AddUObject(this,
		&ThisClass::OnGameModeBreakingOptionPresentStateChanged);

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
	Button_StartFromPreset->SetIsEnabled(false);
	Button_CustomizeFromPreset->SetIsEnabled(true);
}

void UGameModesWidget::OnButtonClicked_SelectedDifficulty(const UBSButton* Button)
{
	PresetSelection_Difficulty = static_cast<EGameModeDifficulty>(Button->GetEnumValue());
	Button_StartFromPreset->SetIsEnabled(true);
}

void UGameModesWidget::OnButtonClicked_DefaultGameMode(const UBSButton* Button)
{
	if (Button == Button_CustomizeFromPreset)
	{
		InitCustomGameModesWidgetOptions(PresetSelection_PresetGameMode, PresetSelection_Difficulty);
		MenuButton_CustomGameModes->SetActive();
	}
	else if (Button == Button_StartFromPreset)
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
	else if (Button == Button_StartFromCustom)
	{
		SynchronizeStartWidgets();
		OnButtonClicked_StartFromCustom();
	}
	else if (Button == CustomGameModesWidget_CreatorView->Widget_Preview->Button_RefreshPreview)
	{
		RefreshGameModePreview();
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
	TArray<UBSButton*> Buttons = GameModeSharingWidget->InitPopup(FText::FromString("Import Custom Game Mode"),
		FText::GetEmpty(), 2);

	Buttons[0]->SetButtonText(FText::FromString("Cancel"));
	Buttons[0]->OnBSButtonButtonPressed_NonDynamic.AddLambda([this]
	{
		GameModeSharingWidget->FadeOut();
	});

	Buttons[1]->SetButtonText(FText::FromString("Import"));
	GameModeSharingWidget->SetImportButton(Buttons[1]);
	Buttons[1]->OnBSButtonButtonPressed_NonDynamic.AddLambda([this]
	{
		const FString ImportString = GameModeSharingWidget->GetImportString();
		GameModeSharingWidget->FadeOut();
		const FBSConfig ImportedConfig = ImportCustomGameMode(ImportString);

		// Can't import default game modes
		if (IsPresetGameMode(ImportedConfig.DefiningConfig.CustomGameModeName))
		{
			SetAndPlaySavedText(
				FText::FromString("Failed to import " + ImportedConfig.DefiningConfig.CustomGameModeName));
			return;
		}
		if (ImportedConfig.DefiningConfig.CustomGameModeName.IsEmpty())
		{
			SetAndPlaySavedText(FText::FromString("Failed to import game mode with empty CustomGameModeName"));
			return;
		}

		// Check if overriding existing Custom Game Mode
		if (IsCustomGameMode(ImportedConfig.DefiningConfig.CustomGameModeName))
		{
			ShowConfirmOverwriteMessage_Import(ImportedConfig);
		}
		else
		{
			SaveCustomAndReselect(
				FText::FromString("Successfully imported " + ImportedConfig.DefiningConfig.CustomGameModeName));
		}
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
		SetAndPlaySavedText(FText::FromString("The selected mode is not a custom game mode"));
		return;
	}

	const FBSConfig SelectedConfig = GetCustomGameModeOptions();
	const FString ExportString = ExportCustomGameMode(SelectedConfig);
	FPlatformApplicationMisc::ClipboardCopy(*ExportString);

	SetAndPlaySavedText(FText::FromString("Export String copied to clipboard"));
}

void UGameModesWidget::OnButtonClicked_ClearRLHistory()
{
	if (!IsCustomGameMode(BSConfig->DefiningConfig.CustomGameModeName))
	{
		return;
	}

	PopupMessageWidget = CreateWidget<UPopupMessageWidget>(this, PopupMessageClass);
	TArray<UBSButton*> Buttons = PopupMessageWidget->InitPopup(FText::FromString("Reset RL History"),
		FText::FromString(
			"Are you sure you want reset the Reinforcement Learning history of " + BSConfig->DefiningConfig.
			CustomGameModeName + "? This will set the all QTable values to zero, "
			"deleting any learning that has taken place."), 2);

	Buttons[0]->SetButtonText(FText::FromString("No"));
	Buttons[0]->OnBSButtonButtonPressed_NonDynamic.AddLambda([this]
	{
		PopupMessageWidget->FadeOut();
	});

	Buttons[1]->SetButtonText(FText::FromString("Yes"));
	Buttons[1]->OnBSButtonButtonPressed_NonDynamic.AddLambda([this]
	{
		PopupMessageWidget->FadeOut();
		const int32 NumReset = ResetQTable(BSConfig->DefiningConfig);
		if (NumReset >= 1)
		{
			SetAndPlaySavedText(
				FText::FromString("Cleared RL History for " + BSConfig->DefiningConfig.CustomGameModeName));
		}
		UpdateSaveStartButtonStates();
	});

	PopupMessageWidget->AddToViewport();
	PopupMessageWidget->FadeIn();
}

void UGameModesWidget::OnButtonClicked_SaveCustom()
{
	const FStartWidgetProperties StartWidgetProperties = CustomGameModesWidget_Current->GetStartWidgetProperties();
	const FString NewCustomGameModeName = StartWidgetProperties.NewCustomGameModeName;

	if ((StartWidgetProperties.DefiningConfig.CustomGameModeName.Equals(NewCustomGameModeName) || StartWidgetProperties.
		NewCustomGameModeName.IsEmpty()) && IsCurrentConfigIdenticalToSelectedCustom())
	{
		CustomGameModesWidget_PropertyView->SetNewCustomGameModeName("");
		CustomGameModesWidget_CreatorView->SetNewCustomGameModeName("");
		SetAndPlaySavedText(
			FText::FromString(StartWidgetProperties.DefiningConfig.CustomGameModeName + " already up to date"));
	}
	// Ask to override
	else if (DoesCustomGameModeExist())
	{
		PopupMessageWidget = CreateWidget<UPopupMessageWidget>(this, PopupMessageClass);
		TArray<UBSButton*> Buttons = PopupMessageWidget->InitPopup(GetWidgetTextFromKey("GM_OverwritePopupTitle"),
			FText::GetEmpty(), 2);

		Buttons[0]->SetButtonText(GetWidgetTextFromKey("GM_OverwriteCancel"));
		Buttons[0]->OnBSButtonButtonPressed_NonDynamic.AddLambda([this]
		{
			PopupMessageWidget->FadeOut();
		});
		Buttons[1]->SetButtonText(GetWidgetTextFromKey("GM_OverwriteConfirm"));
		Buttons[1]->OnBSButtonButtonPressed_NonDynamic.AddLambda([this]
		{
			PopupMessageWidget->FadeOut();
			SaveCustomAndReselect();
		});
		PopupMessageWidget->AddToViewport();
		PopupMessageWidget->FadeIn();
	}
	// New custom game mode
	else
	{
		SaveCustomAndReselect();
	}
}

void UGameModesWidget::OnButtonClicked_StartFromCustom()
{
	// Handle Starting without saving
	const FStartWidgetProperties StartWidgetProperties = CustomGameModesWidget_Current->GetStartWidgetProperties();
	const bool bIsPresetMode = StartWidgetProperties.DefiningConfig.BaseGameMode != EBaseGameMode::None;
	const bool bIsCustomMode = IsCustomGameMode(StartWidgetProperties.DefiningConfig.CustomGameModeName);
	const bool bNewCustomGameModeNameEmpty = StartWidgetProperties.NewCustomGameModeName.IsEmpty();
	const bool bInvalidCustomGameModeName = IsPresetGameMode(StartWidgetProperties.NewCustomGameModeName);

	// Invalid custom game mode name
	if (bInvalidCustomGameModeName)
	{
		PopupMessageWidget = CreateWidget<UPopupMessageWidget>(this, PopupMessageClass);
		TArray<UBSButton*> Buttons = PopupMessageWidget->InitPopup(FText::FromString("Start Without Saving"),
			FText::FromString(
				"The current custom game mode name is not valid. "
				"Do you want to start the game mode without saving it? Your scores will not be saved."), 2);

		Buttons[0]->SetButtonText(FText::FromString("No"));
		Buttons[0]->OnBSButtonButtonPressed_NonDynamic.AddLambda([this]
		{
			PopupMessageWidget->FadeOut();
		});

		Buttons[1]->SetButtonText(FText::FromString("Yes"));
		Buttons[1]->OnBSButtonButtonPressed_NonDynamic.AddLambda([this]
		{
			PopupMessageWidget->FadeOut();
			ShowAudioFormatSelect(false);
		});

		PopupMessageWidget->AddToViewport();
		PopupMessageWidget->FadeIn();
	}
	// No name for default mode or invalid name
	else if (bIsPresetMode && bNewCustomGameModeNameEmpty)
	{
		PopupMessageWidget = CreateWidget<UPopupMessageWidget>(this, PopupMessageClass);
		TArray<UBSButton*> Buttons = PopupMessageWidget->InitPopup(FText::FromString("Play Preset Game Mode"),
			FText::FromString(
				"A preset game mode is selected with no custom game mode name. Do you want to start a preset game mode?"
				" \n\n If you meant to save this as a custom mode, click no and fill out the New Custom Game Mode field."),
			2);

		Buttons[0]->SetButtonText(FText::FromString("No"));
		Buttons[0]->OnBSButtonButtonPressed_NonDynamic.AddLambda([this]
		{
			PopupMessageWidget->FadeOut();
		});

		Buttons[1]->SetButtonText(FText::FromString("Yes"));
		Buttons[1]->OnBSButtonButtonPressed_NonDynamic.AddLambda([this]
		{
			PopupMessageWidget->FadeOut();
			PresetSelection_PresetGameMode = BSConfig->DefiningConfig.BaseGameMode;
			PresetSelection_Difficulty = BSConfig->DefiningConfig.Difficulty;
			ShowAudioFormatSelect(true);
		});

		PopupMessageWidget->AddToViewport();
		PopupMessageWidget->FadeIn();
	}
	// No game mode selected somehow
	else if (!bIsCustomMode && !bIsPresetMode && bNewCustomGameModeNameEmpty)
	{
		CustomGameModesWidget_CreatorView->SetNewCustomGameModeName("");
		CustomGameModesWidget_PropertyView->SetNewCustomGameModeName("");
		InitCustomGameModesWidgetOptions(EBaseGameMode::MultiBeat, EGameModeDifficulty::Normal);
	}
	// Bypass saving if identical to existing
	else if ((StartWidgetProperties.DefiningConfig.CustomGameModeName.
	                                Equals(StartWidgetProperties.NewCustomGameModeName) || StartWidgetProperties.
		NewCustomGameModeName.IsEmpty()) && IsCurrentConfigIdenticalToSelectedCustom())
	{
		CustomGameModesWidget_PropertyView->SetNewCustomGameModeName("");
		CustomGameModesWidget_CreatorView->SetNewCustomGameModeName("");
		SetAndPlaySavedText(FText::FromString(StartWidgetProperties.DefiningConfig.CustomGameModeName + " up to date"));
		ShowAudioFormatSelect(false);
	}
	// Ask to override
	else if (DoesCustomGameModeExist())
	{
		PopupMessageWidget = CreateWidget<UPopupMessageWidget>(this, PopupMessageClass);
		TArray<UBSButton*> Buttons = PopupMessageWidget->InitPopup(GetWidgetTextFromKey("GM_OverwritePopupTitle"),
			FText::GetEmpty(), 2);

		Buttons[0]->SetButtonText(GetWidgetTextFromKey("GM_OverwriteCancel"));
		Buttons[0]->OnBSButtonButtonPressed_NonDynamic.AddLambda([this]
		{
			PopupMessageWidget->FadeOut();
		});

		Buttons[1]->SetButtonText(GetWidgetTextFromKey("GM_OverwriteConfirm"));
		Buttons[1]->OnBSButtonButtonPressed_NonDynamic.AddLambda([this]
		{
			PopupMessageWidget->FadeOut();
			if (SaveCustomAndReselect())
			{
				ShowAudioFormatSelect(false);
			}
		});

		PopupMessageWidget->AddToViewport();
		PopupMessageWidget->FadeIn();
	}
	// New custom game mode
	else
	{
		if (SaveCustomAndReselect())
		{
			ShowAudioFormatSelect(false);
		}
	}
}

void UGameModesWidget::OnButtonClicked_RemoveSelectedCustom()
{
	if (!IsCustomGameMode(BSConfig->DefiningConfig.CustomGameModeName))
	{
		return;
	}

	PopupMessageWidget = CreateWidget<UPopupMessageWidget>(this, PopupMessageClass);
	TArray<UBSButton*> Buttons = PopupMessageWidget->InitPopup(FText::FromString("Removal Confirmation"),
		FText::FromString("Are you sure you want remove " + BSConfig->DefiningConfig.CustomGameModeName + "?"), 3);

	Buttons[0]->SetButtonText(FText::FromString("Cancel"));
	Buttons[0]->SetWrapTextAt(350.f);
	Buttons[0]->OnBSButtonButtonPressed_NonDynamic.AddLambda([this]
	{
		PopupMessageWidget->FadeOut();
	});

	Buttons[1]->SetButtonText(FText::FromString("Only Remove Game Mode"));
	Buttons[1]->SetWrapTextAt(350.f);
	Buttons[1]->OnBSButtonButtonPressed_NonDynamic.AddLambda([this]
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
		InitCustomGameModesWidgetOptions(EBaseGameMode::MultiBeat, EGameModeDifficulty::Normal);
		UpdateSaveStartButtonStates();
	});

	Buttons[2]->SetButtonText(FText::FromString("Remove Game Mode and Scores"));
	Buttons[2]->SetWrapTextAt(350.f);
	Buttons[2]->OnBSButtonButtonPressed_NonDynamic.AddLambda([this]
	{
		PopupMessageWidget->FadeOut();
		const FString GameModeNameToRemove = BSConfig->DefiningConfig.CustomGameModeName;
		OnAccessTokenResponse.BindUObject(this, &ThisClass::OnAccessTokenResponseReceived, GameModeNameToRemove);
		RequestAccessToken(LoadPlayerSettings().User.RefreshCookie, OnAccessTokenResponse);
	});

	PopupMessageWidget->AddToViewport();
	PopupMessageWidget->FadeIn();
}

void UGameModesWidget::OnButtonClicked_RemoveAllCustom()
{
	PopupMessageWidget = CreateWidget<UPopupMessageWidget>(this, PopupMessageClass);
	TArray<UBSButton*> Buttons = PopupMessageWidget->InitPopup(FText::FromString("Removal Confirmation"),
		FText::FromString("Are you sure you want remove all custom game modes?"), 2);

	Buttons[0]->SetButtonText(FText::FromString("No"));
	Buttons[0]->OnBSButtonButtonPressed_NonDynamic.AddLambda([this]
	{
		PopupMessageWidget->FadeOut();
	});

	Buttons[1]->SetButtonText(FText::FromString("Yes"));
	Buttons[1]->OnBSButtonButtonPressed_NonDynamic.AddLambda([this]
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
		const FStartWidgetProperties Update = FStartWidgetProperties(GameModeConfig.DefiningConfig, true);
		CustomGameModesWidget_CreatorView->SetStartWidgetProperties(Update);
		CustomGameModesWidget_PropertyView->SetStartWidgetProperties(Update);
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
	if (IsPresetGameMode(GameModeToSave.DefiningConfig.CustomGameModeName) || GameModeToSave.DefiningConfig.GameModeType
		== EGameModeType::Preset)
	{
		CustomGameModesWidget_PropertyView->SetNewCustomGameModeName("");
		CustomGameModesWidget_CreatorView->SetNewCustomGameModeName("");
		const TArray SavedText = {
			FText::FromString("Error trying to save Custom Game Mode:"),
			FText::FromString(GameModeToSave.DefiningConfig.CustomGameModeName)
		};
		SetAndPlaySavedText(FText::Join(FText::FromString(" "), SavedText));
		return false;
	}

	SaveCustomGameMode(GameModeToSave);

	if (SuccessMessage.IsEmpty())
	{
		const TArray SavedText = {
			FText::FromString(GameModeToSave.DefiningConfig.CustomGameModeName),
			GetWidgetTextFromKey("GM_GameModeSavedText")
		};
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
	const bool bIsPresetMode = StartWidgetProperties.DefiningConfig.BaseGameMode != EBaseGameMode::None;
	const bool bIsCustomMode = IsCustomGameMode(StartWidgetProperties.DefiningConfig.CustomGameModeName);
	const bool bNewCustomGameModeNameEmpty = StartWidgetProperties.NewCustomGameModeName.IsEmpty();
	const bool bInvalidCustomGameModeName = IsPresetGameMode(StartWidgetProperties.NewCustomGameModeName);

	Button_RemoveAllCustom->SetIsEnabled(!LoadCustomGameModes().IsEmpty());
	Button_RemoveSelectedCustom->SetIsEnabled(bIsCustomMode);

	// Invalid options, any remaining buttons disabled
	if (!bAllCustomGameModeOptionsValid)
	{
		Button_SaveCustom->SetIsEnabled(false);
		CustomGameModesWidget_CreatorView->Widget_Preview->Button_Create->SetIsEnabled(false);
		Button_StartFromCustom->SetIsEnabled(false);
		Button_ClearRLHistory->SetIsEnabled(false);
		Button_ExportCustom->SetIsEnabled(false);
		return;
	}

	/* At this point, all custom game mode options are valid. They can play the game mode but not be able to save it */
	Button_StartFromCustom->SetIsEnabled(true);
	Button_ClearRLHistory->SetIsEnabled(bIsCustomMode);
	Button_ExportCustom->SetIsEnabled(bIsCustomMode);

	// No name for default mode or invalid name
	if ((bIsPresetMode && bNewCustomGameModeNameEmpty) || bInvalidCustomGameModeName)
	{
		Button_SaveCustom->SetIsEnabled(false);
		CustomGameModesWidget_CreatorView->Widget_Preview->Button_Create->SetIsEnabled(false);
	}
	// No game mode selected for whatever reason
	else if (!bIsCustomMode && !bIsPresetMode && bNewCustomGameModeNameEmpty)
	{
		Button_SaveCustom->SetIsEnabled(false);
		CustomGameModesWidget_CreatorView->Widget_Preview->Button_Create->SetIsEnabled(false);
	}
	// Can save the custom game mode
	else
	{
		Button_SaveCustom->SetIsEnabled(true);
		CustomGameModesWidget_CreatorView->Widget_Preview->Button_Create->SetIsEnabled(true);
	}
}

void UGameModesWidget::ShowAudioFormatSelect(const bool bStartFromDefaultGameMode)
{
	AudioSelectWidget = CreateWidget<UAudioSelectWidget>(this, AudioSelectClass);

	AudioSelectWidget->OnStartButtonClickedDelegate.BindLambda(
		[this, bStartFromDefaultGameMode](const FBS_AudioConfig& AudioConfig)
		{
			FGameModeTransitionState GameModeTransitionState;

			GameModeTransitionState.bSaveCurrentScores = false;
			GameModeTransitionState.TransitionState = bIsMainMenuChild
				? ETransitionState::StartFromMainMenu
				: ETransitionState::StartFromPostGameMenu;
			GameModeTransitionState.BSConfig = bStartFromDefaultGameMode
				? FindPresetGameMode(PresetSelection_PresetGameMode, PresetSelection_Difficulty)
				: GetCustomGameModeOptions();

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

bool UGameModesWidget::DoesCustomGameModeExist()
{
	const FStartWidgetProperties StartWidgetProperties = CustomGameModesWidget_Current->GetStartWidgetProperties();
	const FString NewCustomGameModeName = StartWidgetProperties.NewCustomGameModeName;

	// If NewCustomGameModeName is blank, ask to override
	if (IsCustomGameMode(StartWidgetProperties.DefiningConfig.CustomGameModeName) && NewCustomGameModeName.IsEmpty())
	{
		return true;
	}

	// If NewCustomGameModeName already exists as a saved custom game mode, ask to override
	if (IsCustomGameMode(NewCustomGameModeName))
	{
		return true;
	}

	return false;
}

void UGameModesWidget::ShowConfirmOverwriteMessage_Import(const FBSConfig& ImportedConfig)
{
	PopupMessageWidget = CreateWidget<UPopupMessageWidget>(this, PopupMessageClass);
	TArray<UBSButton*> Buttons = PopupMessageWidget->InitPopup(GetWidgetTextFromKey("GM_OverwritePopupTitle"),
		FText::GetEmpty(), 2);

	Buttons[0]->SetButtonText(GetWidgetTextFromKey("GM_OverwriteCancel"));
	Buttons[0]->OnBSButtonButtonPressed_NonDynamic.AddLambda([this]
	{
		PopupMessageWidget->FadeOut();
	});

	Buttons[1]->SetButtonText(GetWidgetTextFromKey("GM_OverwriteConfirm"));
	Buttons[1]->OnBSButtonButtonPressed_NonDynamic.AddLambda([this, &ImportedConfig]
	{
		PopupMessageWidget->FadeOut();
		SaveCustomAndReselect(
			FText::FromString("Successfully imported " + ImportedConfig.DefiningConfig.CustomGameModeName));
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

void UGameModesWidget::OnAccessTokenResponseReceived(const FString& AccessToken, FString GameModeNameToRemove)
{
	OnAccessTokenResponse.Unbind();
	OnDeleteScoresResponse.BindUObject(this, &ThisClass::OnDeleteScoresResponseReceived, GameModeNameToRemove);
	DeleteScores(GameModeNameToRemove, LoadPlayerSettings().User.UserID, AccessToken, OnDeleteScoresResponse);
}

void UGameModesWidget::OnDeleteScoresResponseReceived(const int32 NumScoresRemoved, const int32 ResponseCode,
	FString GameModeNameToRemove)
{
	OnDeleteScoresResponse.Unbind();
	if (ResponseCode == 200)
	{
		const int32 NumGameModesRemoved = RemoveCustomGameMode(FindCustomGameMode(GameModeNameToRemove));
		if (NumGameModesRemoved >= 1)
		{
			const FString String = GameModeNameToRemove + " removed and "
				+ FString::FromInt(NumScoresRemoved) + " scores removed";
			SetAndPlaySavedText(FText::FromString(String));
			CustomGameModesWidget_CreatorView->SetNewCustomGameModeName("");
			CustomGameModesWidget_PropertyView->SetNewCustomGameModeName("");
			CustomGameModesWidget_CreatorView->RefreshGameModeTemplateComboBoxOptions();
			CustomGameModesWidget_PropertyView->RefreshGameModeTemplateComboBoxOptions();
			InitCustomGameModesWidgetOptions(EBaseGameMode::MultiBeat, EGameModeDifficulty::Normal);
			UpdateSaveStartButtonStates();
		}
	}
	else
	{
		SetAndPlaySavedText(FText::FromString("Error connecting to database, delete aborted"));
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
