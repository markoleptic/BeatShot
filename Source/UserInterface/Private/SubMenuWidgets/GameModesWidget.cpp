// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


// ReSharper disable CppMemberFunctionMayBeConst
#include "SubMenuWidgets/GameModesWidget.h"
#include "GlobalConstants.h"
#include "Blueprint/WidgetTree.h"
#include "Components/WidgetSwitcher.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "Components/Slider.h"
#include "Components/Border.h"
#include "Components/CheckBox.h"
#include "Components/EditableTextBox.h"
#include "Components/ComboBoxString.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Kismet/KismetStringLibrary.h"
#include "OverlayWidgets/PopupMessageWidget.h"
#include "OverlayWidgets/AudioSelectWidget.h"
#include "WidgetComponents/ConstrainedSlider.h"
#include "WidgetComponents/GameModeButton.h"
#include "WidgetComponents/SavedTextWidget.h"
#include "WidgetComponents/SlideRightButton.h"
#include "WidgetComponents/TooltipWidget.h"

using namespace Constants;

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
	TargetScaleSliderStruct.MinConstraintLower = MinValue_TargetScale;
	TargetScaleSliderStruct.MinConstraintUpper = MaxValue_TargetScale;
	TargetScaleSliderStruct.MaxConstraintLower = MinValue_TargetScale;
	TargetScaleSliderStruct.MaxConstraintUpper = MaxValue_TargetScale;
	TargetScaleSliderStruct.DefaultMinValue = MinValue_TargetScale;
	TargetScaleSliderStruct.DefaultMaxValue = MaxValue_TargetScale;
	TargetScaleSliderStruct.MaxText = FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "GM_MaxTargetScale");
	TargetScaleSliderStruct.MinText = FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "GM_MinTargetScale");
	TargetScaleSliderStruct.CheckboxText = FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "GM_ConstantTargetSize");
	TargetScaleSliderStruct.GridSnapSize = SnapSize_TargetScale;
	TargetScaleConstrained->InitConstrainedSlider(TargetScaleSliderStruct);

	/* BeatGrid Spacing TextBox and Slider */
	FConstrainedSliderStruct BeatGridSliderStruct;
	BeatGridSliderStruct.MinConstraintLower = MinValue_BeatGridHorizontalSpacing;
	BeatGridSliderStruct.MinConstraintUpper = MaxValue_BeatGridHorizontalSpacing;
	BeatGridSliderStruct.MaxConstraintLower = MinValue_BeatGridVerticalSpacing;
	BeatGridSliderStruct.MaxConstraintUpper = MaxValue_BeatGridVerticalSpacing;
	BeatGridSliderStruct.DefaultMinValue = MinValue_TargetScale;
	BeatGridSliderStruct.DefaultMaxValue = MaxValue_TargetScale;
	BeatGridSliderStruct.MinText = FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "GM_BeatGrid_HorizontalSpacing");
	BeatGridSliderStruct.MaxText = FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "GM_BeatGrid_VerticalSpacing");
	BeatGridSliderStruct.CheckboxText = FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "GM_EvenSpacing");
	BeatGridSliderStruct.GridSnapSize = SnapSize_BeatGridVerticalSpacing;
	BeatGridSliderStruct.bShowMaxLock = true;
	BeatGridSliderStruct.bShowMinLock = true;
	BeatGridSpacingConstrained->InitConstrainedSlider(BeatGridSliderStruct);
	
	/* BeatTrack target speed TextBox and Slider */
	FConstrainedSliderStruct TrackingSpeedSliderStruct;
	TrackingSpeedSliderStruct.MinConstraintLower = MinValue_TargetSpeed;
	TrackingSpeedSliderStruct.MinConstraintUpper = MaxValue_TargetSpeed;
	TrackingSpeedSliderStruct.MaxConstraintLower = MinValue_TargetSpeed;
	TrackingSpeedSliderStruct.MaxConstraintUpper = MaxValue_TargetSpeed;
	TrackingSpeedSliderStruct.DefaultMinValue = MinValue_TargetSpeed;
	TrackingSpeedSliderStruct.DefaultMaxValue = MaxValue_TargetSpeed;
	TrackingSpeedSliderStruct.MaxText = FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "GM_MaxTrackingSpeed");
	TrackingSpeedSliderStruct.MinText = FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "GM_MinTrackingSpeed");
	TrackingSpeedSliderStruct.CheckboxText = FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "GM_ConstantTrackingSpeed");
	TrackingSpeedSliderStruct.GridSnapSize = SnapSize_TargetSpeed;
	TargetSpeedConstrained->InitConstrainedSlider(TrackingSpeedSliderStruct);
	
	/* Tooltips */
	{
		SetTooltipWidget(ConstructTooltipWidget());
		AddToTooltipData(GameModeTemplateQMark, FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "GameModeTemplate"));
		AddToTooltipData(BaseGameModeQMark, FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "BaseGameMode"));
		AddToTooltipData(CustomGameModeNameQMark, FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "CustomGameModeName"));
		AddToTooltipData(GameModeDifficultyQMark, FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "GameModeDifficulty"));
		AddToTooltipData(SpawnBeatDelayQMark, FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "SpawnBeatDelay"));
		AddToTooltipData(LifespanQMark, FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "Lifespan"));
		AddToTooltipData(TargetSpawnCDQMark, FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "MinDistance"));
		AddToTooltipData(HeadshotHeightQMark, FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "HeadshotHeight"));
		AddToTooltipData(ForwardSpreadQMark, FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "ForwardSpread"));
		AddToTooltipData(CenterTargetsQMark, FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "CenterTargets"));
		AddToTooltipData(MinDistanceQMark, FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "MinDistance"));
		AddToTooltipData(SpreadTypeQMark, FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "SpreadType"));
		AddToTooltipData(DynamicTargetScaleQMark, FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "DynamicTargetScale"));
		AddToTooltipData(EnableAIQMark, FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "EnableAI"));
		AddToTooltipData(AlphaQMark, FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "Alpha"));
		AddToTooltipData(EpsilonQMark, FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "Epsilon"));
		AddToTooltipData(GammaQMark, FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "Gamma"));
		AddToTooltipData(TargetScaleConstrained->CheckboxQMark, FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "ConstantTargetScale"));
		AddToTooltipData(TargetSpeedConstrained->CheckboxQMark, FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "BeatTrackConstantSpeed"));
	}

	BindAllDelegates();
	OnButtonClicked_DefaultGameModes();
	PopulateGameModeNameComboBox("");
}

UTooltipWidget* UGameModesWidget::ConstructTooltipWidget()
{
	return CreateWidget<UTooltipWidget>(this, TooltipWidgetClass);
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
	TargetScaleConstrained->OnMaxValueChanged.AddUObject(BeatGridSpacingConstrained, &UBeatGridSettingsWidget::OnBeatGridUpdate_MaxTargetScale);
	BeatGridSpacingConstrained->OnBeatGridUpdate_SaveStartButtonStates.BindUObject(this, &UGameModesWidget::UpdateSaveStartButtonStates);
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
	OnSliderChanged(NewPlayerDelay, PlayerDelayValue, SnapSize_PlayerDelay);
}

void UGameModesWidget::OnSliderChanged_Lifespan(const float NewLifespan)
{
	OnSliderChanged(NewLifespan, LifespanValue, SnapSize_Lifespan);
}

void UGameModesWidget::OnSliderChanged_TargetSpawnCD(const float NewTargetSpawnCD)
{
	OnSliderChanged(NewTargetSpawnCD, TargetSpawnCDValue, SnapSize_TargetSpawnCD);
}

void UGameModesWidget::OnSliderChanged_MinTargetDistance(const float NewMinTargetDistance)
{
	OnSliderChanged(NewMinTargetDistance, MinTargetDistanceValue, SnapSize_MinTargetDistance);
}

void UGameModesWidget::OnSliderChanged_HorizontalSpread(const float NewHorizontalSpread)
{
	OnSliderChanged(NewHorizontalSpread, HorizontalSpreadValue, SnapSize_HorizontalSpread);
}

void UGameModesWidget::OnSliderChanged_VerticalSpread(const float NewVerticalSpread)
{
	OnSliderChanged(NewVerticalSpread, VerticalSpreadValue, SnapSize_VerticalSpread);
}

void UGameModesWidget::OnSliderChanged_ForwardSpread(const float NewForwardSpread)
{
	OnSliderChanged(NewForwardSpread, ForwardSpreadValue, SnapSize_HorizontalSpread);
}

void UGameModesWidget::OnSliderChanged_AIAlpha(const float NewAlpha)
{
	OnSliderChanged(NewAlpha, AIAlphaValue, SnapSize_Alpha);
}

void UGameModesWidget::OnSliderChanged_AIEpsilon(const float NewEpsilon)
{
	OnSliderChanged(NewEpsilon, AIEpsilonValue, SnapSize_Epsilon);
}

void UGameModesWidget::OnSliderChanged_AIGamma(const float NewGamma)
{
	OnSliderChanged(NewGamma, AIGammaValue, SnapSize_Gamma);
}

void UGameModesWidget::OnTextCommitted_PlayerDelay(const FText& NewPlayerDelay, ETextCommit::Type CommitType)
{
	OnEditableTextBoxChanged(NewPlayerDelay, PlayerDelayValue, PlayerDelaySlider, SnapSize_PlayerDelay, MinValue_PlayerDelay, MaxValue_PlayerDelay);
}

void UGameModesWidget::OnTextCommitted_Lifespan(const FText& NewLifespan, ETextCommit::Type CommitType)
{
	OnEditableTextBoxChanged(NewLifespan, LifespanValue, LifespanSlider, SnapSize_Lifespan, MinValue_Lifespan, MaxValue_Lifespan);
}

void UGameModesWidget::OnTextCommitted_TargetSpawnCD(const FText& NewTargetSpawnCD, ETextCommit::Type CommitType)
{
	OnEditableTextBoxChanged(NewTargetSpawnCD, TargetSpawnCDValue, TargetSpawnCDSlider, SnapSize_TargetSpawnCD,MinValue_TargetSpawnCD, MaxValue_TargetSpawnCD);
}

void UGameModesWidget::OnTextCommitted_MinTargetDistance(const FText& NewMinTargetDistance, ETextCommit::Type CommitType)
{
	OnEditableTextBoxChanged(NewMinTargetDistance,MinTargetDistanceValue, MinTargetDistanceSlider, SnapSize_MinTargetDistance,MinValue_MinTargetDistance, MaxValue_MinTargetDistance);
}

void UGameModesWidget::OnTextCommitted_HorizontalSpread(const FText& NewHorizontalSpread, ETextCommit::Type CommitType)
{
	OnEditableTextBoxChanged(NewHorizontalSpread, HorizontalSpreadValue, HorizontalSpreadSlider, SnapSize_HorizontalSpread, MinValue_HorizontalSpread, MaxValue_HorizontalSpread);
}

void UGameModesWidget::OnTextCommitted_VerticalSpread(const FText& NewVerticalSpread, ETextCommit::Type CommitType)
{
	OnEditableTextBoxChanged(NewVerticalSpread, VerticalSpreadValue, VerticalSpreadSlider, SnapSize_VerticalSpread, MinValue_VerticalSpread, MaxValue_VerticalSpread);
}

void UGameModesWidget::OnTextCommitted_ForwardSpread(const FText& NewForwardSpread, ETextCommit::Type CommitType)
{
	OnEditableTextBoxChanged(NewForwardSpread, ForwardSpreadValue, ForwardSpreadSlider, SnapSize_HorizontalSpread, MinValue_ForwardSpread, MaxValue_ForwardSpread);
}

void UGameModesWidget::OnTextCommitted_AIAlpha(const FText& NewAlpha, ETextCommit::Type CommitType)
{
	OnEditableTextBoxChanged(NewAlpha, AIAlphaValue, AIAlphaSlider, SnapSize_Alpha,MinValue_Alpha, MaxValue_Alpha);
}

void UGameModesWidget::OnTextCommitted_AIEpsilon(const FText& NewEpsilon, ETextCommit::Type CommitType)
{
	OnEditableTextBoxChanged(NewEpsilon, AIEpsilonValue, AIEpsilonSlider, SnapSize_Epsilon,MinValue_Epsilon, MaxValue_Epsilon);
}

void UGameModesWidget::OnTextCommitted_AIGamma(const FText& NewGamma, ETextCommit::Type CommitType)
{
	OnEditableTextBoxChanged(NewGamma, AIGammaValue, AIGammaSlider, SnapSize_Gamma,MinValue_Gamma, MaxValue_Gamma);
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
	OnEditableTextBoxChanged(FText::AsNumber(MaxValue_VerticalSpread), VerticalSpreadValue, VerticalSpreadSlider, SnapSize_VerticalSpread, MinValue_VerticalSpread, MaxValue_VerticalSpread);
	OnSliderChanged(MaxValue_VerticalSpread, VerticalSpreadValue, SnapSize_VerticalSpread);
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

void UGameModesWidget::PopulateGameModeOptions(const FBSConfig& InBSConfig)
{
	switch(InBSConfig.BaseGameMode)
	{
	case EDefaultMode::Custom:
		break;
	case EDefaultMode::SingleBeat:
		BaseGameModeComboBox->SetSelectedOption("SingleBeat");
		break;
	case EDefaultMode::MultiBeat:
		BaseGameModeComboBox->SetSelectedOption("MultiBeat");
		break;
	case EDefaultMode::BeatGrid:
		BeatGridSpecificSettings->SetVisibility(ESlateVisibility::Visible);
		BaseGameModeComboBox->SetSelectedOption("BeatGrid");
		HorizontalSpreadSlider->SetLocked(true);
		HorizontalSpreadValue->SetIsReadOnly(true);
		VerticalSpreadSlider->SetLocked(true);
		VerticalSpreadValue->SetIsReadOnly(true);
		BeatGridSpacingConstrained->InitializeBeatGrid(InBSConfig.BeatGridConfig, TargetScaleConstrained->TextTooltipBox_Max);
		BeatGridSpacingConstrained->OnBeatGridUpdate_MaxTargetScale(InBSConfig.TargetConfig.MaxTargetScale);
		MinTargetDistanceBox->SetVisibility(ESlateVisibility::Collapsed);
		SpreadTypeBox->SetVisibility(ESlateVisibility::Collapsed);
		AISpecificSettings->SetVisibility(ESlateVisibility::Collapsed);
		break;
	case EDefaultMode::BeatTrack:
		BeatTrackSpecificSettings->SetVisibility(ESlateVisibility::Visible);
		BaseGameModeComboBox->SetSelectedOption("BeatTrack");
		LifespanSlider->SetLocked(true);
		LifespanValue->SetIsReadOnly(true);
		TargetSpeedConstrained->UpdateDefaultValues(InBSConfig.BeatTrackConfig.MinTrackingSpeed, InBSConfig.BeatTrackConfig.MaxTrackingSpeed);
		MinTargetDistanceBox->SetVisibility(ESlateVisibility::Collapsed);
		SpreadTypeBox->SetVisibility(ESlateVisibility::Collapsed);
		AISpecificSettings->SetVisibility(ESlateVisibility::Collapsed);
		break;
	default:
		break;
	}


	if (InBSConfig.BaseGameMode != EDefaultMode::BeatGrid)
	{
		BeatGridSpecificSettings->SetVisibility(ESlateVisibility::Collapsed);
		HorizontalSpreadSlider->SetLocked(false);
		HorizontalSpreadValue->SetIsReadOnly(false);
		VerticalSpreadSlider->SetLocked(false);
		VerticalSpreadValue->SetIsReadOnly(false);
	}
	if (InBSConfig.BaseGameMode != EDefaultMode::BeatTrack)
	{
		BeatTrackSpecificSettings->SetVisibility(ESlateVisibility::Collapsed);
		LifespanSlider->SetLocked(false);
		LifespanValue->SetIsReadOnly(false);
	}
	if (InBSConfig.BaseGameMode != EDefaultMode::BeatTrack && InBSConfig.BaseGameMode != EDefaultMode::BeatGrid)
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

	if (InBSConfig.SpatialConfig.bMoveTargetsForward)
	{
		ForwardSpreadBox->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		ForwardSpreadBox->SetVisibility(ESlateVisibility::Collapsed);
	}
	
	GameModeDifficultyComboBox->SetSelectedOption(UEnum::GetDisplayValueAsText(InBSConfig.GameModeDifficulty).ToString());
	PlayerDelaySlider->SetValue(InBSConfig.AudioConfig.PlayerDelay);
	PlayerDelayValue->SetText(FText::AsNumber(InBSConfig.AudioConfig.PlayerDelay));
	LifespanSlider->SetValue(InBSConfig.TargetConfig.TargetMaxLifeSpan);
	LifespanValue->SetText(FText::AsNumber(InBSConfig.TargetConfig.TargetMaxLifeSpan));
	TargetSpawnCDSlider->SetValue(InBSConfig.TargetConfig.TargetSpawnCD);
	TargetSpawnCDValue->SetText(FText::AsNumber(InBSConfig.TargetConfig.TargetSpawnCD));
	HeadShotOnlyCheckBox->SetIsChecked(InBSConfig.SpatialConfig.bUseHeadshotHeight);
	WallCenteredCheckBox->SetIsChecked(InBSConfig.SpatialConfig.bUseCustomFloorDistance);
	MinTargetDistanceSlider->SetValue(InBSConfig.SpatialConfig.MinDistanceBetweenTargets);
	MinTargetDistanceValue->SetText(FText::AsNumber(InBSConfig.SpatialConfig.MinDistanceBetweenTargets));
	SpreadTypeComboBox->SetSelectedOption(UEnum::GetDisplayValueAsText(InBSConfig.SpatialConfig.SpreadType).ToString());
	HorizontalSpreadSlider->SetValue(InBSConfig.SpatialConfig.BoxBounds.Y);
	HorizontalSpreadValue->SetText(FText::AsNumber(InBSConfig.SpatialConfig.BoxBounds.Y));
	VerticalSpreadSlider->SetValue(InBSConfig.SpatialConfig.BoxBounds.Z);
	VerticalSpreadValue->SetText(FText::AsNumber(InBSConfig.SpatialConfig.BoxBounds.Z));
	ForwardSpreadCheckBox->SetIsChecked(InBSConfig.SpatialConfig.bMoveTargetsForward);
	ForwardSpreadSlider->SetValue(InBSConfig.SpatialConfig.MoveForwardDistance);
	ForwardSpreadValue->SetText(FText::AsNumber(InBSConfig.SpatialConfig.MoveForwardDistance));
	TargetScaleConstrained->UpdateDefaultValues(InBSConfig.TargetConfig.MinTargetScale, InBSConfig.TargetConfig.MaxTargetScale);
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

	for (const EDefaultMode Mode : TEnumRange<EDefaultMode>())
	{
		if (BaseGameModeComboBox->GetSelectedOption().Equals(UEnum::GetDisplayValueAsText(Mode).ToString()))
		{
			ReturnStruct.BaseGameMode = Mode;
		}
	}

	ReturnStruct.GameModeDifficulty = EGameModeDifficulty::None;
	ReturnStruct.AudioConfig.PlayerDelay = FMath::GridSnap(FMath::Clamp(PlayerDelaySlider->GetValue(), MinValue_PlayerDelay, MaxValue_PlayerDelay), SnapSize_PlayerDelay);
	ReturnStruct.TargetConfig.TargetMaxLifeSpan = FMath::GridSnap(FMath::Clamp(LifespanSlider->GetValue(), MinValue_Lifespan, MaxValue_Lifespan), SnapSize_Lifespan);
	ReturnStruct.TargetConfig.TargetSpawnCD = FMath::GridSnap(FMath::Clamp(TargetSpawnCDSlider->GetValue(), MinValue_TargetSpawnCD, MaxValue_TargetSpawnCD), SnapSize_TargetSpawnCD);
	ReturnStruct.SpatialConfig.bUseHeadshotHeight = HeadShotOnlyCheckBox->IsChecked();
	ReturnStruct.SpatialConfig.bUseCustomFloorDistance = WallCenteredCheckBox->IsChecked();
	ReturnStruct.SpatialConfig.MinDistanceBetweenTargets = FMath::GridSnap(FMath::Clamp(MinTargetDistanceSlider->GetValue(), MinValue_MinTargetDistance, MaxValue_MinTargetDistance), SnapSize_MinTargetDistance);
	ReturnStruct.SpatialConfig.SpreadType = GetSpreadType();
	ReturnStruct.SpatialConfig.BoxBounds = FVector(0, FMath::GridSnap(FMath::Clamp(HorizontalSpreadSlider->GetValue(), MinValue_HorizontalSpread, MaxValue_HorizontalSpread), SnapSize_HorizontalSpread),
	                                 FMath::GridSnap(FMath::Clamp(VerticalSpreadSlider->GetValue(), MinValue_VerticalSpread, MaxValue_VerticalSpread), SnapSize_VerticalSpread));
	ReturnStruct.SpatialConfig.bMoveTargetsForward = ForwardSpreadCheckBox->IsChecked();
	ReturnStruct.SpatialConfig.MoveForwardDistance = FMath::GridSnap(FMath::Clamp(ForwardSpreadSlider->GetValue(), MinValue_ForwardSpread, MaxValue_ForwardSpread), SnapSize_HorizontalSpread);
	ReturnStruct.TargetConfig.UseDynamicSizing = DynamicTargetScaleCheckBox->IsChecked();
	ReturnStruct.TargetConfig.MinTargetScale = FMath::GridSnap(FMath::Clamp(TargetScaleConstrained->MinSlider->GetValue(), MinValue_TargetScale, MaxValue_TargetScale), SnapSize_TargetScale);
	ReturnStruct.TargetConfig.MaxTargetScale = FMath::GridSnap(FMath::Clamp(TargetScaleConstrained->MaxSlider->GetValue(), MinValue_TargetScale, MaxValue_TargetScale), SnapSize_TargetScale);
	
	ReturnStruct.AIConfig.bEnableRLAgent = EnableAICheckBox->IsChecked();
	ReturnStruct.AIConfig.Alpha = FMath::GridSnap(FMath::Clamp(AIAlphaSlider->GetValue(), MinValue_Alpha, MaxValue_Alpha), SnapSize_Alpha);
	ReturnStruct.AIConfig.Epsilon = FMath::GridSnap(FMath::Clamp(AIEpsilonSlider->GetValue(), MinValue_Epsilon, MaxValue_Epsilon), SnapSize_Epsilon);
	ReturnStruct.AIConfig.Gamma = FMath::GridSnap(FMath::Clamp(AIGammaSlider->GetValue(), MinValue_Gamma, MaxValue_Gamma), SnapSize_Gamma);
	
	ReturnStruct.BeatGridConfig = BeatGridSpacingConstrained->GetBeatGridConfig();
	
	ReturnStruct.BeatTrackConfig.MinTrackingSpeed = FMath::GridSnap(FMath::Clamp(TargetSpeedConstrained->MinSlider->GetValue(), MinValue_TargetSpeed, MaxValue_TargetSpeed), SnapSize_TargetSpeed);
	ReturnStruct.BeatTrackConfig.MaxTrackingSpeed = FMath::GridSnap(FMath::Clamp(TargetSpeedConstrained->MaxSlider->GetValue(), MinValue_TargetSpeed, MaxValue_TargetSpeed), SnapSize_TargetSpeed);
	
	return ReturnStruct;
}

void UGameModesWidget::SaveCustomGameModeToSlot(FBSConfig& GameModeToSave)
{
	/* Get things like QTable that aren't populated in the menu */
	SetHiddenConfigParameters(GameModeToSave);

	TArray<FBSConfig> CustomGameModesArray = LoadCustomGameModes();

	CustomGameModesArray.RemoveAll([&GameModeToSave](const FBSConfig& MatchingStruct)
	{
		return MatchingStruct.CustomGameModeName == GameModeToSave.CustomGameModeName;
	});
	
	CustomGameModesArray.Add(GameModeToSave);
	SaveCustomGameMode(CustomGameModesArray);
	
	const TArray SavedText = {FText::FromString(GameModeToSave.CustomGameModeName), FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "GM_GameModeSavedText")};
	SavedTextWidget->SetSavedText(FText::Join(FText::FromString(" "), SavedText));
	SavedTextWidget->PlayFadeInFadeOut();
}

void UGameModesWidget::UpdateSaveStartButtonStates()
{
	const bool bNoSavedCustom = LoadCustomGameModes().IsEmpty();
	const bool bBeatGridMode = BaseGameModeComboBox->GetSelectedOption().Equals(UEnum::GetDisplayValueAsText(EDefaultMode::BeatGrid).ToString());
	const bool bBeatGridIsConstrained = BeatGridSpacingConstrained->IsAnyParameterConstrained();
	const bool bDefaultMode = IsDefaultGameMode(GameModeNameComboBox->GetSelectedOption());
	const bool bCustomMode = IsCustomGameMode(GameModeNameComboBox->GetSelectedOption());
	const bool bGameModeNameComboBoxEmpty = GameModeNameComboBox->GetSelectedOption().IsEmpty();
	const bool bCustomTextEmpty = CustomGameModeETB->GetText().IsEmptyOrWhitespace();
	
	if (bNoSavedCustom)
	{
		RemoveAllCustomButton->SetIsEnabled(false);
	}
	else
	{
		RemoveAllCustomButton->SetIsEnabled(true);
	}
	
	if (bBeatGridMode && bBeatGridIsConstrained)
	{
		SaveCustomButton->SetIsEnabled(false);
		SaveCustomAndStartButton->SetIsEnabled(false);
		StartCustomButton->SetIsEnabled(false);
		StartWithoutSavingButton->SetIsEnabled(false);
		return;
	}
	StartWithoutSavingButton->SetIsEnabled(true);
	
	if (bGameModeNameComboBoxEmpty || (bDefaultMode && bCustomTextEmpty))
	{
		SaveCustomButton->SetIsEnabled(false);
		SaveCustomAndStartButton->SetIsEnabled(false);
		StartCustomButton->SetIsEnabled(false);
		RemoveSelectedCustomButton->SetIsEnabled(false);
		return;
	}

	if (bCustomMode)
	{
		SaveCustomButton->SetIsEnabled(true);
		SaveCustomAndStartButton->SetIsEnabled(true);
		StartCustomButton->SetIsEnabled(true);
		RemoveSelectedCustomButton->SetIsEnabled(true);
		return;
	}

	if (bDefaultMode && !bCustomTextEmpty)
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
		BSConfig.AudioConfig.SongTitle = AudioSelectStruct.SongTitle;
		BSConfig.AudioConfig.SongLength = AudioSelectStruct.SongLength;
		BSConfig.AudioConfig.InAudioDevice = AudioSelectStruct.InAudioDevice;
		BSConfig.AudioConfig.OutAudioDevice = AudioSelectStruct.OutAudioDevice;
		BSConfig.AudioConfig.SongPath = AudioSelectStruct.SongPath;
		BSConfig.AudioConfig.bPlaybackAudio = AudioSelectStruct.bPlaybackAudio;
		BSConfig.AudioConfig.AudioFormat = AudioSelectStruct.AudioFormat;
		GameModeTransitionState.BSConfig = BSConfig;
		/* Override the player delay to zero if using Capture */
		if (BSConfig.AudioConfig.AudioFormat == EAudioFormat::Capture)
		{
			BSConfig.AudioConfig.PlayerDelay = 0.f;
		}
		OnGameModeStateChanged.Broadcast(GameModeTransitionState);
		AudioSelectWidget->FadeOut();
	});
	AudioSelectWidget->AddToViewport();
	AudioSelectWidget->FadeIn();
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