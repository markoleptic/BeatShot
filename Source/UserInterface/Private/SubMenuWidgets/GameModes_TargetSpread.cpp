// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SubMenuWidgets/GameModes_TargetSpread.h"
#include "GlobalConstants.h"
#include "Components/CheckBox.h"
#include "Components/ComboBoxString.h"
#include "Components/EditableTextBox.h"
#include "Components/Slider.h"
#include "WidgetComponents/BSHorizontalBox.h"
#include "WidgetComponents/TooltipImage.h"
#include "WidgetComponents/TooltipWidget.h"

using namespace Constants;

UTooltipWidget* UGameModes_TargetSpread::ConstructTooltipWidget()
{
	return CreateWidget<UTooltipWidget>(this, TooltipWidgetClass);
}

void UGameModes_TargetSpread::NativeConstruct()
{
	Super::NativeConstruct();
	
	SetTooltipWidget(ConstructTooltipWidget());
	AddToTooltipData(QMark_HeadshotHeight, FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "HeadshotHeight"));
	AddToTooltipData(QMark_FloorDistance, FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "FloorDistance"));
	AddToTooltipData(QMark_ForwardSpread, FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "ForwardSpread"));
	AddToTooltipData(QMark_MinDistance, FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "MinDistance"));
	AddToTooltipData(QMark_SpreadType, FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "SpreadType"));
	CheckBox_HeadShotOnly->OnCheckStateChanged.AddDynamic(this, &UGameModes_TargetSpread::OnCheckStateChanged_HeadShotOnly);
	CheckBox_ForwardSpread->OnCheckStateChanged.AddDynamic(this, &UGameModes_TargetSpread::OnCheckStateChanged_ForwardSpread);
	Slider_MinTargetDistance->OnValueChanged.AddDynamic(this, &UGameModes_TargetSpread::OnSliderChanged_MinTargetDistance);
	Value_MinTargetDistance->OnTextCommitted.AddDynamic(this, &UGameModes_TargetSpread::OnTextCommitted_MinTargetDistance);
	Slider_HorizontalSpread->OnValueChanged.AddDynamic(this, &UGameModes_TargetSpread::OnSliderChanged_HorizontalSpread);
	Value_HorizontalSpread->OnTextCommitted.AddDynamic(this, &UGameModes_TargetSpread::OnTextCommitted_HorizontalSpread);
	Slider_VerticalSpread->OnValueChanged.AddDynamic(this, &UGameModes_TargetSpread::OnSliderChanged_VerticalSpread);
	Value_VerticalSpread->OnTextCommitted.AddDynamic(this, &UGameModes_TargetSpread::OnTextCommitted_VerticalSpread);
	Slider_ForwardSpread->OnValueChanged.AddDynamic(this, &UGameModes_TargetSpread::OnSliderChanged_ForwardSpread);
	Value_ForwardSpread->OnTextCommitted.AddDynamic(this, &UGameModes_TargetSpread::OnTextCommitted_ForwardSpread);
	Slider_FloorDistance->OnValueChanged.AddDynamic(this, &UGameModes_TargetSpread::OnSliderChanged_FloorDistance);
	Value_FloorDistance->OnTextCommitted.AddDynamic(this, &UGameModes_TargetSpread::OnTextCommitted_FloorDistance);
}

void UGameModes_TargetSpread::InitializeTargetSpread(const FBS_SpatialConfig& SpatialConfig, const EDefaultMode& BaseGameMode)
{
	if (SpatialConfig.bUseHeadshotHeight)
	{
		Slider_VerticalSpread->SetLocked(true);
		Value_VerticalSpread->SetIsReadOnly(true);
		Slider_FloorDistance->SetLocked(true);
		Value_FloorDistance->SetIsReadOnly(true);
	}
	else
	{
		Slider_FloorDistance->SetLocked(false);
		Value_FloorDistance->SetIsReadOnly(false);
		Slider_VerticalSpread->SetLocked(false);
		Value_VerticalSpread->SetIsReadOnly(false);
	}

	if (SpatialConfig.bMoveTargetsForward)
	{
		CheckBox_ForwardSpread->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		CheckBox_ForwardSpread->SetVisibility(ESlateVisibility::Collapsed);
	}

	switch(BaseGameMode)
	{
	case EDefaultMode::Custom:
		break;
	case EDefaultMode::SingleBeat:
		break;
	case EDefaultMode::MultiBeat:
		break;
	case EDefaultMode::BeatGrid:
		Slider_HorizontalSpread->SetLocked(true);
		Value_HorizontalSpread->SetIsReadOnly(true);
		Slider_VerticalSpread->SetLocked(true);
		Value_VerticalSpread->SetIsReadOnly(true);
		BSBox_MinTargetDistance->SetVisibility(ESlateVisibility::Collapsed);
		ComboBox_SpreadType->SetVisibility(ESlateVisibility::Collapsed);
		break;
	case EDefaultMode::BeatTrack:
		BSBox_MinTargetDistance->SetVisibility(ESlateVisibility::Collapsed);
		ComboBox_SpreadType->SetVisibility(ESlateVisibility::Collapsed);
		break;
	default:
		break;
	}

	if (BaseGameMode != EDefaultMode::BeatTrack)
	{
		BSBox_MinTargetDistance->SetVisibility(ESlateVisibility::Visible);
		BSBox_SpreadType->SetVisibility(ESlateVisibility::Visible);
	}

	if (BaseGameMode != EDefaultMode::BeatGrid)
	{
		Slider_HorizontalSpread->SetLocked(false);
		Value_HorizontalSpread->SetIsReadOnly(false);
		Slider_VerticalSpread->SetLocked(false);
		Value_VerticalSpread->SetIsReadOnly(false);
		BSBox_MinTargetDistance->SetVisibility(ESlateVisibility::Visible);
		ComboBox_SpreadType->SetVisibility(ESlateVisibility::Visible);
	}

	CheckBox_HeadShotOnly->SetIsChecked(SpatialConfig.bUseHeadshotHeight);
	Slider_FloorDistance->SetValue(SpatialConfig.FloorDistance);
	Value_FloorDistance->SetText(FText::AsNumber(SpatialConfig.FloorDistance));
	Slider_MinTargetDistance->SetValue(SpatialConfig.MinDistanceBetweenTargets);
	Value_MinTargetDistance->SetText(FText::AsNumber(SpatialConfig.MinDistanceBetweenTargets));
	ComboBox_SpreadType->SetSelectedOption(UEnum::GetDisplayValueAsText(SpatialConfig.SpreadType).ToString());
	Slider_HorizontalSpread->SetValue(SpatialConfig.BoxBounds.Y);
	Value_HorizontalSpread->SetText(FText::AsNumber(SpatialConfig.BoxBounds.Y));
	Slider_VerticalSpread->SetValue(SpatialConfig.BoxBounds.Z);
	Value_VerticalSpread->SetText(FText::AsNumber(SpatialConfig.BoxBounds.Z));
	CheckBox_ForwardSpread->SetIsChecked(SpatialConfig.bMoveTargetsForward);
	Slider_ForwardSpread->SetValue(SpatialConfig.MoveForwardDistance);
	Value_ForwardSpread->SetText(FText::AsNumber(SpatialConfig.MoveForwardDistance));
}

FBS_SpatialConfig UGameModes_TargetSpread::GetSpatialConfig() const
{
	FBS_SpatialConfig SpatialConfig;
	SpatialConfig.bUseHeadshotHeight = CheckBox_HeadShotOnly->IsChecked();
	SpatialConfig.FloorDistance = FMath::GridSnap(FMath::Clamp(Slider_FloorDistance->GetValue(), MinValue_FloorDistance, MaxValue_FloorDistance), SnapSize_FloorDistance);
	SpatialConfig.MinDistanceBetweenTargets = FMath::GridSnap(FMath::Clamp(Slider_MinTargetDistance->GetValue(), MinValue_MinTargetDistance, MaxValue_MinTargetDistance), SnapSize_MinTargetDistance);
	SpatialConfig.SpreadType = GetSpreadType();
	SpatialConfig.BoxBounds = FVector(0, FMath::GridSnap(FMath::Clamp(Slider_HorizontalSpread->GetValue(), MinValue_HorizontalSpread, MaxValue_HorizontalSpread), SnapSize_HorizontalSpread),
									 FMath::GridSnap(FMath::Clamp(Slider_VerticalSpread->GetValue(), MinValue_VerticalSpread, MaxValue_VerticalSpread), SnapSize_VerticalSpread));
	SpatialConfig.bMoveTargetsForward = CheckBox_ForwardSpread->IsChecked();
	SpatialConfig.MoveForwardDistance = FMath::GridSnap(FMath::Clamp(Slider_ForwardSpread->GetValue(), MinValue_ForwardSpread, MaxValue_ForwardSpread), SnapSize_HorizontalSpread);
	return SpatialConfig;
}

void UGameModes_TargetSpread::OnTextCommitted_MinTargetDistance(const FText& NewMinTargetDistance, ETextCommit::Type CommitType)
{
	OnEditableTextBoxChanged(NewMinTargetDistance, Value_MinTargetDistance, Slider_MinTargetDistance, Constants::SnapSize_MinTargetDistance,MinValue_MinTargetDistance, MaxValue_MinTargetDistance);
}

void UGameModes_TargetSpread::OnTextCommitted_HorizontalSpread(const FText& NewHorizontalSpread, ETextCommit::Type CommitType)
{
	OnEditableTextBoxChanged(NewHorizontalSpread, Value_HorizontalSpread, Slider_HorizontalSpread, SnapSize_HorizontalSpread, MinValue_HorizontalSpread, MaxValue_HorizontalSpread);
}

void UGameModes_TargetSpread::OnTextCommitted_VerticalSpread(const FText& NewVerticalSpread, ETextCommit::Type CommitType)
{
	OnEditableTextBoxChanged(NewVerticalSpread, Value_VerticalSpread, Slider_VerticalSpread, SnapSize_VerticalSpread, MinValue_VerticalSpread, MaxValue_VerticalSpread);
}

void UGameModes_TargetSpread::OnTextCommitted_ForwardSpread(const FText& NewForwardSpread, ETextCommit::Type CommitType)
{
	OnEditableTextBoxChanged(NewForwardSpread, Value_ForwardSpread, Slider_ForwardSpread, SnapSize_HorizontalSpread, MinValue_ForwardSpread, MaxValue_ForwardSpread);
}

void UGameModes_TargetSpread::OnTextCommitted_FloorDistance(const FText& NewFloorDistance, ETextCommit::Type CommitType)
{
	OnEditableTextBoxChanged(NewFloorDistance, Value_FloorDistance, Slider_FloorDistance, SnapSize_FloorDistance, MinValue_FloorDistance, MaxValue_FloorDistance);
}

void UGameModes_TargetSpread::OnSliderChanged_MinTargetDistance(const float NewMinTargetDistance)
{
	OnSliderChanged(NewMinTargetDistance, Value_MinTargetDistance, SnapSize_MinTargetDistance);
}

void UGameModes_TargetSpread::OnSliderChanged_HorizontalSpread(const float NewHorizontalSpread)
{
	OnSliderChanged(NewHorizontalSpread, Value_HorizontalSpread, SnapSize_HorizontalSpread);
}

void UGameModes_TargetSpread::OnSliderChanged_VerticalSpread(const float NewVerticalSpread)
{
	OnSliderChanged(NewVerticalSpread, Value_VerticalSpread, SnapSize_VerticalSpread);
}

void UGameModes_TargetSpread::OnSliderChanged_ForwardSpread(const float NewForwardSpread)
{
	OnSliderChanged(NewForwardSpread, Value_ForwardSpread, SnapSize_HorizontalSpread);
}

void UGameModes_TargetSpread::OnSliderChanged_FloorDistance(const float NewFloorDistance)
{
	OnSliderChanged(NewFloorDistance, Value_FloorDistance, SnapSize_FloorDistance);
}

ESpreadType UGameModes_TargetSpread::GetSpreadType() const
{
	const FString SelectedSpread = ComboBox_SpreadType->GetSelectedOption();
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

void UGameModes_TargetSpread::OnCheckStateChanged_HeadShotOnly(const bool bHeadshotOnly)
{
	if (bHeadshotOnly)
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

void UGameModes_TargetSpread::OnCheckStateChanged_ForwardSpread(const bool bUseForwardSpread)
{
	if (bUseForwardSpread)
	{
		CheckBox_ForwardSpread->SetVisibility(ESlateVisibility::Visible);
		return;
	}
	CheckBox_ForwardSpread->SetVisibility(ESlateVisibility::Collapsed);
}
