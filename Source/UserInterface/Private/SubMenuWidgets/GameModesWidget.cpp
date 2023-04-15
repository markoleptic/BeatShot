// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


// ReSharper disable CppMemberFunctionMayBeConst
#include "SubMenuWidgets/GameModesWidget.h"
#include "GlobalConstants.h"
#include "Blueprint/WidgetTree.h"
#include "Components/WidgetSwitcher.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "Components/Border.h"
#include "Components/ComboBoxString.h"
#include "Components/EditableTextBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "OverlayWidgets/PopupMessageWidget.h"
#include "OverlayWidgets/AudioSelectWidget.h"
#include "WidgetComponents/DoubleSyncedSliderAndTextBox.h"
#include "WidgetComponents/GameModeButton.h"
#include "WidgetComponents/SavedTextWidget.h"
#include "WidgetComponents/SlideRightButton.h"
#include "SubMenuWidgets/GameModesWidget_DefiningConfig.h"
#include "SubMenuWidgets/GameModesWidget_SpatialConfig.h"
#include "SubMenuWidgets/GameModesWidget_AIConfig.h"
#include "SubMenuWidgets/GameModesWidget_BeatGridConfig.h"
#include "SubMenuWidgets/GameModesWidget_BeatTrackConfig.h"
#include "SubMenuWidgets/GameModesWidget_TargetConfig.h"

using namespace Constants;

void UGameModesWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	MenuWidgets.Add(DefaultGameModesButton, DefaultGameModes);
	MenuWidgets.Add(CustomGameModesButton, CustomGameModes);
	PlayFromStandardButton->SetIsEnabled(false);
	CustomizeFromStandardButton->SetIsEnabled(false);
	SpreadSelect->SetVisibility(ESlateVisibility::Collapsed);
	SaveCustomButton->SetIsEnabled(false);
	SaveCustomAndStartButton->SetIsEnabled(false);
	StartCustomButton->SetIsEnabled(false);
	RemoveSelectedCustomButton->SetIsEnabled(false);

	if (!LoadCustomGameModes().IsEmpty())
	{
		RemoveAllCustomButton->SetIsEnabled(true);
	}
	else
	{
		RemoveAllCustomButton->SetIsEnabled(false);
	}
	
	/* Default Game Mode widgets */
	SingleBeatNormalButton->SetDefaults(EGameModeDifficulty::Normal, EDefaultMode::SingleBeat, SingleBeatHardButton);
	SingleBeatHardButton->SetDefaults(EGameModeDifficulty::Hard, EDefaultMode::SingleBeat, SingleBeatDeathButton);
	SingleBeatDeathButton->SetDefaults(EGameModeDifficulty::Death, EDefaultMode::SingleBeat, MultiBeatNormalButton);

	MultiBeatNormalButton->SetDefaults(EGameModeDifficulty::Normal, EDefaultMode::MultiBeat, MultiBeatHardButton);
	MultiBeatHardButton->SetDefaults(EGameModeDifficulty::Hard, EDefaultMode::MultiBeat, MultiBeatDeathButton);
	MultiBeatDeathButton->SetDefaults(EGameModeDifficulty::Death, EDefaultMode::MultiBeat, BeatGridNormalButton);

	BeatGridNormalButton->SetDefaults(EGameModeDifficulty::Normal, EDefaultMode::BeatGrid, BeatGridHardButton);
	BeatGridHardButton->SetDefaults(EGameModeDifficulty::Hard, EDefaultMode::BeatGrid, BeatGridDeathButton);
	BeatGridDeathButton->SetDefaults(EGameModeDifficulty::Death, EDefaultMode::BeatGrid, BeatTrackNormalButton);

	BeatTrackNormalButton->SetDefaults(EGameModeDifficulty::Normal, EDefaultMode::BeatTrack, BeatTrackHardButton);
	BeatTrackHardButton->SetDefaults(EGameModeDifficulty::Hard, EDefaultMode::BeatTrack, BeatTrackDeathButton);
	BeatTrackDeathButton->SetDefaults(EGameModeDifficulty::Death, EDefaultMode::BeatTrack, SingleBeatNormalButton);
	
	SpatialConfig = CreateWidget<UGameModesWidget_SpatialConfig>(this, SpatialConfigClass);
	SpatialConfigBox->AddChildToVerticalBox(SpatialConfig);

	BeatGridConfig = CreateWidget<UGameModesWidget_BeatGridConfig>(this, BeatGridConfigClass);
	BeatGridBox->AddChildToVerticalBox(BeatGridConfig);

	AIConfig = CreateWidget<UGameModesWidget_AIConfig>(this, AIConfigClass);
	AIConfigBox->AddChildToVerticalBox(AIConfig);

	BeatTrackConfig = CreateWidget<UGameModesWidget_BeatTrackConfig>(this, BeatTrackConfigClass);
	BeatTrackConfigBox->AddChildToVerticalBox(BeatTrackConfig);

	TargetConfig = CreateWidget<UGameModesWidget_TargetConfig>(this, TargetConfigClass);
	TargetConfigBox->AddChildToVerticalBox(TargetConfig);

	DefiningConfig = CreateWidget<UGameModesWidget_DefiningConfig>(this, DefiningConfigClass);
	DefiningConfigBox->AddChildToVerticalBox(DefiningConfig);
	
	BindAllDelegates();
	OnButtonClicked_DefaultGameModes();
	
	/* Setup default custom game mode options to MultiBeat */
	const FBSConfig DefaultMultiBeatMode = FBSConfig::GetDefaultGameModes()[3];
	DefiningConfig->ComboBox_GameModeName->ClearOptions();
	DefiningConfig->ComboBox_BaseGameMode->ClearOptions();

	/* Add DefaultModes to GameModeName ComboBox and BaseGameMode ComboBox */
	for (const FBSConfig& GameMode : FBSConfig::GetDefaultGameModes())
	{
		const FString GameModeName = UEnum::GetDisplayValueAsText(GameMode.DefiningConfig.DefaultMode).ToString();
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
	DefiningConfig->ComboBox_GameModeName->SetSelectedOption(UEnum::GetDisplayValueAsText(DefaultMultiBeatMode.DefiningConfig.DefaultMode).ToString());
}

void UGameModesWidget::BindAllDelegates()
{
	DefaultGameModesButton->Button->OnClicked.AddDynamic(this, &UGameModesWidget::OnButtonClicked_DefaultGameModes);
	CustomGameModesButton->Button->OnClicked.AddDynamic(this, &UGameModesWidget::OnButtonClicked_CustomGameModes);
	CustomizeFromStandardButton->OnClicked.AddDynamic(this, &UGameModesWidget::OnButtonClicked_CustomizeFromStandard);
	PlayFromStandardButton->OnClicked.AddDynamic(this, &UGameModesWidget::OnButtonClicked_PlayFromStandard);
	SaveCustomButton->OnClicked.AddDynamic(this, &UGameModesWidget::OnButtonClicked_SaveCustom);
	SaveCustomAndStartButton->OnClicked.AddDynamic(this, &UGameModesWidget::OnButtonClicked_SaveCustomAndStart);
	StartCustomButton->OnClicked.AddDynamic(this, &UGameModesWidget::OnButtonClicked_StartCustom);
	StartWithoutSavingButton->OnClicked.AddDynamic(this, &UGameModesWidget::OnButtonClicked_StartWithoutSaving);
	RemoveAllCustomButton->OnClicked.AddDynamic(this, &UGameModesWidget::OnButtonClicked_RemoveAllCustom);
	RemoveSelectedCustomButton->OnClicked.AddDynamic(this, &UGameModesWidget::OnButtonClicked_RemoveSelectedCustom);
	
	// Default Game Modes
	SingleBeatNormalButton->OnGameModeButtonClicked.AddDynamic(this, &UGameModesWidget::OnButtonClicked_DefaultGameMode);
	SingleBeatHardButton->OnGameModeButtonClicked.AddDynamic(this, &UGameModesWidget::OnButtonClicked_DefaultGameMode);
	SingleBeatDeathButton->OnGameModeButtonClicked.AddDynamic(this, &UGameModesWidget::OnButtonClicked_DefaultGameMode);
	MultiBeatNormalButton->OnGameModeButtonClicked.AddDynamic(this, &UGameModesWidget::OnButtonClicked_DefaultGameMode);
	MultiBeatHardButton->OnGameModeButtonClicked.AddDynamic(this, &UGameModesWidget::OnButtonClicked_DefaultGameMode);
	MultiBeatDeathButton->OnGameModeButtonClicked.AddDynamic(this, &UGameModesWidget::OnButtonClicked_DefaultGameMode);
	BeatGridNormalButton->OnGameModeButtonClicked.AddDynamic(this, &UGameModesWidget::OnButtonClicked_DefaultGameMode);
	BeatGridHardButton->OnGameModeButtonClicked.AddDynamic(this, &UGameModesWidget::OnButtonClicked_DefaultGameMode);
	BeatGridDeathButton->OnGameModeButtonClicked.AddDynamic(this, &UGameModesWidget::OnButtonClicked_DefaultGameMode);
	BeatTrackNormalButton->OnGameModeButtonClicked.AddDynamic(this, &UGameModesWidget::OnButtonClicked_DefaultGameMode);
	BeatTrackHardButton->OnGameModeButtonClicked.AddDynamic(this, &UGameModesWidget::OnButtonClicked_DefaultGameMode);
	BeatTrackDeathButton->OnGameModeButtonClicked.AddDynamic(this, &UGameModesWidget::OnButtonClicked_DefaultGameMode);
	DynamicSpreadButton->OnClicked.AddDynamic(this, &UGameModesWidget::OnButtonClicked_DynamicSpread);
	NarrowSpreadButton->OnClicked.AddDynamic(this, &UGameModesWidget::OnButtonClicked_NarrowSpread);
	WideSpreadButton->OnClicked.AddDynamic(this, &UGameModesWidget::OnButtonClicked_WideSpread);

	DefiningConfig->OnRepopulateGameModeOptions.AddUObject(this, &UGameModesWidget::PopulateGameModeOptions);
	DefiningConfig->OnDefiningConfigUpdate_SaveStartButtonStates.AddUObject(this, &UGameModesWidget::UpdateSaveStartButtonStates);
	TargetConfig->TargetScaleConstrained->OnMaxValueChanged.AddUObject(BeatGridConfig, &UGameModesWidget_BeatGridConfig::OnBeatGridUpdate_MaxTargetScale);
	BeatGridConfig->OnBeatGridUpdate_SaveStartButtonStates.AddUObject(this, &UGameModesWidget::UpdateSaveStartButtonStates);
}

void UGameModesWidget::SlideButtons(const USlideRightButton* ActiveButton)
{
	for (const TTuple<USlideRightButton*, UVerticalBox*>& Elem : MenuWidgets)
	{
		if (Elem.Key != ActiveButton)
		{
			Elem.Key->SlideButton(false);
			continue;
		}
		Elem.Key->SlideButton(true);
		MenuSwitcher->SetActiveWidget(Elem.Value);
	}
}

void UGameModesWidget::SetGameModeButtonBackgroundColor(const UGameModeButton* ClickedButton) const
{
	ClickedButton->Button->SetBackgroundColor(BeatShotBlue);
	const EGameModeDifficulty ClickedButtonDifficulty = ClickedButton->Difficulty;
	const EDefaultMode ClickedButtonGameModeName = ClickedButton->DefaultMode;
	const UGameModeButton* Head = ClickedButton->Next;

	/** Change all other button backgrounds to white */
	while (!(Head->Difficulty == ClickedButtonDifficulty && Head->DefaultMode == ClickedButtonGameModeName))
	{
		Head->Button->SetBackgroundColor(FLinearColor::White);
		Head = Head->Next;
	}
}

void UGameModesWidget::OnButtonClicked_DefaultGameMode(const UGameModeButton* GameModeButton)
{
	DefaultMode = GameModeButton->DefaultMode;
	DefaultDifficulty = GameModeButton->Difficulty;
	const FBSConfig SelectedGameMode = FBSConfig(GameModeButton->DefaultMode, GameModeButton->Difficulty);
	DefiningConfig->ComboBox_GameModeName->SetSelectedOption(UEnum::GetDisplayValueAsText(GameModeButton->DefaultMode).ToString());
	PopulateGameModeOptions(SelectedGameMode);
	SetGameModeButtonBackgroundColor(GameModeButton);

	DynamicSpreadButton->SetBackgroundColor(FLinearColor::White);
	WideSpreadButton->SetBackgroundColor(FLinearColor::White);
	NarrowSpreadButton->SetBackgroundColor(FLinearColor::White);

	/** Don't show SpreadSelect if BeatGrid or BeatTrack */
	if (SelectedGameMode.DefiningConfig.DefaultMode == EDefaultMode::BeatGrid || SelectedGameMode.DefiningConfig.DefaultMode == EDefaultMode::BeatTrack)
	{
		SpreadSelect->SetVisibility(ESlateVisibility::Collapsed);
		PlayFromStandardButton->SetIsEnabled(true);
		CustomizeFromStandardButton->SetIsEnabled(true);
		return;
	}
	SpreadSelect->SetVisibility(ESlateVisibility::Visible);
	PlayFromStandardButton->SetIsEnabled(false);
	CustomizeFromStandardButton->SetIsEnabled(true);
}

void UGameModesWidget::OnButtonClicked_DynamicSpread()
{
	/** Change the background colors of the spread select buttons */
	DynamicSpreadButton->SetBackgroundColor(BeatShotBlue);
	NarrowSpreadButton->SetBackgroundColor(FLinearColor::White);
	WideSpreadButton->SetBackgroundColor(FLinearColor::White);
	if (DefaultMode == EDefaultMode::MultiBeat)
	{
		DefaultSpreadType = ESpreadType::DynamicRandom;
	}
	else
	{
		DefaultSpreadType = ESpreadType::DynamicEdgeOnly;
	}
	PopulateGameModeOptions(FBSConfig(DefaultMode, DefaultDifficulty, DefaultSpreadType));
	PlayFromStandardButton->SetIsEnabled(true);
}

void UGameModesWidget::OnButtonClicked_NarrowSpread()
{
	/** Change the background colors of the spread select buttons */
	DynamicSpreadButton->SetBackgroundColor(FLinearColor::White);
	NarrowSpreadButton->SetBackgroundColor(BeatShotBlue);
	WideSpreadButton->SetBackgroundColor(FLinearColor::White);
	DefaultSpreadType = ESpreadType::StaticNarrow;
	PopulateGameModeOptions(FBSConfig(DefaultMode, DefaultDifficulty, ESpreadType::StaticNarrow));
	PlayFromStandardButton->SetIsEnabled(true);
}

void UGameModesWidget::OnButtonClicked_WideSpread()
{
	/** Change the background colors of the spread select buttons */
	DynamicSpreadButton->SetBackgroundColor(FLinearColor::White);
	NarrowSpreadButton->SetBackgroundColor(FLinearColor::White);
	WideSpreadButton->SetBackgroundColor(BeatShotBlue);
	DefaultSpreadType = ESpreadType::StaticWide;
	PopulateGameModeOptions(FBSConfig(DefaultMode, DefaultDifficulty, ESpreadType::StaticWide));
	PlayFromStandardButton->SetIsEnabled(true);
}

void UGameModesWidget::OnButtonClicked_CustomizeFromStandard()
{
	OnButtonClicked_CustomGameModes();
}

void UGameModesWidget::OnButtonClicked_SaveCustom()
{
	if (CheckForExistingAndDisplayOverwriteMessage(false))
	{
		return;
	}
	
	SaveCustomGameModeAndShowSavedText(GetCustomGameModeOptions());
	DefiningConfig->PopulateGameModeNameComboBoxAfterSave();
}

void UGameModesWidget::OnButtonClicked_StartWithoutSaving()
{
	ShowAudioFormatSelect(false);
}

void UGameModesWidget::OnButtonClicked_SaveCustomAndStart()
{
	if (CheckForExistingAndDisplayOverwriteMessage(true))
	{
		return;
	}

	SaveCustomGameModeAndShowSavedText(GetCustomGameModeOptions());
	DefiningConfig->PopulateGameModeNameComboBoxAfterSave();
	ShowAudioFormatSelect(false);
}

void UGameModesWidget::OnButtonClicked_PlayFromStandard()
{
	ShowAudioFormatSelect(true);
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
	switch(InBSConfig.DefiningConfig.BaseGameMode)
	{
	case EDefaultMode::Custom:
		break;
	case EDefaultMode::SingleBeat:
		AIConfigBox->SetVisibility(ESlateVisibility::Visible);
		BeatGridBox->SetVisibility(ESlateVisibility::Collapsed);
		BeatTrackConfigBox->SetVisibility(ESlateVisibility::Collapsed);
		AIConfig->InitializeAIConfig(InBSConfig.AIConfig, InBSConfig.DefiningConfig.BaseGameMode);
		break;
	case EDefaultMode::MultiBeat:
		AIConfigBox->SetVisibility(ESlateVisibility::Visible);
		BeatGridBox->SetVisibility(ESlateVisibility::Collapsed);
		BeatTrackConfigBox->SetVisibility(ESlateVisibility::Collapsed);
		AIConfig->InitializeAIConfig(InBSConfig.AIConfig, InBSConfig.DefiningConfig.BaseGameMode);
		break;
	case EDefaultMode::BeatGrid:
		AIConfigBox->SetVisibility(ESlateVisibility::Collapsed);
		BeatGridBox->SetVisibility(ESlateVisibility::Visible);
		BeatTrackConfigBox->SetVisibility(ESlateVisibility::Collapsed);
		BeatGridConfig->InitializeBeatGrid(InBSConfig.BeatGridConfig, TargetConfig->TargetScaleConstrained->TextTooltipBox_Max);
		BeatGridConfig->OnBeatGridUpdate_MaxTargetScale(InBSConfig.TargetConfig.MaxTargetScale);
		break;
	case EDefaultMode::BeatTrack:
		AIConfigBox->SetVisibility(ESlateVisibility::Collapsed);
		BeatGridBox->SetVisibility(ESlateVisibility::Collapsed);
		BeatTrackConfigBox->SetVisibility(ESlateVisibility::Visible);
		BeatTrackConfig->InitializeBeatTrackConfig(InBSConfig.BeatTrackConfig, InBSConfig.DefiningConfig.BaseGameMode);
		break;
	default:
		break;
	}

	DefiningConfig->InitializeDefiningConfig(InBSConfig.DefiningConfig, InBSConfig.DefiningConfig.BaseGameMode);
	SpatialConfig->InitializeTargetSpread(InBSConfig.SpatialConfig, InBSConfig.DefiningConfig.BaseGameMode);
	TargetConfig->InitializeTargetConfig(InBSConfig.TargetConfig, InBSConfig.DefiningConfig.BaseGameMode);
}

FBSConfig UGameModesWidget::GetCustomGameModeOptions() const
{
	FBSConfig ReturnStruct;
	ReturnStruct.DefiningConfig = DefiningConfig->GetDefiningConfig();
	ReturnStruct.TargetConfig = TargetConfig->GetTargetConfig();
	ReturnStruct.AudioConfig.PlayerDelay = ReturnStruct.TargetConfig.SpawnBeatDelay;
	ReturnStruct.AIConfig = AIConfig->GetAIConfig();
	ReturnStruct.SpatialConfig = SpatialConfig->GetSpatialConfig();
	ReturnStruct.BeatGridConfig = BeatGridConfig->GetBeatGridConfig();
	ReturnStruct.BeatTrackConfig = BeatTrackConfig->GetBeatTrackConfig();
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
	const bool bIsBeatGridMode = DefiningConfig->ComboBox_BaseGameMode->GetSelectedOption().Equals(UEnum::GetDisplayValueAsText(EDefaultMode::BeatGrid).ToString());
	const bool bBeatGridIsConstrained = BeatGridConfig->IsAnyParameterConstrained();
	const bool bIsDefaultMode = IsDefaultGameMode(DefiningConfig->ComboBox_GameModeName->GetSelectedOption());
	const bool bIsCustomMode = IsCustomGameMode(DefiningConfig->ComboBox_GameModeName->GetSelectedOption());
	const bool bGameModeNameComboBoxEmpty = DefiningConfig->ComboBox_GameModeName->GetSelectedOption().IsEmpty();
	const bool bCustomTextEmpty = DefiningConfig->TextBox_CustomGameModeName->GetText().IsEmptyOrWhitespace();

	/* RemoveAll Button */
	if (bNoSavedCustomGameModes)
	{
		RemoveAllCustomButton->SetIsEnabled(false);
	}
	else
	{
		RemoveAllCustomButton->SetIsEnabled(true);
	}

	/* BeatGrid */
	if (bIsBeatGridMode && bBeatGridIsConstrained)
	{
		SaveCustomButton->SetIsEnabled(false);
		SaveCustomAndStartButton->SetIsEnabled(false);
		StartCustomButton->SetIsEnabled(false);
		StartWithoutSavingButton->SetIsEnabled(false);
		return;
	}
	StartWithoutSavingButton->SetIsEnabled(true);
	
	if (bGameModeNameComboBoxEmpty || (bIsDefaultMode && bCustomTextEmpty))
	{
		SaveCustomButton->SetIsEnabled(false);
		SaveCustomAndStartButton->SetIsEnabled(false);
		StartCustomButton->SetIsEnabled(false);
		RemoveSelectedCustomButton->SetIsEnabled(false);
		return;
	}

	if (bIsCustomMode)
	{
		SaveCustomButton->SetIsEnabled(true);
		SaveCustomAndStartButton->SetIsEnabled(true);
		StartCustomButton->SetIsEnabled(true);
		RemoveSelectedCustomButton->SetIsEnabled(true);
		return;
	}

	if (bIsDefaultMode && !bCustomTextEmpty)
	{
		SaveCustomButton->SetIsEnabled(true);
		SaveCustomAndStartButton->SetIsEnabled(true);
		StartCustomButton->SetIsEnabled(false);
		RemoveSelectedCustomButton->SetIsEnabled(false);
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
		PopupMessageWidget->Button1->OnClicked.AddDynamic(this, &UGameModesWidget::OnButtonClicked_ConfirmOverwriteAndStartGame);
	}
	else
	{
		PopupMessageWidget->Button1->OnClicked.AddDynamic(this, &UGameModesWidget::OnButtonClicked_ConfirmOverwrite);
	}
	PopupMessageWidget->Button2->OnClicked.AddDynamic(this, &UGameModesWidget::OnButtonClicked_CancelOverwrite);
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
			BSConfig = FBSConfig(DefaultMode, DefaultDifficulty, DefaultSpreadType);
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
		GameModeTransitionState.BSConfig = BSConfig;
		/* Override the player delay to zero if using Capture */
		if (BSConfig.AudioConfig.AudioFormat == EAudioFormat::Capture)
		{
			BSConfig.AudioConfig.PlayerDelay = 0.f;
			BSConfig.TargetConfig.SpawnBeatDelay = 0.f;
		}
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
	if (IsCustomGameMode(SelectedGameModeName) && CustomGameModeName.IsEmpty())
	{
		ShowConfirmOverwriteMessage(bStartGameAfter);
		return true;
	}
	return false;
}