// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

// ReSharper disable CppMemberFunctionMayBeConst
#include "SubMenuWidgets/GameModesWidget_TargetConfig.h"
#include "WidgetComponents/DoubleSyncedSliderAndTextBox.h"
#include "GlobalConstants.h"
#include "BSWidgetInterface.h"
#include "Components/CheckBox.h"
#include "Components/ComboBoxString.h"
#include "Components/EditableTextBox.h"
#include "Components/Slider.h"
#include "WidgetComponents/BSComboBoxEntry.h"
#include "WidgetComponents/BSComboBoxString.h"
#include "WidgetComponents/BSHorizontalBox.h"
#include "WidgetComponents/BSVerticalBox.h"

using namespace Constants;

TSharedRef<SWidget> UGameModesWidget_TargetConfig::RebuildWidget()
{
	if (!IsDesignTime())
	{
		/* Create MainContainer before calling Super NativeConstruct since the parent calls InitSettingCategoryWidget in NativeConstruct */
		TargetScaleConstrained = CreateWidget<UDoubleSyncedSliderAndTextBox>(this, TargetScaleConstrainedClass);
		TargetSpeedConstrained = CreateWidget<UDoubleSyncedSliderAndTextBox>(this, TargetSpeedConstrainedClass);
	
		int32 NewIndex = MainContainer->GetChildIndex(BSBox_LifetimeTargetScalePolicy) - 1;
		MainContainer->InsertChildAt(NewIndex, Cast<UWidget>(TargetScaleConstrained.Get()));
	
		NewIndex = MainContainer->GetChildIndex(BSBox_MoveTargets) + 1;
		MainContainer->InsertChildAt(NewIndex, Cast<UWidget>(TargetSpeedConstrained.Get()));
	}
	return Super::RebuildWidget();
}

void UGameModesWidget_TargetConfig::NativeConstruct()
{
	Super::NativeConstruct();
	
	/* Target Scale TextBox and Slider */
	FSyncedSlidersParams TargetScaleSliderStruct;
	TargetScaleSliderStruct.MinConstraintLower = MinValue_TargetScale;
	TargetScaleSliderStruct.MinConstraintUpper = MaxValue_TargetScale;
	TargetScaleSliderStruct.MaxConstraintLower = MinValue_TargetScale;
	TargetScaleSliderStruct.MaxConstraintUpper = MaxValue_TargetScale;
	TargetScaleSliderStruct.DefaultMinValue = MinValue_TargetScale;
	TargetScaleSliderStruct.DefaultMaxValue = MaxValue_TargetScale;
	TargetScaleSliderStruct.MaxText = FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "GM_MaxTargetScale");
	TargetScaleSliderStruct.MinText = FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "GM_MinTargetScale");
	TargetScaleSliderStruct.GridSnapSize = SnapSize_TargetScale;
	TargetScaleSliderStruct.bShowCheckBox = false;
	TargetScaleSliderStruct.bShowMinLock = true;
	TargetScaleSliderStruct.bShowMaxLock = true;
	TargetScaleSliderStruct.bSyncSlidersAndValues = true;
	TargetScaleSliderStruct.bLocksOnlySync = true;
	TargetScaleConstrained->InitConstrainedSlider(TargetScaleSliderStruct);

	/* BeatTrack target speed TextBox and Slider */
	FSyncedSlidersParams TrackingSpeedSliderStruct;
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
	TrackingSpeedSliderStruct.bShowCheckBox = false;
	TrackingSpeedSliderStruct.bShowMinLock = true;
	TrackingSpeedSliderStruct.bShowMaxLock = true;
	TrackingSpeedSliderStruct.bSyncSlidersAndValues = true;
	TrackingSpeedSliderStruct.bLocksOnlySync = true;
	TargetSpeedConstrained->InitConstrainedSlider(TrackingSpeedSliderStruct);
	
	SetupTooltip(QMark_Lifespan, GetTooltipTextFromKey("Lifespan"));
	SetupTooltip(QMark_TargetSpawnCD, GetTooltipTextFromKey("MinDistance"));
	SetupTooltip(QMark_SpawnBeatDelay, GetTooltipTextFromKey("SpawnBeatDelay"));
	SetupTooltip(QMark_FloorDistance, GetTooltipTextFromKey("FloorDistance"));
	SetupTooltip(QMark_ForwardSpread, GetTooltipTextFromKey("ForwardSpread"));
	SetupTooltip(QMark_MoveTargetsForward, GetTooltipTextFromKey("MoveTargetsForward"));
	SetupTooltip(QMark_MinDistance, GetTooltipTextFromKey("MinDistance"));
	SetupTooltip(QMark_MaxHealth, GetTooltipTextFromKey("MaxHealth"));
	SetupTooltip(QMark_ConsecutiveChargeScaleMultiplier, GetTooltipTextFromKey("ConsecutiveChargeScaleMultiplier"));
	SetupTooltip(QMark_ApplyImmunityOnSpawn, GetTooltipTextFromKey("ApplyImmunityOnSpawn"));
	SetupTooltip(QMark_MaxNumRecentTargets, GetTooltipTextFromKey("MaxNumRecentTargets"));
	SetupTooltip(QMark_RecentTargetTimeLength, GetTooltipTextFromKey("RecentTargetTimeLength"));
	SetupTooltip(QMark_ExpirationHealthPenalty, GetTooltipTextFromKey("ExpirationHealthPenalty"));
	SetupTooltip(QMark_BoundsScalingPolicy, GetTooltipTextFromKey("BoundsScalingPolicy"));
	SetupTooltip(QMark_ConsecutiveTargetScalePolicy, GetTooltipTextFromKey("ConsecutiveTargetScalePolicy"));
	SetupTooltip(QMark_LifetimeTargetScalePolicy, GetTooltipTextFromKey("LifetimeTargetScalePolicy"));
	SetupTooltip(QMark_RecentTargetMemoryPolicy, GetTooltipTextFromKey("RecentTargetMemoryPolicy"));
	SetupTooltip(QMark_TargetActivationSelectionPolicy, GetTooltipTextFromKey("TargetActivationSelectionPolicy"));
	SetupTooltip(QMark_TargetDamageType, GetTooltipTextFromKey("TargetDamageType"));
	SetupTooltip(QMark_TargetDistributionPolicy, GetTooltipTextFromKey("TargetDistributionPolicy"));
	SetupTooltip(QMark_TargetSpawningPolicy, GetTooltipTextFromKey("TargetSpawningPolicy"));
	SetupTooltip(QMark_TargetActivationResponses, GetTooltipTextFromKey("TargetActivationResponses"));
	SetupTooltip(QMark_TargetDeactivationConditions, GetTooltipTextFromKey("TargetDeactivationConditions"));
	SetupTooltip(QMark_TargetDeactivationResponses, GetTooltipTextFromKey("TargetDeactivationResponses"));
	SetupTooltip(QMark_TargetDestructionConditions, GetTooltipTextFromKey("TargetDestructionConditions"));
	SetupTooltip(QMark_ContinuouslySpawn, GetTooltipTextFromKey("ContinuouslySpawn"));
	SetupTooltip(QMark_MoveTargets, GetTooltipTextFromKey("MoveTargets"));
	SetupTooltip(QMark_SpawnAtOriginWheneverPossible, GetTooltipTextFromKey("SpawnAtOriginWheneverPossible"));
	SetupTooltip(QMark_SpawnEveryOtherTargetInCenter, GetTooltipTextFromKey("SpawnEveryOtherTargetInCenter"));
	SetupTooltip(QMark_NumUpfrontTargetsToSpawn, GetTooltipTextFromKey("NumUpfrontTargetsToSpawn"));
	
	Slider_Lifespan->OnValueChanged.AddDynamic(this, &ThisClass::OnSliderChanged_Lifespan);
	Slider_TargetSpawnCD->OnValueChanged.AddDynamic(this, &ThisClass::OnSliderChanged_TargetSpawnCD);
	Slider_SpawnBeatDelay->OnValueChanged.AddDynamic(this, &ThisClass::OnSliderChanged_SpawnBeatDelay);
	Slider_ConsecutiveChargeScaleMultiplier->OnValueChanged.AddDynamic(this, &ThisClass::OnSliderChanged_ChargeScaleMultiplier);
	Slider_MinTargetDistance->OnValueChanged.AddDynamic(this, &ThisClass::OnSliderChanged_MinTargetDistance);
	Slider_HorizontalSpread->OnValueChanged.AddDynamic(this, &ThisClass::OnSliderChanged_HorizontalSpread);
	Slider_VerticalSpread->OnValueChanged.AddDynamic(this, &ThisClass::OnSliderChanged_VerticalSpread);
	Slider_ForwardSpread->OnValueChanged.AddDynamic(this, &ThisClass::OnSliderChanged_ForwardSpread);
	Slider_FloorDistance->OnValueChanged.AddDynamic(this, &ThisClass::OnSliderChanged_FloorDistance);
	Slider_MaxNumRecentTargets->OnValueChanged.AddDynamic(this, &ThisClass::OnSliderChanged_MaxNumRecentTargets);
	Slider_RecentTargetTimeLength->OnValueChanged.AddDynamic(this, &ThisClass::OnSliderChanged_RecentTargetTimeLength);
	Slider_NumUpfrontTargetsToSpawn->OnValueChanged.AddDynamic(this, &ThisClass::OnSliderChanged_NumUpfrontTargetsToSpawn);
	Slider_ExpirationHealthPenalty->OnValueChanged.AddDynamic(this, &ThisClass::OnSliderChanged_ExpirationHealthPenalty);
	Slider_MaxHealth->OnValueChanged.AddDynamic(this, &ThisClass::OnSliderChanged_MaxHealth);
	
	Value_Lifespan->OnTextCommitted.AddDynamic(this, &ThisClass::OnTextCommitted_Lifespan);
	Value_TargetSpawnCD->OnTextCommitted.AddDynamic(this, &ThisClass::OnTextCommitted_TargetSpawnCD);
	Value_SpawnBeatDelay->OnTextCommitted.AddDynamic(this, &ThisClass::OnTextCommitted_SpawnBeatDelay);
	Value_ConsecutiveChargeScaleMultiplier->OnTextCommitted.AddDynamic(this, &ThisClass::OnTextCommitted_ChargeScaleMultiplier);
	Value_MinTargetDistance->OnTextCommitted.AddDynamic(this, &ThisClass::OnTextCommitted_MinTargetDistance);
	Value_HorizontalSpread->OnTextCommitted.AddDynamic(this, &ThisClass::OnTextCommitted_HorizontalSpread);
	Value_VerticalSpread->OnTextCommitted.AddDynamic(this, &ThisClass::OnTextCommitted_VerticalSpread);
	Value_ForwardSpread->OnTextCommitted.AddDynamic(this, &ThisClass::OnTextCommitted_ForwardSpread);
	Value_FloorDistance->OnTextCommitted.AddDynamic(this, &ThisClass::OnTextCommitted_FloorDistance);
	Value_MaxNumRecentTargets->OnTextCommitted.AddDynamic(this, &ThisClass::OnTextCommitted_MaxNumRecentTargets);
	Value_RecentTargetTimeLength->OnTextCommitted.AddDynamic(this, &ThisClass::OnTextCommitted_RecentTargetTimeLength);
	Value_NumUpfrontTargetsToSpawn->OnTextCommitted.AddDynamic(this, &ThisClass::OnTextCommitted_NumUpfrontTargetsToSpawn);
	Value_ExpirationHealthPenalty->OnTextCommitted.AddDynamic(this, &ThisClass::OnTextCommitted_ExpirationHealthPenalty);
	Value_MaxHealth->OnTextCommitted.AddDynamic(this, &ThisClass::OnTextCommitted_MaxHealth);

	CheckBox_ApplyImmunityOnSpawn->OnCheckStateChanged.AddDynamic(this, &ThisClass::OnCheckStateChanged_ApplyImmunityOnSpawn);
	CheckBox_MoveTargetsForward->OnCheckStateChanged.AddDynamic(this, &ThisClass::OnCheckStateChanged_MoveTargetsForward);
	CheckBox_ContinuouslySpawn->OnCheckStateChanged.AddDynamic(this, &ThisClass::OnCheckStateChanged_ContinuouslySpawn);
	CheckBox_MoveTargets->OnCheckStateChanged.AddDynamic(this, &ThisClass::OnCheckStateChanged_MoveTargets);
	CheckBox_SpawnAtOriginWheneverPossible->OnCheckStateChanged.AddDynamic(this, &ThisClass::OnCheckStateChanged_SpawnAtOriginWheneverPossible);
	CheckBox_SpawnEveryOtherTargetInCenter->OnCheckStateChanged.AddDynamic(this, &ThisClass::OnCheckStateChanged_SpawnEveryOtherTargetInCenter);

	ComboBox_BoundsScalingPolicy->OnSelectionChanged.AddDynamic(this, &ThisClass::OnSelectionChanged_BoundsScalingPolicy);
	ComboBox_ConsecutiveTargetScalePolicy->OnSelectionChanged.AddDynamic(this, &ThisClass::OnSelectionChanged_ConsecutiveTargetScalePolicy);
	ComboBox_LifetimeTargetScalePolicy->OnSelectionChanged.AddDynamic(this, &ThisClass::OnSelectionChanged_LifetimeTargetScalePolicy);
	ComboBox_RecentTargetMemoryPolicy->OnSelectionChanged.AddDynamic(this, &ThisClass::OnSelectionChanged_RecentTargetMemoryPolicy);
	ComboBox_TargetActivationSelectionPolicy->OnSelectionChanged.AddDynamic(this, &ThisClass::OnSelectionChanged_TargetActivationSelectionPolicy);
	ComboBox_TargetDamageType->OnSelectionChanged.AddDynamic(this, &ThisClass::OnSelectionChanged_TargetDamageType);
	ComboBox_TargetDistributionPolicy->OnSelectionChanged.AddDynamic(this, &ThisClass::OnSelectionChanged_TargetDistributionPolicy);
	ComboBox_TargetSpawningPolicy->OnSelectionChanged.AddDynamic(this, &ThisClass::OnSelectionChanged_TargetSpawningPolicy);
	ComboBox_TargetActivationResponses->OnSelectionChanged.AddDynamic(this, &ThisClass::OnSelectionChanged_TargetActivationResponses);
	ComboBox_TargetDeactivationConditions->OnSelectionChanged.AddDynamic(this, &ThisClass::OnSelectionChanged_TargetDeactivationConditions);
	ComboBox_TargetDeactivationResponses->OnSelectionChanged.AddDynamic(this, &ThisClass::OnSelectionChanged_TargetDeactivationResponses);
	ComboBox_TargetDestructionConditions->OnSelectionChanged.AddDynamic(this, &ThisClass::OnSelectionChanged_TargetDestructionConditions);
	
	ComboBox_BoundsScalingPolicy->OnGenerateWidgetEventDelegate.BindDynamic(this, &ThisClass::OnGenerateWidgetEvent);
	ComboBox_ConsecutiveTargetScalePolicy->OnGenerateWidgetEventDelegate.BindDynamic(this, &ThisClass::OnGenerateWidgetEvent);
	ComboBox_LifetimeTargetScalePolicy->OnGenerateWidgetEventDelegate.BindDynamic(this, &ThisClass::OnGenerateWidgetEvent);
	ComboBox_RecentTargetMemoryPolicy->OnGenerateWidgetEventDelegate.BindDynamic(this, &ThisClass::OnGenerateWidgetEvent);
	ComboBox_TargetActivationSelectionPolicy->OnGenerateWidgetEventDelegate.BindDynamic(this, &ThisClass::OnGenerateWidgetEvent);
	ComboBox_TargetDamageType->OnGenerateWidgetEventDelegate.BindDynamic(this, &ThisClass::OnGenerateWidgetEvent);
	ComboBox_TargetDistributionPolicy->OnGenerateWidgetEventDelegate.BindDynamic(this, &ThisClass::OnGenerateWidgetEvent);
	ComboBox_TargetSpawningPolicy->OnGenerateWidgetEventDelegate.BindDynamic(this, &ThisClass::OnGenerateWidgetEvent);
	ComboBox_TargetActivationResponses->OnGenerateWidgetEventDelegate.BindDynamic(this, &ThisClass::OnGenerateWidgetEvent);
	ComboBox_TargetDeactivationConditions->OnGenerateWidgetEventDelegate.BindDynamic(this, &ThisClass::OnGenerateWidgetEvent);
	ComboBox_TargetDeactivationResponses->OnGenerateWidgetEventDelegate.BindDynamic(this, &ThisClass::OnGenerateWidgetEvent);
	ComboBox_TargetDestructionConditions->OnGenerateWidgetEventDelegate.BindDynamic(this, &ThisClass::OnGenerateWidgetEvent);

	ComboBox_BoundsScalingPolicy->OnSelectionChanged_GenerateWidgetForMultiSelection.BindDynamic(this, &ThisClass::OnSelectionChanged_GenerateMultiSelectionItem);
	ComboBox_ConsecutiveTargetScalePolicy->OnSelectionChanged_GenerateWidgetForMultiSelection.BindDynamic(this, &ThisClass::OnSelectionChanged_GenerateMultiSelectionItem);
	ComboBox_LifetimeTargetScalePolicy->OnSelectionChanged_GenerateWidgetForMultiSelection.BindDynamic(this, &ThisClass::OnSelectionChanged_GenerateMultiSelectionItem);
	ComboBox_RecentTargetMemoryPolicy->OnSelectionChanged_GenerateWidgetForMultiSelection.BindDynamic(this, &ThisClass::OnSelectionChanged_GenerateMultiSelectionItem);
	ComboBox_TargetActivationSelectionPolicy->OnSelectionChanged_GenerateWidgetForMultiSelection.BindDynamic(this, &ThisClass::OnSelectionChanged_GenerateMultiSelectionItem);
	ComboBox_TargetDamageType->OnSelectionChanged_GenerateWidgetForMultiSelection.BindDynamic(this, &ThisClass::OnSelectionChanged_GenerateMultiSelectionItem);
	ComboBox_TargetDistributionPolicy->OnSelectionChanged_GenerateWidgetForMultiSelection.BindDynamic(this, &ThisClass::OnSelectionChanged_GenerateMultiSelectionItem);
	ComboBox_TargetSpawningPolicy->OnSelectionChanged_GenerateWidgetForMultiSelection.BindDynamic(this, &ThisClass::OnSelectionChanged_GenerateMultiSelectionItem);
	ComboBox_TargetActivationResponses->OnSelectionChanged_GenerateWidgetForMultiSelection.BindDynamic(this, &ThisClass::OnSelectionChanged_GenerateMultiSelectionItem);
	ComboBox_TargetDeactivationConditions->OnSelectionChanged_GenerateWidgetForMultiSelection.BindDynamic(this, &ThisClass::OnSelectionChanged_GenerateMultiSelectionItem);
	ComboBox_TargetDeactivationResponses->OnSelectionChanged_GenerateWidgetForMultiSelection.BindDynamic(this, &ThisClass::OnSelectionChanged_GenerateMultiSelectionItem);
	ComboBox_TargetDestructionConditions->OnSelectionChanged_GenerateWidgetForMultiSelection.BindDynamic(this, &ThisClass::OnSelectionChanged_GenerateMultiSelectionItem);
	
	for (const EBoundsScalingPolicy& Method : TEnumRange<EBoundsScalingPolicy>())
	{
		ComboBox_BoundsScalingPolicy->AddOption(UEnum::GetDisplayValueAsText(Method).ToString());
	}
	for (const EConsecutiveTargetScalePolicy& Method : TEnumRange<EConsecutiveTargetScalePolicy>())
	{
		ComboBox_ConsecutiveTargetScalePolicy->AddOption(UEnum::GetDisplayValueAsText(Method).ToString());
	}
	for (const ELifetimeTargetScalePolicy& Method : TEnumRange<ELifetimeTargetScalePolicy>())
	{
		ComboBox_LifetimeTargetScalePolicy->AddOption(UEnum::GetDisplayValueAsText(Method).ToString());
	}
	for (const ERecentTargetMemoryPolicy& Method : TEnumRange<ERecentTargetMemoryPolicy>())
	{
		ComboBox_RecentTargetMemoryPolicy->AddOption(UEnum::GetDisplayValueAsText(Method).ToString());
	}
	for (const ETargetActivationSelectionPolicy& Method : TEnumRange<ETargetActivationSelectionPolicy>())
	{
		ComboBox_TargetActivationSelectionPolicy->AddOption(UEnum::GetDisplayValueAsText(Method).ToString());
	}
	for (const ETargetDamageType& Method : TEnumRange<ETargetDamageType>())
	{
		ComboBox_TargetDamageType->AddOption(UEnum::GetDisplayValueAsText(Method).ToString());
	}
	for (const ETargetDistributionPolicy& Method : TEnumRange<ETargetDistributionPolicy>())
	{
		ComboBox_TargetDistributionPolicy->AddOption(UEnum::GetDisplayValueAsText(Method).ToString());
	}
	for (const ETargetSpawningPolicy& Method : TEnumRange<ETargetSpawningPolicy>())
	{
		ComboBox_TargetSpawningPolicy->AddOption(UEnum::GetDisplayValueAsText(Method).ToString());
	}
	for (const ETargetActivationResponse& Method : TEnumRange<ETargetActivationResponse>())
	{
		ComboBox_TargetActivationResponses->AddOption(UEnum::GetDisplayValueAsText(Method).ToString());
	}
	for (const ETargetDeactivationCondition& Method : TEnumRange<ETargetDeactivationCondition>())
	{
		ComboBox_TargetDeactivationConditions->AddOption(UEnum::GetDisplayValueAsText(Method).ToString());
	}
	for (const ETargetDeactivationResponse& Method : TEnumRange<ETargetDeactivationResponse>())
	{
		ComboBox_TargetDeactivationResponses->AddOption(UEnum::GetDisplayValueAsText(Method).ToString());
	}
	for (const ETargetDestructionCondition& Method : TEnumRange<ETargetDestructionCondition>())
	{
		ComboBox_TargetDestructionConditions->AddOption(UEnum::GetDisplayValueAsText(Method).ToString());
	}
}

void UGameModesWidget_TargetConfig::InitSettingCategoryWidget()
{
	Super::InitSettingCategoryWidget();
}

void UGameModesWidget_TargetConfig::InitializeTargetConfig(const FBS_TargetConfig& InTargetConfig, const EBaseGameMode& BaseGameMode)
{
	/*switch(BaseGameMode)
	{
	case EBaseGameMode::SingleBeat:
		BSBox_ConsecutiveChargeScaleMultiplier->SetVisibility(ESlateVisibility::Collapsed);
		BSBox_MinTargetDistance->SetVisibility(ESlateVisibility::Visible);
		BSBox_BoundsScalingPolicy->SetVisibility(ESlateVisibility::Visible);
		Slider_HorizontalSpread->SetLocked(false);
		Value_HorizontalSpread->SetIsReadOnly(false);
		Slider_VerticalSpread->SetLocked(false);
		Value_VerticalSpread->SetIsReadOnly(false);
		break;
	case EBaseGameMode::MultiBeat:
		BSBox_ConsecutiveChargeScaleMultiplier->SetVisibility(ESlateVisibility::Collapsed);
		BSBox_MinTargetDistance->SetVisibility(ESlateVisibility::Visible);
		BSBox_BoundsScalingPolicy->SetVisibility(ESlateVisibility::Visible);
		Slider_HorizontalSpread->SetLocked(false);
		Value_HorizontalSpread->SetIsReadOnly(false);
		Slider_VerticalSpread->SetLocked(false);
		Value_VerticalSpread->SetIsReadOnly(false);
		break;
	case EBaseGameMode::BeatGrid:
		BSBox_ConsecutiveChargeScaleMultiplier->SetVisibility(ESlateVisibility::Collapsed);
		Slider_HorizontalSpread->SetLocked(true);
		Value_HorizontalSpread->SetIsReadOnly(true);
		Slider_VerticalSpread->SetLocked(true);
		Value_VerticalSpread->SetIsReadOnly(true);
		BSBox_MinTargetDistance->SetVisibility(ESlateVisibility::Collapsed);
		BSBox_BoundsScalingPolicy->SetVisibility(ESlateVisibility::Collapsed);
		break;
	case EBaseGameMode::BeatTrack:
		BSBox_ConsecutiveChargeScaleMultiplier->SetVisibility(ESlateVisibility::Collapsed);
		BSBox_MinTargetDistance->SetVisibility(ESlateVisibility::Collapsed);
		BSBox_BoundsScalingPolicy->SetVisibility(ESlateVisibility::Collapsed);
		break;
	case EBaseGameMode::ChargedBeatTrack:
		BSBox_ConsecutiveChargeScaleMultiplier->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		break;
	case EBaseGameMode::None:
		break;
	}*/

	Slider_FloorDistance->SetValue(InTargetConfig.FloorDistance);
	Slider_MinTargetDistance->SetValue(InTargetConfig.MinDistanceBetweenTargets);
	Slider_HorizontalSpread->SetValue(InTargetConfig.BoxBounds.Y);
	Slider_VerticalSpread->SetValue(InTargetConfig.BoxBounds.Z);
	Slider_ForwardSpread->SetValue(InTargetConfig.MoveForwardDistance);
	Slider_Lifespan->SetValue(InTargetConfig.TargetMaxLifeSpan);
	Slider_TargetSpawnCD->SetValue(InTargetConfig.TargetSpawnCD);
	Slider_SpawnBeatDelay->SetValue(InTargetConfig.SpawnBeatDelay);
	Slider_ConsecutiveChargeScaleMultiplier->SetValue(InTargetConfig.ConsecutiveChargeScaleMultiplier);
	Slider_MaxNumRecentTargets->SetValue(InTargetConfig.MaxNumRecentTargets);
	Slider_RecentTargetTimeLength->SetValue(InTargetConfig.RecentTargetTimeLength);
	Slider_NumUpfrontTargetsToSpawn->SetValue(InTargetConfig.NumUpfrontTargetsToSpawn);
	Slider_ExpirationHealthPenalty->SetValue(InTargetConfig.ExpirationHealthPenalty);
	Slider_MaxHealth->SetValue(InTargetConfig.MaxHealth);
	
	Value_FloorDistance->SetText(FText::AsNumber(InTargetConfig.FloorDistance));
	Value_MinTargetDistance->SetText(FText::AsNumber(InTargetConfig.MinDistanceBetweenTargets));
	Value_HorizontalSpread->SetText(FText::AsNumber(InTargetConfig.BoxBounds.Y));
	Value_VerticalSpread->SetText(FText::AsNumber(InTargetConfig.BoxBounds.Z));
	Value_ForwardSpread->SetText(FText::AsNumber(InTargetConfig.MoveForwardDistance));
	Value_Lifespan->SetText(FText::AsNumber(InTargetConfig.TargetMaxLifeSpan));
	Value_TargetSpawnCD->SetText(FText::AsNumber(InTargetConfig.TargetSpawnCD));
	Value_SpawnBeatDelay->SetText(FText::AsNumber(InTargetConfig.SpawnBeatDelay));
	Value_ConsecutiveChargeScaleMultiplier->SetText(FText::AsNumber(InTargetConfig.ConsecutiveChargeScaleMultiplier));
	Value_MaxNumRecentTargets->SetText(FText::AsNumber(InTargetConfig.MaxNumRecentTargets));
	Value_RecentTargetTimeLength->SetText(FText::AsNumber(InTargetConfig.RecentTargetTimeLength));
	Value_NumUpfrontTargetsToSpawn->SetText(FText::AsNumber(InTargetConfig.NumUpfrontTargetsToSpawn));
	Value_ExpirationHealthPenalty->SetText(FText::AsNumber(InTargetConfig.ExpirationHealthPenalty));
	Value_MaxHealth->SetText(FText::AsNumber(InTargetConfig.MaxHealth));
	
	CheckBox_MoveTargetsForward->SetIsChecked(InTargetConfig.bMoveTargetsForward);
	CheckBox_ApplyImmunityOnSpawn->SetIsChecked(InTargetConfig.bApplyImmunityOnSpawn);
	CheckBox_ContinuouslySpawn->SetIsChecked(InTargetConfig.bContinuouslySpawn);
	CheckBox_MoveTargets->SetIsChecked(InTargetConfig.bMoveTargets);
	CheckBox_SpawnAtOriginWheneverPossible->SetIsChecked(InTargetConfig.bSpawnAtOriginWheneverPossible);
	CheckBox_SpawnEveryOtherTargetInCenter->SetIsChecked(InTargetConfig.bSpawnEveryOtherTargetInCenter);
	
	Slider_ConsecutiveChargeScaleMultiplier->SetMaxValue(MaxValue_ConsecutiveChargeScaleMultiplier);
	Slider_ConsecutiveChargeScaleMultiplier->SetMinValue(MinValue_ConsecutiveChargeScaleMultiplier);
	
	TargetScaleConstrained->UpdateDefaultValuesAbsolute(InTargetConfig.MinTargetScale, InTargetConfig.MaxTargetScale, InTargetConfig.MinTargetScale == InTargetConfig.MaxTargetScale);
	TargetSpeedConstrained->UpdateDefaultValuesAbsolute(InTargetConfig.MinTargetSpeed, InTargetConfig.MaxTargetSpeed, InTargetConfig.MinTargetSpeed == InTargetConfig.MaxTargetSpeed);
	
	ComboBox_BoundsScalingPolicy->SetSelectedOption(UEnum::GetDisplayValueAsText(InTargetConfig.BoundsScalingPolicy).ToString());
	ComboBox_ConsecutiveTargetScalePolicy->SetSelectedOption(UEnum::GetDisplayValueAsText(InTargetConfig.ConsecutiveTargetScalePolicy).ToString());
	ComboBox_LifetimeTargetScalePolicy->SetSelectedOption(UEnum::GetDisplayValueAsText(InTargetConfig.LifetimeTargetScalePolicy).ToString());
	ComboBox_RecentTargetMemoryPolicy->SetSelectedOption(UEnum::GetDisplayValueAsText(InTargetConfig.RecentTargetMemoryPolicy).ToString());
	ComboBox_TargetActivationSelectionPolicy->SetSelectedOption(UEnum::GetDisplayValueAsText(InTargetConfig.TargetActivationSelectionPolicy).ToString());
	ComboBox_TargetDamageType->SetSelectedOption(UEnum::GetDisplayValueAsText(InTargetConfig.TargetDamageType).ToString());
	ComboBox_TargetDistributionPolicy->SetSelectedOption(UEnum::GetDisplayValueAsText(InTargetConfig.TargetDistributionPolicy).ToString());
	ComboBox_TargetSpawningPolicy->SetSelectedOption(UEnum::GetDisplayValueAsText(InTargetConfig.TargetSpawningPolicy).ToString());
	
	ComboBox_TargetActivationResponses->SetSelectedOptions(GetStringArrayFromEnumArray(InTargetConfig.TargetActivationResponses));
	ComboBox_TargetDeactivationConditions->SetSelectedOptions(GetStringArrayFromEnumArray(InTargetConfig.TargetDeactivationConditions));
	ComboBox_TargetDeactivationResponses->SetSelectedOptions(GetStringArrayFromEnumArray(InTargetConfig.TargetDeactivationResponses));
	ComboBox_TargetDestructionConditions->SetSelectedOptions(GetStringArrayFromEnumArray(InTargetConfig.TargetDestructionConditions));
	
	OnSelectionChanged_TargetDistributionPolicy(TArray({UEnum::GetDisplayValueAsText(InTargetConfig.TargetDistributionPolicy).ToString()}), ESelectInfo::Type::Direct);
	OnSelectionChanged_RecentTargetMemoryPolicy(TArray({UEnum::GetDisplayValueAsText(InTargetConfig.RecentTargetMemoryPolicy).ToString()}), ESelectInfo::Type::Direct);
	OnCheckStateChanged_MoveTargetsForward(InTargetConfig.bMoveTargetsForward);
	OnCheckStateChanged_MoveTargets(InTargetConfig.bMoveTargets);
	
	UpdateBrushColors();
}

FBS_TargetConfig UGameModesWidget_TargetConfig::GetTargetConfig() const
{
	FBS_TargetConfig ReturnConfig;
	
	ReturnConfig.TargetMaxLifeSpan = FMath::GridSnap(FMath::Clamp(Slider_Lifespan->GetValue(), MinValue_Lifespan, MaxValue_Lifespan), SnapSize_Lifespan);
	ReturnConfig.TargetSpawnCD = FMath::GridSnap(FMath::Clamp(Slider_TargetSpawnCD->GetValue(), MinValue_TargetSpawnCD, MaxValue_TargetSpawnCD), SnapSize_TargetSpawnCD);
	ReturnConfig.MinTargetScale = FMath::GridSnap(FMath::Clamp(TargetScaleConstrained->GetMinValue(), MinValue_TargetScale, MaxValue_TargetScale), SnapSize_TargetScale);
	ReturnConfig.MaxTargetScale = FMath::GridSnap(FMath::Clamp(TargetScaleConstrained->GetMaxValue(), MinValue_TargetScale, MaxValue_TargetScale), SnapSize_TargetScale);
	ReturnConfig.SpawnBeatDelay = FMath::GridSnap(FMath::Clamp(Slider_SpawnBeatDelay->GetValue(), MinValue_PlayerDelay, MaxValue_PlayerDelay), SnapSize_PlayerDelay);
	ReturnConfig.ConsecutiveChargeScaleMultiplier = FMath::GridSnap(FMath::Clamp(Slider_ConsecutiveChargeScaleMultiplier->GetValue(), MinValue_ConsecutiveChargeScaleMultiplier,
		MaxValue_ConsecutiveChargeScaleMultiplier), SnapSize_ConsecutiveChargeScaleMultiplier);
	ReturnConfig.MinTargetSpeed = FMath::GridSnap(FMath::Clamp(TargetSpeedConstrained->GetMinValue(), MinValue_TargetSpeed, MaxValue_TargetSpeed), SnapSize_TargetSpeed);
	ReturnConfig.MaxTargetSpeed = FMath::GridSnap(FMath::Clamp(TargetSpeedConstrained->GetMaxValue(), MinValue_TargetSpeed, MaxValue_TargetSpeed), SnapSize_TargetSpeed);
	ReturnConfig.FloorDistance = FMath::GridSnap(FMath::Clamp(Slider_FloorDistance->GetValue(), MinValue_FloorDistance, MaxValue_FloorDistance), SnapSize_FloorDistance);
	ReturnConfig.MinDistanceBetweenTargets = FMath::GridSnap(FMath::Clamp(Slider_MinTargetDistance->GetValue(), MinValue_MinTargetDistance, MaxValue_MinTargetDistance), SnapSize_MinTargetDistance);
	ReturnConfig.BoxBounds = FVector(0, FMath::GridSnap(FMath::Clamp(Slider_HorizontalSpread->GetValue(), MinValue_HorizontalSpread, MaxValue_HorizontalSpread), SnapSize_HorizontalSpread),
									 FMath::GridSnap(FMath::Clamp(Slider_VerticalSpread->GetValue(), MinValue_VerticalSpread, MaxValue_VerticalSpread), SnapSize_VerticalSpread));
	ReturnConfig.MoveForwardDistance = FMath::GridSnap(FMath::Clamp(Slider_ForwardSpread->GetValue(), MinValue_ForwardSpread, MaxValue_ForwardSpread), SnapSize_HorizontalSpread);
	ReturnConfig.MaxNumRecentTargets = FMath::GridSnap(FMath::Clamp(Slider_MaxNumRecentTargets->GetValue(), MinValue_MaxNumRecentTargets, MaxValue_MaxNumRecentTargets), SnapSize_MaxNumRecentTargets);
	ReturnConfig.RecentTargetTimeLength = FMath::GridSnap(FMath::Clamp(Slider_RecentTargetTimeLength->GetValue(), MinValue_RecentTargetTimeLength, MaxValue_RecentTargetTimeLength), SnapSize_RecentTargetTimeLength);
	ReturnConfig.NumUpfrontTargetsToSpawn = FMath::GridSnap(FMath::Clamp(Slider_NumUpfrontTargetsToSpawn->GetValue(), MinValue_NumUpfrontTargetsToSpawn, MaxValue_NumUpfrontTargetsToSpawn), SnapSize_NumUpfrontTargetsToSpawn);
	ReturnConfig.ExpirationHealthPenalty = FMath::GridSnap(FMath::Clamp(Slider_ExpirationHealthPenalty->GetValue(), MinValue_ExpirationHealthPenalty, MaxValue_ExpirationHealthPenalty), SnapSize_ExpirationHealthPenalty);
	ReturnConfig.MaxHealth = FMath::GridSnap(FMath::Clamp(Slider_MaxHealth->GetValue(), MinValue_MaxHealth, MaxValue_MaxHealth), SnapSize_MaxHealth);
	
	ReturnConfig.BoundsScalingPolicy = GetEnumFromString<EBoundsScalingPolicy>(ComboBox_BoundsScalingPolicy->GetSelectedOption(), EBoundsScalingPolicy::None);
	ReturnConfig.ConsecutiveTargetScalePolicy = GetEnumFromString<EConsecutiveTargetScalePolicy>(ComboBox_ConsecutiveTargetScalePolicy->GetSelectedOption(), EConsecutiveTargetScalePolicy::None);
	ReturnConfig.LifetimeTargetScalePolicy = GetEnumFromString<ELifetimeTargetScalePolicy>(ComboBox_LifetimeTargetScalePolicy->GetSelectedOption(), ELifetimeTargetScalePolicy::None);
	ReturnConfig.RecentTargetMemoryPolicy = GetEnumFromString<ERecentTargetMemoryPolicy>(ComboBox_RecentTargetMemoryPolicy->GetSelectedOption(), ERecentTargetMemoryPolicy::None);
	ReturnConfig.TargetActivationSelectionPolicy = GetEnumFromString<ETargetActivationSelectionPolicy>(ComboBox_TargetActivationSelectionPolicy->GetSelectedOption(), ETargetActivationSelectionPolicy::None);
	ReturnConfig.TargetDamageType = GetEnumFromString<ETargetDamageType>(ComboBox_TargetDamageType->GetSelectedOption(), ETargetDamageType::None);
	ReturnConfig.TargetDistributionPolicy = GetEnumFromString<ETargetDistributionPolicy>(ComboBox_TargetDistributionPolicy->GetSelectedOption(), ETargetDistributionPolicy::None);
	ReturnConfig.TargetSpawningPolicy = GetEnumFromString<ETargetSpawningPolicy>(ComboBox_TargetSpawningPolicy->GetSelectedOption(), ETargetSpawningPolicy::None);
	for (const FString& String : ComboBox_TargetActivationResponses->GetSelectedOptions())
	{
		ReturnConfig.TargetActivationResponses.AddUnique(GetEnumFromString<ETargetActivationResponse>(String, ETargetActivationResponse::None));
	}
	for (const FString& String : ComboBox_TargetDeactivationConditions->GetSelectedOptions())
	{
		ReturnConfig.TargetDeactivationConditions.AddUnique(GetEnumFromString<ETargetDeactivationCondition>(String, ETargetDeactivationCondition::None));
	}
	for (const FString& String : ComboBox_TargetDeactivationResponses->GetSelectedOptions())
	{
		ReturnConfig.TargetDeactivationResponses.AddUnique(GetEnumFromString<ETargetDeactivationResponse>(String, ETargetDeactivationResponse::None));
	}
	for (const FString& String : ComboBox_TargetDestructionConditions->GetSelectedOptions())
	{
		ReturnConfig.TargetDestructionConditions.AddUnique(GetEnumFromString<ETargetDestructionCondition>(String, ETargetDestructionCondition::None));
	}
	
	ReturnConfig.bApplyImmunityOnSpawn = CheckBox_ApplyImmunityOnSpawn->IsChecked();
	ReturnConfig.bMoveTargetsForward = CheckBox_MoveTargetsForward->IsChecked();
	ReturnConfig.bContinuouslySpawn = CheckBox_ContinuouslySpawn->IsChecked();
	ReturnConfig.bMoveTargets = CheckBox_MoveTargets->IsChecked();
	ReturnConfig.bSpawnAtOriginWheneverPossible = CheckBox_SpawnAtOriginWheneverPossible->IsChecked();
	ReturnConfig.bSpawnEveryOtherTargetInCenter = CheckBox_SpawnEveryOtherTargetInCenter->IsChecked();

	return ReturnConfig;
}

void UGameModesWidget_TargetConfig::OnSliderChanged_Lifespan(const float NewLifespan)
{
	OnSliderChanged(NewLifespan, Value_Lifespan, SnapSize_Lifespan);
}

void UGameModesWidget_TargetConfig::OnSliderChanged_TargetSpawnCD(const float NewTargetSpawnCD)
{
	OnSliderChanged(NewTargetSpawnCD, Value_TargetSpawnCD, SnapSize_TargetSpawnCD);
}

void UGameModesWidget_TargetConfig::OnSliderChanged_SpawnBeatDelay(const float NewPlayerDelay)
{
	OnSliderChanged(NewPlayerDelay, Value_SpawnBeatDelay, SnapSize_PlayerDelay);
}

void UGameModesWidget_TargetConfig::OnSliderChanged_ChargeScaleMultiplier(const float NewChargeScaleMultiplier)
{
	OnSliderChanged(NewChargeScaleMultiplier, Value_ConsecutiveChargeScaleMultiplier, SnapSize_ConsecutiveChargeScaleMultiplier);
}

void UGameModesWidget_TargetConfig::OnSliderChanged_FloorDistance(const float NewFloorDistance)
{
	OnSliderChanged(NewFloorDistance, Value_FloorDistance, SnapSize_FloorDistance);
}

void UGameModesWidget_TargetConfig::OnSliderChanged_MinTargetDistance(const float NewMinTargetDistance)
{
	OnSliderChanged(NewMinTargetDistance, Value_MinTargetDistance, SnapSize_MinTargetDistance);
}

void UGameModesWidget_TargetConfig::OnSliderChanged_HorizontalSpread(const float NewHorizontalSpread)
{
	OnSliderChanged(NewHorizontalSpread, Value_HorizontalSpread, SnapSize_HorizontalSpread);
}

void UGameModesWidget_TargetConfig::OnSliderChanged_VerticalSpread(const float NewVerticalSpread)
{
	OnSliderChanged(NewVerticalSpread, Value_VerticalSpread, SnapSize_VerticalSpread);
}

void UGameModesWidget_TargetConfig::OnSliderChanged_ForwardSpread(const float NewForwardSpread)
{
	OnSliderChanged(NewForwardSpread, Value_ForwardSpread, SnapSize_HorizontalSpread);
}

void UGameModesWidget_TargetConfig::OnSliderChanged_MaxNumRecentTargets(const float NewMaxNum)
{
	OnSliderChanged(NewMaxNum, Value_MaxNumRecentTargets, SnapSize_MaxNumRecentTargets);
}

void UGameModesWidget_TargetConfig::OnSliderChanged_RecentTargetTimeLength(const float NewTimeLength)
{
	OnSliderChanged(NewTimeLength, Value_RecentTargetTimeLength, SnapSize_RecentTargetTimeLength);
}

void UGameModesWidget_TargetConfig::OnSliderChanged_NumUpfrontTargetsToSpawn(const float NewNum)
{
	OnSliderChanged(NewNum, Value_NumUpfrontTargetsToSpawn, SnapSize_NumUpfrontTargetsToSpawn);
}

void UGameModesWidget_TargetConfig::OnSliderChanged_ExpirationHealthPenalty(const float NewHealthPenalty)
{
	OnSliderChanged(NewHealthPenalty, Value_ExpirationHealthPenalty, SnapSize_ExpirationHealthPenalty);
}

void UGameModesWidget_TargetConfig::OnSliderChanged_MaxHealth(const float NewMaxHealth)
{
	OnSliderChanged(NewMaxHealth, Value_MaxHealth, SnapSize_MaxHealth);
}

void UGameModesWidget_TargetConfig::OnTextCommitted_MaxNumRecentTargets(const FText& NewMaxNum, ETextCommit::Type CommitType)
{
	OnEditableTextBoxChanged(NewMaxNum, Value_MaxNumRecentTargets, Slider_MaxNumRecentTargets, SnapSize_MaxNumRecentTargets, MinValue_MaxNumRecentTargets, MaxValue_MaxNumRecentTargets);
}

void UGameModesWidget_TargetConfig::OnTextCommitted_RecentTargetTimeLength(const FText& NewTimeLength, ETextCommit::Type CommitType)
{
	OnEditableTextBoxChanged(NewTimeLength, Value_RecentTargetTimeLength, Slider_RecentTargetTimeLength, SnapSize_RecentTargetTimeLength, MinValue_RecentTargetTimeLength, MaxValue_RecentTargetTimeLength);
}

void UGameModesWidget_TargetConfig::OnTextCommitted_NumUpfrontTargetsToSpawn(const FText& NewNum, ETextCommit::Type CommitType)
{
	OnEditableTextBoxChanged(NewNum, Value_NumUpfrontTargetsToSpawn, Slider_NumUpfrontTargetsToSpawn, SnapSize_NumUpfrontTargetsToSpawn, MinValue_NumUpfrontTargetsToSpawn, MaxValue_NumUpfrontTargetsToSpawn);
}

void UGameModesWidget_TargetConfig::OnTextCommitted_ExpirationHealthPenalty(const FText& NewHealthPenalty, ETextCommit::Type CommitType)
{
	OnEditableTextBoxChanged(NewHealthPenalty, Value_ExpirationHealthPenalty, Slider_ExpirationHealthPenalty, SnapSize_ExpirationHealthPenalty, MinValue_ExpirationHealthPenalty, MaxValue_ExpirationHealthPenalty);
}

void UGameModesWidget_TargetConfig::OnTextCommitted_MaxHealth(const FText& NewMaxHealth, ETextCommit::Type CommitType)
{
	OnEditableTextBoxChanged(NewMaxHealth, Value_MaxHealth, Slider_MaxHealth, SnapSize_MaxHealth, MinValue_MaxHealth, MaxValue_MaxHealth);
}

void UGameModesWidget_TargetConfig::OnTextCommitted_FloorDistance(const FText& NewFloorDistance, ETextCommit::Type CommitType)
{
	OnEditableTextBoxChanged(NewFloorDistance, Value_FloorDistance, Slider_FloorDistance, SnapSize_FloorDistance, MinValue_FloorDistance, MaxValue_FloorDistance);
}

void UGameModesWidget_TargetConfig::OnTextCommitted_MinTargetDistance(const FText& NewMinTargetDistance, ETextCommit::Type CommitType)
{
	OnEditableTextBoxChanged(NewMinTargetDistance, Value_MinTargetDistance, Slider_MinTargetDistance, SnapSize_MinTargetDistance,MinValue_MinTargetDistance, MaxValue_MinTargetDistance);
}

void UGameModesWidget_TargetConfig::OnTextCommitted_HorizontalSpread(const FText& NewHorizontalSpread, ETextCommit::Type CommitType)
{
	OnEditableTextBoxChanged(NewHorizontalSpread, Value_HorizontalSpread, Slider_HorizontalSpread, SnapSize_HorizontalSpread, MinValue_HorizontalSpread, MaxValue_HorizontalSpread);
}

void UGameModesWidget_TargetConfig::OnTextCommitted_VerticalSpread(const FText& NewVerticalSpread, ETextCommit::Type CommitType)
{
	OnEditableTextBoxChanged(NewVerticalSpread, Value_VerticalSpread, Slider_VerticalSpread, SnapSize_VerticalSpread, MinValue_VerticalSpread, MaxValue_VerticalSpread);
}

void UGameModesWidget_TargetConfig::OnTextCommitted_ForwardSpread(const FText& NewForwardSpread, ETextCommit::Type CommitType)
{
	OnEditableTextBoxChanged(NewForwardSpread, Value_ForwardSpread, Slider_ForwardSpread, SnapSize_HorizontalSpread, MinValue_ForwardSpread, MaxValue_ForwardSpread);
}

void UGameModesWidget_TargetConfig::OnCheckStateChanged_ApplyImmunityOnSpawn(const bool bApplyImmunityOnSpawn)
{
}

void UGameModesWidget_TargetConfig::OnCheckStateChanged_MoveTargetsForward(const bool bUseForwardSpread)
{
	if (bUseForwardSpread)
	{
		BSBox_ForwardSpread->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		return;
	}
	BSBox_ForwardSpread->SetVisibility(ESlateVisibility::Collapsed);
}

void UGameModesWidget_TargetConfig::OnCheckStateChanged_ContinuouslySpawn(const bool bContinuouslySpawn)
{
}

void UGameModesWidget_TargetConfig::OnCheckStateChanged_MoveTargets(const bool bMoveTargets)
{
	// Hide target speed boxes if move targets forward is not enabled, otherwise show
	if (bMoveTargets)
	{
		TargetSpeedConstrained->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		TargetSpeedConstrained->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UGameModesWidget_TargetConfig::OnCheckStateChanged_SpawnAtOriginWheneverPossible(const bool bSpawnAtOrigin)
{
}

void UGameModesWidget_TargetConfig::OnCheckStateChanged_SpawnEveryOtherTargetInCenter(const bool bSpawnEveryOther)
{
}

UWidget* UGameModesWidget_TargetConfig::OnGenerateWidgetEvent(const UBSComboBoxString* ComboBoxString, FString Method)
{
	const FText EntryText = Method.IsEmpty() ? FText::FromString("None Selected") : FText::FromString(Method);
	const FText TooltipText = GetTooltipTextFromKey(GetStringTableKeyFromComboBox(ComboBoxString, Method));

	if (UBSComboBoxEntry* Entry = CreateWidget<UBSComboBoxEntry>(this, ComboBoxString->GetComboboxEntryWidget()))
	{
		ComboBoxString->InitializeComboBoxEntry(Entry, EntryText, false, TooltipText);
		return Entry;
	}
	return nullptr;
}

UWidget* UGameModesWidget_TargetConfig::OnSelectionChanged_GenerateSingleSelectionItem(const UBSComboBoxString* ComboBoxString, FString SelectedOption)
{
	const FText EntryText = SelectedOption.IsEmpty() ? FText::FromString("None Selected") : FText::FromString(SelectedOption);
	const FText TooltipText = GetTooltipTextFromKey(GetStringTableKeyFromComboBox(ComboBoxString, SelectedOption));

	if (UBSComboBoxEntry* Entry = CreateWidget<UBSComboBoxEntry>(this, ComboBoxString->GetComboboxEntryWidget()))
	{
		ComboBoxString->InitializeComboBoxEntry(Entry, EntryText, false, TooltipText);
		return Entry;
	}
	return nullptr;
}

UWidget* UGameModesWidget_TargetConfig::OnSelectionChanged_GenerateMultiSelectionItem(const UBSComboBoxString* ComboBoxString, const TArray<FString>& SelectedOptions)
{
	FText TooltipText = FText::GetEmpty();
	FString EntryString = FString();

	if (!SelectedOptions.IsEmpty())
	{
		for (int i = 0; i < SelectedOptions.Num(); i++)
		{
			if (!SelectedOptions[i].IsEmpty())
			{
				EntryString.Append(SelectedOptions[i]);
				if (i < SelectedOptions.Num() - 1)
				{
					EntryString.Append(", ");
				}
			}
		}
	}
	if (SelectedOptions.Num() == 1)
	{
		TooltipText = GetTooltipTextFromKey(GetStringTableKeyFromComboBox(ComboBoxString, SelectedOptions[0]));
	}

	const FText EntryText = FText::FromString(EntryString);
	
	if (UBSComboBoxEntry* Entry = CreateWidget<UBSComboBoxEntry>(this, ComboBoxString->GetComboboxEntryWidget()))
	{
		ComboBoxString->InitializeComboBoxEntry(Entry, EntryText, false, TooltipText);
		return Entry;
	}
	
	return nullptr;
}

void UGameModesWidget_TargetConfig::OnTextCommitted_ChargeScaleMultiplier(const FText& NewChargeScaleMultiplier, ETextCommit::Type CommitType)
{
	OnEditableTextBoxChanged(NewChargeScaleMultiplier, Value_ConsecutiveChargeScaleMultiplier, Slider_ConsecutiveChargeScaleMultiplier, SnapSize_ConsecutiveChargeScaleMultiplier,
		MinValue_ConsecutiveChargeScaleMultiplier, MaxValue_ConsecutiveChargeScaleMultiplier);
}

void UGameModesWidget_TargetConfig::OnTextCommitted_Lifespan(const FText& NewLifespan, ETextCommit::Type CommitType)
{
	OnEditableTextBoxChanged(NewLifespan, Value_Lifespan, Slider_Lifespan, SnapSize_Lifespan, MinValue_Lifespan, MaxValue_Lifespan);
}

void UGameModesWidget_TargetConfig::OnTextCommitted_TargetSpawnCD(const FText& NewTargetSpawnCD, ETextCommit::Type CommitType)
{
	OnEditableTextBoxChanged(NewTargetSpawnCD, Value_TargetSpawnCD, Slider_TargetSpawnCD, SnapSize_TargetSpawnCD,MinValue_TargetSpawnCD, MaxValue_TargetSpawnCD);
}

void UGameModesWidget_TargetConfig::OnTextCommitted_SpawnBeatDelay(const FText& NewPlayerDelay, ETextCommit::Type CommitType)
{
	OnEditableTextBoxChanged(NewPlayerDelay, Value_SpawnBeatDelay, Slider_SpawnBeatDelay, SnapSize_PlayerDelay, MinValue_PlayerDelay, MaxValue_PlayerDelay);
}

void UGameModesWidget_TargetConfig::OnSelectionChanged_BoundsScalingPolicy(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType)
{
}

void UGameModesWidget_TargetConfig::OnSelectionChanged_ConsecutiveTargetScalePolicy(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType)
{
	if (Selected.Num() != 1)
	{
		return;
	}
	switch (GetEnumFromString<EConsecutiveTargetScalePolicy>(Selected[0], EConsecutiveTargetScalePolicy::None))
	{
	case EConsecutiveTargetScalePolicy::Static:
		TargetScaleConstrained->SetLocksEnabled(false);
		/*if (!TargetScaleConstrained->GetIsSynced())
		{
			TargetScaleConstrained->UpdateDefaultValues(TargetScaleConstrained->GetMinValue(), TargetScaleConstrained->GetMaxValue(), true);
			TargetScaleConstrained->SetLocksEnabled(false);
		}*/
		break;
	case EConsecutiveTargetScalePolicy::Random:
	case EConsecutiveTargetScalePolicy::SkillBased:
		TargetScaleConstrained->SetLocksEnabled(true);
		/*if (TargetScaleConstrained->GetIsSynced())
		{
			TargetScaleConstrained->UpdateDefaultValues(TargetScaleConstrained->GetMinValue(), TargetScaleConstrained->GetMaxValue(), TargetScaleConstrained->GetMinValue() == TargetScaleConstrained->GetMaxValue());
		}*/
		break;
	default:
		break;
	}
}

void UGameModesWidget_TargetConfig::OnSelectionChanged_LifetimeTargetScalePolicy(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType)
{
	if (!Selected.Contains(UEnum::GetDisplayValueAsText(ELifetimeTargetScalePolicy::None).ToString()))
	{
		/*if (TargetScaleConstrained->GetIsSynced())
		{
			TargetScaleConstrained->UpdateDefaultValues(TargetScaleConstrained->GetMinValue(), TargetScaleConstrained->GetMaxValue(), TargetScaleConstrained->GetMinValue() == TargetScaleConstrained->GetMaxValue());
		}*/
	}
}

void UGameModesWidget_TargetConfig::OnSelectionChanged_RecentTargetMemoryPolicy(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType)
{
	if (Selected.Num() != 1)
	{
		return;
	}
	const ERecentTargetMemoryPolicy Policy = GetEnumFromString<ERecentTargetMemoryPolicy>(Selected[0], ERecentTargetMemoryPolicy::None);
	switch (Policy)
	{
	case ERecentTargetMemoryPolicy::CustomTimeBased:
		BSBox_RecentTargetTimeLength->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		BSBox_MaxNumRecentTargets->SetVisibility(ESlateVisibility::Collapsed);
		break;
	case ERecentTargetMemoryPolicy::UseTargetSpawnCD:
		BSBox_RecentTargetTimeLength->SetVisibility(ESlateVisibility::Collapsed);
		BSBox_MaxNumRecentTargets->SetVisibility(ESlateVisibility::Collapsed);
		break;
	case ERecentTargetMemoryPolicy::None:
	case ERecentTargetMemoryPolicy::NumTargetsBased:
		BSBox_MaxNumRecentTargets->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		BSBox_RecentTargetTimeLength->SetVisibility(ESlateVisibility::Collapsed);
		break;
	}
	UpdateBrushColors();
}

void UGameModesWidget_TargetConfig::OnSelectionChanged_TargetActivationSelectionPolicy(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType)
{
}

void UGameModesWidget_TargetConfig::OnSelectionChanged_TargetActivationResponses(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType)
{
	TArray<ETargetActivationResponse> SelectedEnums;
	for (const FString& String : Selected)
	{
		SelectedEnums.Emplace(GetEnumFromString<ETargetActivationResponse>(String, ETargetActivationResponse::None));
	}
}

void UGameModesWidget_TargetConfig::OnSelectionChanged_TargetDeactivationConditions(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType)
{
	TArray<ETargetDeactivationCondition> SelectedEnums;
	for (const FString& String : Selected)
	{
		SelectedEnums.Emplace(GetEnumFromString<ETargetDeactivationCondition>(String, ETargetDeactivationCondition::None));
	}
}

void UGameModesWidget_TargetConfig::OnSelectionChanged_TargetDeactivationResponses(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType)
{
	TArray<ETargetDeactivationResponse> SelectedEnums;
	for (const FString& String : Selected)
	{
		SelectedEnums.Emplace(GetEnumFromString<ETargetDeactivationResponse>(String, ETargetDeactivationResponse::None));
	}
}

void UGameModesWidget_TargetConfig::OnSelectionChanged_TargetDestructionConditions(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType)
{
	TArray<ETargetDestructionCondition> SelectedEnums;
	for (const FString& String : Selected)
	{
		SelectedEnums.Emplace(GetEnumFromString<ETargetDestructionCondition>(String, ETargetDestructionCondition::None));
	}
}

void UGameModesWidget_TargetConfig::OnSelectionChanged_TargetDistributionPolicy(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType)
{
	if (Selected.Num() != 1)
	{
		return;
	}
	if (GetEnumFromString<ETargetDistributionPolicy>(Selected[0], ETargetDistributionPolicy::None) == ETargetDistributionPolicy::HeadshotHeightOnly)
	{
		Slider_VerticalSpread->SetValue(0);
		Value_VerticalSpread->SetText(FText::AsNumber(0));
		Slider_VerticalSpread->SetLocked(true);
		Value_VerticalSpread->SetIsReadOnly(true);
	
		Slider_FloorDistance->SetValue(DistanceFromFloor);
		Value_FloorDistance->SetText(FText::AsNumber(DistanceFromFloor));
		Slider_FloorDistance->SetLocked(true);
		Value_FloorDistance->SetIsReadOnly(true);
		return;
	}
	
	Slider_FloorDistance->SetLocked(false);
	Value_FloorDistance->SetIsReadOnly(false);
	Slider_VerticalSpread->SetLocked(false);
	Value_VerticalSpread->SetIsReadOnly(false);
	OnEditableTextBoxChanged(FText::AsNumber(MaxValue_VerticalSpread), Value_VerticalSpread, Slider_VerticalSpread, SnapSize_VerticalSpread, MinValue_VerticalSpread, MaxValue_VerticalSpread);
	OnSliderChanged(MaxValue_VerticalSpread, Value_VerticalSpread, SnapSize_VerticalSpread);
}

void UGameModesWidget_TargetConfig::OnSelectionChanged_TargetSpawningPolicy(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType)
{
	if (Selected.Num() != 1)
	{
		return;
	}
	switch (GetEnumFromString<ETargetSpawningPolicy>(Selected[0], ETargetSpawningPolicy::None))
	{
	case ETargetSpawningPolicy::UpfrontOnly:
		BSBox_NumUpfrontTargetsToSpawn->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		break;
	case ETargetSpawningPolicy::None:
	case ETargetSpawningPolicy::RuntimeOnly:
		BSBox_NumUpfrontTargetsToSpawn->SetVisibility(ESlateVisibility::Collapsed);
		break;
	}
	UpdateBrushColors();
}

void UGameModesWidget_TargetConfig::OnSelectionChanged_TargetDamageType(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType)
{
}

FString UGameModesWidget_TargetConfig::GetStringTableKeyFromComboBox(const UBSComboBoxString* ComboBoxString, const FString& EnumString)
{
	if (ComboBoxString == ComboBox_BoundsScalingPolicy)
	{
		const EBoundsScalingPolicy EnumValue = GetEnumFromString<EBoundsScalingPolicy>(EnumString, EBoundsScalingPolicy::None);
		return GetStringTableKeyNameFromEnum(EnumValue);
	}
	if (ComboBoxString == ComboBox_ConsecutiveTargetScalePolicy)
	{
		const EConsecutiveTargetScalePolicy EnumValue = GetEnumFromString<EConsecutiveTargetScalePolicy>(EnumString, EConsecutiveTargetScalePolicy::None);
		return GetStringTableKeyNameFromEnum(EnumValue);
	}
	if (ComboBoxString == ComboBox_LifetimeTargetScalePolicy)
	{
		return GetStringTableKeyNameFromEnum<ELifetimeTargetScalePolicy>(GetEnumFromString<ELifetimeTargetScalePolicy>(EnumString, ELifetimeTargetScalePolicy::None));
	}
	if (ComboBoxString == ComboBox_RecentTargetMemoryPolicy)
	{
		return GetStringTableKeyNameFromEnum<ERecentTargetMemoryPolicy>(GetEnumFromString<ERecentTargetMemoryPolicy>(EnumString, ERecentTargetMemoryPolicy::None));
	}
	if (ComboBoxString == ComboBox_TargetActivationSelectionPolicy)
	{
		return GetStringTableKeyNameFromEnum<ETargetActivationSelectionPolicy>(GetEnumFromString<ETargetActivationSelectionPolicy>(EnumString, ETargetActivationSelectionPolicy::None));
	}
	if (ComboBoxString == ComboBox_TargetDamageType)
	{
		return GetStringTableKeyNameFromEnum<ETargetDamageType>(GetEnumFromString<ETargetDamageType>(EnumString, ETargetDamageType::None));
	}
	if (ComboBoxString == ComboBox_TargetDistributionPolicy)
	{
		return GetStringTableKeyNameFromEnum<ETargetDistributionPolicy>(GetEnumFromString<ETargetDistributionPolicy>(EnumString, ETargetDistributionPolicy::None));
	}
	if (ComboBoxString == ComboBox_TargetSpawningPolicy)
	{
		return GetStringTableKeyNameFromEnum<ETargetSpawningPolicy>(GetEnumFromString<ETargetSpawningPolicy>(EnumString, ETargetSpawningPolicy::None));
	}
	if (ComboBoxString == ComboBox_TargetActivationResponses)
	{
		const ETargetActivationResponse EnumValue = GetEnumFromString<ETargetActivationResponse>(EnumString, ETargetActivationResponse::None);
		return GetStringTableKeyNameFromEnum(EnumValue);
	}
	if (ComboBoxString == ComboBox_TargetDeactivationConditions)
	{
		const ETargetDeactivationCondition EnumValue = GetEnumFromString<ETargetDeactivationCondition>(EnumString, ETargetDeactivationCondition::None);
		return GetStringTableKeyNameFromEnum(EnumValue);
	}
	if (ComboBoxString == ComboBox_TargetDeactivationResponses)
	{
		const ETargetDeactivationResponse EnumValue = GetEnumFromString<ETargetDeactivationResponse>(EnumString, ETargetDeactivationResponse::None);
		return GetStringTableKeyNameFromEnum(EnumValue);
	}
	if (ComboBoxString == ComboBox_TargetDestructionConditions)
	{
		const ETargetDestructionCondition EnumValue = GetEnumFromString<ETargetDestructionCondition>(EnumString, ETargetDestructionCondition::None);
		return GetStringTableKeyNameFromEnum(EnumValue);
	}
	UE_LOG(LogTemp, Display, TEXT("Couldn't find matching value for %s in UGameModesWidget_TargetConfig"), *ComboBoxString->GetName());
	return "INVALID_STRING";
}
