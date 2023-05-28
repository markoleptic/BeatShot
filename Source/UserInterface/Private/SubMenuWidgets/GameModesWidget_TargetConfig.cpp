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

void UGameModesWidget_TargetConfig::NativeConstruct()
{
	/* Create MainContainer before calling Super NativeConstruct since the parent calls InitSettingCategoryWidget in NativeConstruct */
	TargetScaleConstrained = CreateWidget<UDoubleSyncedSliderAndTextBox>(this, TargetScaleConstrainedClass);
	TargetSpeedConstrained = CreateWidget<UDoubleSyncedSliderAndTextBox>(this, TargetSpeedConstrainedClass);
	
	MainContainer->AddChildToVerticalBox(TargetSpeedConstrained.Get());
	MainContainer->AddChildToVerticalBox(TargetScaleConstrained.Get());
	
	Super::NativeConstruct();

	SetTooltipWidget(ConstructTooltipWidget());

	/* Target Scale TextBox and Slider */
	FSyncedSlidersParams TargetScaleSliderStruct;
	TargetScaleSliderStruct.MinConstraintLower = MinValue_TargetScale;
	TargetScaleSliderStruct.MinConstraintUpper = MaxValue_TargetScale;
	TargetScaleSliderStruct.MaxConstraintLower = MinValue_TargetScale;
	TargetScaleSliderStruct.MaxConstraintUpper = MaxValue_TargetScale;
	TargetScaleSliderStruct.DefaultMinValue = MinValue_TargetScale;
	TargetScaleSliderStruct.DefaultMaxValue = MaxValue_TargetScale;
	TargetScaleSliderStruct.bShowCheckBox = false;
	TargetScaleSliderStruct.MaxText = FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "GM_MaxTargetScale");
	TargetScaleSliderStruct.MinText = FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "GM_MinTargetScale");
	TargetScaleSliderStruct.GridSnapSize = SnapSize_TargetScale;
	TargetScaleSliderStruct.bSyncSlidersAndValues = false;
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
	TrackingSpeedSliderStruct.bSyncSlidersAndValues = false;
	TargetSpeedConstrained->InitConstrainedSlider(TrackingSpeedSliderStruct);

	AddToTooltipData(TargetSpeedConstrained->GetCheckBoxQMark(), FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "BeatTrackConstantSpeed"));
	AddToTooltipData(QMark_Lifespan, FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "Lifespan"));
	AddToTooltipData(QMark_TargetSpawnCD, FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "MinDistance"));
	AddToTooltipData(QMark_SpawnBeatDelay, FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "SpawnBeatDelay"));
	AddToTooltipData(QMark_ConsecutiveTargetScalePolicy, FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "ConsecutiveTargetScalePolicy"));
	AddToTooltipData(QMark_LifetimeTargetScalePolicy, FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "LifetimeTargetScalePolicy"));
	AddToTooltipData(QMark_NumCharges, FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "NumCharges"));
	AddToTooltipData(QMark_ConsecutiveChargeScaleMultiplier, FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "ConsecutiveChargeScaleMultiplier"));
	AddToTooltipData(QMark_TargetSpawningPolicy, FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "TargetSpawningPolicy"));
	AddToTooltipData(QMark_TargetDestructionPolicy, FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "TargetDestructionPolicy"));
	AddToTooltipData(QMark_TargetActivationPolicy, FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "TargetActivationPolicy"));
	AddToTooltipData(QMark_TargetDeactivationPolicy, FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "TargetDeactivationPolicy"));
	AddToTooltipData(QMark_TargetDamageType, FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "TargetDamageType"));
	AddToTooltipData(QMark_ApplyImmunityOnSpawn, FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "ApplyImmunityOnSpawn"));
	AddToTooltipData(QMark_ResetPositionOnDeactivation, FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "ResetPositionOnDeactivation"));

	Slider_Lifespan->OnValueChanged.AddDynamic(this, &ThisClass::OnSliderChanged_Lifespan);
	Value_Lifespan->OnTextCommitted.AddDynamic(this, &ThisClass::OnTextCommitted_Lifespan);
	
	Slider_TargetSpawnCD->OnValueChanged.AddDynamic(this, &ThisClass::OnSliderChanged_TargetSpawnCD);
	Value_TargetSpawnCD->OnTextCommitted.AddDynamic(this, &ThisClass::OnTextCommitted_TargetSpawnCD);
	
	Slider_SpawnBeatDelay->OnValueChanged.AddDynamic(this, &ThisClass::OnSliderChanged_SpawnBeatDelay);
	Value_SpawnBeatDelay->OnTextCommitted.AddDynamic(this, &ThisClass::OnTextCommitted_SpawnBeatDelay);

	Slider_NumCharges->OnValueChanged.AddDynamic(this, &ThisClass::OnSliderChanged_NumCharges);
	Value_NumCharges->OnTextCommitted.AddDynamic(this, &ThisClass::OnTextCommitted_NumCharges);

	Slider_ConsecutiveChargeScaleMultiplier->OnValueChanged.AddDynamic(this, &ThisClass::OnSliderChanged_ChargeScaleMultiplier);
	Value_ConsecutiveChargeScaleMultiplier->OnTextCommitted.AddDynamic(this, &ThisClass::OnTextCommitted_ChargeScaleMultiplier);

	CheckBox_ApplyImmunityOnSpawn->OnCheckStateChanged.AddDynamic(this, &ThisClass::OnCheckStateChanged_ApplyImmunityOnSpawn);
	CheckBox_ResetPositionOnDeactivation->OnCheckStateChanged.AddDynamic(this, &ThisClass::OnCheckStateChanged_ResetPositionOnDeactivation);
	
	ComboBox_LifetimeTargetScalePolicy->OnSelectionChanged.AddDynamic(this, &ThisClass::OnSelectionChanged_LifetimeTargetScaleMethod);
	ComboBox_ConsecutiveTargetScalePolicy->OnSelectionChanged.AddDynamic(this, &ThisClass::OnSelectionChanged_ConsecutiveTargetScale);
	ComboBox_TargetSpawningPolicy->OnSelectionChanged.AddDynamic(this, &ThisClass::OnSelectionChanged_TargetSpawningPolicy);
	ComboBox_TargetDestructionPolicy->OnSelectionChanged.AddDynamic(this, &ThisClass::OnSelectionChanged_TargetDestructionPolicy);
	ComboBox_TargetActivationPolicy->OnSelectionChanged.AddDynamic(this, &ThisClass::OnSelectionChanged_TargetActivationPolicy);
	ComboBox_TargetDeactivationPolicy->OnSelectionChanged.AddDynamic(this, &ThisClass::OnSelectionChanged_TargetDeactivationPolicy);
	ComboBox_TargetDamageType->OnSelectionChanged.AddDynamic(this, &ThisClass::OnSelectionChanged_TargetDamageType);
	
	ComboBox_LifetimeTargetScalePolicy->OnSelectionChangedGenerateWidgetEvent.BindDynamic(this,  &ThisClass::OnSelectionChangedGenerateWidgetEvent_LifetimeTargetScale);
	ComboBox_ConsecutiveTargetScalePolicy->OnSelectionChangedGenerateWidgetEvent.BindDynamic(this, &ThisClass::OnSelectionChangedGenerateWidgetEvent_ConsecutiveTargetScale);
	ComboBox_TargetSpawningPolicy->OnSelectionChangedGenerateWidgetEvent.BindDynamic(this, &ThisClass::OnSelectionChangedGenerateWidgetEvent_TargetSpawningPolicy);
	ComboBox_TargetDestructionPolicy->OnSelectionChangedGenerateWidgetEvent.BindDynamic(this, &ThisClass::OnSelectionChangedGenerateWidgetEvent_TargetDestructionPolicy);
	ComboBox_TargetActivationPolicy->OnSelectionChangedGenerateWidgetEvent.BindDynamic(this, &ThisClass::OnSelectionChangedGenerateWidgetEvent_TargetActivationPolicy);
	ComboBox_TargetDeactivationPolicy->OnSelectionChangedGenerateWidgetEvent.BindDynamic(this, &ThisClass::OnSelectionChangedGenerateWidgetEvent_TargetDeactivationPolicy);
	ComboBox_TargetDamageType->OnSelectionChangedGenerateWidgetEvent.BindDynamic(this, &ThisClass::OnSelectionChangedGenerateWidgetEvent_TargetDamageType);

	ComboBox_LifetimeTargetScalePolicy->OnGenerateWidgetEvent.BindDynamic(this, &ThisClass::OnGenerateWidgetEvent_LifetimeTargetScale);
	ComboBox_ConsecutiveTargetScalePolicy->OnGenerateWidgetEvent.BindDynamic(this, &ThisClass::OnGenerateWidgetEvent_ConsecutiveTargetScale);
	ComboBox_TargetSpawningPolicy->OnGenerateWidgetEvent.BindDynamic(this, &ThisClass::OnGenerateWidgetEvent_TargetSpawningPolicy);
	ComboBox_TargetDestructionPolicy->OnGenerateWidgetEvent.BindDynamic(this, &ThisClass::OnGenerateWidgetEvent_TargetDestructionPolicy);
	ComboBox_TargetActivationPolicy->OnGenerateWidgetEvent.BindDynamic(this, &ThisClass::OnGenerateWidgetEvent_TargetActivationPolicy);
	ComboBox_TargetDeactivationPolicy->OnGenerateWidgetEvent.BindDynamic(this, &ThisClass::OnGenerateWidgetEvent_TargetDeactivationPolicy);
	ComboBox_TargetDamageType->OnGenerateWidgetEvent.BindDynamic(this, &ThisClass::OnGenerateWidgetEvent_TargetDamageType);



	for (const ETargetSpawningPolicy& Method : TEnumRange<ETargetSpawningPolicy>())
	{
		ComboBox_TargetSpawningPolicy->AddOption(UEnum::GetDisplayValueAsText(Method).ToString());
	}
	for (const ETargetDestructionPolicy& Method : TEnumRange<ETargetDestructionPolicy>())
	{
		ComboBox_TargetDestructionPolicy->AddOption(UEnum::GetDisplayValueAsText(Method).ToString());
	}
	for (const ETargetActivationPolicy& Method : TEnumRange<ETargetActivationPolicy>())
    {
    	ComboBox_TargetActivationPolicy->AddOption(UEnum::GetDisplayValueAsText(Method).ToString());
    }
	for (const ETargetDeactivationPolicy& Method : TEnumRange<ETargetDeactivationPolicy>())
	{
		ComboBox_TargetDeactivationPolicy->AddOption(UEnum::GetDisplayValueAsText(Method).ToString());
	}
	for (const ETargetDamageType& Method : TEnumRange<ETargetDamageType>())
	{
		ComboBox_TargetDamageType->AddOption(UEnum::GetDisplayValueAsText(Method).ToString());
	}
	for (const EConsecutiveTargetScalePolicy& Method : TEnumRange<EConsecutiveTargetScalePolicy>())
	{
		ComboBox_ConsecutiveTargetScalePolicy->AddOption(UEnum::GetDisplayValueAsText(Method).ToString());
	}
	for (const ELifetimeTargetScalePolicy& Method : TEnumRange<ELifetimeTargetScalePolicy>())
	{
		ComboBox_LifetimeTargetScalePolicy->AddOption(UEnum::GetDisplayValueAsText(Method).ToString());
	}
}

void UGameModesWidget_TargetConfig::InitSettingCategoryWidget()
{
	if (TargetScaleConstrained)
	{
		AddWidgetBoxPair(TargetScaleConstrained.Get(), TargetScaleConstrained->GetMainContainer());
	}
	if (TargetSpeedConstrained)
	{
		AddWidgetBoxPair(TargetSpeedConstrained.Get(), TargetSpeedConstrained->GetMainContainer());
	}
	Super::InitSettingCategoryWidget();
}

void UGameModesWidget_TargetConfig::InitializeTargetConfig(const FBS_TargetConfig& InTargetConfig, const EBaseGameMode& BaseGameMode)
{
	switch(BaseGameMode)
	{
	case EBaseGameMode::SingleBeat:
		Slider_Lifespan->SetLocked(false);
		Value_Lifespan->SetIsReadOnly(false);
		BSBox_NumCharges->SetVisibility(ESlateVisibility::Collapsed);
		BSBox_ConsecutiveChargeScaleMultiplier->SetVisibility(ESlateVisibility::Collapsed);
		break;
	case EBaseGameMode::MultiBeat:
		Slider_Lifespan->SetLocked(false);
		Value_Lifespan->SetIsReadOnly(false);
		BSBox_NumCharges->SetVisibility(ESlateVisibility::Collapsed);
		BSBox_ConsecutiveChargeScaleMultiplier->SetVisibility(ESlateVisibility::Collapsed);
		break;
	case EBaseGameMode::BeatGrid:
		Slider_Lifespan->SetLocked(false);
		Value_Lifespan->SetIsReadOnly(false);
		BSBox_NumCharges->SetVisibility(ESlateVisibility::Collapsed);
		BSBox_ConsecutiveChargeScaleMultiplier->SetVisibility(ESlateVisibility::Collapsed);
		break;
	case EBaseGameMode::BeatTrack:
		Slider_Lifespan->SetLocked(true);
		Value_Lifespan->SetIsReadOnly(true);
		BSBox_NumCharges->SetVisibility(ESlateVisibility::Collapsed);
		BSBox_ConsecutiveChargeScaleMultiplier->SetVisibility(ESlateVisibility::Collapsed);
		break;
	case EBaseGameMode::ChargedBeatTrack:
		Slider_Lifespan->SetLocked(false);
		Value_Lifespan->SetIsReadOnly(false);
		BSBox_NumCharges->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		BSBox_ConsecutiveChargeScaleMultiplier->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		break;
	case EBaseGameMode::None:
		break;
	}
	
	Slider_Lifespan->SetValue(InTargetConfig.TargetMaxLifeSpan);
	Value_Lifespan->SetText(FText::AsNumber(InTargetConfig.TargetMaxLifeSpan));
	
	Slider_TargetSpawnCD->SetValue(InTargetConfig.TargetSpawnCD);
	Value_TargetSpawnCD->SetText(FText::AsNumber(InTargetConfig.TargetSpawnCD));
	
	Slider_SpawnBeatDelay->SetValue(InTargetConfig.SpawnBeatDelay);
	Value_SpawnBeatDelay->SetText(FText::AsNumber(InTargetConfig.SpawnBeatDelay));

	Slider_NumCharges->SetValue(InTargetConfig.NumCharges);
	Slider_NumCharges->SetMaxValue(MaxValue_NumCharges);
	Slider_NumCharges->SetMinValue(MinValue_NumCharges);
	Value_NumCharges->SetText(FText::AsNumber(InTargetConfig.NumCharges));

	Slider_ConsecutiveChargeScaleMultiplier->SetValue(InTargetConfig.ConsecutiveChargeScaleMultiplier);
	Slider_ConsecutiveChargeScaleMultiplier->SetMaxValue(MaxValue_ConsecutiveChargeScaleMultiplier);
	Slider_ConsecutiveChargeScaleMultiplier->SetMinValue(MinValue_ConsecutiveChargeScaleMultiplier);
	Value_ConsecutiveChargeScaleMultiplier->SetText(FText::AsNumber(InTargetConfig.ConsecutiveChargeScaleMultiplier));
	
	TargetScaleConstrained->UpdateDefaultValues(InTargetConfig.MinTargetScale, InTargetConfig.MaxTargetScale, InTargetConfig.MinTargetScale == InTargetConfig.MaxTargetScale);
	TargetSpeedConstrained->UpdateDefaultValues(InTargetConfig.MinTrackingSpeed, InTargetConfig.MaxTrackingSpeed, InTargetConfig.MinTrackingSpeed == InTargetConfig.MaxTrackingSpeed);

	ComboBox_LifetimeTargetScalePolicy->SetSelectedOption(UEnum::GetDisplayValueAsText(InTargetConfig.LifetimeTargetScalePolicy).ToString());
	ComboBox_ConsecutiveTargetScalePolicy->SetSelectedOption(UEnum::GetDisplayValueAsText(InTargetConfig.ConsecutiveTargetScalePolicy).ToString());
	ComboBox_TargetSpawningPolicy->SetSelectedOption(UEnum::GetDisplayValueAsText(InTargetConfig.TargetSpawningPolicy).ToString());
	ComboBox_TargetDestructionPolicy->SetSelectedOption(UEnum::GetDisplayValueAsText(InTargetConfig.TargetDestructionPolicy).ToString());
	ComboBox_TargetActivationPolicy->SetSelectedOption(UEnum::GetDisplayValueAsText(InTargetConfig.TargetActivationPolicy).ToString());
	ComboBox_TargetDeactivationPolicy->SetSelectedOption(UEnum::GetDisplayValueAsText(InTargetConfig.TargetDeactivationPolicy).ToString());
	ComboBox_TargetDamageType->SetSelectedOption(UEnum::GetDisplayValueAsText(InTargetConfig.TargetDamageType).ToString());

	CheckBox_ApplyImmunityOnSpawn->SetIsChecked(InTargetConfig.bApplyImmunityOnSpawn);
	CheckBox_ResetPositionOnDeactivation->SetIsChecked(InTargetConfig.bResetPositionOnDeactivation);
	
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
	ReturnConfig.LifetimeTargetScalePolicy = GetEnumFromString<ELifetimeTargetScalePolicy>(ComboBox_LifetimeTargetScalePolicy->GetSelectedOption(), ELifetimeTargetScalePolicy::None);
	ReturnConfig.ConsecutiveTargetScalePolicy = GetEnumFromString<EConsecutiveTargetScalePolicy>(ComboBox_ConsecutiveTargetScalePolicy->GetSelectedOption(), EConsecutiveTargetScalePolicy::None);
	ReturnConfig.TargetSpawningPolicy = GetEnumFromString<ETargetSpawningPolicy>(ComboBox_ConsecutiveTargetScalePolicy->GetSelectedOption(), ETargetSpawningPolicy::None);
	ReturnConfig.TargetDestructionPolicy = GetEnumFromString<ETargetDestructionPolicy>(ComboBox_ConsecutiveTargetScalePolicy->GetSelectedOption(), ETargetDestructionPolicy::None);
	ReturnConfig.TargetActivationPolicy = GetEnumFromString<ETargetActivationPolicy>(ComboBox_ConsecutiveTargetScalePolicy->GetSelectedOption(), ETargetActivationPolicy::None);
	ReturnConfig.TargetDeactivationPolicy = GetEnumFromString<ETargetDeactivationPolicy>(ComboBox_ConsecutiveTargetScalePolicy->GetSelectedOption(), ETargetDeactivationPolicy::None);
	ReturnConfig.TargetDamageType = GetEnumFromString<ETargetDamageType>(ComboBox_ConsecutiveTargetScalePolicy->GetSelectedOption(), ETargetDamageType::None);
	ReturnConfig.NumCharges = FMath::GridSnap(FMath::Clamp(Slider_NumCharges->GetValue(), MinValue_NumCharges, MaxValue_NumCharges), SnapSize_NumCharges);
	ReturnConfig.ConsecutiveChargeScaleMultiplier = FMath::GridSnap(FMath::Clamp(Slider_ConsecutiveChargeScaleMultiplier->GetValue(), MinValue_ConsecutiveChargeScaleMultiplier,
		MaxValue_ConsecutiveChargeScaleMultiplier), SnapSize_ConsecutiveChargeScaleMultiplier);
	ReturnConfig.MinTrackingSpeed = FMath::GridSnap(FMath::Clamp(TargetSpeedConstrained->GetMinValue(), MinValue_TargetSpeed, MaxValue_TargetSpeed), SnapSize_TargetSpeed);
	ReturnConfig.MaxTrackingSpeed = FMath::GridSnap(FMath::Clamp(TargetSpeedConstrained->GetMaxValue(), MinValue_TargetSpeed, MaxValue_TargetSpeed), SnapSize_TargetSpeed);
	ReturnConfig.bApplyImmunityOnSpawn = CheckBox_ApplyImmunityOnSpawn->IsChecked();
	ReturnConfig.bResetPositionOnDeactivation = CheckBox_ResetPositionOnDeactivation->IsChecked();
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

void UGameModesWidget_TargetConfig::OnSliderChanged_NumCharges(const float NewCharges)
{
	OnSliderChanged(NewCharges, Value_NumCharges, SnapSize_NumCharges);
}

void UGameModesWidget_TargetConfig::OnSliderChanged_ChargeScaleMultiplier(const float NewChargeScaleMultiplier)
{
	OnSliderChanged(NewChargeScaleMultiplier, Value_ConsecutiveChargeScaleMultiplier, SnapSize_ConsecutiveChargeScaleMultiplier);
}

void UGameModesWidget_TargetConfig::OnTextCommitted_ChargeScaleMultiplier(const FText& NewChargeScaleMultiplier, ETextCommit::Type CommitType)
{
	OnEditableTextBoxChanged(NewChargeScaleMultiplier, Value_ConsecutiveChargeScaleMultiplier, Slider_ConsecutiveChargeScaleMultiplier, SnapSize_ConsecutiveChargeScaleMultiplier,
		MinValue_ConsecutiveChargeScaleMultiplier, MaxValue_ConsecutiveChargeScaleMultiplier);
}

void UGameModesWidget_TargetConfig::OnTextCommitted_NumCharges(const FText& NewCharges, ETextCommit::Type CommitType)
{
	OnEditableTextBoxChanged(NewCharges, Value_NumCharges, Slider_NumCharges, SnapSize_NumCharges, MinValue_NumCharges, MaxValue_NumCharges);
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

void UGameModesWidget_TargetConfig::OnSelectionChanged_LifetimeTargetScaleMethod(const FString SelectedMethod, const ESelectInfo::Type SelectionType)
{
	if (!SelectedMethod.Equals(UEnum::GetDisplayValueAsText(ELifetimeTargetScalePolicy::None).ToString()))
	{
		if (TargetScaleConstrained->GetIsSynced())
		{
			TargetScaleConstrained->UpdateDefaultValues(TargetScaleConstrained->GetMinValue(), TargetScaleConstrained->GetMaxValue(), false);
		}
	}
}

void UGameModesWidget_TargetConfig::OnSelectionChanged_ConsecutiveTargetScale(const FString SelectedMethod, const ESelectInfo::Type SelectionType)
{
	switch (GetEnumFromString<EConsecutiveTargetScalePolicy>(SelectedMethod, EConsecutiveTargetScalePolicy::None))
	{
	case EConsecutiveTargetScalePolicy::Static:
		if (!TargetScaleConstrained->GetIsSynced())
		{
			TargetScaleConstrained->UpdateDefaultValues(TargetScaleConstrained->GetMinValue(), TargetScaleConstrained->GetMaxValue(), true);
		}
		break;
	case EConsecutiveTargetScalePolicy::Random:
	case EConsecutiveTargetScalePolicy::SkillBased:
		if (TargetScaleConstrained->GetIsSynced())
		{
			TargetScaleConstrained->UpdateDefaultValues(TargetScaleConstrained->GetMinValue(), TargetScaleConstrained->GetMaxValue(), false);
		}
		break;
	default:
		break;
	}
}

void UGameModesWidget_TargetConfig::OnSelectionChanged_TargetSpawningPolicy(const FString SelectedPolicy, const ESelectInfo::Type SelectionType)
{
}

void UGameModesWidget_TargetConfig::OnSelectionChanged_TargetDestructionPolicy(const FString SelectedPolicy, const ESelectInfo::Type SelectionType)
{
}

void UGameModesWidget_TargetConfig::OnSelectionChanged_TargetActivationPolicy(const FString SelectedPolicy, const ESelectInfo::Type SelectionType)
{
}

void UGameModesWidget_TargetConfig::OnSelectionChanged_TargetDeactivationPolicy(const FString SelectedPolicy, const ESelectInfo::Type SelectionType)
{
}

void UGameModesWidget_TargetConfig::OnSelectionChanged_TargetDamageType(const FString SelectedPolicy, const ESelectInfo::Type SelectionType)
{
}

void UGameModesWidget_TargetConfig::OnCheckStateChanged_ApplyImmunityOnSpawn(const bool bApplyImmunityOnSpawn)
{
}

void UGameModesWidget_TargetConfig::OnCheckStateChanged_ResetPositionOnDeactivation(const bool bApplyImmunityOnSpawn)
{
}

UWidget* UGameModesWidget_TargetConfig::OnGenerateWidgetEvent_ConsecutiveTargetScale(FString Method)
{
	UBSComboBoxEntry* Entry = CreateWidget<UBSComboBoxEntry>(ComboBox_ConsecutiveTargetScalePolicy, ComboboxEntryWidget);
	Entry->SetText(FText::FromString(Method));
	/* For whatever reason, setting the selected option through code will trigger this function immediately
	 * after calling OnSelectionChangedGenerateWidgetEvent, so this is a sketchy way to deal with it for now */
	if (bHideTooltipImage_ConsecutiveTargetScale)
	{
		Entry->ToggleTooltipImageVisibility(false);
		bHideTooltipImage_ConsecutiveTargetScale = false;
		return Cast<UWidget>(Entry);
	}
	const FText TooltipText = FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips",
		GetStringTableKeyNameFromEnum(GetEnumFromString<EConsecutiveTargetScalePolicy>(Method, EConsecutiveTargetScalePolicy::None)));

	AddToTooltipData(Entry->TooltipImage, TooltipText, false);
	return Cast<UWidget>(Entry);
}

UWidget* UGameModesWidget_TargetConfig::OnGenerateWidgetEvent_LifetimeTargetScale(FString Method)
{
	UBSComboBoxEntry* Entry = CreateWidget<UBSComboBoxEntry>(ComboBox_LifetimeTargetScalePolicy, ComboboxEntryWidget);
	Entry->SetText(FText::FromString(Method));
	/* For whatever reason, setting the selected option through code will trigger this function immediately
	 * after calling OnSelectionChangedGenerateWidgetEvent, so this is a sketchy way to deal with it for now */
	if (bHideTooltipImage_LifetimeTargetScale)
	{
		Entry->ToggleTooltipImageVisibility(false);
		bHideTooltipImage_LifetimeTargetScale = false;
		return Cast<UWidget>(Entry);
	}

	const FText TooltipText = FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips",
	GetStringTableKeyNameFromEnum(GetEnumFromString<ELifetimeTargetScalePolicy>(Method, ELifetimeTargetScalePolicy::None)));

	AddToTooltipData(Entry->TooltipImage, TooltipText, false);
	return Cast<UWidget>(Entry);
}

UWidget* UGameModesWidget_TargetConfig::OnGenerateWidgetEvent_TargetSpawningPolicy(FString Method)
{
	UBSComboBoxEntry* Entry = CreateWidget<UBSComboBoxEntry>(ComboBox_TargetSpawningPolicy, ComboboxEntryWidget);
	Entry->SetText(FText::FromString(Method));
	/* For whatever reason, setting the selected option through code will trigger this function immediately
	 * after calling OnSelectionChangedGenerateWidgetEvent, so this is a sketchy way to deal with it for now */
	if (bHideTooltipImage_TargetSpawningPolicy)
	{
		Entry->ToggleTooltipImageVisibility(false);
		bHideTooltipImage_TargetSpawningPolicy = false;
		return Cast<UWidget>(Entry);
	}
	
	const FText TooltipText = FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips",
	GetStringTableKeyNameFromEnum(GetEnumFromString<ETargetSpawningPolicy>(Method, ETargetSpawningPolicy::None)));

	AddToTooltipData(Entry->TooltipImage, TooltipText, false);
	return Cast<UWidget>(Entry);
}

UWidget* UGameModesWidget_TargetConfig::OnGenerateWidgetEvent_TargetDestructionPolicy(FString Method)
{
	UBSComboBoxEntry* Entry = CreateWidget<UBSComboBoxEntry>(ComboBox_TargetDestructionPolicy, ComboboxEntryWidget);
	Entry->SetText(FText::FromString(Method));
	/* For whatever reason, setting the selected option through code will trigger this function immediately
	 * after calling OnSelectionChangedGenerateWidgetEvent, so this is a sketchy way to deal with it for now */
	if (bHideTooltipImage_TargetDestructionPolicy)
	{
		Entry->ToggleTooltipImageVisibility(false);
		bHideTooltipImage_TargetDestructionPolicy = false;
		return Cast<UWidget>(Entry);
	}
	
	const FText TooltipText = FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips",
	GetStringTableKeyNameFromEnum(GetEnumFromString<ETargetDestructionPolicy>(Method, ETargetDestructionPolicy::None)));

	AddToTooltipData(Entry->TooltipImage, TooltipText, false);
	return Cast<UWidget>(Entry);
}

UWidget* UGameModesWidget_TargetConfig::OnGenerateWidgetEvent_TargetActivationPolicy(FString Method)
{
	UBSComboBoxEntry* Entry = CreateWidget<UBSComboBoxEntry>(ComboBox_TargetActivationPolicy, ComboboxEntryWidget);
	Entry->SetText(FText::FromString(Method));
	/* For whatever reason, setting the selected option through code will trigger this function immediately
	 * after calling OnSelectionChangedGenerateWidgetEvent, so this is a sketchy way to deal with it for now */
	if (bHideTooltipImage_TargetActivationPolicy)
	{
		Entry->ToggleTooltipImageVisibility(false);
		bHideTooltipImage_TargetActivationPolicy = false;
		return Cast<UWidget>(Entry);
	}
	
	const FText TooltipText = FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips",
	GetStringTableKeyNameFromEnum(GetEnumFromString<ETargetActivationPolicy>(Method, ETargetActivationPolicy::None)));

	AddToTooltipData(Entry->TooltipImage, TooltipText, false);
	return Cast<UWidget>(Entry);
}

UWidget* UGameModesWidget_TargetConfig::OnGenerateWidgetEvent_TargetDeactivationPolicy(FString Method)
{
	UBSComboBoxEntry* Entry = CreateWidget<UBSComboBoxEntry>(ComboBox_TargetDeactivationPolicy, ComboboxEntryWidget);
	Entry->SetText(FText::FromString(Method));
	/* For whatever reason, setting the selected option through code will trigger this function immediately
	 * after calling OnSelectionChangedGenerateWidgetEvent, so this is a sketchy way to deal with it for now */
	if (bHideTooltipImage_TargetDeactivationPolicy)
	{
		Entry->ToggleTooltipImageVisibility(false);
		bHideTooltipImage_TargetDeactivationPolicy = false;
		return Cast<UWidget>(Entry);
	}
	
	const FText TooltipText = FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips",
	GetStringTableKeyNameFromEnum(GetEnumFromString<ETargetDeactivationPolicy>(Method, ETargetDeactivationPolicy::None)));

	AddToTooltipData(Entry->TooltipImage, TooltipText, false);
	return Cast<UWidget>(Entry);
}

UWidget* UGameModesWidget_TargetConfig::OnGenerateWidgetEvent_TargetDamageType(FString Method)
{
	UBSComboBoxEntry* Entry = CreateWidget<UBSComboBoxEntry>(ComboBox_TargetDamageType, ComboboxEntryWidget);
	Entry->SetText(FText::FromString(Method));
	/* For whatever reason, setting the selected option through code will trigger this function immediately
	 * after calling OnSelectionChangedGenerateWidgetEvent, so this is a sketchy way to deal with it for now */
	if (bHideTooltipImage_TargetDamageType)
	{
		Entry->ToggleTooltipImageVisibility(false);
		bHideTooltipImage_TargetDamageType = false;
		return Cast<UWidget>(Entry);
	}
	
	const FText TooltipText = FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips",
	GetStringTableKeyNameFromEnum(GetEnumFromString<ETargetDamageType>(Method, ETargetDamageType::None)));

	AddToTooltipData(Entry->TooltipImage, TooltipText, false);
	return Cast<UWidget>(Entry);
}

UWidget* UGameModesWidget_TargetConfig::OnSelectionChangedGenerateWidgetEvent_ConsecutiveTargetScale(FString Method)
{
	bHideTooltipImage_ConsecutiveTargetScale = true;
	UBSComboBoxEntry* Entry = CreateWidget<UBSComboBoxEntry>(ComboBox_ConsecutiveTargetScalePolicy, ComboboxEntryWidget);
	Entry->SetText(FText::FromString(Method));
	Entry->ToggleTooltipImageVisibility(false);
	return Cast<UWidget>(Entry);
}

UWidget* UGameModesWidget_TargetConfig::OnSelectionChangedGenerateWidgetEvent_LifetimeTargetScale(FString Method)
{
	bHideTooltipImage_LifetimeTargetScale = true;
	UBSComboBoxEntry* Entry = CreateWidget<UBSComboBoxEntry>(ComboBox_LifetimeTargetScalePolicy, ComboboxEntryWidget);
	Entry->SetText(FText::FromString(Method));
	Entry->ToggleTooltipImageVisibility(false);
	return Cast<UWidget>(Entry);
}

UWidget* UGameModesWidget_TargetConfig::OnSelectionChangedGenerateWidgetEvent_TargetSpawningPolicy(FString Method)
{
	bHideTooltipImage_TargetSpawningPolicy = true;
	UBSComboBoxEntry* Entry = CreateWidget<UBSComboBoxEntry>(ComboBox_TargetSpawningPolicy, ComboboxEntryWidget);
	Entry->SetText(FText::FromString(Method));
	Entry->ToggleTooltipImageVisibility(false);
	return Cast<UWidget>(Entry);
}

UWidget* UGameModesWidget_TargetConfig::OnSelectionChangedGenerateWidgetEvent_TargetDestructionPolicy(FString Method)
{
	bHideTooltipImage_TargetDestructionPolicy = true;
	UBSComboBoxEntry* Entry = CreateWidget<UBSComboBoxEntry>(ComboBox_TargetDestructionPolicy, ComboboxEntryWidget);
	Entry->SetText(FText::FromString(Method));
	Entry->ToggleTooltipImageVisibility(false);
	return Cast<UWidget>(Entry);
}

UWidget* UGameModesWidget_TargetConfig::OnSelectionChangedGenerateWidgetEvent_TargetActivationPolicy(FString Method)
{
	bHideTooltipImage_TargetActivationPolicy = true;
	UBSComboBoxEntry* Entry = CreateWidget<UBSComboBoxEntry>(ComboBox_TargetActivationPolicy, ComboboxEntryWidget);
	Entry->SetText(FText::FromString(Method));
	Entry->ToggleTooltipImageVisibility(false);
	return Cast<UWidget>(Entry);
}

UWidget* UGameModesWidget_TargetConfig::OnSelectionChangedGenerateWidgetEvent_TargetDeactivationPolicy(FString Method)
{
	bHideTooltipImage_TargetDeactivationPolicy = true;
	UBSComboBoxEntry* Entry = CreateWidget<UBSComboBoxEntry>(ComboBox_TargetDeactivationPolicy, ComboboxEntryWidget);
	Entry->SetText(FText::FromString(Method));
	Entry->ToggleTooltipImageVisibility(false);
	return Cast<UWidget>(Entry);
}

UWidget* UGameModesWidget_TargetConfig::OnSelectionChangedGenerateWidgetEvent_TargetDamageType(FString Method)
{
	bHideTooltipImage_TargetDamageType = true;
	UBSComboBoxEntry* Entry = CreateWidget<UBSComboBoxEntry>(ComboBox_TargetDamageType, ComboboxEntryWidget);
	Entry->SetText(FText::FromString(Method));
	Entry->ToggleTooltipImageVisibility(false);
	return Cast<UWidget>(Entry);
}
