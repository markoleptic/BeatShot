// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.
// ReSharper disable CppMemberFunctionMayBeConst


#include "SubMenuWidgets/GameModesWidget.h"
#include "GlobalConstants.h"
#include "Blueprint/WidgetTree.h"
#include "Components/WidgetSwitcher.h"
#include "Components/VerticalBox.h"
#include "Components/Border.h"
#include "Components/ComboBoxString.h"
#include "Components/EditableTextBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "OverlayWidgets/PopupMessageWidget.h"
#include "OverlayWidgets/AudioSelectWidget.h"
#include "WidgetComponents/DoubleSyncedSliderAndTextBox.h"
#include "WidgetComponents/SavedTextWidget.h"
#include "SubMenuWidgets/GameModesWidget_DefiningConfig.h"
#include "SubMenuWidgets/GameModesWidget_AIConfig.h"
#include "SubMenuWidgets/GameModesWidget_BeatGridConfig.h"
#include "SubMenuWidgets/GameModesWidget_TargetConfig.h"
#include "WidgetComponents/MenuButton.h"

using namespace Constants;

void UGameModesWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	Button_PlayFromStandard->SetIsEnabled(false);
	Button_CustomizeFromStandard->SetIsEnabled(false);
	Border_DifficultySelect->SetVisibility(ESlateVisibility::Collapsed);
	Button_SaveCustom->SetIsEnabled(false);
	Button_SaveCustomAndStart->SetIsEnabled(false);
	Button_StartCustom->SetIsEnabled(false);
	Button_RemoveSelectedCustom->SetIsEnabled(false);

	if (!LoadCustomGameModes().IsEmpty())
	{
		Button_RemoveAllCustom->SetIsEnabled(true);
	}
	else
	{
		Button_RemoveAllCustom->SetIsEnabled(false);
	}
	
	/* Default Game Mode widgets */
	Button_NormalDifficulty->SetDefaults(static_cast<uint8>(EGameModeDifficulty::Normal), Button_HardDifficulty);
	Button_HardDifficulty->SetDefaults(static_cast<uint8>(EGameModeDifficulty::Hard), Button_DeathDifficulty);
	Button_DeathDifficulty->SetDefaults(static_cast<uint8>(EGameModeDifficulty::Death), Button_NormalDifficulty);

	Button_BeatGrid->SetDefaults(static_cast<uint8>(EBaseGameMode::BeatGrid), Button_BeatTrack);
	Button_BeatTrack->SetDefaults(static_cast<uint8>(EBaseGameMode::BeatTrack), Button_MultiBeat);
	Button_MultiBeat->SetDefaults(static_cast<uint8>(EBaseGameMode::MultiBeat), Button_SingleBeat);
	Button_SingleBeat->SetDefaults(static_cast<uint8>(EBaseGameMode::SingleBeat), Button_BeatGrid);

	MenuButton_DefaultGameModes->SetDefaults(Box_DefaultGameModes, MenuButton_CustomGameModes);
	MenuButton_CustomGameModes->SetDefaults(Box_CustomGameModes, MenuButton_DefaultGameModes);

	BeatGridConfig = CreateWidget<UGameModesWidget_BeatGridConfig>(this, BeatGridConfigClass);
	Box_BeatGridConfig->AddChildToVerticalBox(BeatGridConfig);

	AIConfig = CreateWidget<UGameModesWidget_AIConfig>(this, AIConfigClass);
	Box_AIConfig->AddChildToVerticalBox(AIConfig);

	TargetConfig = CreateWidget<UGameModesWidget_TargetConfig>(this, TargetConfigClass);
	Box_TargetConfig->AddChildToVerticalBox(TargetConfig);

	DefiningConfig = CreateWidget<UGameModesWidget_DefiningConfig>(this, DefiningConfigClass);
	Box_DefiningConfig->AddChildToVerticalBox(DefiningConfig);
	
	BindAllDelegates();
	MenuButton_DefaultGameModes->SetActive();
	
	/* Setup default custom game mode options to MultiBeat */
	const FBSConfig DefaultMultiBeatMode = FBSConfig::GetPresetGameModes()[3];
	
	DefiningConfig->ComboBox_GameModeName->ClearOptions();
	DefiningConfig->ComboBox_BaseGameMode->ClearOptions();

	/* Add DefaultModes to GameModeName ComboBox and BaseGameMode ComboBox */
	for (const FBSConfig& GameMode : FBSConfig::GetPresetGameModes())
	{
		const FString GameModeName = UEnum::GetDisplayValueAsText(GameMode.DefiningConfig.BaseGameMode).ToString();
		DefiningConfig->ComboBox_GameModeName->AddOption(GameModeName);
		DefiningConfig->ComboBox_BaseGameMode->AddOption(GameModeName);
	}

	/* Add Custom Game Modes to GameModeName ComboBox */
	for (const FBSConfig& GameMode : LoadCustomGameModes())
	{
		DefiningConfig->ComboBox_GameModeName->AddOption(GameMode.DefiningConfig.CustomGameModeName);
	}

	/* Add difficulties to GameModeDifficulty ComboBox */
	for (const EGameModeDifficulty& Mode : TEnumRange<EGameModeDifficulty>())
	{
		DefiningConfig->ComboBox_GameModeDifficulty->AddOption(UEnum::GetDisplayValueAsText(Mode).ToString());
	}
	
	PopulateGameModeOptions(DefaultMultiBeatMode);
}

void UGameModesWidget::BindAllDelegates()
{
	MenuButton_DefaultGameModes->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_MenuButton);
	MenuButton_CustomGameModes->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_MenuButton);
	
	Button_CustomizeFromStandard->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_CustomGameModeButton);
	Button_PlayFromStandard->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_CustomGameModeButton);
	Button_SaveCustom->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_CustomGameModeButton);
	Button_SaveCustomAndStart->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_CustomGameModeButton);
	Button_StartCustom->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_CustomGameModeButton);
	Button_StartWithoutSaving->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_CustomGameModeButton);
	Button_RemoveAllCustom->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_CustomGameModeButton);
	Button_RemoveSelectedCustom->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnButtonClicked_CustomGameModeButton);
	
	Button_NormalDifficulty->OnBSButtonPressed.AddDynamic(this, &UGameModesWidget::OnButtonClicked_Difficulty);
	Button_HardDifficulty->OnBSButtonPressed.AddDynamic(this, &UGameModesWidget::OnButtonClicked_Difficulty);
	Button_DeathDifficulty->OnBSButtonPressed.AddDynamic(this, &UGameModesWidget::OnButtonClicked_Difficulty);
	
	// Default Game Modes
	Button_BeatGrid->OnBSButtonPressed.AddDynamic(this, &UGameModesWidget::OnButtonClicked_DefaultGameMode);
	Button_BeatTrack->OnBSButtonPressed.AddDynamic(this, &UGameModesWidget::OnButtonClicked_DefaultGameMode);
	Button_MultiBeat->OnBSButtonPressed.AddDynamic(this, &UGameModesWidget::OnButtonClicked_DefaultGameMode);
	Button_SingleBeat->OnBSButtonPressed.AddDynamic(this, &UGameModesWidget::OnButtonClicked_DefaultGameMode);

	DefiningConfig->OnRepopulateGameModeOptions.AddUObject(this, &UGameModesWidget::PopulateGameModeOptions);
	DefiningConfig->OnDefiningConfigUpdate_SaveStartButtonStates.AddUObject(this, &UGameModesWidget::UpdateSaveStartButtonStates);
	TargetConfig->TargetScaleConstrained->OnValueChanged_Max.AddUObject(BeatGridConfig, &UGameModesWidget_BeatGridConfig::OnBeatGridUpdate_MaxTargetScale);
	BeatGridConfig->OnBeatGridUpdate_SaveStartButtonStates.AddUObject(this, &UGameModesWidget::UpdateSaveStartButtonStates);
}

void UGameModesWidget::OnButtonClicked_DefaultGameMode(const UBSButton* GameModeButton)
{
	PresetSelection_PresetGameMode = static_cast<EBaseGameMode>(GameModeButton->GetEnumValue());
	DefiningConfig->ComboBox_GameModeName->SetSelectedOption(UEnum::GetDisplayValueAsText(PresetSelection_PresetGameMode).ToString());
	PopulateGameModeOptions(FBSConfig::MakePresetConfig(PresetSelection_PresetGameMode, EGameModeDifficulty::Normal));

	Button_NormalDifficulty->SetInActive();
	Button_HardDifficulty->SetInActive();
	Button_DeathDifficulty->SetInActive();
	PresetSelection_Difficulty = EGameModeDifficulty::Normal;
	
	Border_DifficultySelect->SetVisibility(ESlateVisibility::Visible);
	Button_PlayFromStandard->SetIsEnabled(false);
	Button_CustomizeFromStandard->SetIsEnabled(true);
}

void UGameModesWidget::OnButtonClicked_Difficulty(const UBSButton* GameModeButton)
{
	PresetSelection_Difficulty = static_cast<EGameModeDifficulty>(GameModeButton->GetEnumValue());
	const FBSConfig SelectedGameMode = FBSConfig::MakePresetConfig(PresetSelection_PresetGameMode, PresetSelection_Difficulty);
	PopulateGameModeOptions(SelectedGameMode);
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
	else if (Button == Button_StartCustom)
	{
		OnButtonClicked_StartCustom();
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
}

void UGameModesWidget::OnButtonClicked_MenuButton(const UBSButton* Button)
{
	MenuSwitcher->SetActiveWidget(Cast<UMenuButton>(Button)->GetBox());
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

void UGameModesWidget::OnButtonClicked_StartCustom()
{
	ShowAudioFormatSelect(false);
}

void UGameModesWidget::OnButtonClicked_RemoveSelectedCustom()
{
	const FString SelectedCustomGameMode = DefiningConfig->ComboBox_GameModeName->GetSelectedOption();
	if (IsCustomGameMode(SelectedCustomGameMode))
	{
		const int32 NumRemoved = RemoveCustomGameMode(FindCustomGameMode(SelectedCustomGameMode));
		if (NumRemoved >= 1)
		{
			SavedTextWidget->SetSavedText(FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "GM_GameModeRemovedText"));
			SavedTextWidget->PlayFadeInFadeOut();
		}
		DefiningConfig->PopulateGameModeNameComboBox("");
	}
}

void UGameModesWidget::OnButtonClicked_RemoveAllCustom()
{
	RemoveAllCustomGameModes();
	SavedTextWidget->SetSavedText(FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "GM_AllGameModesRemovedText"));
	SavedTextWidget->PlayFadeInFadeOut();
	DefiningConfig->PopulateGameModeNameComboBox("");
}

void UGameModesWidget::OnButtonClicked_ConfirmOverwrite()
{
	PopupMessageWidget->FadeOut();
	SaveCustomGameModeAndShowSavedText(GetCustomGameModeOptions());
	DefiningConfig->PopulateGameModeNameComboBoxAfterSave();
}

void UGameModesWidget::OnButtonClicked_ConfirmOverwriteAndStartGame()
{
	PopupMessageWidget->FadeOut();
	SaveCustomGameModeAndShowSavedText(GetCustomGameModeOptions());
	ShowAudioFormatSelect(false);
}

void UGameModesWidget::OnButtonClicked_CancelOverwrite()
{
	PopupMessageWidget->FadeOut();
}

void UGameModesWidget::PopulateGameModeOptions(const FBSConfig& InBSConfig)
{
	DefiningConfig->InitializeDefiningConfig(InBSConfig.DefiningConfig, InBSConfig.DefiningConfig.BaseGameMode);
	TargetConfig->InitializeTargetConfig(InBSConfig.TargetConfig, InBSConfig.DefiningConfig.BaseGameMode);
	BeatGridConfig->InitializeBeatGrid(InBSConfig.GridConfig, TargetConfig->TargetScaleConstrained->GetTextTooltipBox_Max());
	BeatGridConfig->OnBeatGridUpdate_MaxTargetScale(InBSConfig.TargetConfig.MaxTargetScale);
	AIConfig->InitializeAIConfig(InBSConfig.AIConfig, InBSConfig.DefiningConfig.BaseGameMode);
	
	/*switch(InBSConfig.DefiningConfig.BaseGameMode)
	{
	case EBaseGameMode::SingleBeat:
		Box_AIConfig->SetVisibility(ESlateVisibility::Visible);
		Box_BeatGridConfig->SetVisibility(ESlateVisibility::Collapsed);
		AIConfig->InitializeAIConfig(InBSConfig.AIConfig, InBSConfig.DefiningConfig.BaseGameMode);
		break;
	case EBaseGameMode::MultiBeat:
		Box_AIConfig->SetVisibility(ESlateVisibility::Visible);
		Box_BeatGridConfig->SetVisibility(ESlateVisibility::Collapsed);
		AIConfig->InitializeAIConfig(InBSConfig.AIConfig, InBSConfig.DefiningConfig.BaseGameMode);
		break;
	case EBaseGameMode::BeatGrid:
		Box_AIConfig->SetVisibility(ESlateVisibility::Collapsed);
		Box_BeatGridConfig->SetVisibility(ESlateVisibility::Visible);
		BeatGridConfig->InitializeBeatGrid(InBSConfig.GridConfig, TargetConfig->TargetScaleConstrained->GetTextTooltipBox_Max());
		BeatGridConfig->OnBeatGridUpdate_MaxTargetScale(InBSConfig.TargetConfig.MaxTargetScale);
		break;
	case EBaseGameMode::BeatTrack:
		Box_AIConfig->SetVisibility(ESlateVisibility::Collapsed);
		Box_BeatGridConfig->SetVisibility(ESlateVisibility::Collapsed);
		break;
	case EBaseGameMode::ChargedBeatTrack:
		Box_AIConfig->SetVisibility(ESlateVisibility::Collapsed);
		Box_BeatGridConfig->SetVisibility(ESlateVisibility::Collapsed);
		break;
	case EBaseGameMode::None:
		break;
	}*/
}

FBSConfig UGameModesWidget::GetCustomGameModeOptions() const
{
	FBSConfig ReturnStruct;
	ReturnStruct.DefiningConfig = DefiningConfig->GetDefiningConfig();
	ReturnStruct.TargetConfig = TargetConfig->GetTargetConfig();
	ReturnStruct.AudioConfig.PlayerDelay = ReturnStruct.TargetConfig.SpawnBeatDelay;
	ReturnStruct.AIConfig = AIConfig->GetAIConfig();
	ReturnStruct.GridConfig = BeatGridConfig->GetBeatGridConfig();
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
	const bool bNoSavedCustomGameModes = LoadCustomGameModes().IsEmpty();
	const bool bIsBeatGridMode = DefiningConfig->ComboBox_BaseGameMode->GetSelectedOption().Equals(UEnum::GetDisplayValueAsText(EBaseGameMode::BeatGrid).ToString());
	const bool bBeatGridIsConstrained = BeatGridConfig->IsAnyParameterConstrained();
	const bool bIsDefaultMode = IsPresetGameMode(DefiningConfig->ComboBox_GameModeName->GetSelectedOption());
	const bool bIsCustomMode = IsCustomGameMode(DefiningConfig->ComboBox_GameModeName->GetSelectedOption());
	const bool bGameModeNameComboBoxEmpty = DefiningConfig->ComboBox_GameModeName->GetSelectedOption().IsEmpty();
	const bool bCustomTextEmpty = DefiningConfig->TextBox_CustomGameModeName->GetText().IsEmptyOrWhitespace();
	const bool bInvalidCustomGameModeName = IsPresetGameMode(DefiningConfig->TextBox_CustomGameModeName->GetText().ToString());

	/* RemoveAll Button */
	if (bNoSavedCustomGameModes)
	{
		Button_RemoveAllCustom->SetIsEnabled(false);
	}
	else
	{
		Button_RemoveAllCustom->SetIsEnabled(true);
	}

	/* BeatGrid */
	if (bIsBeatGridMode && bBeatGridIsConstrained)
	{
		Button_SaveCustom->SetIsEnabled(false);
		Button_SaveCustomAndStart->SetIsEnabled(false);
		Button_StartCustom->SetIsEnabled(false);
		Button_StartWithoutSaving->SetIsEnabled(false);
		return;
	}
	Button_StartWithoutSaving->SetIsEnabled(true);
	
	if (bGameModeNameComboBoxEmpty || (bIsDefaultMode && bCustomTextEmpty) || bInvalidCustomGameModeName)
	{
		Button_SaveCustom->SetIsEnabled(false);
		Button_SaveCustomAndStart->SetIsEnabled(false);
		Button_StartCustom->SetIsEnabled(false);
		Button_RemoveSelectedCustom->SetIsEnabled(false);
		return;
	}

	if (bIsCustomMode || (bIsDefaultMode && !bCustomTextEmpty))
	{
		Button_SaveCustom->SetIsEnabled(true);
		Button_SaveCustomAndStart->SetIsEnabled(true);
		Button_StartCustom->SetIsEnabled(true);
		Button_RemoveSelectedCustom->SetIsEnabled(true);
	}
}

void UGameModesWidget::ShowConfirmOverwriteMessage(const bool bStartGameAfter)
{
	PopupMessageWidget = CreateWidget<UPopupMessageWidget>(this, PopupMessageClass);
	if (PopupMessageWidget)
	{
		PopupMessageWidget->InitPopup(FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "GM_OverwritePopupTitle"), FText::GetEmpty(),
									  FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "GM_OverwriteConfirm"),
									  FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "GM_OverwriteCancel"));
		PopupMessageWidget->AddToViewport();
		PopupMessageWidget->FadeIn();
	}

	if (bStartGameAfter)
	{
		PopupMessageWidget->OnButton1Pressed.AddDynamic(this, &UGameModesWidget::OnButtonClicked_ConfirmOverwriteAndStartGame);
	}
	else
	{
		PopupMessageWidget->OnButton1Pressed.AddDynamic(this, &UGameModesWidget::OnButtonClicked_ConfirmOverwrite);
	}
	PopupMessageWidget->OnButton2Pressed.AddDynamic(this, &UGameModesWidget::OnButtonClicked_CancelOverwrite);
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
			BSConfig = FBSConfig::MakePresetConfig(PresetSelection_PresetGameMode, PresetSelection_Difficulty);
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
