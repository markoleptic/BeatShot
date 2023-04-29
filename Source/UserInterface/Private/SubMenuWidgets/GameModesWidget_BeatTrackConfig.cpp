// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SubMenuWidgets/GameModesWidget_BeatTrackConfig.h"
#include "Components/Slider.h"
#include "WidgetComponents/BSVerticalBox.h"
#include "WidgetComponents/DoubleSyncedSliderAndTextBox.h"

using namespace Constants;

void UGameModesWidget_BeatTrackConfig::NativeConstruct()
{
	/* Create MainContainer before calling Super NativeConstruct since the parent calls InitSettingCategoryWidget in NativeConstruct */
	TargetSpeedConstrained = CreateWidget<UDoubleSyncedSliderAndTextBox>(this, TargetSpeedConstrainedClass);
	MainContainer->AddChildToVerticalBox(TargetSpeedConstrained.Get());
	
	Super::NativeConstruct();

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
}

void UGameModesWidget_BeatTrackConfig::InitSettingCategoryWidget()
{
	if (TargetSpeedConstrained)
	{
		AddWidgetBoxPair(TargetSpeedConstrained.Get(), TargetSpeedConstrained->GetMainContainer());
	}
	Super::InitSettingCategoryWidget();
}

void UGameModesWidget_BeatTrackConfig::InitializeBeatTrackConfig(const FBS_BeatTrackConfig& InBeatTrackConfig, const EDefaultMode& BaseGameMode)
{
	TargetSpeedConstrained->UpdateDefaultValues(InBeatTrackConfig.MinTrackingSpeed, InBeatTrackConfig.MaxTrackingSpeed, InBeatTrackConfig.MinTrackingSpeed == InBeatTrackConfig.MaxTrackingSpeed);
	UpdateBrushColors();
}

FBS_BeatTrackConfig UGameModesWidget_BeatTrackConfig::GetBeatTrackConfig() const
{
	FBS_BeatTrackConfig ReturnConfig;
	ReturnConfig.MinTrackingSpeed = FMath::GridSnap(FMath::Clamp(TargetSpeedConstrained->GetMinValue(), MinValue_TargetSpeed, MaxValue_TargetSpeed), SnapSize_TargetSpeed);
	ReturnConfig.MaxTrackingSpeed = FMath::GridSnap(FMath::Clamp(TargetSpeedConstrained->GetMaxValue(), MinValue_TargetSpeed, MaxValue_TargetSpeed), SnapSize_TargetSpeed);
	return ReturnConfig;
}
