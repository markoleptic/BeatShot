// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


// ReSharper disable CppMemberFunctionMayBeConst
#include "SubMenuWidgets/GameModesWidget.h"
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
#include "Kismet/KismetStringLibrary.h"
#include "Misc/DefaultValueHelper.h"
#include "OverlayWidgets/PopupMessageWidget.h"
#include "OverlayWidgets/AudioSelectWidget.h"
#include "WidgetComponents/ConstrainedSlider.h"
#include "WidgetComponents/GameModeButton.h"
#include "WidgetComponents/SavedTextWidget.h"
#include "WidgetComponents/SlideRightButton.h"
#include "WidgetComponents/TooltipWidget.h"

void UGameModesWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	MenuWidgets.Add(DefaultGameModesButton, DefaultGameModes);
	MenuWidgets.Add(CustomGameModesButton, CustomGameModes);
	PlayFromStandardButton->SetIsEnabled(false);
	CustomizeFromStandardButton->SetIsEnabled(false);

	SpreadSelect->SetVisibility(ESlateVisibility::Collapsed);
	DefaultModes = FBSConfig::GetDefaultGameModes();
	
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
	
	/* Target Scale TextBox and Slider */
	FConstrainedSliderStruct TargetScaleSliderStruct;
	TargetScaleSliderStruct.MinConstraintLower = MinTargetScaleValue;
	TargetScaleSliderStruct.MinConstraintUpper = MaxTargetScaleValue;
	TargetScaleSliderStruct.MaxConstraintLower = MinTargetScaleValue;
	TargetScaleSliderStruct.MaxConstraintUpper = MaxTargetScaleValue;
	TargetScaleSliderStruct.DefaultMinValue = MinTargetScaleValue;
	TargetScaleSliderStruct.DefaultMaxValue = MaxTargetScaleValue;
	TargetScaleSliderStruct.MaxText = FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "GM_MaxTargetScale");
	TargetScaleSliderStruct.MinText = FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "GM_MinTargetScale");
	TargetScaleSliderStruct.CheckboxText = FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "GM_ConstantTargetSize");
	TargetScaleSliderStruct.bSyncSlidersAndValues = false;
	TargetScaleSliderStruct.GridSnapSize = TargetScaleSnapSize;
	TargetScaleConstrained->InitConstrainedSlider(TargetScaleSliderStruct);

	/* BeatGrid Spacing TextBox and Slider */
	FConstrainedSliderStruct BeatGridSliderStruct;
	BeatGridSliderStruct.MinConstraintLower = MinBeatGridHorizontalSpacingValue;
	BeatGridSliderStruct.MinConstraintUpper = MaxBeatGridHorizontalSpacingValue;
	BeatGridSliderStruct.MaxConstraintLower = MinBeatGridVerticalSpacingValue;
	BeatGridSliderStruct.MaxConstraintUpper = MaxBeatGridVerticalSpacingValue;
	BeatGridSliderStruct.DefaultMinValue = MinTargetScaleValue;
	BeatGridSliderStruct.DefaultMaxValue = MaxTargetScaleValue;
	BeatGridSliderStruct.MinText = FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "GM_BeatGrid_HorizontalSpacing");
	BeatGridSliderStruct.MaxText = FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "GM_BeatGrid_VerticalSpacing");
	BeatGridSliderStruct.CheckboxText = FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "GM_EvenSpacing");
	BeatGridSliderStruct.bSyncSlidersAndValues = true;
	BeatGridSliderStruct.GridSnapSize = BeatGridVerticalSpacingSnapSize;
	BeatGridSpacingConstrained->InitConstrainedSlider(BeatGridSliderStruct);
	
	/* BeatTrack target speed TextBox and Slider */
	FConstrainedSliderStruct TrackingSpeedSliderStruct;
	TrackingSpeedSliderStruct.MinConstraintLower = MinTargetSpeedValue;
	TrackingSpeedSliderStruct.MinConstraintUpper = MaxTargetSpeedValue;
	TrackingSpeedSliderStruct.MaxConstraintLower = MinTargetSpeedValue;
	TrackingSpeedSliderStruct.MaxConstraintUpper = MaxTargetSpeedValue;
	TrackingSpeedSliderStruct.DefaultMinValue = MinTargetSpeedValue;
	TrackingSpeedSliderStruct.DefaultMaxValue = MaxTargetSpeedValue;
	TrackingSpeedSliderStruct.MaxText = FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "GM_MaxTrackingSpeed");
	TrackingSpeedSliderStruct.MinText = FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "GM_MinTrackingSpeed");
	TrackingSpeedSliderStruct.CheckboxText = FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "GM_ConstantTrackingSpeed");
	TrackingSpeedSliderStruct.bSyncSlidersAndValues = false;
	TrackingSpeedSliderStruct.GridSnapSize = TargetSpeedSnapSize;
	TargetSpeedConstrained->InitConstrainedSlider(TrackingSpeedSliderStruct);
	
	/* Tooltips */
	{
		Tooltip = CreateWidget<UTooltipWidget>(this, TooltipWidgetClass);
		GameModeTemplateQMark->TooltipText = FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "GameModeTemplate");
		CustomGameModeNameQMark->TooltipText = FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "CustomGameModeName");
		BaseGameModeQMark->TooltipText = FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "BaseGameMode");
		GameModeDifficultyQMark->TooltipText = FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "GameModeDifficulty");
		SpawnBeatDelayQMark->TooltipText = FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "SpawnBeatDelay");
		LifespanQMark->TooltipText = FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "Lifespan");
		TargetSpawnCDQMark->TooltipText = FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "MinDistance");
		HeadshotHeightQMark->TooltipText = FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "HeadshotHeight");
		ForwardSpreadQMark->TooltipText = FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "ForwardSpread");
		CenterTargetsQMark->TooltipText = FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "CenterTargets");
		MinDistanceQMark->TooltipText = FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "MinDistance");
		SpreadTypeQMark->TooltipText = FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "SpreadType");
		DynamicTargetScaleQMark->TooltipText = FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "DynamicTargetScale");
		BeatGridAdjacentOnlyQMark->TooltipText = FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "BeatGridAdjacentOnly");
		EnableAIQMark->TooltipText = FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "EnableAI");
		AlphaQMark->TooltipText = FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "Alpha");
		EpsilonQMark->TooltipText = FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "Epsilon");
		GammaQMark->TooltipText = FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "Gamma");
		TargetScaleConstrained->CheckboxQMark->TooltipText = FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "ConstantTargetScale");
		TargetSpeedConstrained->CheckboxQMark->TooltipText = FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "BeatTrackConstantSpeed");
		BeatGridSpacingConstrained->CheckboxQMark->TooltipText = FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "BeatGridEvenSpacing");
	}

	BindAllDelegates();
	OnButtonClicked_DefaultGameModes();
	PopulateGameModeNameComboBox("");
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

	// Custom Game Modes
	GameModeNameComboBox->OnSelectionChanged.AddDynamic(this, &UGameModesWidget::OnSelectionChanged_GameModeName);
	CustomGameModeETB->OnTextChanged.AddDynamic(this, &UGameModesWidget::OnTextChanged_CustomGameMode);
	BaseGameModeComboBox->OnSelectionChanged.AddDynamic(this, &UGameModesWidget::OnSelectionChanged_BaseGameMode);
	GameModeDifficultyComboBox->OnSelectionChanged.AddDynamic(this, &UGameModesWidget::OnSelectionChanged_GameModeDifficulty);
	PlayerDelaySlider->OnValueChanged.AddDynamic(this, &UGameModesWidget::OnSliderChanged_PlayerDelay);
	PlayerDelayValue->OnTextCommitted.AddDynamic(this, &UGameModesWidget::OnTextCommitted_PlayerDelay);
	LifespanSlider->OnValueChanged.AddDynamic(this, &UGameModesWidget::OnSliderChanged_Lifespan);
	LifespanValue->OnTextCommitted.AddDynamic(this, &UGameModesWidget::OnTextCommitted_Lifespan);
	TargetSpawnCDSlider->OnValueChanged.AddDynamic(this, &UGameModesWidget::OnSliderChanged_TargetSpawnCD);
	TargetSpawnCDValue->OnTextCommitted.AddDynamic(this, &UGameModesWidget::OnTextCommitted_TargetSpawnCD);
	HeadShotOnlyCheckBox->OnCheckStateChanged.AddDynamic(this, &UGameModesWidget::OnCheckStateChanged_HeadShotOnly);
	MinTargetDistanceSlider->OnValueChanged.AddDynamic(this, &UGameModesWidget::OnSliderChanged_MinTargetDistance);
	MinTargetDistanceValue->OnTextCommitted.AddDynamic(this, &UGameModesWidget::OnTextCommitted_MinTargetDistance);
	HorizontalSpreadSlider->OnValueChanged.AddDynamic(this, &UGameModesWidget::OnSliderChanged_HorizontalSpread);
	HorizontalSpreadValue->OnTextCommitted.AddDynamic(this, &UGameModesWidget::OnTextCommitted_HorizontalSpread);
	VerticalSpreadSlider->OnValueChanged.AddDynamic(this, &UGameModesWidget::OnSliderChanged_VerticalSpread);
	VerticalSpreadValue->OnTextCommitted.AddDynamic(this, &UGameModesWidget::OnTextCommitted_VerticalSpread);
	ForwardSpreadCheckBox->OnCheckStateChanged.AddDynamic(this, &UGameModesWidget::OnCheckStateChanged_ForwardSpread);
	ForwardSpreadSlider->OnValueChanged.AddDynamic(this, &UGameModesWidget::OnSliderChanged_ForwardSpread);
	ForwardSpreadValue->OnTextCommitted.AddDynamic(this, &UGameModesWidget::OnTextCommitted_ForwardSpread);
	EnableAICheckBox->OnCheckStateChanged.AddDynamic(this, &UGameModesWidget::OnCheckStateChanged_EnableAI);
	AIAlphaSlider->OnValueChanged.AddDynamic(this, &UGameModesWidget::OnSliderChanged_AIAlpha);
	AIAlphaValue->OnTextCommitted.AddDynamic(this, &UGameModesWidget::OnTextCommitted_AIAlpha);
	AIEpsilonSlider->OnValueChanged.AddDynamic(this, &UGameModesWidget::OnSliderChanged_AIEpsilon);
	AIEpsilonValue->OnTextCommitted.AddDynamic(this, &UGameModesWidget::OnTextCommitted_AIEpsilon);
	AIGammaSlider->OnValueChanged.AddDynamic(this, &UGameModesWidget::OnSliderChanged_AIGamma);
	AIGammaValue->OnTextCommitted.AddDynamic(this, &UGameModesWidget::OnTextCommitted_AIGamma);

	/** BeatGrid Options */
	BeatGridNumHorizontalTargetsSlider->OnValueChanged.AddDynamic(this, &UGameModesWidget::OnSliderChanged_BeatGridNumHorizontalTargets);
	BeatGridNumHorizontalTargetsValue->OnTextCommitted.AddDynamic(this, &UGameModesWidget::OnTextCommitted_BeatGridNumHorizontalTargets);
	BeatGridNumVerticalTargetsSlider->OnValueChanged.AddDynamic(this, &UGameModesWidget::OnSliderChanged_BeatGridNumVerticalTargets);
	BeatGridNumVerticalTargetsValue->OnTextCommitted.AddDynamic(this, &UGameModesWidget::OnTextCommitted_BeatGridNumVerticalTargets);
	BeatGridUpdate_HorizontalSpread.BindUObject(BeatGridSpacingConstrained, &UConstrainedSlider_BeatGrid::OnBeatGridUpdate_HorizontalSpread);
	BeatGridUpdate_VerticalSpread.BindUObject(BeatGridSpacingConstrained, &UConstrainedSlider_BeatGrid::OnBeatGridUpdate_VerticalSpread);
	BeatGridUpdate_NumVerticalTargets.BindUObject(BeatGridSpacingConstrained, &UConstrainedSlider_BeatGrid::OnBeatGridUpdate_NumVerticalTargets);
	BeatGridUpdate_NumHorizontalTargets.BindUObject(BeatGridSpacingConstrained, &UConstrainedSlider_BeatGrid::OnBeatGridUpdate_NumHorizontalTargets);
	TargetScaleConstrained->OnMaxValueChanged.AddUObject(BeatGridSpacingConstrained, &UConstrainedSlider_BeatGrid::OnBeatGridUpdate_MaxTargetScale);
	BeatGridSpacingConstrained->OnBeatGridSpacingConstrained.BindUObject(this, &UGameModesWidget::OnBeatGridSpacingConstrained);
	
	// Tooltips
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
	BeatGridAdjacentOnlyQMark->OnTooltipImageHovered.AddDynamic(this, &UGameModesWidget::OnTooltipImageHovered);
	EnableAIQMark->OnTooltipImageHovered.AddDynamic(this, &UGameModesWidget::OnTooltipImageHovered);
	AlphaQMark->OnTooltipImageHovered.AddDynamic(this, &UGameModesWidget::OnTooltipImageHovered);
	EpsilonQMark->OnTooltipImageHovered.AddDynamic(this, &UGameModesWidget::OnTooltipImageHovered);
	GammaQMark->OnTooltipImageHovered.AddDynamic(this, &UGameModesWidget::OnTooltipImageHovered);
	TargetScaleConstrained->CheckboxQMark->OnTooltipImageHovered.AddDynamic(this, &UGameModesWidget::OnTooltipImageHovered);
	TargetSpeedConstrained->CheckboxQMark->OnTooltipImageHovered.AddDynamic(this, &UGameModesWidget::OnTooltipImageHovered);
	BeatGridSpacingConstrained->CheckboxQMark->OnTooltipImageHovered.AddDynamic(this, &UGameModesWidget::OnTooltipImageHovered);
}

void UGameModesWidget::SetHiddenConfigParameters(FBSConfig& Config)
{
	const TArray<FBSConfig> CustomGameModesArray = LoadCustomGameModes();
	TArray<FBSConfig> FilteredModes = CustomGameModesArray.FilterByPredicate([&Config](const FBSConfig& MatchingStruct)
	{
		return MatchingStruct.CustomGameModeName == Config.CustomGameModeName;
	});
	if (!FilteredModes.IsEmpty())
	{
		Config.AIConfig.QTable = FilteredModes[0].AIConfig.QTable;
	}
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
		Head->Button->SetBackgroundColor(White);
		Head = Head->Next;
	}
}

void UGameModesWidget::OnTextChanged_CustomGameMode(const FText& NewCustomGameModeText)
{
	UpdateSaveStartButtonStates();
}

void UGameModesWidget::OnSelectionChanged_GameModeName(const FString SelectedGameModeName, const ESelectInfo::Type SelectionType)
{
	if (IsDefaultGameMode(SelectedGameModeName))
	{
		BaseGameModeBox->SetVisibility(ESlateVisibility::Collapsed);
		PopulateGameModeOptions(FindDefaultGameMode(SelectedGameModeName));
	}
	if (IsCustomGameMode(SelectedGameModeName))
	{
		BaseGameModeBox->SetVisibility(ESlateVisibility::Visible);
		PopulateGameModeOptions(FindCustomGameMode(SelectedGameModeName));
	}
	UpdateSaveStartButtonStates();
}

void UGameModesWidget::OnSelectionChanged_BaseGameMode(const FString SelectedBaseGameMode, const ESelectInfo::Type SelectionType)
{
	/** TODO: Is this necessary*/
	if (SelectionType != ESelectInfo::Type::Direct)
	{
		PopulateGameModeOptions(FindDefaultGameMode(SelectedBaseGameMode));
	}
}

void UGameModesWidget::OnSelectionChanged_GameModeDifficulty(const FString SelectedDifficulty, const ESelectInfo::Type SelectionType)
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
		const FBSConfig BSConfig = FBSConfig(FindDefaultGameMode(BaseGameModeComboBox->GetSelectedOption()).DefaultMode, EnumDifficulty);
		PopulateGameModeOptions(BSConfig);
	}
}

void UGameModesWidget::OnSliderChanged_PlayerDelay(const float NewPlayerDelay)
{
	OnSliderChanged(NewPlayerDelay, PlayerDelayValue, PlayerDelayGridSnapSize);
}

void UGameModesWidget::OnSliderChanged_Lifespan(const float NewLifespan)
{
	OnSliderChanged(NewLifespan, LifespanValue, LifespanGridSnapSize);
}

void UGameModesWidget::OnSliderChanged_TargetSpawnCD(const float NewTargetSpawnCD)
{
	OnSliderChanged(NewTargetSpawnCD, TargetSpawnCDValue, TargetSpawnCDGridSnapSize);
}

void UGameModesWidget::OnSliderChanged_MinTargetDistance(const float NewMinTargetDistance)
{
	OnSliderChanged(NewMinTargetDistance, MinTargetDistanceValue, MinTargetDistanceGridSnapSize);
}

void UGameModesWidget::OnSliderChanged_HorizontalSpread(const float NewHorizontalSpread)
{
	const float SnappedValue = OnSliderChanged(NewHorizontalSpread, HorizontalSpreadValue, HorizontalSpreadGridSnapSize);
	BeatGridUpdate_HorizontalSpread.Execute(SnappedValue);
}

void UGameModesWidget::OnSliderChanged_VerticalSpread(const float NewVerticalSpread)
{
	const float SnappedValue = OnSliderChanged(NewVerticalSpread, VerticalSpreadValue, VerticalSpreadGridSnapSize);
	BeatGridUpdate_VerticalSpread.Execute(SnappedValue);
}

void UGameModesWidget::OnSliderChanged_ForwardSpread(const float NewForwardSpread)
{
	OnSliderChanged(NewForwardSpread, ForwardSpreadValue, HorizontalSpreadGridSnapSize);
}

void UGameModesWidget::OnSliderChanged_AIAlpha(const float NewAlpha)
{
	OnSliderChanged(NewAlpha, AIAlphaValue, AlphaSnapSize);
}

void UGameModesWidget::OnSliderChanged_AIEpsilon(const float NewEpsilon)
{
	OnSliderChanged(NewEpsilon, AIEpsilonValue, EpsilonSnapSize);
}

void UGameModesWidget::OnSliderChanged_AIGamma(const float NewGamma)
{
	OnSliderChanged(NewGamma, AIGammaValue, GammaSnapSize);
}

void UGameModesWidget::OnSliderChanged_BeatGridNumHorizontalTargets(const float NewNumHorizontalTargets)
{
	const float Value = OnSliderChanged(NewNumHorizontalTargets, BeatGridNumHorizontalTargetsValue, NumBeatGridHorizontalTargetsSnapSize);
	BeatGridUpdate_NumHorizontalTargets.Execute(Value);
}

void UGameModesWidget::OnSliderChanged_BeatGridNumVerticalTargets(const float NewNumVerticalTargets)
{
	const float Value = OnSliderChanged(NewNumVerticalTargets, BeatGridNumVerticalTargetsValue, NumBeatGridVerticalTargetsSnapSize);
	BeatGridUpdate_NumVerticalTargets.Execute(Value);
}

void UGameModesWidget::OnTextCommitted_PlayerDelay(const FText& NewPlayerDelay, ETextCommit::Type CommitType)
{
	OnEditableTextBoxChanged(NewPlayerDelay, PlayerDelayValue, PlayerDelaySlider, PlayerDelayGridSnapSize, MinPlayerDelayValue, MaxPlayerDelayValue);
}

void UGameModesWidget::OnTextCommitted_Lifespan(const FText& NewLifespan, ETextCommit::Type CommitType)
{
	OnEditableTextBoxChanged(NewLifespan, LifespanValue, LifespanSlider, LifespanGridSnapSize, MinLifespanValue, MaxLifespanValue);
}

void UGameModesWidget::OnTextCommitted_TargetSpawnCD(const FText& NewTargetSpawnCD, ETextCommit::Type CommitType)
{
	OnEditableTextBoxChanged(NewTargetSpawnCD, TargetSpawnCDValue, TargetSpawnCDSlider, TargetSpawnCDGridSnapSize, MinTargetSpawnCDValue, MaxTargetSpawnCDValue);
}

void UGameModesWidget::OnTextCommitted_MinTargetDistance(const FText& NewMinTargetDistance, ETextCommit::Type CommitType)
{
	OnEditableTextBoxChanged(NewMinTargetDistance, MinTargetDistanceValue, MinTargetDistanceSlider, MinTargetDistanceGridSnapSize, MinMinTargetDistanceValue, MaxMinTargetDistanceValue);
}

void UGameModesWidget::OnTextCommitted_HorizontalSpread(const FText& NewHorizontalSpread, ETextCommit::Type CommitType)
{
	const float SnappedValue = OnEditableTextBoxChanged(NewHorizontalSpread, HorizontalSpreadValue, HorizontalSpreadSlider, HorizontalSpreadGridSnapSize, MinHorizontalSpreadValue, MaxHorizontalSpreadValue);
	BeatGridUpdate_HorizontalSpread.Execute(SnappedValue);
}

void UGameModesWidget::OnTextCommitted_VerticalSpread(const FText& NewVerticalSpread, ETextCommit::Type CommitType)
{
	const float SnappedValue = OnEditableTextBoxChanged(NewVerticalSpread, VerticalSpreadValue, VerticalSpreadSlider, VerticalSpreadGridSnapSize, MinVerticalSpreadValue, MaxVerticalSpreadValue);
	BeatGridUpdate_VerticalSpread.Execute(SnappedValue);
}

void UGameModesWidget::OnTextCommitted_ForwardSpread(const FText& NewForwardSpread, ETextCommit::Type CommitType)
{
	OnEditableTextBoxChanged(NewForwardSpread, ForwardSpreadValue, ForwardSpreadSlider, HorizontalSpreadGridSnapSize, MinForwardSpreadValue, MaxForwardSpreadValue);
}

void UGameModesWidget::OnTextCommitted_BeatGridNumHorizontalTargets(const FText& NewNumHorizontalTargets, ETextCommit::Type CommitType)
{
	const float Value = OnEditableTextBoxChanged(NewNumHorizontalTargets, BeatGridNumHorizontalTargetsValue, BeatGridNumHorizontalTargetsSlider, NumBeatGridHorizontalTargetsSnapSize, MinBeatGridHorizontalSpacingValue, MaxBeatGridHorizontalSpacingValue);
	BeatGridUpdate_NumHorizontalTargets.Execute(Value);
}

void UGameModesWidget::OnTextCommitted_BeatGridNumVerticalTargets(const FText& NewNumVerticalTargets, ETextCommit::Type CommitType)
{
	const float Value = OnEditableTextBoxChanged(NewNumVerticalTargets, BeatGridNumVerticalTargetsValue, BeatGridNumVerticalTargetsSlider, NumBeatGridVerticalTargetsSnapSize, MinBeatGridVerticalSpacingValue, MaxBeatGridVerticalSpacingValue);
	BeatGridUpdate_NumVerticalTargets.Execute(Value);
}

void UGameModesWidget::OnTextCommitted_AIAlpha(const FText& NewAlpha, ETextCommit::Type CommitType)
{
	OnEditableTextBoxChanged(NewAlpha, AIAlphaValue, AIAlphaSlider, AlphaSnapSize, MinAlphaValue, MaxAlphaValue);
}

void UGameModesWidget::OnTextCommitted_AIEpsilon(const FText& NewEpsilon, ETextCommit::Type CommitType)
{
	OnEditableTextBoxChanged(NewEpsilon, AIEpsilonValue, AIEpsilonSlider, EpsilonSnapSize, MinEpsilonValue, MaxEpsilonValue);
}

void UGameModesWidget::OnTextCommitted_AIGamma(const FText& NewGamma, ETextCommit::Type CommitType)
{
	OnEditableTextBoxChanged(NewGamma, AIGammaValue, AIGammaSlider, GammaSnapSize, MinGammaValue, MaxGammaValue);
}

void UGameModesWidget::OnButtonClicked_DefaultGameMode(const UGameModeButton* GameModeButton)
{
	DefaultMode = GameModeButton->DefaultMode;
	DefaultDifficulty = GameModeButton->Difficulty;
	const FBSConfig SelectedGameMode = FBSConfig(GameModeButton->DefaultMode, GameModeButton->Difficulty);
	GameModeNameComboBox->SetSelectedOption(UEnum::GetDisplayValueAsText(GameModeButton->DefaultMode).ToString());
	PopulateGameModeOptions(SelectedGameMode);
	SetGameModeButtonBackgroundColor(GameModeButton);
	DynamicSpreadButton->SetBackgroundColor(White);
	WideSpreadButton->SetBackgroundColor(White);
	NarrowSpreadButton->SetBackgroundColor(White);

	/** Don't show SpreadSelect if BeatGrid or BeatTrack */
	if (SelectedGameMode.DefaultMode == EDefaultMode::BeatGrid || SelectedGameMode.DefaultMode == EDefaultMode::BeatTrack)
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
	NarrowSpreadButton->SetBackgroundColor(White);
	WideSpreadButton->SetBackgroundColor(White);
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
	DynamicSpreadButton->SetBackgroundColor(White);
	NarrowSpreadButton->SetBackgroundColor(BeatShotBlue);
	WideSpreadButton->SetBackgroundColor(White);
	DefaultSpreadType = ESpreadType::StaticNarrow;
	PopulateGameModeOptions(FBSConfig(DefaultMode, DefaultDifficulty, ESpreadType::StaticNarrow));
	PlayFromStandardButton->SetIsEnabled(true);
}

void UGameModesWidget::OnButtonClicked_WideSpread()
{
	/** Change the background colors of the spread select buttons */
	DynamicSpreadButton->SetBackgroundColor(White);
	NarrowSpreadButton->SetBackgroundColor(White);
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
	const FString SelectedGameModeName = GameModeNameComboBox->GetSelectedOption();
	const FString CustomGameModeName = CustomGameModeETB->GetText().ToString();
	if (IsCustomGameMode(SelectedGameModeName) && CustomGameModeName.IsEmpty())
	{
		ShowConfirmOverwriteMessage(false);
		return;
	}
	FBSConfig Config = GetCustomGameModeOptions();
	SaveCustomGameModeToSlot(Config);
	/** If user is saving custom game mode based on DefaultGameMode from GameModeNameComboBox */
	if (IsDefaultGameMode(SelectedGameModeName))
	{
		PopulateGameModeNameComboBox(CustomGameModeName);
	}
	/** If user is saving custom game mode based on CustomGameMode from GameModeNameComboBox */
	else
	{
		PopulateGameModeNameComboBox(CustomGameModeName);
	}
	CustomGameModeETB->SetText(FText::GetEmpty());
}

void UGameModesWidget::OnButtonClicked_StartWithoutSaving()
{
	ShowAudioFormatSelect(false);
}

void UGameModesWidget::OnButtonClicked_SaveCustomAndStart()
{
	const FString SelectedGameModeName = GameModeNameComboBox->GetSelectedOption();
	const FString CustomGameModeName = CustomGameModeETB->GetText().ToString();
	if (IsCustomGameMode(SelectedGameModeName) && CustomGameModeName.IsEmpty())
	{
		ShowConfirmOverwriteMessage(true);
		return;
	}
	FBSConfig Config = GetCustomGameModeOptions();
	SaveCustomGameModeToSlot(Config);
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
	const FString SelectedCustomGameMode = GameModeNameComboBox->GetSelectedOption();
	if (IsCustomGameMode(SelectedCustomGameMode))
	{
		TArray<FBSConfig> CustomGameModesArray = LoadCustomGameModes();
		const int32 NumRemoved = CustomGameModesArray.RemoveAll([&SelectedCustomGameMode](const FBSConfig& MatchingStruct)
		{
			return MatchingStruct.CustomGameModeName == SelectedCustomGameMode;
		});
		CustomGameModesArray.Shrink();
		SaveCustomGameMode(CustomGameModesArray);
		if (NumRemoved >= 1)
		{
			SavedTextWidget->SetSavedText(FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "GM_GameModeRemovedText"));
			SavedTextWidget->PlayFadeInFadeOut();
		}
		PopulateGameModeNameComboBox("");
	}
}

void UGameModesWidget::OnButtonClicked_RemoveAllCustom()
{
	TArray<FBSConfig> CustomGameModesArray = LoadCustomGameModes();
	CustomGameModesArray.Empty();
	CustomGameModesArray.Shrink();
	SaveCustomGameMode(CustomGameModesArray);
	SavedTextWidget->SetSavedText(FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "GM_AllGameModesRemovedText"));
	SavedTextWidget->PlayFadeInFadeOut();
	PopulateGameModeNameComboBox("");
}

void UGameModesWidget::OnButtonClicked_ConfirmOverwrite()
{
	PopupMessageWidget->FadeOut();
	FBSConfig Config = GetCustomGameModeOptions();
	SaveCustomGameModeToSlot(Config);
}

void UGameModesWidget::OnButtonClicked_ConfirmOverwriteAndStartGame()
{
	PopupMessageWidget->FadeOut();
	const FString SelectedGameModeName = GameModeNameComboBox->GetSelectedOption();
	const FString CustomGameModeName = CustomGameModeETB->GetText().ToString();
	FBSConfig Config = GetCustomGameModeOptions();
	SaveCustomGameModeToSlot(Config);
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
	ShowAudioFormatSelect(false);
}

void UGameModesWidget::OnButtonClicked_CancelOverwrite()
{
	PopupMessageWidget->FadeOut();
}

void UGameModesWidget::OnCheckStateChanged_HeadShotOnly(const bool bHeadshotOnly)
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
	OnEditableTextBoxChanged(FText::AsNumber(MaxVerticalSpreadValue), VerticalSpreadValue, VerticalSpreadSlider, VerticalSpreadGridSnapSize, MinVerticalSpreadValue, MaxVerticalSpreadValue);
	OnSliderChanged(MaxVerticalSpreadValue, VerticalSpreadValue, VerticalSpreadGridSnapSize);
}

void UGameModesWidget::OnCheckStateChanged_ForwardSpread(const bool bUseForwardSpread)
{
	if (bUseForwardSpread)
	{
		ForwardSpreadBox->SetVisibility(ESlateVisibility::Visible);
		return;
	}
	ForwardSpreadBox->SetVisibility(ESlateVisibility::Collapsed);
}

void UGameModesWidget::OnCheckStateChanged_EnableAI(const bool bEnableAI)
{
	if (bEnableAI)
	{
		AIAlphaBox->SetVisibility(ESlateVisibility::Visible);
		AIEpsilonBox->SetVisibility(ESlateVisibility::Visible);
		AIGammaBox->SetVisibility(ESlateVisibility::Visible);
		return;
	}
	AIAlphaBox->SetVisibility(ESlateVisibility::Collapsed);
	AIEpsilonBox->SetVisibility(ESlateVisibility::Collapsed);
	AIGammaBox->SetVisibility(ESlateVisibility::Collapsed);
}

void UGameModesWidget::OnBeatGridSpacingConstrained(const FBeatGridConstraints& BeatGridConstraints)
{
	
}

void UGameModesWidget::PopulateGameModeOptions(const FBSConfig& InBSConfig)
{
	if (InBSConfig.IsBeatGridMode)
	{
		BeatGridSpecificSettings->SetVisibility(ESlateVisibility::Visible);
		BaseGameModeComboBox->SetSelectedOption("BeatGrid");
		BeatGridSpacingConstrained->UpdateDefaultValues(InBSConfig.BeatGridSpacing.X, InBSConfig.BeatGridSpacing.Y);
		BeatGridSpacingConstrained->UpdateBeatGridConstraints(InBSConfig.NumHorizontalBeatGridTargets, InBSConfig.NumVerticalBeatGridTargets,
			InBSConfig.BoxBounds.Y, InBSConfig.BoxBounds.Z, InBSConfig.MaxTargetScale);
		RandomizeNextBeatGridTargetCheckBox->SetIsChecked(InBSConfig.RandomizeBeatGrid);
	}
	else
	{
		BeatGridSpecificSettings->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (InBSConfig.IsBeatTrackMode)
	{
		BeatTrackSpecificSettings->SetVisibility(ESlateVisibility::Visible);
		BaseGameModeComboBox->SetSelectedOption("BeatTrack");
		LifespanSlider->SetLocked(true);
		LifespanValue->SetIsReadOnly(true);
		TargetSpeedConstrained->UpdateDefaultValues(InBSConfig.MinTrackingSpeed, InBSConfig.MaxTrackingSpeed);
	}
	else
	{
		BeatTrackSpecificSettings->SetVisibility(ESlateVisibility::Collapsed);
		LifespanSlider->SetLocked(false);
		LifespanValue->SetIsReadOnly(false);
	}

	if (InBSConfig.IsBeatTrackMode || InBSConfig.IsBeatGridMode)
	{
		MinTargetDistanceBox->SetVisibility(ESlateVisibility::Collapsed);
		SpreadTypeBox->SetVisibility(ESlateVisibility::Collapsed);
		AISpecificSettings->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		MinTargetDistanceBox->SetVisibility(ESlateVisibility::Visible);
		SpreadTypeBox->SetVisibility(ESlateVisibility::Visible);
		AISpecificSettings->SetVisibility(ESlateVisibility::Visible);
		EnableAICheckBox->SetIsChecked(InBSConfig.AIConfig.bEnableRLAgent);
		AIAlphaSlider->SetValue(InBSConfig.AIConfig.Alpha);
		AIAlphaValue->SetText(FText::AsNumber(InBSConfig.AIConfig.Alpha));
		AIGammaSlider->SetValue(InBSConfig.AIConfig.Gamma);
		AIGammaValue->SetText(FText::AsNumber(InBSConfig.AIConfig.Gamma));
		AIEpsilonSlider->SetValue(InBSConfig.AIConfig.Epsilon);
		AIEpsilonValue->SetText(FText::AsNumber(InBSConfig.AIConfig.Epsilon));

		if (InBSConfig.AIConfig.bEnableRLAgent)
		{
			AIAlphaBox->SetVisibility(ESlateVisibility::Visible);
			AIGammaBox->SetVisibility(ESlateVisibility::Visible);
			AIEpsilonBox->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			AIAlphaBox->SetVisibility(ESlateVisibility::Collapsed);
			AIGammaBox->SetVisibility(ESlateVisibility::Collapsed);
			AIEpsilonBox->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
	
	if (InBSConfig.IsSingleBeatMode)
	{
		BaseGameModeComboBox->SetSelectedOption("SingleBeat");
	}
	if (!InBSConfig.IsBeatGridMode && !InBSConfig.IsBeatTrackMode && !InBSConfig.IsSingleBeatMode)
	{
		BaseGameModeComboBox->SetSelectedOption("MultiBeat");
		LifespanSlider->SetLocked(false);
		LifespanValue->SetIsReadOnly(false);
	}
	
	GameModeDifficultyComboBox->SetSelectedOption(UEnum::GetDisplayValueAsText(InBSConfig.GameModeDifficulty).ToString());
	PlayerDelaySlider->SetValue(InBSConfig.PlayerDelay);
	PlayerDelayValue->SetText(FText::AsNumber(InBSConfig.PlayerDelay));
	LifespanSlider->SetValue(InBSConfig.TargetMaxLifeSpan);
	LifespanValue->SetText(FText::AsNumber(InBSConfig.TargetMaxLifeSpan));
	TargetSpawnCDSlider->SetValue(InBSConfig.TargetSpawnCD);
	TargetSpawnCDValue->SetText(FText::AsNumber(InBSConfig.TargetSpawnCD));
	HeadShotOnlyCheckBox->SetIsChecked(InBSConfig.HeadshotHeight);
	WallCenteredCheckBox->SetIsChecked(InBSConfig.WallCentered);
	MinTargetDistanceSlider->SetValue(InBSConfig.MinDistanceBetweenTargets);
	MinTargetDistanceValue->SetText(FText::AsNumber(InBSConfig.MinDistanceBetweenTargets));
	SpreadTypeComboBox->SetSelectedOption(UEnum::GetDisplayValueAsText(InBSConfig.SpreadType).ToString());
	HorizontalSpreadSlider->SetValue(InBSConfig.BoxBounds.Y);
	HorizontalSpreadValue->SetText(FText::AsNumber(InBSConfig.BoxBounds.Y));
	VerticalSpreadSlider->SetValue(InBSConfig.BoxBounds.Z);
	VerticalSpreadValue->SetText(FText::AsNumber(InBSConfig.BoxBounds.Z));
	ForwardSpreadCheckBox->SetIsChecked(InBSConfig.bMoveTargetsForward);
	BeatGridNumHorizontalTargetsSlider->SetValue(InBSConfig.NumHorizontalBeatGridTargets);
	BeatGridNumHorizontalTargetsSlider->SetMinValue(MinNumBeatGridHorizontalTargetsValue);
	BeatGridNumHorizontalTargetsSlider->SetMaxValue(MaxNumBeatGridHorizontalTargetsValue);
	BeatGridNumHorizontalTargetsValue->SetText(FText::AsNumber(InBSConfig.NumHorizontalBeatGridTargets));
	BeatGridNumVerticalTargetsSlider->SetValue(InBSConfig.NumVerticalBeatGridTargets);
	BeatGridNumVerticalTargetsSlider->SetMinValue(MinNumBeatGridVerticalTargetsValue);
	BeatGridNumVerticalTargetsSlider->SetMaxValue(MaxNumBeatGridVerticalTargetsValue);
	BeatGridNumVerticalTargetsValue->SetText(FText::AsNumber(InBSConfig.NumVerticalBeatGridTargets));
	
	if (InBSConfig.bMoveTargetsForward)
	{
		ForwardSpreadBox->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		ForwardSpreadBox->SetVisibility(ESlateVisibility::Collapsed);
	}
	
	ForwardSpreadSlider->SetValue(InBSConfig.MoveForwardDistance);
	ForwardSpreadValue->SetText(FText::AsNumber(InBSConfig.MoveForwardDistance));
	TargetScaleConstrained->UpdateDefaultValues(InBSConfig.MinTargetScale, InBSConfig.MaxTargetScale);
}

void UGameModesWidget::PopulateGameModeNameComboBox(const FString& GameModeOptionToSelect)
{
	GameModeNameComboBox->ClearOptions();
	TArray<FBSConfig> CustomGameModesArray = LoadCustomGameModes();
	for (const FBSConfig GameMode : DefaultModes)
	{
		GameModeNameComboBox->AddOption(UEnum::GetDisplayValueAsText(GameMode.DefaultMode).ToString());
	}
	for (const FBSConfig CustomGameMode : CustomGameModesArray)
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

FBSConfig UGameModesWidget::GetCustomGameModeOptions() const
{
	FBSConfig ReturnStruct;
	ReturnStruct.DefaultMode = EDefaultMode::Custom;
	if (IsDefaultGameMode(GameModeNameComboBox->GetSelectedOption()) && CustomGameModeETB->GetText().IsEmptyOrWhitespace())
	{
		ReturnStruct.CustomGameModeName = "";
	}
	else if (!CustomGameModeETB->GetText().IsEmptyOrWhitespace())
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
	ReturnStruct.PlayerDelay = FMath::GridSnap(FMath::Clamp(PlayerDelaySlider->GetValue(), MinPlayerDelayValue, MaxPlayerDelayValue), PlayerDelayGridSnapSize);
	ReturnStruct.TargetMaxLifeSpan = FMath::GridSnap(FMath::Clamp(LifespanSlider->GetValue(), MinLifespanValue, MaxLifespanValue), LifespanGridSnapSize);
	ReturnStruct.TargetSpawnCD = FMath::GridSnap(FMath::Clamp(TargetSpawnCDSlider->GetValue(), MinTargetSpawnCDValue, MaxTargetSpawnCDValue), TargetSpawnCDGridSnapSize);
	ReturnStruct.HeadshotHeight = HeadShotOnlyCheckBox->IsChecked();
	ReturnStruct.WallCentered = WallCenteredCheckBox->IsChecked();
	ReturnStruct.MinDistanceBetweenTargets = FMath::GridSnap(FMath::Clamp(MinTargetDistanceSlider->GetValue(), MinMinTargetDistanceValue, MaxMinTargetDistanceValue), MinTargetDistanceGridSnapSize);
	ReturnStruct.SpreadType = GetSpreadType();
	ReturnStruct.BoxBounds = FVector(0, FMath::GridSnap(FMath::Clamp(HorizontalSpreadSlider->GetValue(), MinHorizontalSpreadValue, MaxHorizontalSpreadValue), HorizontalSpreadGridSnapSize),
	                                 FMath::GridSnap(FMath::Clamp(VerticalSpreadSlider->GetValue(), MinVerticalSpreadValue, MaxVerticalSpreadValue), VerticalSpreadGridSnapSize));
	ReturnStruct.bMoveTargetsForward = ForwardSpreadCheckBox->IsChecked();
	ReturnStruct.MoveForwardDistance = FMath::GridSnap(FMath::Clamp(ForwardSpreadSlider->GetValue(), MinForwardSpreadValue, MaxForwardSpreadValue), HorizontalSpreadGridSnapSize);
	ReturnStruct.UseDynamicSizing = DynamicTargetScaleCheckBox->IsChecked();
	ReturnStruct.MinTargetScale = FMath::GridSnap(FMath::Clamp(TargetScaleConstrained->MinSlider->GetValue(), MinTargetScaleValue, MaxTargetScaleValue), TargetScaleSnapSize);
	ReturnStruct.MaxTargetScale = FMath::GridSnap(FMath::Clamp(TargetScaleConstrained->MaxSlider->GetValue(), MinTargetScaleValue, MaxTargetScaleValue), TargetScaleSnapSize);
	
	ReturnStruct.AIConfig.bEnableRLAgent = EnableAICheckBox->IsChecked();
	ReturnStruct.AIConfig.Alpha = FMath::GridSnap(FMath::Clamp(AIAlphaSlider->GetValue(), MinAlphaValue, MaxAlphaValue), AlphaSnapSize);
	ReturnStruct.AIConfig.Epsilon = FMath::GridSnap(FMath::Clamp(AIEpsilonSlider->GetValue(), MinEpsilonValue, MaxEpsilonValue), EpsilonSnapSize);
	ReturnStruct.AIConfig.Gamma = FMath::GridSnap(FMath::Clamp(AIGammaSlider->GetValue(), MinGammaValue, MaxGammaValue), GammaSnapSize);
	
	ReturnStruct.RandomizeBeatGrid = RandomizeNextBeatGridTargetCheckBox->IsChecked();
	ReturnStruct.BeatGridSpacing = FVector2D(
		FMath::GridSnap(FMath::Clamp(BeatGridSpacingConstrained->MinSlider->GetValue(), MinBeatGridHorizontalSpacingValue, MaxBeatGridHorizontalSpacingValue), BeatGridHorizontalSpacingSnapSize),
		FMath::GridSnap(FMath::Clamp(BeatGridSpacingConstrained->MaxSlider->GetValue(), MinBeatGridVerticalSpacingValue, MaxBeatGridVerticalSpacingValue), BeatGridVerticalSpacingSnapSize));
	ReturnStruct.NumHorizontalBeatGridTargets = FMath::GridSnap(FMath::Clamp(BeatGridNumHorizontalTargetsSlider->GetValue(), MinNumBeatGridHorizontalTargetsValue, MaxNumBeatGridHorizontalTargetsValue), NumBeatGridHorizontalTargetsSnapSize);
	ReturnStruct.NumVerticalBeatGridTargets = FMath::GridSnap(FMath::Clamp(BeatGridNumVerticalTargetsSlider->GetValue(), MinNumBeatGridVerticalTargetsValue, MaxNumBeatGridVerticalTargetsValue), NumBeatGridVerticalTargetsSnapSize);
	
	ReturnStruct.MinTrackingSpeed = FMath::GridSnap(FMath::Clamp(TargetSpeedConstrained->MinSlider->GetValue(), MinTargetSpeedValue, MaxTargetSpeedValue), TargetSpeedSnapSize);
	ReturnStruct.MaxTrackingSpeed = FMath::GridSnap(FMath::Clamp(TargetSpeedConstrained->MaxSlider->GetValue(), MinTargetSpeedValue, MaxTargetSpeedValue), TargetSpeedSnapSize);
	
	return ReturnStruct;
}

void UGameModesWidget::SaveCustomGameModeToSlot(FBSConfig& GameModeToSave)
{
	/* Get things like QTable that aren't populated in the menu */
	SetHiddenConfigParameters(GameModeToSave);

	TArray<FBSConfig> CustomGameModesArray = LoadCustomGameModes();

	const int32 NumRemoved = CustomGameModesArray.RemoveAll([&GameModeToSave](const FBSConfig& MatchingStruct)
	{
		return MatchingStruct.CustomGameModeName == GameModeToSave.CustomGameModeName;
	});
	
	UE_LOG(LogTemp, Display, TEXT("NumRemoved %d"), NumRemoved);
	
	CustomGameModesArray.Shrink();
	CustomGameModesArray.Add(GameModeToSave);
	SaveCustomGameMode(CustomGameModesArray);
	
	const TArray SavedText = {FText::FromString(GameModeToSave.CustomGameModeName), FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "GM_GameModeSavedText")};
	SavedTextWidget->SetSavedText(FText::Join(FText::FromString(" "), SavedText));
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
	if (IsDefaultGameMode(GameModeNameComboBox->GetSelectedOption()) && CustomGameModeETB->GetText().IsEmptyOrWhitespace())
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

	if (IsDefaultGameMode(GameModeNameComboBox->GetSelectedOption()) && !CustomGameModeETB->GetText().IsEmptyOrWhitespace())
	{
		SaveCustomButton->SetIsEnabled(true);
		SaveCustomAndStartButton->SetIsEnabled(true);
		StartCustomButton->SetIsEnabled(false);
		SaveCustomAndStartButton->SetIsEnabled(true);
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
	AudioSelectWidget->OnStartButtonClickedDelegate.BindLambda([this, bStartFromDefaultGameMode](const FAudioSelectStruct AudioSelectStruct)
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
			
			/* Get things like QTable that aren't populated in the menu */
			SetHiddenConfigParameters(BSConfig);
		}
		BSConfig.SongTitle = AudioSelectStruct.SongTitle;
		BSConfig.GameModeLength = AudioSelectStruct.SongLength;
		BSConfig.InAudioDevice = AudioSelectStruct.InAudioDevice;
		BSConfig.OutAudioDevice = AudioSelectStruct.OutAudioDevice;
		BSConfig.SongPath = AudioSelectStruct.SongPath;
		BSConfig.bPlaybackAudio = AudioSelectStruct.bPlaybackAudio;
		BSConfig.AudioFormat = AudioSelectStruct.AudioFormat;
		GameModeTransitionState.BSConfig = BSConfig;
		/* Override the player delay to zero if using Capture */
		if (BSConfig.AudioFormat == EAudioFormat::Capture)
		{
			BSConfig.PlayerDelay = 0.f;
		}
		OnGameModeStateChanged.Broadcast(GameModeTransitionState);
		AudioSelectWidget->FadeOut();
	});
	AudioSelectWidget->AddToViewport();
	AudioSelectWidget->FadeIn();
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
		if (UEnum::GetDisplayValueAsText(Spread).ToString().Equals(SelectedSpread))
		{
			return Spread;
		}
	}
	UE_LOG(LogTemp, Display, TEXT("Didn't get ESpreadType match"));
	return ESpreadType::None;
}

FBSConfig UGameModesWidget::FindDefaultGameMode(const FString& GameModeName) const
{
	for (const FBSConfig Mode : DefaultModes)
	{
		if (GameModeName.Equals(UEnum::GetDisplayValueAsText(Mode.DefaultMode).ToString()))
		{
			return Mode;
		}
	}
	return FBSConfig(EDefaultMode::Custom, EGameModeDifficulty::Normal);
}

FBSConfig UGameModesWidget::FindCustomGameMode(const FString& CustomGameModeName) const
{
	TArray<FBSConfig> CustomGameModesArray = LoadCustomGameModes();
	for (const FBSConfig Elem : CustomGameModesArray)
	{
		if (Elem.CustomGameModeName.Equals(CustomGameModeName))
		{
			return Elem;
		}
	}
	return FBSConfig(EDefaultMode::Custom, EGameModeDifficulty::Normal);
}

bool UGameModesWidget::IsDefaultGameMode(const FString& GameModeName) const
{
	for (const FBSConfig Mode : DefaultModes)
	{
		if (GameModeName.Equals(UEnum::GetDisplayValueAsText(Mode.DefaultMode).ToString()))
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
	TArray<FBSConfig> CustomGameModesArray = LoadCustomGameModes();
	for (const FBSConfig GameMode : CustomGameModesArray)
	{
		if (GameMode.CustomGameModeName.Equals(GameModeName))
		{
			return true;
		}
	}
	return false;
}

float UGameModesWidget::OnEditableTextBoxChanged(const FText& NewTextValue, UEditableTextBox* TextBoxToChange, USlider* SliderToChange, const float GridSnapSize, const float Min, const float Max) const
{
	const FString StringTextValue = UKismetStringLibrary::Replace(NewTextValue.ToString(), ",", "");
	const float ClampedValue = FMath::Clamp(FCString::Atof(*StringTextValue), Min, Max);
	const float SnappedValue = FMath::GridSnap(ClampedValue, GridSnapSize);
	TextBoxToChange->SetText(FText::AsNumber(SnappedValue));
	SliderToChange->SetValue(SnappedValue);
	return SnappedValue;
}

float UGameModesWidget::OnSliderChanged(const float NewValue, UEditableTextBox* TextBoxToChange, const float GridSnapSize) const
{
	const float ReturnValue = FMath::GridSnap(NewValue, GridSnapSize);
	TextBoxToChange->SetText(FText::AsNumber(ReturnValue));
	return ReturnValue;
}