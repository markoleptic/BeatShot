// Fill out your copyright notice in the Description page of Project Settings.


#include "SubMenuWidgets/GameModesWidget.h"
#include "JsonObjectConverter.h"
#include "SaveGameCustomGameMode.h"
#include "Components/WidgetSwitcher.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "Components/Slider.h"
#include "Components/Border.h"
#include "Components/CheckBox.h"
#include "Components/TextBlock.h"
#include "Components/EditableTextBox.h"
#include "Components/ComboBoxString.h"
#include "Components/HorizontalBox.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetStringLibrary.h"
#include "Misc/DefaultValueHelper.h"
#include "OverlayWidgets/PopupMessageWidget.h"
#include "WidgetComponents/ConstrainedSlider.h"
#include "WidgetComponents/GameModeButton.h"
#include "WidgetComponents/SavedTextWidget.h"
#include "WidgetComponents/SlideRightButton.h"
#include "WidgetComponents/TooltipWidget.h"

void UGameModesWidget::NativeConstruct()
{
	Super::NativeConstruct();

	/** Navigation widgets */
	{
		MenuWidgets.Add(DefaultGameModesButton, DefaultGameModes);
		MenuWidgets.Add(CustomGameModesButton, CustomGameModes);

		DefaultGameModesButton->Button->OnClicked.AddDynamic(this, &UGameModesWidget::OnDefaultGameModesButtonClicked);
		CustomGameModesButton->Button->OnClicked.AddDynamic(this, &UGameModesWidget::OnCustomGameModesButtonClicked);
		CustomizeFromStandardButton->OnClicked.
		                             AddDynamic(this, &UGameModesWidget::OnCustomizeFromStandardButtonClicked);
		PlayFromStandardButton->OnClicked.AddDynamic(this, &UGameModesWidget::OnPlayFromStandardButtonClicked);

		PlayFromStandardButton->SetIsEnabled(false);
		CustomizeFromStandardButton->SetIsEnabled(false);
	}

	/** Default Game Mode widgets */
	{
		BeatGridNormalButton->Difficulty = EGameModeDifficulty::Normal;
		BeatGridHardButton->Difficulty = EGameModeDifficulty::Hard;
		BeatGridDeathButton->Difficulty = EGameModeDifficulty::Death;
		BeatGridNormalButton->GameModeName = EGameModeActorName::BeatGrid;
		BeatGridHardButton->GameModeName = EGameModeActorName::BeatGrid;
		BeatGridDeathButton->GameModeName = EGameModeActorName::BeatGrid;
		BeatGridNormalButton->OnGameModeButtonClicked.AddDynamic(
			this, &UGameModesWidget::OnDefaultGameModeButtonClicked);
		BeatGridHardButton->OnGameModeButtonClicked.AddDynamic(this, &UGameModesWidget::OnDefaultGameModeButtonClicked);
		BeatGridDeathButton->OnGameModeButtonClicked.
		                     AddDynamic(this, &UGameModesWidget::OnDefaultGameModeButtonClicked);

		MultiBeatNormalButton->Difficulty = EGameModeDifficulty::Normal;
		MultiBeatHardButton->Difficulty = EGameModeDifficulty::Hard;
		MultiBeatDeathButton->Difficulty = EGameModeDifficulty::Death;
		MultiBeatNormalButton->GameModeName = EGameModeActorName::MultiBeat;
		MultiBeatHardButton->GameModeName = EGameModeActorName::MultiBeat;
		MultiBeatDeathButton->GameModeName = EGameModeActorName::MultiBeat;
		MultiBeatNormalButton->OnGameModeButtonClicked.AddDynamic(
			this, &UGameModesWidget::OnDefaultGameModeButtonClicked);
		MultiBeatHardButton->OnGameModeButtonClicked.
		                     AddDynamic(this, &UGameModesWidget::OnDefaultGameModeButtonClicked);
		MultiBeatDeathButton->OnGameModeButtonClicked.AddDynamic(
			this, &UGameModesWidget::OnDefaultGameModeButtonClicked);

		SingleBeatNormalButton->Difficulty = EGameModeDifficulty::Normal;
		SingleBeatHardButton->Difficulty = EGameModeDifficulty::Hard;
		SingleBeatDeathButton->Difficulty = EGameModeDifficulty::Death;
		SingleBeatNormalButton->GameModeName = EGameModeActorName::SingleBeat;
		SingleBeatHardButton->GameModeName = EGameModeActorName::SingleBeat;
		SingleBeatDeathButton->GameModeName = EGameModeActorName::SingleBeat;
		SingleBeatNormalButton->OnGameModeButtonClicked.AddDynamic(
			this, &UGameModesWidget::OnDefaultGameModeButtonClicked);
		SingleBeatHardButton->OnGameModeButtonClicked.AddDynamic(
			this, &UGameModesWidget::OnDefaultGameModeButtonClicked);
		SingleBeatDeathButton->OnGameModeButtonClicked.AddDynamic(
			this, &UGameModesWidget::OnDefaultGameModeButtonClicked);

		BeatTrackNormalButton->Difficulty = EGameModeDifficulty::Normal;
		BeatTrackHardButton->Difficulty = EGameModeDifficulty::Hard;
		BeatTrackDeathButton->Difficulty = EGameModeDifficulty::Death;
		BeatTrackNormalButton->GameModeName = EGameModeActorName::BeatTrack;
		BeatTrackHardButton->GameModeName = EGameModeActorName::BeatTrack;
		BeatTrackDeathButton->GameModeName = EGameModeActorName::BeatTrack;
		BeatTrackNormalButton->OnGameModeButtonClicked.AddDynamic(
			this, &UGameModesWidget::OnDefaultGameModeButtonClicked);
		BeatTrackHardButton->OnGameModeButtonClicked.
		                     AddDynamic(this, &UGameModesWidget::OnDefaultGameModeButtonClicked);
		BeatTrackDeathButton->OnGameModeButtonClicked.AddDynamic(
			this, &UGameModesWidget::OnDefaultGameModeButtonClicked);

		BeatGridNormalButton->Next = BeatGridHardButton;
		BeatGridHardButton->Next = BeatGridDeathButton;
		BeatGridDeathButton->Next = MultiBeatNormalButton;
		MultiBeatNormalButton->Next = MultiBeatHardButton;
		MultiBeatHardButton->Next = MultiBeatDeathButton;
		MultiBeatDeathButton->Next = SingleBeatNormalButton;
		SingleBeatNormalButton->Next = SingleBeatHardButton;
		SingleBeatHardButton->Next = SingleBeatDeathButton;
		SingleBeatDeathButton->Next = BeatTrackNormalButton;
		BeatTrackNormalButton->Next = BeatTrackHardButton;
		BeatTrackHardButton->Next = BeatTrackDeathButton;
		BeatTrackDeathButton->Next = BeatGridNormalButton;
	}

	/** Spread Type widgets */
	{
		SpreadSelect->SetVisibility(ESlateVisibility::Collapsed);
		DynamicSpreadButton->OnClicked.AddDynamic(this, &UGameModesWidget::OnDynamicSpreadButtonClicked);
		NarrowSpreadButton->OnClicked.AddDynamic(this, &UGameModesWidget::OnNarrowSpreadButtonClicked);
		WideSpreadButton->OnClicked.AddDynamic(this, &UGameModesWidget::OnWideSpreadButtonClicked);
	}

	/** Default GameModes Array */
	{
		GameModeActorDefaults.Add(
			FGameModeActorStruct(EGameModeActorName::BeatGrid, EGameModeDifficulty::Normal));
		GameModeActorDefaults.Add(
			FGameModeActorStruct(EGameModeActorName::BeatTrack, EGameModeDifficulty::Normal));
		GameModeActorDefaults.Add(FGameModeActorStruct(EGameModeActorName::SingleBeat,
		                                               EGameModeDifficulty::Normal,
		                                               ESpreadType::DynamicEdgeOnly));
		GameModeActorDefaults.Add(FGameModeActorStruct(EGameModeActorName::MultiBeat,
		                                               EGameModeDifficulty::Normal,
		                                               ESpreadType::DynamicRandom));
	}

	/** Save Start Custom Buttons */
	{
		SaveCustomButton->SetIsEnabled(false);
		SaveCustomAndStartButton->SetIsEnabled(false);
		StartCustomButton->SetIsEnabled(false);
		RemoveAllCustomButton->SetIsEnabled(false);
		RemoveSelectedCustomButton->SetIsEnabled(false);

		SaveCustomButton->OnClicked.AddDynamic(this, &UGameModesWidget::OnSaveCustomButtonClicked);
		SaveCustomAndStartButton->OnClicked.AddDynamic(this, &UGameModesWidget::OnSaveCustomAndStartButtonClicked);
		StartCustomButton->OnClicked.AddDynamic(this, &UGameModesWidget::OnStartCustomButtonClicked);
		StartWithoutSavingButton->OnClicked.AddDynamic(this, &UGameModesWidget::OnStartWithoutSavingButtonClicked);
		RemoveAllCustomButton->OnClicked.AddDynamic(this, &UGameModesWidget::OnRemoveAllCustomButtonClicked);
		RemoveSelectedCustomButton->OnClicked.AddDynamic(this, &UGameModesWidget::OnRemoveSelectedCustomButtonClicked);
	}

	/** Custom Game Mode Options */
	{
		/** Load Custom Game Modes and repopulate GameModeNameComboBox to include them */
		if (!LoadCustomGameModes().IsEmpty())
		{
			RemoveAllCustomButton->SetIsEnabled(true);
		}
		GameModeNameComboBox->OnSelectionChanged.AddDynamic(this, &UGameModesWidget::OnGameModeNameSelectionChange);
		CustomGameModeETB->OnTextChanged.AddDynamic(this, &UGameModesWidget::OnCustomGameModeETBChange);
		BaseGameModeComboBox->OnSelectionChanged.AddDynamic(this, &UGameModesWidget::OnBaseGameModeSelectionChange);
		GameModeDifficultyComboBox->OnSelectionChanged.AddDynamic(
			this, &UGameModesWidget::OnGameModeDifficultySelectionChange);
		PlayerDelaySlider->OnValueChanged.AddDynamic(this, &UGameModesWidget::OnPlayerDelaySliderChanged);
		PlayerDelayValue->OnTextCommitted.AddDynamic(this, &UGameModesWidget::OnPlayerDelayValueCommitted);
		LifespanSlider->OnValueChanged.AddDynamic(this, &UGameModesWidget::OnLifespanSliderChanged);
		LifespanValue->OnTextCommitted.AddDynamic(this, &UGameModesWidget::OnLifespanValueCommitted);
		TargetSpawnCDSlider->OnValueChanged.AddDynamic(this, &UGameModesWidget::OnTargetSpawnCDSliderChanged);
		TargetSpawnCDValue->OnTextCommitted.AddDynamic(this, &UGameModesWidget::OnTargetSpawnCDValueCommitted);
		HeadShotOnlyCheckBox->OnCheckStateChanged.AddDynamic(this, &UGameModesWidget::OnHeadShotOnlyCheckStateChanged);
		MinTargetDistanceSlider->OnValueChanged.AddDynamic(this, &UGameModesWidget::OnMinTargetDistanceSliderChanged);
		MinTargetDistanceValue->OnTextCommitted.AddDynamic(this, &UGameModesWidget::OnMinTargetDistanceValueCommitted);
		HorizontalSpreadSlider->OnValueChanged.AddDynamic(this, &UGameModesWidget::OnHorizontalSpreadSliderChanged);
		HorizontalSpreadValue->OnTextCommitted.AddDynamic(this, &UGameModesWidget::OnHorizontalSpreadValueCommitted);
		VerticalSpreadSlider->OnValueChanged.AddDynamic(this, &UGameModesWidget::OnVerticalSpreadSliderChanged);
		VerticalSpreadValue->OnTextCommitted.AddDynamic(this, &UGameModesWidget::OnVerticalSpreadValueCommitted);
		ForwardSpreadCheckBox->OnCheckStateChanged.AddDynamic(this, &UGameModesWidget::OnForwardSpreadCheckStateChanged);
		ForwardSpreadSlider->OnValueChanged.AddDynamic(this, &UGameModesWidget::OnForwardSpreadSliderChanged);
		ForwardSpreadValue->OnTextCommitted.AddDynamic(this, &UGameModesWidget::OnForwardSpreadValueCommitted);
		ConstantBeatGridSpacingCheckBox->OnCheckStateChanged.AddDynamic(
			this, &UGameModesWidget::OnConstantBeatGridSpacingCheckStateChanged);

		FConstrainedSliderStruct TargetScaleSliderStruct;
		TargetScaleSliderStruct.MinConstraintLower = MinTargetScaleValue;
		TargetScaleSliderStruct.MinConstraintUpper = MaxTargetScaleValue;
		TargetScaleSliderStruct.MaxConstraintLower = MinTargetScaleValue;
		TargetScaleSliderStruct.MaxConstraintUpper = MaxTargetScaleValue;
		TargetScaleSliderStruct.DefaultMinValue = MinTargetScaleValue;
		TargetScaleSliderStruct.DefaultMaxValue = MaxTargetScaleValue;
		TargetScaleSliderStruct.MaxText = FText::FromString("Max Target Scale");
		TargetScaleSliderStruct.MinText = FText::FromString("Min Target Scale");
		TargetScaleSliderStruct.CheckboxText = FText::FromString("Constant Target Size?");
		TargetScaleSliderStruct.bSyncSlidersAndValues = false;
		TargetScaleSliderStruct.GridSnapSize = TargetScaleSnapSize;
		TargetScaleConstrained->InitConstrainedSlider(TargetScaleSliderStruct);
	}

	/** BeatGrid Options */
	{
		//VerticalSpreadSlider->OnValueChanged.AddDynamic(this, &UGameModesWidget::BeatGridSpawnAreaConstrained);
		//HorizontalSpreadSlider->OnValueChanged.AddDynamic(this, &UGameModesWidget::BeatGridSpawnAreaConstrained);
		//BeatGridHorizontalSpacingSlider->OnValueChanged.AddDynamic(this, &UGameModesWidget::BeatGridSpacingConstrained);
		//BeatGridVerticalSpacingSlider->OnValueChanged.AddDynamic(this, &UGameModesWidget::BeatGridSpacingConstrained);
		//MinTargetScaleSlider->OnValueChanged.AddDynamic(this, &UGameModesWidget::BeatGridTargetSizeConstrained);
		//MaxTargetScaleSlider->OnValueChanged.AddDynamic(this, &UGameModesWidget::BeatGridTargetSizeConstrained);
		//NumBeatGridTargetsComboBox->OnSelectionChanged.AddDynamic(
		//	this, &UGameModesWidget::BeatGridNumberOfTargetsConstrained);
	}

	/** BeatTrack Options */
	{
		FConstrainedSliderStruct TrackingSpeedSliderStruct;
		TrackingSpeedSliderStruct.MinConstraintLower = MinTargetSpeedValue;
		TrackingSpeedSliderStruct.MinConstraintUpper = MaxTargetSpeedValue;
		TrackingSpeedSliderStruct.MaxConstraintLower = MinTargetSpeedValue;
		TrackingSpeedSliderStruct.MaxConstraintUpper = MaxTargetSpeedValue;
		TrackingSpeedSliderStruct.DefaultMinValue = MinTargetSpeedValue;
		TrackingSpeedSliderStruct.DefaultMaxValue = MaxTargetSpeedValue;
		TrackingSpeedSliderStruct.MaxText = FText::FromString("Max Tracking Speed");
		TrackingSpeedSliderStruct.MinText = FText::FromString("Min Tracking Speed");
		TrackingSpeedSliderStruct.CheckboxText = FText::FromString("Constant Tracking Speed?");
		TrackingSpeedSliderStruct.bSyncSlidersAndValues = false;
		TrackingSpeedSliderStruct.GridSnapSize = TargetSpeedSnapSize;
		TargetSpeedConstrained->InitConstrainedSlider(TrackingSpeedSliderStruct);
	}

	/** Tooltip */
	{
		Tooltip = CreateWidget<UTooltipWidget>(this, TooltipWidgetClass);

		GameModeTemplateQMark->TooltipText = FText::FromStringTable(
			"/Game/StringTables/ST_GameModesWidget.ST_GameModesWidget", "GameModeTemplate");
		CustomGameModeNameQMark->TooltipText = FText::FromStringTable(
			"/Game/StringTables/ST_GameModesWidget.ST_GameModesWidget", "CustomGameModeName");
		BaseGameModeQMark->TooltipText = FText::FromStringTable(
			"/Game/StringTables/ST_GameModesWidget.ST_GameModesWidget", "BaseGameMode");
		GameModeDifficultyQMark->TooltipText = FText::FromStringTable(
			"/Game/StringTables/ST_GameModesWidget.ST_GameModesWidget", "GameModeDifficulty");
		SpawnBeatDelayQMark->TooltipText = FText::FromStringTable(
			"/Game/StringTables/ST_GameModesWidget.ST_GameModesWidget", "SpawnBeatDelay");
		LifespanQMark->TooltipText = FText::FromStringTable(
			"/Game/StringTables/ST_GameModesWidget.ST_GameModesWidget", "Lifespan");
		TargetSpawnCDQMark->TooltipText = FText::FromStringTable(
			"/Game/StringTables/ST_GameModesWidget.ST_GameModesWidget", "MinDistance");
		HeadshotHeightQMark->TooltipText = FText::FromStringTable(
			"/Game/StringTables/ST_GameModesWidget.ST_GameModesWidget", "HeadshotHeight");
		ForwardSpreadQMark->TooltipText = FText::FromStringTable(
			"/Game/StringTables/ST_GameModesWidget.ST_GameModesWidget", "ForwardSpread");
		CenterTargetsQMark->TooltipText = FText::FromStringTable(
			"/Game/StringTables/ST_GameModesWidget.ST_GameModesWidget", "CenterTargets");
		MinDistanceQMark->TooltipText = FText::FromStringTable(
			"/Game/StringTables/ST_GameModesWidget.ST_GameModesWidget", "MinDistance");
		SpreadTypeQMark->TooltipText = FText::FromStringTable(
			"/Game/StringTables/ST_GameModesWidget.ST_GameModesWidget", "SpreadType");
		DynamicTargetScaleQMark->TooltipText = FText::FromStringTable(
			"/Game/StringTables/ST_GameModesWidget.ST_GameModesWidget", "DynamicTargetScale");
		BeatGridEvenSpacingQMark->TooltipText = FText::FromStringTable(
			"/Game/StringTables/ST_GameModesWidget.ST_GameModesWidget", "BeatGridEvenSpacing");
		BeatGridAdjacentOnlyQMark->TooltipText = FText::FromStringTable(
			"/Game/StringTables/ST_GameModesWidget.ST_GameModesWidget", "BeatGridAdjacentOnly");
		BeatGridNumTargetsQMark->TooltipText = FText::FromStringTable(
			"/Game/StringTables/ST_GameModesWidget.ST_GameModesWidget", "BeatGridNumTargets");
		TargetScaleConstrained->CheckboxQMark->TooltipText = FText::FromStringTable(
			"/Game/StringTables/ST_GameModesWidget.ST_GameModesWidget", "ConstantTargetScale");
		TargetSpeedConstrained->CheckboxQMark->TooltipText = FText::FromStringTable(
			"/Game/StringTables/ST_GameModesWidget.ST_GameModesWidget", "BeatTrackConstantSpeed");

		GameModeTemplateQMark->OnTooltipImageHovered.AddDynamic(this, &UGameModesWidget::OnTooltipImageHovered);
		CustomGameModeNameQMark->OnTooltipImageHovered.AddDynamic(this, &UGameModesWidget::OnTooltipImageHovered);
		BaseGameModeQMark->OnTooltipImageHovered.AddDynamic(this, &UGameModesWidget::OnTooltipImageHovered);
		GameModeDifficultyQMark->OnTooltipImageHovered.AddDynamic(this, &UGameModesWidget::OnTooltipImageHovered);
		SpawnBeatDelayQMark->OnTooltipImageHovered.AddDynamic(this, &UGameModesWidget::OnTooltipImageHovered);
		LifespanQMark->OnTooltipImageHovered.AddDynamic(this, &UGameModesWidget::OnTooltipImageHovered);
		TargetSpawnCDQMark->OnTooltipImageHovered.AddDynamic(this, &UGameModesWidget::OnTooltipImageHovered);
		CenterTargetsQMark->OnTooltipImageHovered.AddDynamic(this, &UGameModesWidget::OnTooltipImageHovered);
		HeadshotHeightQMark->OnTooltipImageHovered.AddDynamic(this, &UGameModesWidget::OnTooltipImageHovered);
		ForwardSpreadQMark->OnTooltipImageHovered.AddDynamic(this, &UGameModesWidget::OnTooltipImageHovered);
		MinDistanceQMark->OnTooltipImageHovered.AddDynamic(this, &UGameModesWidget::OnTooltipImageHovered);
		SpreadTypeQMark->OnTooltipImageHovered.AddDynamic(this, &UGameModesWidget::OnTooltipImageHovered);
		DynamicTargetScaleQMark->OnTooltipImageHovered.AddDynamic(this, &UGameModesWidget::OnTooltipImageHovered);
		BeatGridEvenSpacingQMark->OnTooltipImageHovered.AddDynamic(this, &UGameModesWidget::OnTooltipImageHovered);
		BeatGridAdjacentOnlyQMark->OnTooltipImageHovered.AddDynamic(this, &UGameModesWidget::OnTooltipImageHovered);
		BeatGridNumTargetsQMark->OnTooltipImageHovered.AddDynamic(this, &UGameModesWidget::OnTooltipImageHovered);
		TargetScaleConstrained->CheckboxQMark->OnTooltipImageHovered.AddDynamic(
			this, &UGameModesWidget::OnTooltipImageHovered);
		TargetSpeedConstrained->CheckboxQMark->OnTooltipImageHovered.AddDynamic(
			this, &UGameModesWidget::OnTooltipImageHovered);
	}

	OnDefaultGameModesButtonClicked();
	PopulateGameModeNameComboBox("");
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

void UGameModesWidget::OnDefaultGameModeButtonClicked(const UGameModeButton* GameModeButton)
{
	DefaultGameModeActorName = GameModeButton->GameModeName;
	DefaultGameModeDifficulty = GameModeButton->Difficulty;
	const FGameModeActorStruct SelectedGameMode = FGameModeActorStruct(GameModeButton->GameModeName,
	                                                                   GameModeButton->Difficulty);
	GameModeNameComboBox->SetSelectedOption(UEnum::GetDisplayValueAsText(GameModeButton->GameModeName).ToString());
	PopulateGameModeOptions(SelectedGameMode);
	SetGameModeButtonBackgroundColor(GameModeButton);
	DynamicSpreadButton->SetBackgroundColor(White);
	WideSpreadButton->SetBackgroundColor(White);
	NarrowSpreadButton->SetBackgroundColor(White);

	/** Don't show SpreadSelect if BeatGrid or BeatTrack */
	if (SelectedGameMode.GameModeActorName == EGameModeActorName::BeatGrid ||
		SelectedGameMode.GameModeActorName == EGameModeActorName::BeatTrack)
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

void UGameModesWidget::SetGameModeButtonBackgroundColor(const UGameModeButton* ClickedButton) const
{
	ClickedButton->Button->SetBackgroundColor(BeatshotBlue);
	const EGameModeDifficulty ClickedButtonDifficulty = ClickedButton->Difficulty;
	const EGameModeActorName ClickedButtonGameModeName = ClickedButton->GameModeName;
	const UGameModeButton* Head = ClickedButton->Next;

	/** Change all other button backgrounds to white */
	while (!(Head->Difficulty == ClickedButtonDifficulty && Head->GameModeName == ClickedButtonGameModeName))
	{
		Head->Button->SetBackgroundColor(White);
		Head = Head->Next;
	}
}

void UGameModesWidget::OnDynamicSpreadButtonClicked()
{
	/** Change the background colors of the spread select buttons */
	DynamicSpreadButton->SetBackgroundColor(BeatshotBlue);
	NarrowSpreadButton->SetBackgroundColor(White);
	WideSpreadButton->SetBackgroundColor(White);
	if (DefaultGameModeActorName == EGameModeActorName::MultiBeat)
	{
		DefaultGameModeSpreadType = ESpreadType::DynamicRandom;
	}
	else
	{
		DefaultGameModeSpreadType = ESpreadType::DynamicEdgeOnly;
	}
	PopulateGameModeOptions(FGameModeActorStruct(DefaultGameModeActorName,
	                                             DefaultGameModeDifficulty, DefaultGameModeSpreadType));
	PlayFromStandardButton->SetIsEnabled(true);
}

void UGameModesWidget::OnNarrowSpreadButtonClicked()
{
	/** Change the background colors of the spread select buttons */
	DynamicSpreadButton->SetBackgroundColor(White);
	NarrowSpreadButton->SetBackgroundColor(BeatshotBlue);
	WideSpreadButton->SetBackgroundColor(White);
	DefaultGameModeSpreadType = ESpreadType::StaticNarrow;
	PopulateGameModeOptions(FGameModeActorStruct(DefaultGameModeActorName,
	                                             DefaultGameModeDifficulty, ESpreadType::StaticNarrow));
	PlayFromStandardButton->SetIsEnabled(true);
}

void UGameModesWidget::OnWideSpreadButtonClicked()
{
	/** Change the background colors of the spread select buttons */
	DynamicSpreadButton->SetBackgroundColor(White);
	NarrowSpreadButton->SetBackgroundColor(White);
	WideSpreadButton->SetBackgroundColor(BeatshotBlue);
	DefaultGameModeSpreadType = ESpreadType::StaticWide;
	PopulateGameModeOptions(FGameModeActorStruct(DefaultGameModeActorName,
	                                             DefaultGameModeDifficulty, ESpreadType::StaticWide));
	PlayFromStandardButton->SetIsEnabled(true);
}

void UGameModesWidget::OnCustomizeFromStandardButtonClicked()
{
	OnCustomGameModesButtonClicked();
}


void UGameModesWidget::OnGameModeNameSelectionChange(const FString SelectedGameModeName,
                                                     const ESelectInfo::Type SelectionType)
{
	if (IsDefaultGameMode(SelectedGameModeName))
	{
		BaseGameModeBox->SetVisibility(ESlateVisibility::Collapsed);
		PopulateGameModeOptions(GetDefaultGameMode(SelectedGameModeName));
	}
	if (IsCustomGameMode(SelectedGameModeName))
	{
		BaseGameModeBox->SetVisibility(ESlateVisibility::Visible);
		PopulateGameModeOptions(GetCustomGameMode(SelectedGameModeName));
	}
	UpdateSaveStartButtonStates();
}

void UGameModesWidget::OnCustomGameModeETBChange(const FText& NewCustomGameModeText)
{
	UpdateSaveStartButtonStates();
}

void UGameModesWidget::OnBaseGameModeSelectionChange(const FString SelectedBaseGameMode,
                                                     const ESelectInfo::Type SelectionType)
{
	/** TODO: Is this necessary*/
	if (SelectionType != ESelectInfo::Type::Direct)
	{
		PopulateGameModeOptions(GetDefaultGameMode(SelectedBaseGameMode));
	}
}

void UGameModesWidget::OnGameModeDifficultySelectionChange(const FString SelectedDifficulty,
                                                           const ESelectInfo::Type SelectionType)
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
		const FGameModeActorStruct GameModeActorStruct = FGameModeActorStruct(
			GetDefaultGameMode(BaseGameModeComboBox->GetSelectedOption()).GameModeActorName, EnumDifficulty);
		PopulateGameModeOptions(GameModeActorStruct);
	}
}

void UGameModesWidget::OnPlayerDelaySliderChanged(const float NewPlayerDelay)
{
	OnSliderChanged(NewPlayerDelay, PlayerDelayValue, PlayerDelayGridSnapSize);
}

void UGameModesWidget::OnPlayerDelayValueCommitted(const FText& NewPlayerDelay, ETextCommit::Type CommitType)
{
	OnEditableTextBoxChanged(NewPlayerDelay, PlayerDelayValue, PlayerDelaySlider, PlayerDelayGridSnapSize, MinPlayerDelayValue,
						 MaxPlayerDelayValue);
}

void UGameModesWidget::OnLifespanSliderChanged(const float NewLifespan)
{
	OnSliderChanged(NewLifespan, LifespanValue, LifespanGridSnapSize);
}

void UGameModesWidget::OnLifespanValueCommitted(const FText& NewLifespan, ETextCommit::Type CommitType)
{
	OnEditableTextBoxChanged(NewLifespan, LifespanValue, LifespanSlider, LifespanGridSnapSize, MinLifespanValue,
	                         MaxLifespanValue);
}

void UGameModesWidget::OnTargetSpawnCDSliderChanged(const float NewTargetSpawnCD)
{
	OnSliderChanged(NewTargetSpawnCD, TargetSpawnCDValue, TargetSpawnCDGridSnapSize);
}

void UGameModesWidget::OnTargetSpawnCDValueCommitted(const FText& NewTargetSpawnCD, ETextCommit::Type CommitType)
{
	OnEditableTextBoxChanged(NewTargetSpawnCD, TargetSpawnCDValue, TargetSpawnCDSlider, TargetSpawnCDGridSnapSize,
	                         MinTargetSpawnCDValue, MaxTargetSpawnCDValue);
}

void UGameModesWidget::OnHeadShotOnlyCheckStateChanged(const bool bHeadshotOnly)
{
	if (bHeadshotOnly)
	{
		VerticalSpreadSlider->SetValue(0);
		VerticalSpreadValue->SetText(FText::AsNumber(0));
		VerticalSpreadSlider->SetLocked(true);
		VerticalSpreadValue->SetIsReadOnly(true);
		return;
	}

	VerticalSpreadSlider->SetLocked(false);
	VerticalSpreadValue->SetIsReadOnly(false);
	OnEditableTextBoxChanged(FText::AsNumber(MaxVerticalSpreadValue), VerticalSpreadValue, VerticalSpreadSlider,
	                         SpreadGridSnapSize, MinVerticalSpreadValue, MaxVerticalSpreadValue);
	OnSliderChanged(MaxVerticalSpreadValue, VerticalSpreadValue, SpreadGridSnapSize);
}

void UGameModesWidget::OnMinTargetDistanceSliderChanged(const float NewMinTargetDistance)
{
	OnSliderChanged(NewMinTargetDistance, MinTargetDistanceValue, MinTargetDistanceGridSnapSize);
}

void UGameModesWidget::OnMinTargetDistanceValueCommitted(const FText& NewMinTargetDistance,
                                                         ETextCommit::Type CommitType)
{
	OnEditableTextBoxChanged(NewMinTargetDistance, MinTargetDistanceValue, MinTargetDistanceSlider,
	                         MinTargetDistanceGridSnapSize, MinMinTargetDistanceValue, MaxMinTargetDistanceValue);
}

void UGameModesWidget::OnHorizontalSpreadSliderChanged(const float NewHorizontalSpread)
{
	OnSliderChanged(NewHorizontalSpread, HorizontalSpreadValue, SpreadGridSnapSize);
}

void UGameModesWidget::OnHorizontalSpreadValueCommitted(const FText& NewHorizontalSpread, ETextCommit::Type CommitType)
{
	OnEditableTextBoxChanged(NewHorizontalSpread, HorizontalSpreadValue, HorizontalSpreadSlider, SpreadGridSnapSize,
	                         MinHorizontalSpreadValue, MaxHorizontalSpreadValue);
}

void UGameModesWidget::OnVerticalSpreadSliderChanged(const float NewVerticalSpread)
{
	OnSliderChanged(NewVerticalSpread, VerticalSpreadValue, SpreadGridSnapSize);
}

void UGameModesWidget::OnVerticalSpreadValueCommitted(const FText& NewVerticalSpread, ETextCommit::Type CommitType)
{
	OnEditableTextBoxChanged(NewVerticalSpread, VerticalSpreadValue, VerticalSpreadSlider, SpreadGridSnapSize,
	                         MinVerticalSpreadValue, MaxVerticalSpreadValue);
}

void UGameModesWidget::OnForwardSpreadCheckStateChanged(const bool bUseForwardSpread)
{
	if (bUseForwardSpread)
	{
		ForwardSpreadBox->SetVisibility(ESlateVisibility::Visible);
		return;
	}
	ForwardSpreadBox->SetVisibility(ESlateVisibility::Collapsed);
}

void UGameModesWidget::OnForwardSpreadSliderChanged(const float NewForwardSpread)
{
	OnSliderChanged(NewForwardSpread, ForwardSpreadValue, SpreadGridSnapSize);
}

void UGameModesWidget::OnForwardSpreadValueCommitted(const FText& NewForwardSpread, ETextCommit::Type CommitType)
{
	OnEditableTextBoxChanged(NewForwardSpread, ForwardSpreadValue, ForwardSpreadSlider, SpreadGridSnapSize,
						 MinForwardSpreadValue, MaxForwardSpreadValue);
}

void UGameModesWidget::OnConstantBeatGridSpacingCheckStateChanged(const bool bConstantBeatGridSpacing)
{
	/** TODO: Constrain BeatGridSpacing */
}

void UGameModesWidget::BeatGridSpacingConstrained(float Value)
{
	if (BaseGameModeComboBox->GetSelectedOption() != "BeatGrid")
	{
		return;
	}

	const float Width = round(HorizontalSpreadSlider->GetValue());
	const float TargetWidth = ceil(TargetScaleConstrained->MaxSlider->GetValue() * SphereDiameter * 100) / 100;
	const float HSpacing = BeatGridHorizontalSpacingSlider->GetValue();
	const float Height = round(VerticalSpreadSlider->GetValue());
	const float TargetHeight = ceil(TargetScaleConstrained->MaxSlider->GetValue() * SphereDiameter * 100) / 100;
	const float VSpacing = BeatGridVerticalSpacingSlider->GetValue();
	const FString ToConvert = NumBeatGridTargetsComboBox->GetSelectedOption();
	int32 MaxTargets;
	FDefaultValueHelper::ParseInt(ToConvert, MaxTargets);
	MaxTargets = sqrt(MaxTargets);

	if (HSpacing >= (Width - TargetWidth * MaxTargets - 200) / (MaxTargets - 1))
	{
		UE_LOG(LogTemp, Display, TEXT("Spacing width contraint %f"), HSpacing);
	}
	if (VSpacing >= (Height - TargetHeight * MaxTargets - 200) / (MaxTargets - 1))
	{
		UE_LOG(LogTemp, Display, TEXT("Spacing height contraint %f"), VSpacing);
	}
}

void UGameModesWidget::BeatGridTargetSizeConstrained(float Value)
{
	if (BaseGameModeComboBox->GetSelectedOption() != "Beat Grid")
	{
		return;
	}
	const float Width = round(HorizontalSpreadSlider->GetValue());
	const float TargetWidth = ceil(TargetScaleConstrained->MaxSlider->GetValue() * SphereDiameter * 100) / 100;
	const float HSpacing = BeatGridHorizontalSpacingSlider->GetValue();
	const float Height = round(VerticalSpreadSlider->GetValue());
	const float TargetHeight = ceil(TargetScaleConstrained->MaxSlider->GetValue() * SphereDiameter * 100) / 100;
	const float VSpacing = BeatGridVerticalSpacingSlider->GetValue();
	const FString ToConvert = NumBeatGridTargetsComboBox->GetSelectedOption();
	int32 MaxTargets;
	FDefaultValueHelper::ParseInt(ToConvert, MaxTargets);
	MaxTargets = sqrt(MaxTargets);

	if (TargetWidth >= (Width - 200 - HSpacing * MaxTargets + HSpacing) / MaxTargets)
	{
		UE_LOG(LogTemp, Display, TEXT("Scale width contraint %f"), TargetWidth);
	}
	if (TargetHeight >= (Height - 200 - VSpacing * MaxTargets + VSpacing) / MaxTargets)
	{
		UE_LOG(LogTemp, Display, TEXT("Scale height contraint %f"), TargetHeight);
	}
}

void UGameModesWidget::BeatGridSpawnAreaConstrained(float Value)
{
	if (BaseGameModeComboBox->GetSelectedOption() != "Beat Grid")
	{
		return;
	}

	const float Width = round(HorizontalSpreadSlider->GetValue());
	const float TargetWidth = ceil(TargetScaleConstrained->MaxSlider->GetValue() * SphereDiameter * 100) / 100;
	const float HSpacing = BeatGridHorizontalSpacingSlider->GetValue();
	const float Height = round(VerticalSpreadSlider->GetValue());
	const float TargetHeight = ceil(TargetScaleConstrained->MaxSlider->GetValue() * SphereDiameter * 100) / 100;
	const float VSpacing = BeatGridVerticalSpacingSlider->GetValue();
	const FString ToConvert = NumBeatGridTargetsComboBox->GetSelectedOption();
	int32 MaxTargets;
	FDefaultValueHelper::ParseInt(ToConvert, MaxTargets);
	MaxTargets = sqrt(MaxTargets);

	if (Width <= TargetWidth * MaxTargets + HSpacing * MaxTargets - HSpacing + 200)
	{
		UE_LOG(LogTemp, Display, TEXT("Width contraint"));
	}
	if (Height <= TargetHeight * MaxTargets + VSpacing * MaxTargets - VSpacing + 200)
	{
		UE_LOG(LogTemp, Display, TEXT("Height contraint"));
	}
}

void UGameModesWidget::BeatGridNumberOfTargetsConstrained(FString SelectedSong, ESelectInfo::Type SelectionType)
{
	if (BaseGameModeComboBox->GetSelectedOption() != "Beat Grid")
	{
		return;
	}

	const float Width = round(HorizontalSpreadSlider->GetValue());
	const float TargetWidth = ceil(TargetScaleConstrained->MaxSlider->GetValue() * SphereDiameter * 100) / 100;
	const float HSpacing = BeatGridHorizontalSpacingSlider->GetValue();
	const float Height = round(VerticalSpreadSlider->GetValue());
	const float TargetHeight = ceil(TargetScaleConstrained->MaxSlider->GetValue() * SphereDiameter * 100) / 100;
	const float VSpacing = BeatGridVerticalSpacingSlider->GetValue();
	const FString ToConvert = NumBeatGridTargetsComboBox->GetSelectedOption();
	int32 MaxTargets;
	FDefaultValueHelper::ParseInt(ToConvert, MaxTargets);
	MaxTargets = sqrt(MaxTargets);

	// WidthORHeight = TargetScale*MaxTargets + Spacing*MaxTargets - Spacing + 200

	if (MaxTargets >= (Width - 200 - HSpacing * MaxTargets + HSpacing) / TargetWidth)
	{
		UE_LOG(LogTemp, Display, TEXT("MaxTargets height contraint"));
	}
	if (MaxTargets >= (Height - 200 - VSpacing * MaxTargets + VSpacing) / TargetHeight)
	{
		UE_LOG(LogTemp, Display, TEXT("MaxTargets width contraint"));
	}
}

bool UGameModesWidget::CheckAllBeatGridConstraints()
{
	if (BaseGameModeComboBox->GetSelectedOption() != "BeatGrid")
	{
		return true;
	}

	const float Width = round(HorizontalSpreadSlider->GetValue());
	const float TargetWidth = ceil(TargetScaleConstrained->MaxSlider->GetValue() * SphereDiameter * 100) / 100;
	const float HSpacing = BeatGridHorizontalSpacingSlider->GetValue();
	const float Height = round(VerticalSpreadSlider->GetValue());
	const float TargetHeight = ceil(TargetScaleConstrained->MaxSlider->GetValue() * SphereDiameter * 100) / 100;
	const float VSpacing = BeatGridVerticalSpacingSlider->GetValue();
	const FString ToConvert = NumBeatGridTargetsComboBox->GetSelectedOption();
	int32 MaxTargets;
	FDefaultValueHelper::ParseInt(ToConvert, MaxTargets);
	MaxTargets = sqrt(MaxTargets);

	if (
		TargetWidth >= ((Width - 200 - HSpacing * MaxTargets + HSpacing) / MaxTargets) ||
		TargetHeight >= ((Height - 200 - VSpacing * MaxTargets + VSpacing) / MaxTargets) ||
		Width <= (TargetWidth * MaxTargets + HSpacing * MaxTargets - HSpacing + 200) ||
		Height <= (TargetHeight * MaxTargets + VSpacing * MaxTargets - VSpacing + 200) ||
		MaxTargets >= ((Width - 200 - HSpacing * MaxTargets + HSpacing) / TargetWidth) ||
		MaxTargets >= ((Height - 200 - VSpacing * MaxTargets + VSpacing) / TargetHeight) ||
		HSpacing >= ((Width - TargetWidth * MaxTargets - 200) / (MaxTargets - 1)) ||
		VSpacing >= ((Height - TargetHeight * MaxTargets - 200) / (MaxTargets - 1))
	)
	{
		return false;
	}
	return true;
}

void UGameModesWidget::PopulateGameModeOptions(const FGameModeActorStruct& InputGameModeActorStruct)
{
	if (InputGameModeActorStruct.IsBeatGridMode)
	{
		BeatGridSpecificSettings->SetVisibility(ESlateVisibility::Visible);
		BaseGameModeComboBox->SetSelectedOption("BeatGrid");
		ConstantBeatGridSpacingCheckBox->SetIsChecked(true);
		RandomizeNextBeatGridTargetCheckBox->SetIsChecked(InputGameModeActorStruct.RandomizeBeatGrid);
		NumBeatGridTargetsComboBox->SetSelectedOption(
			FString::FromInt(InputGameModeActorStruct.NumTargetsAtOnceBeatGrid));
	}
	else
	{
		BeatGridSpecificSettings->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (InputGameModeActorStruct.IsBeatTrackMode)
	{
		BeatTrackSpecificSettings->SetVisibility(ESlateVisibility::Visible);
		BaseGameModeComboBox->SetSelectedOption("BeatTrack");
		LifespanSlider->SetLocked(true);
		LifespanValue->SetIsReadOnly(true);
		MinTargetDistanceBox->SetVisibility(ESlateVisibility::Collapsed);
		SpreadTypeBox->SetVisibility(ESlateVisibility::Collapsed);
		TargetSpeedConstrained->UpdateDefaultValues(InputGameModeActorStruct.MinTrackingSpeed,
		                                            InputGameModeActorStruct.MaxTrackingSpeed);
	}
	else
	{
		BeatTrackSpecificSettings->SetVisibility(ESlateVisibility::Collapsed);
		LifespanSlider->SetLocked(false);
		LifespanValue->SetIsReadOnly(false);
		MinTargetDistanceBox->SetVisibility(ESlateVisibility::Visible);
		SpreadTypeBox->SetVisibility(ESlateVisibility::Visible);
	}


	if (InputGameModeActorStruct.IsSingleBeatMode)
	{
		BaseGameModeComboBox->SetSelectedOption("SingleBeat");
	}
	if (!InputGameModeActorStruct.IsBeatGridMode &&
		!InputGameModeActorStruct.IsBeatTrackMode &&
		!InputGameModeActorStruct.IsSingleBeatMode)
	{
		BaseGameModeComboBox->SetSelectedOption("MultiBeat");
		LifespanSlider->SetLocked(false);
		LifespanValue->SetIsReadOnly(false);
	}

	GameModeDifficultyComboBox->SetSelectedOption(
		UEnum::GetDisplayValueAsText(InputGameModeActorStruct.GameModeDifficulty).ToString());
	PlayerDelaySlider->SetValue(InputGameModeActorStruct.PlayerDelay);
	PlayerDelayValue->SetText(FText::AsNumber(InputGameModeActorStruct.PlayerDelay));
	LifespanSlider->SetValue(InputGameModeActorStruct.TargetMaxLifeSpan);
	LifespanValue->SetText(FText::AsNumber(InputGameModeActorStruct.TargetMaxLifeSpan));
	TargetSpawnCDSlider->SetValue(InputGameModeActorStruct.TargetSpawnCD);
	TargetSpawnCDValue->SetText(FText::AsNumber(InputGameModeActorStruct.TargetSpawnCD));
	HeadShotOnlyCheckBox->SetIsChecked(InputGameModeActorStruct.HeadshotHeight);
	WallCenteredCheckBox->SetIsChecked(InputGameModeActorStruct.WallCentered);
	MinTargetDistanceSlider->SetValue(InputGameModeActorStruct.MinDistanceBetweenTargets);
	MinTargetDistanceValue->SetText(FText::AsNumber(InputGameModeActorStruct.MinDistanceBetweenTargets));
	SpreadTypeComboBox->SetSelectedOption(UEnum::GetDisplayValueAsText(InputGameModeActorStruct.SpreadType).ToString());
	HorizontalSpreadSlider->SetValue(InputGameModeActorStruct.BoxBounds.Y);
	HorizontalSpreadValue->SetText(FText::AsNumber(InputGameModeActorStruct.BoxBounds.Y));
	VerticalSpreadSlider->SetValue(InputGameModeActorStruct.BoxBounds.Z);
	VerticalSpreadValue->SetText(FText::AsNumber(InputGameModeActorStruct.BoxBounds.Z));
	ForwardSpreadCheckBox->SetIsChecked(InputGameModeActorStruct.bMoveTargetsForward);
	ForwardSpreadSlider->SetValue(InputGameModeActorStruct.MoveForwardDistance);
	ForwardSpreadValue->SetText(FText::AsNumber(InputGameModeActorStruct.MoveForwardDistance));
	TargetScaleConstrained->UpdateDefaultValues(InputGameModeActorStruct.MinTargetScale,
	                                            InputGameModeActorStruct.MaxTargetScale);

	// convert JsonScores struct to JSON
	const TSharedRef<FJsonObject> OutJsonObject = MakeShareable(new FJsonObject);
	FJsonObjectConverter::UStructToJsonObject(
		FGameModeActorStruct::StaticStruct(),
		&InputGameModeActorStruct,
		OutJsonObject);
	FString OutputString;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(OutJsonObject, Writer);
	//UE_LOG(LogTemp, Display, TEXT("%s"), *OutputString);
}

void UGameModesWidget::PopulateGameModeNameComboBox(const FString& GameModeOptionToSelect)
{
	GameModeNameComboBox->ClearOptions();
	TArray<FGameModeActorStruct> CustomGameModesArray = LoadCustomGameModes();
	for (const FGameModeActorStruct GameMode : GameModeActorDefaults)
	{
		GameModeNameComboBox->AddOption(UEnum::GetDisplayValueAsText(GameMode.GameModeActorName).ToString());
	}
	for (const FGameModeActorStruct CustomGameMode : CustomGameModesArray)
	{
		GameModeNameComboBox->AddOption(CustomGameMode.CustomGameModeName);
	}
	if (GameModeOptionToSelect.IsEmpty())
	{
		GameModeNameComboBox->ClearSelection();
		return;
	}
	GameModeNameComboBox->SetSelectedOption(GameModeOptionToSelect);
}

FGameModeActorStruct UGameModesWidget::GetCustomGameModeOptions()
{
	if (IsDefaultGameMode(GameModeNameComboBox->GetSelectedOption()) && CustomGameModeETB->GetText().IsEmpty())
	{
		return FGameModeActorStruct();
	}
	FGameModeActorStruct ReturnStruct;
	ReturnStruct.GameModeActorName = EGameModeActorName::Custom;
	if (!CustomGameModeETB->GetText().IsEmptyOrWhitespace())
	{
		ReturnStruct.CustomGameModeName = CustomGameModeETB->GetText().ToString();
	}
	else
	{
		ReturnStruct.CustomGameModeName = GameModeNameComboBox->GetSelectedOption();
	}

	if (BaseGameModeComboBox->GetSelectedOption().Equals("BeatGrid"))
	{
		ReturnStruct.IsBeatGridMode = true;
		ReturnStruct.IsBeatTrackMode = false;
		ReturnStruct.IsSingleBeatMode = false;
	}
	else if (BaseGameModeComboBox->GetSelectedOption().Equals("BeatTrack"))
	{
		ReturnStruct.IsBeatGridMode = false;
		ReturnStruct.IsBeatTrackMode = true;
		ReturnStruct.IsSingleBeatMode = false;
	}
	else if (BaseGameModeComboBox->GetSelectedOption().Equals("SingleBeat"))
	{
		ReturnStruct.IsBeatGridMode = false;
		ReturnStruct.IsBeatTrackMode = false;
		ReturnStruct.IsSingleBeatMode = true;
	}
	else
	{
		ReturnStruct.IsBeatGridMode = false;
		ReturnStruct.IsBeatTrackMode = false;
		ReturnStruct.IsSingleBeatMode = false;
	}
	ReturnStruct.GameModeDifficulty = EGameModeDifficulty::None;
	ReturnStruct.PlayerDelay = FMath::GridSnap(
		FMath::Clamp(PlayerDelaySlider->GetValue(), MinPlayerDelayValue, MaxPlayerDelayValue), PlayerDelayGridSnapSize);
	ReturnStruct.TargetMaxLifeSpan = FMath::GridSnap(
		FMath::Clamp(LifespanSlider->GetValue(), MinLifespanValue, MaxLifespanValue), LifespanGridSnapSize);
	ReturnStruct.TargetSpawnCD = FMath::GridSnap(
		FMath::Clamp(TargetSpawnCDSlider->GetValue(), MinTargetSpawnCDValue, MaxTargetSpawnCDValue),
		TargetSpawnCDGridSnapSize);
	ReturnStruct.HeadshotHeight = HeadShotOnlyCheckBox->IsChecked();
	ReturnStruct.WallCentered = WallCenteredCheckBox->IsChecked();
	ReturnStruct.MinDistanceBetweenTargets = FMath::GridSnap(
		FMath::Clamp(MinTargetDistanceSlider->GetValue(), MinMinTargetDistanceValue, MaxMinTargetDistanceValue),
		MinTargetDistanceGridSnapSize);
	ReturnStruct.SpreadType = GetSpreadType();
	ReturnStruct.BoxBounds = FVector(0,
	                                 FMath::GridSnap(
		                                 FMath::Clamp(HorizontalSpreadSlider->GetValue(), MinHorizontalSpreadValue,
		                                              MaxHorizontalSpreadValue), SpreadGridSnapSize),
	                                 FMath::GridSnap(
		                                 FMath::Clamp(VerticalSpreadSlider->GetValue(), MinVerticalSpreadValue,
		                                              MaxVerticalSpreadValue), SpreadGridSnapSize));
	ReturnStruct.bMoveTargetsForward = ForwardSpreadCheckBox->IsChecked();
	ReturnStruct.MoveForwardDistance = FMath::GridSnap(FMath::Clamp(ForwardSpreadSlider->GetValue(), MinForwardSpreadValue, MaxForwardSpreadValue), SpreadGridSnapSize);
	ReturnStruct.UseDynamicSizing = DynamicTargetScaleCheckBox->IsChecked();
	ReturnStruct.MinTargetScale = FMath::GridSnap(
		FMath::Clamp(TargetScaleConstrained->MinSlider->GetValue(), MinTargetScaleValue, MaxTargetScaleValue),
		TargetScaleSnapSize);
	ReturnStruct.MaxTargetScale = FMath::GridSnap(
		FMath::Clamp(TargetScaleConstrained->MaxSlider->GetValue(), MinTargetScaleValue, MaxTargetScaleValue),
		TargetScaleSnapSize);
	ReturnStruct.RandomizeBeatGrid = RandomizeNextBeatGridTargetCheckBox->IsChecked();
	/** TODO: Constant BeatGrid Spacing option*/
	ReturnStruct.NumTargetsAtOnceBeatGrid = FCString::Atoi(*NumBeatGridTargetsComboBox->GetSelectedOption());
	ReturnStruct.MinTrackingSpeed = FMath::GridSnap(
		FMath::Clamp(TargetSpeedConstrained->MinSlider->GetValue(), MinTargetSpeedValue, MaxTargetSpeedValue),
		TargetSpeedSnapSize);
	ReturnStruct.MaxTrackingSpeed = FMath::GridSnap(
		FMath::Clamp(TargetSpeedConstrained->MaxSlider->GetValue(), MinTargetSpeedValue, MaxTargetSpeedValue),
		TargetSpeedSnapSize);
	return ReturnStruct;
}

void UGameModesWidget::SaveCustomGameModeToSlot(const FGameModeActorStruct GameModeToSave) const
{
	TArray<FGameModeActorStruct> CustomGameModesArray = ISaveLoadInterface::LoadCustomGameModes();
	const int32 NumRemoved = CustomGameModesArray.RemoveAll(
		[&GameModeToSave](const FGameModeActorStruct& MatchingStruct)
		{
			return MatchingStruct.CustomGameModeName == GameModeToSave.CustomGameModeName;
		});
	UE_LOG(LogTemp, Display, TEXT("NumRemoved %d"), NumRemoved);
	CustomGameModesArray.Shrink();
	CustomGameModesArray.Add(GameModeToSave);
	if (USaveGameCustomGameMode* SaveCustomGameModeObject = Cast<USaveGameCustomGameMode>(
		UGameplayStatics::CreateSaveGameObject(USaveGameCustomGameMode::StaticClass())))
	{
		SaveCustomGameModeObject->CustomGameModes = CustomGameModesArray;
		UGameplayStatics::SaveGameToSlot(SaveCustomGameModeObject, TEXT("CustomGameModesSlot"), 3);
	}
		SavedTextWidget->SetSavedText(FText::FromString(GameModeToSave.CustomGameModeName + " saved"));
		SavedTextWidget->PlayFadeInFadeOut();
}

void UGameModesWidget::RemoveCustomGameMode(const FString& CustomGameModeName)
{
	TArray<FGameModeActorStruct> CustomGameModesArray = ISaveLoadInterface::LoadCustomGameModes();
	const int32 NumRemoved = CustomGameModesArray.RemoveAll(
	[&CustomGameModeName](const FGameModeActorStruct& MatchingStruct)
	{
		return MatchingStruct.CustomGameModeName == CustomGameModeName;
	});
	CustomGameModesArray.Shrink();
	SaveCustomGameMode(CustomGameModesArray);
	if (NumRemoved >= 1)
	{
		SavedTextWidget->SetSavedText(FText::FromString(CustomGameModeName + " removed"));
		SavedTextWidget->PlayFadeInFadeOut();
	}
}

void UGameModesWidget::RemoveAllCustomGameModes()
{
	TArray<FGameModeActorStruct> CustomGameModesArray = LoadCustomGameModes();
	CustomGameModesArray.Empty();
	CustomGameModesArray.Shrink();
	SaveCustomGameMode(CustomGameModesArray);
	SavedTextWidget->SetSavedText(FText::FromString("All custom game modes removed"));
	SavedTextWidget->PlayFadeInFadeOut();
}

void UGameModesWidget::UpdateSaveStartButtonStates()
{
	if (LoadCustomGameModes().IsEmpty())
	{
		RemoveAllCustomButton->SetIsEnabled(false);
	}
	else
	{
		RemoveAllCustomButton->SetIsEnabled(true);
	}
	if (GameModeNameComboBox->GetSelectedOption().IsEmpty())
	{
		SaveCustomButton->SetIsEnabled(false);
		SaveCustomAndStartButton->SetIsEnabled(false);
		StartCustomButton->SetIsEnabled(false);
		SaveCustomAndStartButton->SetIsEnabled(false);
		RemoveSelectedCustomButton->SetIsEnabled(false);
		return;
	}
	if (IsDefaultGameMode(GameModeNameComboBox->GetSelectedOption()) && CustomGameModeETB->GetText().
		IsEmptyOrWhitespace())
	{
		SaveCustomButton->SetIsEnabled(false);
		SaveCustomAndStartButton->SetIsEnabled(false);
		StartCustomButton->SetIsEnabled(false);
		SaveCustomAndStartButton->SetIsEnabled(false);
		RemoveSelectedCustomButton->SetIsEnabled(false);
		return;
	}

	if (IsCustomGameMode(GameModeNameComboBox->GetSelectedOption()))
	{
		SaveCustomButton->SetIsEnabled(true);
		SaveCustomAndStartButton->SetIsEnabled(true);
		StartCustomButton->SetIsEnabled(true);
		SaveCustomAndStartButton->SetIsEnabled(true);
		RemoveSelectedCustomButton->SetIsEnabled(true);
		return;
	}

	if (IsDefaultGameMode(GameModeNameComboBox->GetSelectedOption()) && !CustomGameModeETB->GetText().
		IsEmptyOrWhitespace())
	{
		SaveCustomButton->SetIsEnabled(true);
		SaveCustomAndStartButton->SetIsEnabled(true);
		StartCustomButton->SetIsEnabled(false);
		SaveCustomAndStartButton->SetIsEnabled(true);
		RemoveSelectedCustomButton->SetIsEnabled(false);
	}
}

void UGameModesWidget::OnSaveCustomButtonClicked()
{
	const FString SelectedGameModeName = GameModeNameComboBox->GetSelectedOption();
	const FString CustomGameModeName = CustomGameModeETB->GetText().ToString();
	if (IsCustomGameMode(SelectedGameModeName) && CustomGameModeName.IsEmpty())
	{
		ShowConfirmOverwriteMessage(false);
		return;
	}
	SaveCustomGameModeToSlot(GetCustomGameModeOptions());
	/** If user is saving custom game mode based on DefaultGameMode from GameModeNameComboBox */
	if (IsDefaultGameMode(SelectedGameModeName))
	{
		PopulateGameModeNameComboBox(CustomGameModeName);
	}
	/** If user is saving custom game mode based on CustomGameMode from GameModeNameComboBox */
	else
	{
		PopulateGameModeNameComboBox(SelectedGameModeName);
	}
	CustomGameModeETB->SetText(FText::GetEmpty());
}

void UGameModesWidget::OnStartWithoutSavingButtonClicked()
{
	InitializeExit(false);
}

void UGameModesWidget::OnSaveCustomAndStartButtonClicked()
{
	const FString SelectedGameModeName = GameModeNameComboBox->GetSelectedOption();
	const FString CustomGameModeName = CustomGameModeETB->GetText().ToString();
	if (IsCustomGameMode(SelectedGameModeName) && CustomGameModeName.IsEmpty())
	{
		ShowConfirmOverwriteMessage(true);
		return;
	}
	SaveCustomGameModeToSlot(GetCustomGameModeOptions());
	/** If user is saving custom game mode based on DefaultGameMode from GameModeNameComboBox */
	if (IsDefaultGameMode(SelectedGameModeName))
	{
		PopulateGameModeNameComboBox(CustomGameModeName);
	}
	/** If user is saving custom game mode based on CustomGameMode from GameModeNameComboBox */
	else
	{
		PopulateGameModeNameComboBox(SelectedGameModeName);
	}
	CustomGameModeETB->SetText(FText::GetEmpty());
	InitializeExit(false);
}

void UGameModesWidget::OnPlayFromStandardButtonClicked()
{
	InitializeExit(true);
}

void UGameModesWidget::OnStartCustomButtonClicked()
{
	InitializeExit(false);
}

void UGameModesWidget::OnRemoveSelectedCustomButtonClicked()
{
	if (IsCustomGameMode(GameModeNameComboBox->GetSelectedOption()))
	{
		RemoveCustomGameMode(GameModeNameComboBox->GetSelectedOption());
		PopulateGameModeNameComboBox("");
	}
}

void UGameModesWidget::OnRemoveAllCustomButtonClicked()
{
	RemoveAllCustomGameModes();
	PopulateGameModeNameComboBox("");
}

void UGameModesWidget::ShowConfirmOverwriteMessage(const bool bStartGameAfter)
{
	PopupMessageWidget = CreateWidget<UPopupMessageWidget>(this, PopupMessageClass);
	if (PopupMessageWidget)
	{
		PopupMessageWidget->InitPopup("Overwrite Existing Game Mode?",
							  "",
							  "Confirm", "Cancel");
		PopupMessageWidget->AddToViewport();
		PopupMessageWidget->FadeIn();
	}

	if (bStartGameAfter)
	{
		PopupMessageWidget->Button1->OnClicked.AddDynamic(
			this, &UGameModesWidget::OnConfirmOverwriteButtonClickedAndStartGame);
	}
	else
	{
		PopupMessageWidget->Button1->OnClicked.AddDynamic(
			this, &UGameModesWidget::OnConfirmOverwriteButtonClicked);
	}
	PopupMessageWidget->Button2->OnClicked.AddDynamic(
		this, &UGameModesWidget::OnCancelOverwriteButtonClicked);
}

void UGameModesWidget::OnConfirmOverwriteButtonClicked()
{
	PopupMessageWidget->FadeOut();
	SaveCustomGameModeToSlot(GetCustomGameModeOptions());
}

void UGameModesWidget::OnConfirmOverwriteButtonClickedAndStartGame()
{
	PopupMessageWidget->FadeOut();
	const FString SelectedGameModeName = GameModeNameComboBox->GetSelectedOption();
	const FString CustomGameModeName = CustomGameModeETB->GetText().ToString();
	SaveCustomGameModeToSlot(GetCustomGameModeOptions());
	/** If user is saving custom game mode based on DefaultGameMode from GameModeNameComboBox */
	if (IsDefaultGameMode(SelectedGameModeName))
	{
		PopulateGameModeNameComboBox(CustomGameModeName);
	}
	/** If user is saving custom game mode based on CustomGameMode from GameModeNameComboBox */
	else
	{
		PopulateGameModeNameComboBox(SelectedGameModeName);
	}
	CustomGameModeETB->SetText(FText::GetEmpty());
	InitializeExit(false);
}

void UGameModesWidget::OnCancelOverwriteButtonClicked()
{
	PopupMessageWidget->FadeOut();
}

void UGameModesWidget::InitializeExit(const bool bStartFromDefaultGameMode)
{
	if (bStartFromDefaultGameMode)
	{
		OnGameModeSelected.ExecuteIfBound(FGameModeActorStruct(DefaultGameModeActorName, DefaultGameModeDifficulty, DefaultGameModeSpreadType));
	}
	else
	{
		OnGameModeSelected.ExecuteIfBound(GetCustomGameModeOptions());
	}
	if (!OnStartGameMode.ExecuteIfBound())
	{
		UE_LOG(LogTemp, Display, TEXT("StartGameMode not bound."));
	}
}

void UGameModesWidget::OnTooltipImageHovered(UTooltipImage* HoveredTooltipImage, const FText& TooltipTextToShow)
{
	Tooltip->TooltipDescriptor->SetText(TooltipTextToShow);
	HoveredTooltipImage->SetToolTip(Tooltip);
}

ESpreadType UGameModesWidget::GetSpreadType() const
{
	const FString SelectedSpread = SpreadTypeComboBox->GetSelectedOption();
	if (SelectedSpread.IsEmpty())
	{
		return ESpreadType::None;
	}
	for (const ESpreadType Spread : TEnumRange<ESpreadType>())
	{
		if (SelectedSpread.Equals(UEnum::GetDisplayValueAsText(Spread).ToString()))
		{
			return Spread;
		}
	}
	return ESpreadType::None;
}

FGameModeActorStruct UGameModesWidget::GetDefaultGameMode(const FString& GameModeName) const
{
	for (const FGameModeActorStruct GameModeActor : GameModeActorDefaults)
	{
		if (GameModeName.Equals(UEnum::GetDisplayValueAsText(GameModeActor.GameModeActorName).ToString()))
		{
			return GameModeActor;
		}
	}
	return FGameModeActorStruct(EGameModeActorName::Custom, EGameModeDifficulty::Normal);
}

FGameModeActorStruct UGameModesWidget::GetCustomGameMode(const FString& CustomGameModeName) const
{
	TArray<FGameModeActorStruct> CustomGameModesArray = LoadCustomGameModes();
	for (const FGameModeActorStruct Elem : CustomGameModesArray)
	{
		if (Elem.CustomGameModeName.Equals(CustomGameModeName))
		{
			return Elem;
		}
	}
	return FGameModeActorStruct(EGameModeActorName::Custom, EGameModeDifficulty::Normal);
}

bool UGameModesWidget::IsDefaultGameMode(const FString& GameModeName) const
{
	for (const FGameModeActorStruct GameModeActor : GameModeActorDefaults)
	{
		if (GameModeName.Equals(UEnum::GetDisplayValueAsText(GameModeActor.GameModeActorName).ToString()))
		{
			return true;
		}
	}
	return false;
}

bool UGameModesWidget::IsCustomGameMode(const FString& GameModeName) const
{
	if (IsDefaultGameMode(GameModeName))
	{
		return false;
	}
	TArray<FGameModeActorStruct> CustomGameModesArray = LoadCustomGameModes();
	for (const FGameModeActorStruct GameMode : CustomGameModesArray)
	{
		if (GameMode.CustomGameModeName.Equals(GameModeName))
		{
			return true;
		}
	}
	return false;
}

void UGameModesWidget::OnEditableTextBoxChanged(const FText& NewTextValue, UEditableTextBox* TextBoxToChange,
                                                USlider* SliderToChange,
                                                const float GridSnapSize, const float Min, const float Max)
{
	const FString StringTextValue = UKismetStringLibrary::Replace(NewTextValue.ToString(), ",", "");
	const float ClampedValue = FMath::Clamp(FCString::Atof(*StringTextValue), Min, Max);
	const float SnappedValue = FMath::GridSnap(ClampedValue, GridSnapSize);
	TextBoxToChange->SetText(FText::AsNumber(SnappedValue));
	SliderToChange->SetValue(SnappedValue);
}

void UGameModesWidget::OnSliderChanged(const float NewValue, UEditableTextBox* TextBoxToChange,
                                       const float GridSnapSize)
{
	TextBoxToChange->SetText(FText::AsNumber(FMath::GridSnap(NewValue, GridSnapSize)));
}
