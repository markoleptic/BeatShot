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
#include "WidgetComponents/BSVerticalBox.h"

using namespace Constants;

void UGameModesWidget_TargetConfig::NativeConstruct()
{
	/* Create MainContainer before calling Super NativeConstruct since the parent calls InitSettingCategoryWidget in NativeConstruct */
	TargetScaleConstrained = CreateWidget<UDoubleSyncedSliderAndTextBox>(this, TargetScaleConstrainedClass);
	MainContainer->AddChildToVerticalBox(TargetScaleConstrained.Get());
	
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
	TargetScaleSliderStruct.CheckboxText = FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "GM_ConstantTargetSize");
	TargetScaleSliderStruct.GridSnapSize = SnapSize_TargetScale;
	TargetScaleSliderStruct.bSyncSlidersAndValues = false;
	TargetScaleConstrained->InitConstrainedSlider(TargetScaleSliderStruct);

	for (const ELifetimeTargetScaleMethod& Method : TEnumRange<ELifetimeTargetScaleMethod>())
	{
		ComboBox_LifetimeTargetScale->AddOption(UEnum::GetDisplayValueAsText(Method).ToString());
	}

	SetTooltipWidget(ConstructTooltipWidget());
	
	AddToTooltipData(QMark_Lifespan, FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "Lifespan"));
	AddToTooltipData(QMark_TargetSpawnCD, FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "MinDistance"));
	AddToTooltipData(QMark_DynamicTargetScale, FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "DynamicTargetScale"));
	AddToTooltipData(TargetScaleConstrained->GetCheckBoxQMark(), FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "ConstantTargetScale"));
	AddToTooltipData(QMark_SpawnBeatDelay, FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "SpawnBeatDelay"));
	const TArray LifetimeTargetScaleJoin = { FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "LifetimeTargetScale"),
		FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "LifetimeTargetScale_Grow"),
		FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "LifetimeTargetScale_Shrink") };
	AddToTooltipData(QMark_LifetimeTargetScale, FText::Join(NewLineDelimit, LifetimeTargetScaleJoin));

	Slider_Lifespan->OnValueChanged.AddDynamic(this, &UGameModesWidget_TargetConfig::OnSliderChanged_Lifespan);
	Value_Lifespan->OnTextCommitted.AddDynamic(this, &UGameModesWidget_TargetConfig::OnTextCommitted_Lifespan);
	Slider_TargetSpawnCD->OnValueChanged.AddDynamic(this, &UGameModesWidget_TargetConfig::OnSliderChanged_TargetSpawnCD);
	Value_TargetSpawnCD->OnTextCommitted.AddDynamic(this, &UGameModesWidget_TargetConfig::OnTextCommitted_TargetSpawnCD);
	Slider_SpawnBeatDelay->OnValueChanged.AddDynamic(this, &UGameModesWidget_TargetConfig::OnSliderChanged_SpawnBeatDelay);
	Value_SpawnBeatDelay->OnTextCommitted.AddDynamic(this, &UGameModesWidget_TargetConfig::OnTextCommitted_SpawnBeatDelay);
	CheckBox_DynamicTargetScale->OnCheckStateChanged.AddDynamic(this, &UGameModesWidget_TargetConfig::OnCheckStateChanged_DynamicTargetScale);
	TargetScaleConstrained->OnCheckStateChanged_Sync.AddUObject(this, &UGameModesWidget_TargetConfig::OnCheckStateChanged_ConstantTargetScale);
	ComboBox_LifetimeTargetScale->OnSelectionChanged.AddDynamic(this, &UGameModesWidget_TargetConfig::OnSelectionChanged_LifetimeTargetScaleMethod);
}

void UGameModesWidget_TargetConfig::InitSettingCategoryWidget()
{
	if (TargetScaleConstrained)
	{
		AddWidgetBoxPair(TargetScaleConstrained.Get(), TargetScaleConstrained->GetMainContainer());
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
		break;
	case EBaseGameMode::MultiBeat:
		Slider_Lifespan->SetLocked(false);
		Value_Lifespan->SetIsReadOnly(false);
		break;
	case EBaseGameMode::BeatGrid:
		Slider_Lifespan->SetLocked(false);
		Value_Lifespan->SetIsReadOnly(false);
		break;
	case EBaseGameMode::BeatTrack:
		Slider_Lifespan->SetLocked(true);
		Value_Lifespan->SetIsReadOnly(true);
		break;
	default:
		break;
	}
	
	Slider_Lifespan->SetValue(InTargetConfig.TargetMaxLifeSpan);
	Value_Lifespan->SetText(FText::AsNumber(InTargetConfig.TargetMaxLifeSpan));
	Slider_TargetSpawnCD->SetValue(InTargetConfig.TargetSpawnCD);
	Value_TargetSpawnCD->SetText(FText::AsNumber(InTargetConfig.TargetSpawnCD));
	Slider_SpawnBeatDelay->SetValue(InTargetConfig.SpawnBeatDelay);
	Value_SpawnBeatDelay->SetText(FText::AsNumber(InTargetConfig.SpawnBeatDelay));
	CheckBox_DynamicTargetScale->SetIsChecked(InTargetConfig.ConsecutiveTargetScaleMethod == EConsecutiveTargetScaleMethod::SkillBased);
	TargetScaleConstrained->UpdateDefaultValues(InTargetConfig.MinTargetScale, InTargetConfig.MaxTargetScale, InTargetConfig.MinTargetScale == InTargetConfig.MaxTargetScale);
	ComboBox_LifetimeTargetScale->SetSelectedOption(UEnum::GetDisplayValueAsText(InTargetConfig.LifetimeTargetScaleMethod).ToString());
	
	UpdateBrushColors();
}

FBS_TargetConfig UGameModesWidget_TargetConfig::GetTargetConfig() const
{
	FBS_TargetConfig ReturnConfig;
	ReturnConfig.TargetMaxLifeSpan = FMath::GridSnap(FMath::Clamp(Slider_Lifespan->GetValue(), MinValue_Lifespan, MaxValue_Lifespan), SnapSize_Lifespan);
	ReturnConfig.TargetSpawnCD = FMath::GridSnap(FMath::Clamp(Slider_TargetSpawnCD->GetValue(), MinValue_TargetSpawnCD, MaxValue_TargetSpawnCD), SnapSize_TargetSpawnCD);
	if (CheckBox_DynamicTargetScale->IsChecked())
	{
		ReturnConfig.ConsecutiveTargetScaleMethod = EConsecutiveTargetScaleMethod::SkillBased;
	}
	else
	{
		ReturnConfig.ConsecutiveTargetScaleMethod = EConsecutiveTargetScaleMethod::Random;
	}
	ReturnConfig.MinTargetScale = FMath::GridSnap(FMath::Clamp(TargetScaleConstrained->GetMinValue(), MinValue_TargetScale, MaxValue_TargetScale), SnapSize_TargetScale);
	ReturnConfig.MaxTargetScale = FMath::GridSnap(FMath::Clamp(TargetScaleConstrained->GetMaxValue(), MinValue_TargetScale, MaxValue_TargetScale), SnapSize_TargetScale);
	ReturnConfig.SpawnBeatDelay = FMath::GridSnap(FMath::Clamp(Slider_SpawnBeatDelay->GetValue(), MinValue_PlayerDelay, MaxValue_PlayerDelay), SnapSize_PlayerDelay);

	for (const ELifetimeTargetScaleMethod& Method : TEnumRange<ELifetimeTargetScaleMethod>())
	{
		if (ComboBox_LifetimeTargetScale->GetSelectedOption().Equals(UEnum::GetDisplayValueAsText(Method).ToString()))
		{
			ReturnConfig.LifetimeTargetScaleMethod = Method;
			break;
		}
	}

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
	if (!SelectedMethod.Equals(UEnum::GetDisplayValueAsText(ELifetimeTargetScaleMethod::None).ToString()))
	{
		if (TargetScaleConstrained->GetIsSynced())
		{
			TargetScaleConstrained->UpdateDefaultValues(TargetScaleConstrained->GetMinValue(), TargetScaleConstrained->GetMaxValue(), false);
		}
	}
}

void UGameModesWidget_TargetConfig::OnCheckStateChanged_DynamicTargetScale(const bool bIsChecked)
{
	if (bIsChecked && TargetScaleConstrained->GetIsSynced())
	{
		TargetScaleConstrained->UpdateDefaultValues(TargetScaleConstrained->GetMinValue(), TargetScaleConstrained->GetMaxValue(), false);
	}
}

void UGameModesWidget_TargetConfig::OnCheckStateChanged_ConstantTargetScale(const bool bIsChecked)
{
	if (bIsChecked)
	{
		if (CheckBox_DynamicTargetScale->IsChecked())
		{
			CheckBox_DynamicTargetScale->SetIsChecked(false);
		}
		if (!ComboBox_LifetimeTargetScale->GetSelectedOption().Equals(UEnum::GetDisplayValueAsText(ELifetimeTargetScaleMethod::None).ToString()))
		{
			ComboBox_LifetimeTargetScale->SetSelectedOption(UEnum::GetDisplayValueAsText(ELifetimeTargetScaleMethod::None).ToString());
		}
	}
}
