// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

// ReSharper disable CppMemberFunctionMayBeConst
#include "SubMenuWidgets/GameModesWidget_TargetConfig.h"
#include "WidgetComponents/DoubleSyncedSliderAndTextBox.h"
#include "GlobalConstants.h"
#include "UserInterface.h"
#include "Components/CheckBox.h"
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

	SetTooltipWidget(ConstructTooltipWidget());

	AddToTooltipData(QMark_Lifespan, FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "Lifespan"));
	AddToTooltipData(QMark_TargetSpawnCD, FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "MinDistance"));
	AddToTooltipData(QMark_DynamicTargetScale, FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "DynamicTargetScale"));
	AddToTooltipData(TargetScaleConstrained->CheckboxQMark, FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "ConstantTargetScale"));
	AddToTooltipData(QMark_SpawnBeatDelay, FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "SpawnBeatDelay"));

	Slider_Lifespan->OnValueChanged.AddDynamic(this, &UGameModesWidget_TargetConfig::OnSliderChanged_Lifespan);
	Value_Lifespan->OnTextCommitted.AddDynamic(this, &UGameModesWidget_TargetConfig::OnTextCommitted_Lifespan);
	Slider_TargetSpawnCD->OnValueChanged.AddDynamic(this, &UGameModesWidget_TargetConfig::OnSliderChanged_TargetSpawnCD);
	Value_TargetSpawnCD->OnTextCommitted.AddDynamic(this, &UGameModesWidget_TargetConfig::OnTextCommitted_TargetSpawnCD);
	Slider_SpawnBeatDelay->OnValueChanged.AddDynamic(this, &UGameModesWidget_TargetConfig::OnSliderChanged_SpawnBeatDelay);
	Value_SpawnBeatDelay->OnTextCommitted.AddDynamic(this, &UGameModesWidget_TargetConfig::OnTextCommitted_SpawnBeatDelay);
}

void UGameModesWidget_TargetConfig::InitSettingCategoryWidget()
{
	if (TargetScaleConstrained)
	{
		AddWidgetBoxPair(TargetScaleConstrained.Get(), TargetScaleConstrained->MainContainer);
	}
	Super::InitSettingCategoryWidget();
}

void UGameModesWidget_TargetConfig::InitializeTargetConfig(const FBS_TargetConfig& InTargetConfig, const EDefaultMode& BaseGameMode)
{
	switch(BaseGameMode)
	{
	case EDefaultMode::Custom:
		Slider_Lifespan->SetLocked(false);
		Value_Lifespan->SetIsReadOnly(false);
		break;
	case EDefaultMode::SingleBeat:
		Slider_Lifespan->SetLocked(false);
		Value_Lifespan->SetIsReadOnly(false);
		break;
	case EDefaultMode::MultiBeat:
		Slider_Lifespan->SetLocked(false);
		Value_Lifespan->SetIsReadOnly(false);
		break;
	case EDefaultMode::BeatGrid:
		Slider_Lifespan->SetLocked(false);
		Value_Lifespan->SetIsReadOnly(false);
		break;
	case EDefaultMode::BeatTrack:
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
	TargetScaleConstrained->UpdateDefaultValues(InTargetConfig.MinTargetScale, InTargetConfig.MaxTargetScale);
	Slider_SpawnBeatDelay->SetValue(InTargetConfig.SpawnBeatDelay);
	Value_SpawnBeatDelay->SetText(FText::AsNumber(InTargetConfig.SpawnBeatDelay));
}

FBS_TargetConfig UGameModesWidget_TargetConfig::GetTargetConfig() const
{
	FBS_TargetConfig ReturnConfig;
	ReturnConfig.TargetMaxLifeSpan = FMath::GridSnap(FMath::Clamp(Slider_Lifespan->GetValue(), MinValue_Lifespan, MaxValue_Lifespan), SnapSize_Lifespan);
	ReturnConfig.TargetSpawnCD = FMath::GridSnap(FMath::Clamp(Slider_TargetSpawnCD->GetValue(), MinValue_TargetSpawnCD, MaxValue_TargetSpawnCD), SnapSize_TargetSpawnCD);
	ReturnConfig.UseDynamicSizing = CheckBox_DynamicTargetScale->IsChecked();
	ReturnConfig.MinTargetScale = FMath::GridSnap(FMath::Clamp(TargetScaleConstrained->MinSlider->GetValue(), MinValue_TargetScale, MaxValue_TargetScale), SnapSize_TargetScale);
	ReturnConfig.MaxTargetScale = FMath::GridSnap(FMath::Clamp(TargetScaleConstrained->MaxSlider->GetValue(), MinValue_TargetScale, MaxValue_TargetScale), SnapSize_TargetScale);
	ReturnConfig.SpawnBeatDelay = FMath::GridSnap(FMath::Clamp(Slider_SpawnBeatDelay->GetValue(), MinValue_PlayerDelay, MaxValue_PlayerDelay), SnapSize_PlayerDelay);
	return ReturnConfig;
}

void UGameModesWidget_TargetConfig::OnSliderChanged_Lifespan(const float NewLifespan)
{
	UUserInterface::OnSliderChanged(NewLifespan, Value_Lifespan, SnapSize_Lifespan);
}

void UGameModesWidget_TargetConfig::OnSliderChanged_TargetSpawnCD(const float NewTargetSpawnCD)
{
	UUserInterface::OnSliderChanged(NewTargetSpawnCD, Value_TargetSpawnCD, SnapSize_TargetSpawnCD);
}

void UGameModesWidget_TargetConfig::OnSliderChanged_SpawnBeatDelay(const float NewPlayerDelay)
{
	UUserInterface::OnSliderChanged(NewPlayerDelay, Value_SpawnBeatDelay, SnapSize_PlayerDelay);
}

void UGameModesWidget_TargetConfig::OnTextCommitted_Lifespan(const FText& NewLifespan, ETextCommit::Type CommitType)
{
	UUserInterface::OnEditableTextBoxChanged(NewLifespan, Value_Lifespan, Slider_Lifespan, SnapSize_Lifespan, MinValue_Lifespan, MaxValue_Lifespan);
}

void UGameModesWidget_TargetConfig::OnTextCommitted_TargetSpawnCD(const FText& NewTargetSpawnCD, ETextCommit::Type CommitType)
{
	UUserInterface::OnEditableTextBoxChanged(NewTargetSpawnCD, Value_TargetSpawnCD, Slider_TargetSpawnCD, SnapSize_TargetSpawnCD,MinValue_TargetSpawnCD, MaxValue_TargetSpawnCD);
}

void UGameModesWidget_TargetConfig::OnTextCommitted_SpawnBeatDelay(const FText& NewPlayerDelay, ETextCommit::Type CommitType)
{
	UUserInterface::OnEditableTextBoxChanged(NewPlayerDelay, Value_SpawnBeatDelay, Slider_SpawnBeatDelay, SnapSize_PlayerDelay, MinValue_PlayerDelay, MaxValue_PlayerDelay);
}
