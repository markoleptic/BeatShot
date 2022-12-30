// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModesWidget.h"
#include "DefaultGameInstance.h"
#include "DefaultGameMode.h"
#include "DefaultPlayerController.h"
#include "GameModeButton.h"
#include "JsonObjectConverter.h"
#include "SaveGameCustomGameMode.h"
#include "SlideRightButton.h"
#include "TooltipWidget.h"
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
#include "Misc/DefaultValueHelper.h"

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
			GameModeActorStructConstructor(EGameModeActorName::BeatGrid, EGameModeDifficulty::Normal));
		GameModeActorDefaults.Add(
			GameModeActorStructConstructor(EGameModeActorName::BeatTrack, EGameModeDifficulty::Normal));
		GameModeActorDefaults.Add(GameModeActorStructConstructor(EGameModeActorName::SingleBeat,
		                                                         EGameModeDifficulty::Normal,
		                                                         ESpreadType::DynamicEdgeOnly));
		GameModeActorDefaults.Add(GameModeActorStructConstructor(EGameModeActorName::MultiBeat,
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
		PlayerDelayComboBox->OnSelectionChanged.AddDynamic(this, &UGameModesWidget::OnPlayerDelaySelectionChanged);
		LifespanSlider->OnValueChanged.AddDynamic(this, &UGameModesWidget::OnLifespanSliderChanged);
		LifespanValue->OnTextCommitted.AddDynamic(this, &UGameModesWidget::OnLifespanValueCommitted);
		TargetSpawnCDSlider->OnValueChanged.AddDynamic(this, &UGameModesWidget::OnTargetSpawnCDSliderChanged);
		TargetSpawnCDValue->OnTextCommitted.AddDynamic(this, &UGameModesWidget::OnTargetSpawnCDValueCommitted);
		HeadShotOnlyCheckBox->OnCheckStateChanged.AddDynamic(this, &UGameModesWidget::OnHeadShotOnlyCheckStateChanged);
		WallCenteredCheckBox->OnCheckStateChanged.AddDynamic(this, &UGameModesWidget::OnWallCenteredCheckStateChanged);
		MinTargetDistanceSlider->OnValueChanged.AddDynamic(this, &UGameModesWidget::OnMinTargetDistanceSliderChanged);
		MinTargetDistanceValue->OnTextCommitted.AddDynamic(this, &UGameModesWidget::OnMinTargetDistanceValueCommitted);
		SpreadTypeComboBox->OnSelectionChanged.AddDynamic(this, &UGameModesWidget::OnSpreadTypeSelectionChanged);
		HorizontalSpreadSlider->OnValueChanged.AddDynamic(this, &UGameModesWidget::OnHorizontalSpreadSliderChanged);
		HorizontalSpreadValue->OnTextCommitted.AddDynamic(this, &UGameModesWidget::OnHorizontalSpreadValueCommitted);
		VerticalSpreadSlider->OnValueChanged.AddDynamic(this, &UGameModesWidget::OnVerticalSpreadSliderChanged);
		VerticalSpreadValue->OnTextCommitted.AddDynamic(this, &UGameModesWidget::OnVerticalSpreadValueCommitted);
		DynamicTargetScaleCheckBox->OnCheckStateChanged.AddDynamic(
			this, &UGameModesWidget::OnDynamicTargetScaleCheckStateChanged);
		ConstantBeatGridSpacingCheckBox->OnCheckStateChanged.AddDynamic(
			this, &UGameModesWidget::OnConstantBeatGridSpacingCheckStateChanged);
		RandomizeNextBeatGridTargetCheckBox->OnCheckStateChanged.AddDynamic(
			this, &UGameModesWidget::OnRandomizeNextBeatGridTargetCheckStateChanged);
		NumBeatGridTargetsComboBox->OnSelectionChanged.AddDynamic(
			this, &UGameModesWidget::OnNumBeatGridTargetsSelectionChanged);

		FConstrainedSliderStruct TargetScaleSliderStruct;
		TargetScaleSliderStruct.MinConstraintLower = 0.1;
		TargetScaleSliderStruct.MinConstraintUpper = 2.0;
		TargetScaleSliderStruct.MaxConstraintLower = 0.1;
		TargetScaleSliderStruct.MaxConstraintUpper = 2.0;
		TargetScaleSliderStruct.DefaultMinValue = 1;
		TargetScaleSliderStruct.DefaultMaxValue = 1;
		TargetScaleSliderStruct.MaxText = FText::FromString("Max Target Scale");
		TargetScaleSliderStruct.MinText = FText::FromString("Min Target Scale");
		TargetScaleSliderStruct.CheckboxText = FText::FromString("Constant Target Size?");
		TargetScaleSliderStruct.bSyncSlidersAndValues = false;
		TargetScaleSliderStruct.GridSnapSize = 0.01;
		TargetScaleConstrained->InitConstrainedSlider(TargetScaleSliderStruct);
		TargetScaleConstrained->OnMinValueChanged.BindUFunction(this, FName("OnMinTargetScaleConstrainedChange"));
		TargetScaleConstrained->OnMaxValueChanged.BindUFunction(this, FName("OnMaxTargetScaleConstrainedChange"));
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
		TrackingSpeedSliderStruct.MinConstraintLower = 300;
		TrackingSpeedSliderStruct.MinConstraintUpper = 1000;
		TrackingSpeedSliderStruct.MaxConstraintLower = 300;
		TrackingSpeedSliderStruct.MaxConstraintUpper = 1000;
		TrackingSpeedSliderStruct.DefaultMinValue = 1;
		TrackingSpeedSliderStruct.DefaultMaxValue = 1;
		TrackingSpeedSliderStruct.MaxText = FText::FromString("Max Tracking Speed");
		TrackingSpeedSliderStruct.MinText = FText::FromString("Min Tracking Speed");
		TrackingSpeedSliderStruct.CheckboxText = FText::FromString("Constant Tracking Speed?");
		TrackingSpeedSliderStruct.bSyncSlidersAndValues = false;
		TrackingSpeedSliderStruct.GridSnapSize = 10;
		TargetSpeedConstrained->InitConstrainedSlider(TrackingSpeedSliderStruct);
		TargetSpeedConstrained->OnMinValueChanged.BindUFunction(this, FName("OnMinTargetSpeedConstrainedChange"));
		TargetSpeedConstrained->OnMaxValueChanged.BindUFunction(this, FName("OnMaxTargetSpeedConstrainedChange"));
	}

	/** Tooltip */
	{
		Tooltip = CreateWidget<UTooltipWidget>(this, TooltipWidgetClass);

		GameModeTemplateQMark->TooltipText = FText::FromStringTable(
			"/Game/UI/StringTables/ST_GameModesWidget.ST_GameModesWidget", "GameModeTemplate");
		CustomGameModeNameQMark->TooltipText = FText::FromStringTable(
			"/Game/UI/StringTables/ST_GameModesWidget.ST_GameModesWidget", "CustomGameModeName");
		BaseGameModeQMark->TooltipText = FText::FromStringTable(
			"/Game/UI/StringTables/ST_GameModesWidget.ST_GameModesWidget", "BaseGameMode");
		GameModeDifficultyQMark->TooltipText = FText::FromStringTable(
			"/Game/UI/StringTables/ST_GameModesWidget.ST_GameModesWidget", "GameModeDifficulty");
		SpawnBeatDelayQMark->TooltipText = FText::FromStringTable(
			"/Game/UI/StringTables/ST_GameModesWidget.ST_GameModesWidget", "SpawnBeatDelay");
		LifespanQMark->TooltipText = FText::FromStringTable(
			"/Game/UI/StringTables/ST_GameModesWidget.ST_GameModesWidget", "Lifespan");
		TargetSpawnCDQMark->TooltipText = FText::FromStringTable(
			"/Game/UI/StringTables/ST_GameModesWidget.ST_GameModesWidget", "MinDistance");
		HeadshotHeightQMark->TooltipText = FText::FromStringTable(
			"/Game/UI/StringTables/ST_GameModesWidget.ST_GameModesWidget", "HeadshotHeight");
		CenterTargetsQMark->TooltipText = FText::FromStringTable(
			"/Game/UI/StringTables/ST_GameModesWidget.ST_GameModesWidget", "CenterTargets");
		MinDistanceQMark->TooltipText = FText::FromStringTable(
			"/Game/UI/StringTables/ST_GameModesWidget.ST_GameModesWidget", "MinDistance");
		SpreadTypeQMark->TooltipText = FText::FromStringTable(
			"/Game/UI/StringTables/ST_GameModesWidget.ST_GameModesWidget", "SpreadType");
		DynamicTargetScaleQMark->TooltipText = FText::FromStringTable(
			"/Game/UI/StringTables/ST_GameModesWidget.ST_GameModesWidget", "DynamicTargetScale");
		BeatGridEvenSpacingQMark->TooltipText = FText::FromStringTable(
			"/Game/UI/StringTables/ST_GameModesWidget.ST_GameModesWidget", "BeatGridEvenSpacing");
		BeatGridAdjacentOnlyQMark->TooltipText = FText::FromStringTable(
			"/Game/UI/StringTables/ST_GameModesWidget.ST_GameModesWidget", "BeatGridAdjacentOnly");
		BeatGridNumTargetsQMark->TooltipText = FText::FromStringTable(
			"/Game/UI/StringTables/ST_GameModesWidget.ST_GameModesWidget", "BeatGridNumTargets");
		TargetScaleConstrained->CheckboxQMark->TooltipText = FText::FromStringTable(
			"/Game/UI/StringTables/ST_GameModesWidget.ST_GameModesWidget", "ConstantTargetScale");
		TargetSpeedConstrained->CheckboxQMark->TooltipText = FText::FromStringTable(
			"/Game/UI/StringTables/ST_GameModesWidget.ST_GameModesWidget", "BeatTrackConstantSpeed");

		GameModeTemplateQMark->OnTooltipImageHovered.AddDynamic(this, &UGameModesWidget::OnTooltipImageHovered);
		CustomGameModeNameQMark->OnTooltipImageHovered.AddDynamic(this, &UGameModesWidget::OnTooltipImageHovered);
		BaseGameModeQMark->OnTooltipImageHovered.AddDynamic(this, &UGameModesWidget::OnTooltipImageHovered);
		GameModeDifficultyQMark->OnTooltipImageHovered.AddDynamic(this, &UGameModesWidget::OnTooltipImageHovered);
		SpawnBeatDelayQMark->OnTooltipImageHovered.AddDynamic(this, &UGameModesWidget::OnTooltipImageHovered);
		LifespanQMark->OnTooltipImageHovered.AddDynamic(this, &UGameModesWidget::OnTooltipImageHovered);
		TargetSpawnCDQMark->OnTooltipImageHovered.AddDynamic(this, &UGameModesWidget::OnTooltipImageHovered);
		CenterTargetsQMark->OnTooltipImageHovered.AddDynamic(this, &UGameModesWidget::OnTooltipImageHovered);
		HeadshotHeightQMark->OnTooltipImageHovered.AddDynamic(this, &UGameModesWidget::OnTooltipImageHovered);
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
	PopulateGameModeNameComboBox("MultiBeat");
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

#pragma region DefaultGameModes

void UGameModesWidget::OnDefaultGameModeButtonClicked(UGameModeButton* GameModeButton)
{
	DefaultGameModeActorName = GameModeButton->GameModeName;
	DefaultGameModeDifficulty = GameModeButton->Difficulty;
	FGameModeActorStruct SelectedGameMode = GameModeActorStructConstructor(GameModeButton->GameModeName, GameModeButton->Difficulty);
	PopulateGameModeOptions(GameModeActorStructConstructor(GameModeButton->GameModeName, GameModeButton->Difficulty));
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

void UGameModesWidget::SetGameModeButtonBackgroundColor(const UGameModeButton* ClickedButton)
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
	ESpreadType SpreadType;
	if (DefaultGameModeActorName == EGameModeActorName::MultiBeat)
	{
		SpreadType = ESpreadType::DynamicRandom;
	}
	else
	{
		SpreadType = ESpreadType::DynamicEdgeOnly;
	}
	PopulateGameModeOptions(GameModeActorStructConstructor(DefaultGameModeActorName,
													  DefaultGameModeDifficulty, SpreadType));
	PlayFromStandardButton->SetIsEnabled(true);
}

void UGameModesWidget::OnNarrowSpreadButtonClicked()
{
	/** Change the background colors of the spread select buttons */
	DynamicSpreadButton->SetBackgroundColor(White);
	NarrowSpreadButton->SetBackgroundColor(BeatshotBlue);
	WideSpreadButton->SetBackgroundColor(White);
	//SelectedGameMode = GameModeActorStructConstructor(SelectedGameMode.GameModeActorName,
	//                                                  SelectedGameMode.GameModeDifficulty, ESpreadType::StaticNarrow);
	PopulateGameModeOptions(GameModeActorStructConstructor(DefaultGameModeActorName,
													  DefaultGameModeDifficulty, ESpreadType::StaticNarrow));
	PlayFromStandardButton->SetIsEnabled(true);
}

void UGameModesWidget::OnWideSpreadButtonClicked()
{
	/** Change the background colors of the spread select buttons */
	DynamicSpreadButton->SetBackgroundColor(White);
	NarrowSpreadButton->SetBackgroundColor(White);
	WideSpreadButton->SetBackgroundColor(BeatshotBlue);
	//SelectedGameMode = GameModeActorStructConstructor(SelectedGameMode.GameModeActorName,
	//                                                  SelectedGameMode.GameModeDifficulty, ESpreadType::StaticWide);
	PopulateGameModeOptions(GameModeActorStructConstructor(DefaultGameModeActorName,
													  DefaultGameModeDifficulty, ESpreadType::StaticWide));
	PlayFromStandardButton->SetIsEnabled(true);
}

void UGameModesWidget::OnCustomizeFromStandardButtonClicked()
{
	OnCustomGameModesButtonClicked();
}

void UGameModesWidget::OnPlayFromStandardButtonClicked()
{
	InitializeExit();
}

#pragma endregion

#pragma region CustomGeneral

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
	UpdateSaveStartButtonStates(SelectedGameModeName);
	if (IsCustomGameMode(SelectedGameModeName))
	{
		//SelectedGameMode.CustomGameModeName = SelectedGameModeName;
	}
}

void UGameModesWidget::OnCustomGameModeETBChange(const FText& NewCustomGameModeText)
{
	UpdateSaveStartButtonStates(NewCustomGameModeText.ToString());
	if (const FString NewCustomGameModeName = NewCustomGameModeText.ToString(); !IsDefaultGameMode(
		NewCustomGameModeName))
	{
		//SelectedGameMode.CustomGameModeName = NewCustomGameModeName;
	}
}

void UGameModesWidget::OnBaseGameModeSelectionChange(const FString SelectedBaseGameMode,
                                                     const ESelectInfo::Type SelectionType)
{
	/*if (SelectedBaseGameMode.Equals("BeatGrid"))
	{
		SelectedGameMode.IsBeatGridMode = true;
		SelectedGameMode.IsSingleBeatMode = false;
		SelectedGameMode.IsBeatTrackMode = false;
	}
	else if (SelectedBaseGameMode.Equals("BeatTrack"))
	{
		SelectedGameMode.IsBeatTrackMode = true;
		SelectedGameMode.IsBeatGridMode = false;
		SelectedGameMode.IsSingleBeatMode = false;
	}
	else if (SelectedBaseGameMode.Equals("SingleBeat"))
	{
		SelectedGameMode.IsSingleBeatMode = true;
		SelectedGameMode.IsBeatGridMode = false;
		SelectedGameMode.IsBeatTrackMode = false;
	}
	else
	{
		SelectedGameMode.IsBeatGridMode = false;
		SelectedGameMode.IsBeatTrackMode = false;
		SelectedGameMode.IsSingleBeatMode = false;
	}*/

	/** TODO: Is this necessary*/
	if (SelectionType != ESelectInfo::Type::Direct)
	{
		PopulateGameModeOptions(GetDefaultGameMode(SelectedBaseGameMode));
	}
}

void UGameModesWidget::OnGameModeDifficultySelectionChange(const FString SelectedDifficulty,
                                                           const ESelectInfo::Type SelectionType)
{
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
		/*const FGameModeActorStruct GameModeActorStruct =  GameModeActorStructConstructor(GetDefaultGameMode(BaseGameModeComboBox->GetSelectedOption()).GameModeActorName, EnumDifficulty);
		SelectedGameMode.GameModeDifficulty = GameModeActorStruct.GameModeDifficulty;
		SelectedGameMode.PlayerDelay = GameModeActorStruct.PlayerDelay;
		SelectedGameMode.TargetSpawnCD = GameModeActorStruct.TargetSpawnCD;
		SelectedGameMode.TargetMaxLifeSpan = GameModeActorStruct.TargetMaxLifeSpan;
		SelectedGameMode.MinTargetScale = GameModeActorStruct.MinTargetScale;
		SelectedGameMode.MaxTargetScale = GameModeActorStruct.MaxTargetScale;
		SelectedGameMode.MinTrackingSpeed = GameModeActorStruct.MinTrackingSpeed;
		SelectedGameMode.MaxTrackingSpeed = GameModeActorStruct.MaxTrackingSpeed;
		SelectedGameMode.BeatGridSize = GameModeActorStruct.BeatGridSize;*/
	}
	if (SelectionType != ESelectInfo::Type::Direct)
	{
		//PopulateGameModeOptions(SelectedGameMode);
	}
}

#pragma endregion

#pragma region Update

void UGameModesWidget::PopulateGameModeOptions(const FGameModeActorStruct& InputGameModeActorStruct)
{
	if (InputGameModeActorStruct.IsBeatGridMode)
	{
		BaseGameModeComboBox->SetSelectedOption("BeatGrid");
	}
	else if (InputGameModeActorStruct.IsBeatTrackMode)
	{
		BaseGameModeComboBox->SetSelectedOption("BeatTrack");
	}
	else if (InputGameModeActorStruct.IsSingleBeatMode)
	{
		BaseGameModeComboBox->SetSelectedOption("SingleBeat");
	}
	else
	{
		BaseGameModeComboBox->SetSelectedOption("MultiBeat");
	}

	GameModeDifficultyComboBox->SetSelectedOption(
		UEnum::GetDisplayValueAsText(InputGameModeActorStruct.GameModeDifficulty).ToString());

	/** Match player delay with options from ComboBox */
	if (InputGameModeActorStruct.PlayerDelay <= 0.f)
	{
		PlayerDelayComboBox->SetSelectedOption("No Delay");
	}
	else if (FString::SanitizeFloat(InputGameModeActorStruct.PlayerDelay).Len() == 3)
	{
		PlayerDelayComboBox->SetSelectedOption(FString::SanitizeFloat(InputGameModeActorStruct.PlayerDelay) + "0s");
	}
	else
	{
		PlayerDelayComboBox->SetSelectedOption(FString::SanitizeFloat(InputGameModeActorStruct.PlayerDelay) + "s");
	}

	PlayerDelayComboBox->SetSelectedOption(FString::SanitizeFloat(InputGameModeActorStruct.PlayerDelay) + "s");
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

	if (InputGameModeActorStruct.GameModeActorName == EGameModeActorName::BeatGrid)
	{
		BeatGridSpecificSettings->SetVisibility(ESlateVisibility::Visible);
		ConstantBeatGridSpacingCheckBox->SetIsChecked(true);
		RandomizeNextBeatGridTargetCheckBox->SetIsChecked(InputGameModeActorStruct.RandomizeBeatGrid);
		NumBeatGridTargetsComboBox->SetSelectedOption(
			FString::FromInt(InputGameModeActorStruct.NumTargetsAtOnceBeatGrid));
	}
	else
	{
		BeatGridSpecificSettings->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (InputGameModeActorStruct.GameModeActorName == EGameModeActorName::BeatTrack)
	{
		BeatTrackSpecificSettings->SetVisibility(ESlateVisibility::Visible);
		TargetSpeedConstrained->UpdateDefaultValues(InputGameModeActorStruct.MinTrackingSpeed,
		                                            InputGameModeActorStruct.MaxTrackingSpeed);
	}
	else
	{
		BeatTrackSpecificSettings->SetVisibility(ESlateVisibility::Collapsed);
	}

	TargetScaleConstrained->UpdateDefaultValues(InputGameModeActorStruct.MinTargetScale,
	                                            InputGameModeActorStruct.MaxTargetScale);

	// convert JsonScores struct to JSON
	/*const TSharedRef<FJsonObject> OutJsonObject = MakeShareable(new FJsonObject);
	FJsonObjectConverter::UStructToJsonObject(
		FGameModeActorStruct::StaticStruct(),
		&SelectedGameMode,
		OutJsonObject);
	FString OutputString;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(OutJsonObject, Writer);
	UE_LOG(LogTemp, Display, TEXT("%s"), *OutputString);*/
}

void UGameModesWidget::PopulateGameModeNameComboBox(const FString& GameModeOptionToSelect)
{
	GameModeNameComboBox->ClearOptions();
	TArray<FGameModeActorStruct> CustomGameModesArray = LoadCustomGameModes();
	for (const FGameModeActorStruct& GameMode : GameModeActorDefaults)
	{
		GameModeNameComboBox->AddOption(UEnum::GetDisplayValueAsText(GameMode.GameModeActorName).ToString());
	}
	for (const FGameModeActorStruct& CustomGameMode : CustomGameModesArray)
	{
		GameModeNameComboBox->AddOption(CustomGameMode.CustomGameModeName);
	}
	GameModeNameComboBox->SetSelectedOption(GameModeOptionToSelect);
}

FGameModeActorStruct UGameModesWidget::GetCustomGameModeOptions()
{
	if (IsDefaultGameMode(GameModeNameComboBox->GetSelectedOption()) && CustomGameModeETB->GetText().IsEmpty())
	{
		FGameModeActorStruct();
	}
	FGameModeActorStruct Gmas;
	Gmas.GameModeActorName = EGameModeActorName::Custom;
	if (IsCustomGameMode(GameModeNameComboBox->GetSelectedOption()))
	{
		Gmas.CustomGameModeName = GameModeNameComboBox->GetSelectedOption();
	}
	else
	{
		Gmas.CustomGameModeName = CustomGameModeETB->GetText().ToString();
	}
	if (BaseGameModeComboBox->GetSelectedOption().Equals("BeatGrid"))
	{
		Gmas.IsBeatGridMode = true;
		Gmas.IsBeatTrackMode = false;
		Gmas.IsSingleBeatMode = false;
	}
	else if (BaseGameModeComboBox->GetSelectedOption().Equals("BeatTrack"))
	{
		Gmas.IsBeatGridMode = false;
		Gmas.IsBeatTrackMode = true;
		Gmas.IsSingleBeatMode = false;
	}
	else if (BaseGameModeComboBox->GetSelectedOption().Equals("SingleBeat"))
	{
		Gmas.IsBeatGridMode = false;
		Gmas.IsBeatTrackMode = false;
		Gmas.IsSingleBeatMode = true;
	}
	else
	{
		Gmas.IsBeatGridMode = false;
		Gmas.IsBeatTrackMode = false;
		Gmas.IsSingleBeatMode = false;
	}
	Gmas.GameModeDifficulty = EGameModeDifficulty::None;
	if (PlayerDelayComboBox->GetSelectedOption().Equals("No Delay"))
	{
		Gmas.PlayerDelay = 0.f;
	}
	else
	{
		Gmas.PlayerDelay = FCString::Atof(*PlayerDelayComboBox->GetSelectedOption().LeftChop(1));
	}
	Gmas.TargetMaxLifeSpan = LifespanSlider->GetValue();
	Gmas.TargetSpawnCD = TargetSpawnCDSlider->GetValue();
	Gmas.HeadshotHeight = HeadShotOnlyCheckBox->IsChecked();
	Gmas.WallCentered = WallCenteredCheckBox->IsChecked();
	Gmas.MinDistanceBetweenTargets = MinTargetDistanceSlider->GetValue();
	Gmas.SpreadType = GetSpreadType();
	Gmas.BoxBounds = FVector(0, HorizontalSpreadSlider->GetValue(), VerticalSpreadSlider->GetValue());
	Gmas.UseDynamicSizing = DynamicTargetScaleCheckBox->IsChecked();
	Gmas.MinTargetScale = TargetScaleConstrained->MinSlider->GetValue();
	Gmas.MaxTargetScale = TargetScaleConstrained->MaxSlider->GetValue();
	Gmas.RandomizeBeatGrid = RandomizeNextBeatGridTargetCheckBox->IsChecked();
	/** TODO: Constant BeatGrid Spacing option*/
	Gmas.NumTargetsAtOnceBeatGrid = FCString::Atoi(*NumBeatGridTargetsComboBox->GetSelectedOption());
	Gmas.MinTrackingSpeed = TargetSpeedConstrained->MinSlider->GetValue();
	Gmas.MaxTrackingSpeed = TargetSpeedConstrained->MaxSlider->GetValue();
	return Gmas;
}

void UGameModesWidget::OnMinTargetSpeedConstrainedChange(const float NewMin)
{
	//SelectedGameMode.MinTrackingSpeed = NewMin;
}

void UGameModesWidget::OnMaxTargetSpeedConstrainedChange(const float NewMax)
{
	//SelectedGameMode.MaxTrackingSpeed = NewMax;
}

void UGameModesWidget::OnMinTargetScaleConstrainedChange(const float NewMin)
{
	//SelectedGameMode.MinTargetScale = NewMin;
}

void UGameModesWidget::OnMaxTargetScaleConstrainedChange(const float NewMax)
{
	//SelectedGameMode.MaxTargetScale = NewMax;
}

#pragma endregion;

#pragma region SaveStart

void UGameModesWidget::SaveCustomGameMode(const FGameModeActorStruct GameModeToSave) const
{
	TArray<FGameModeActorStruct> CustomGameModesArray = LoadCustomGameModes();
	for (const FGameModeActorStruct Elem : CustomGameModesArray)
	{
		if (Elem.CustomGameModeName.Equals(GameModeToSave.CustomGameModeName))
		{
			CustomGameModesArray.Remove(Elem);
			CustomGameModesArray.Shrink();
		}
	}
	CustomGameModesArray.Add(GameModeToSave);
	if (USaveGameCustomGameMode* SaveCustomGameModeObject = Cast<USaveGameCustomGameMode>(
	UGameplayStatics::CreateSaveGameObject(USaveGameCustomGameMode::StaticClass())))
	{
		SaveCustomGameModeObject->CustomGameModes = CustomGameModesArray;
		UGameplayStatics::SaveGameToSlot(SaveCustomGameModeObject, TEXT("CustomGameModesSlot"), 3);
	}
}

void UGameModesWidget::RemoveCustomGameMode(const FString& CustomGameModeName) const
{
	TArray<FGameModeActorStruct> CustomGameModesArray = LoadCustomGameModes();
	CustomGameModesArray.Remove(GetCustomGameMode(CustomGameModeName));
	CustomGameModesArray.Shrink();
	if (USaveGameCustomGameMode* SaveCustomGameModeObject = Cast<USaveGameCustomGameMode>(
UGameplayStatics::CreateSaveGameObject(USaveGameCustomGameMode::StaticClass())))
	{
		SaveCustomGameModeObject->CustomGameModes = CustomGameModesArray;
		UGameplayStatics::SaveGameToSlot(SaveCustomGameModeObject, TEXT("CustomGameModesSlot"), 3);
	}
}

void UGameModesWidget::RemoveAllCustomGameModes() const
{
	TArray<FGameModeActorStruct> CustomGameModesArray = LoadCustomGameModes();
	CustomGameModesArray.Empty();
	CustomGameModesArray.Shrink();
	if (USaveGameCustomGameMode* SaveCustomGameModeObject = Cast<USaveGameCustomGameMode>(
UGameplayStatics::CreateSaveGameObject(USaveGameCustomGameMode::StaticClass())))
	{
		SaveCustomGameModeObject->CustomGameModes = CustomGameModesArray;
		UGameplayStatics::SaveGameToSlot(SaveCustomGameModeObject, TEXT("CustomGameModesSlot"), 3);
	}
}

TArray<FGameModeActorStruct> UGameModesWidget::LoadCustomGameModes() const
{
	USaveGameCustomGameMode* SaveGameCustomGameMode;
	if (UGameplayStatics::DoesSaveGameExist(TEXT("CustomGameModesSlot"), 3))
	{
		SaveGameCustomGameMode = Cast<USaveGameCustomGameMode>(
			UGameplayStatics::LoadGameFromSlot(TEXT("CustomGameModesSlot"), 3));
	}
	else
	{
		SaveGameCustomGameMode = Cast<USaveGameCustomGameMode>(
			UGameplayStatics::CreateSaveGameObject(USaveGameCustomGameMode::StaticClass()));
	}
	if (SaveGameCustomGameMode)
	{
		return SaveGameCustomGameMode->CustomGameModes;
	}
	return TArray<FGameModeActorStruct>();
}

void UGameModesWidget::UpdateSaveStartButtonStates(const FString& CustomGameModeName)
{
	/** Invalid Non-Empty CustomGameModeName, which is only DefaultGameMode names */
	if (IsDefaultGameMode(CustomGameModeName) ||
	/** Empty CustomGameModeName and custom game mode is not selected in GameModeNameComboBox */
		(CustomGameModeName.IsEmpty() && !IsCustomGameMode(GameModeNameComboBox->GetSelectedOption())))
	{
		SaveCustomButton->SetIsEnabled(false);
		SaveCustomAndStartButton->SetIsEnabled(false);
		StartCustomButton->SetIsEnabled(false);
		SaveCustomAndStartButton->SetIsEnabled(false);
		RemoveSelectedCustomButton->SetIsEnabled(false);
		return;
	}
	
	/** Existing custom game mode or a new custom game mode */
	SaveCustomButton->SetIsEnabled(true);
	SaveCustomAndStartButton->SetIsEnabled(true);
	StartCustomButton->SetIsEnabled(true);
	SaveCustomAndStartButton->SetIsEnabled(true);
	RemoveSelectedCustomButton->SetIsEnabled(true);
}

void UGameModesWidget::OnSaveCustomButtonClicked()
{
	const FString SelectedGameModeName = GameModeNameComboBox->GetSelectedOption();
	const FString CustomGameModeName = CustomGameModeETB->GetText().ToString();
	if (IsCustomGameMode(SelectedGameModeName))
		//CustomGameModeName.IsEmpty() || SelectedGameModeName.Equals(CustomGameModeName) ||))
	{
		ShowConfirmOverwriteMessage();
		return;
	}
	SaveCustomGameMode(GetCustomGameModeOptions());
}

void UGameModesWidget::OnStartWithoutSavingButtonClicked()
{
	//SelectedGameMode.GameModeActorName = EGameModeActorName::Custom;
	InitializeExit();
}

void UGameModesWidget::OnSaveCustomAndStartButtonClicked()
{
	SaveCustomGameMode(GetCustomGameModeOptions());
	InitializeExit();
}

void UGameModesWidget::OnStartCustomButtonClicked()
{
	InitializeExit();
}

void UGameModesWidget::OnRemoveSelectedCustomButtonClicked()
{
	if (IsCustomGameMode(GameModeNameComboBox->GetSelectedOption()))
	{
		RemoveCustomGameMode(GameModeNameComboBox->GetSelectedOption());
	}
}

void UGameModesWidget::OnRemoveAllCustomButtonClicked()
{
	RemoveAllCustomGameModes();
}

void UGameModesWidget::ShowConfirmOverwriteMessage()
{
	ADefaultPlayerController* PlayerController = Cast<ADefaultPlayerController>(
		UGameplayStatics::GetPlayerController(
			GetWorld(), 0));
	UPopupMessageWidget* PopupMessageWidget = PlayerController->CreatePopupMessageWidget(true, 1);
	PopupMessageWidget->InitPopup("Overwrite Existing Game Mode?",
	                              "",
	                              "Confirm", "Cancel");
	PopupMessageWidget->Button1->OnClicked.AddDynamic(
		this, &UGameModesWidget::OnConfirmOverwriteButtonClicked);
	PopupMessageWidget->Button2->OnClicked.AddDynamic(
		this, &UGameModesWidget::OnCancelOverwriteButtonClicked);
}

void UGameModesWidget::OnConfirmOverwriteButtonClicked()
{
	Cast<ADefaultPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0))->HidePopupMessage();
	SaveCustomGameMode(GetCustomGameModeOptions());
}

void UGameModesWidget::OnCancelOverwriteButtonClicked()
{
	Cast<ADefaultPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0))->HidePopupMessage();
}

void UGameModesWidget::InitializeExit()
{
	/** TODO: Check for valid game mode and constraint stuff */
	ADefaultPlayerController* PlayerController = Cast<ADefaultPlayerController>(
		UGameplayStatics::GetPlayerController(
			GetWorld(), 0));
	PlayerController->OnScreenFadeToBlackFinish.AddDynamic(this, &UGameModesWidget::StartGame);
	PlayerController->FadeScreenToBlack();
}

void UGameModesWidget::StartGame()
{
	/** TODO Pass SelectedGameMode to Game Instance */
	Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()))->GameModeActorStruct = GetCustomGameModeOptions();
	ADefaultPlayerController* PlayerController = Cast<ADefaultPlayerController>(
		UGameplayStatics::GetPlayerController(GetWorld(), 0));
	PlayerController->HideMainMenu();
	if (PlayerController->IsPostGameMenuActive())
	{
		PlayerController->HidePostGameMenu();
		Cast<ADefaultGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->InitializeGameMode();
	}
	UGameplayStatics::OpenLevel(GetWorld(), FName("Range"));
}

#pragma endregion

#pragma region BeatGrid

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

#pragma endregion

#pragma region Utility

FGameModeActorStruct UGameModesWidget::GetDefaultGameMode(const FString& GameModeName)
{
	for (const FGameModeActorStruct& GameModeActor : GameModeActorDefaults)
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
	for (const FGameModeActorStruct& Elem : CustomGameModesArray)
	{
		if (Elem.CustomGameModeName.Equals(CustomGameModeName))
		{
			return Elem;
		}
	}
	return FGameModeActorStruct(EGameModeActorName::Custom, EGameModeDifficulty::Normal);
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

bool UGameModesWidget::IsDefaultGameMode(const FString& GameModeName)
{
	for (const FGameModeActorStruct& GameModeActor : GameModeActorDefaults)
	{
		if (GameModeName == UEnum::GetDisplayValueAsText(GameModeActor.GameModeActorName).ToString())
		{
			return true;
		}
	}
	return false;
}

bool UGameModesWidget::IsCustomGameMode(const FString& GameModeName)
{
	if (IsDefaultGameMode(GameModeName))
	{
		return false;
	}
	TArray<FGameModeActorStruct> CustomGameModesArray = LoadCustomGameModes();
	for (const FGameModeActorStruct& GameMode : CustomGameModesArray)
	{
		if (GameMode.CustomGameModeName.Equals(GameModeName))
		{
			return true;
		}
	}
	return false;
}

FGameModeActorStruct UGameModesWidget::GameModeActorStructConstructor(const EGameModeActorName GameModeActor,
                                                                      const EGameModeDifficulty NewGameModeDifficulty,
                                                                      const ESpreadType NewSpreadType)
{
	return FGameModeActorStruct(GameModeActor, NewGameModeDifficulty, NewSpreadType);
}

void UGameModesWidget::OnEditableTextBoxChanged(const FText& NewTextValue, UEditableTextBox* TextBoxToChange,
                                                USlider* SliderToChange,
                                                const float GridSnapSize, const float Min, const float Max)
{
	const float ClampedValue = FMath::Clamp(FCString::Atof(*NewTextValue.ToString()), Min, Max);
	const float SnappedValue = FMath::GridSnap(ClampedValue, GridSnapSize);
	TextBoxToChange->SetText(FText::AsNumber(SnappedValue));
	SliderToChange->SetValue(SnappedValue);
}

void UGameModesWidget::OnSliderChanged(const float NewValue, UEditableTextBox* TextBoxToChange,
                                       const float GridSnapSize)
{
	TextBoxToChange->SetText(FText::AsNumber(FMath::GridSnap(NewValue, GridSnapSize)));
}

void UGameModesWidget::OnTooltipImageHovered(UTooltipImage* HoveredTooltipImage, const FText& TooltipTextToShow)
{
	Tooltip->TooltipDescriptor->SetText(TooltipTextToShow);
	HoveredTooltipImage->SetToolTip(Tooltip);
}

#pragma endregion
