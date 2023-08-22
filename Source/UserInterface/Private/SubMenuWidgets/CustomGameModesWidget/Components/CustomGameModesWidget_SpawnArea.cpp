﻿// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SubMenuWidgets/CustomGameModesWidget/Components/CustomGameModesWidget_SpawnArea.h"
#include "WidgetComponents/BSComboBoxString.h"
#include "WidgetComponents/MenuOptionWidgets/ComboBoxOptionWidget.h"
#include "WidgetComponents/MenuOptionWidgets/SliderTextBoxWidget.h"

using namespace Constants;

void UCustomGameModesWidget_SpawnArea::InitComponent(FBSConfig* InConfigPtr, TObjectPtr<UCustomGameModesWidgetComponent> InNext)
{
	Super::InitComponent(InConfigPtr, InNext);
}

void UCustomGameModesWidget_SpawnArea::NativeConstruct()
{
	Super::NativeConstruct();

	SetupTooltip(ComboBoxOption_BoundsScalingPolicy->GetTooltipImage(), ComboBoxOption_BoundsScalingPolicy->GetTooltipRegularText());
	SetupTooltip(SliderTextBoxOption_NumHorizontalGridTargets->GetTooltipImage(), SliderTextBoxOption_NumHorizontalGridTargets->GetTooltipRegularText());
	SetupTooltip(SliderTextBoxOption_NumVerticalGridTargets->GetTooltipImage(), SliderTextBoxOption_NumVerticalGridTargets->GetTooltipRegularText());
	SetupTooltip(SliderTextBoxOption_HorizontalSpacing->GetTooltipImage(), SliderTextBoxOption_HorizontalSpacing->GetTooltipRegularText());
	SetupTooltip(SliderTextBoxOption_VerticalSpacing->GetTooltipImage(), SliderTextBoxOption_VerticalSpacing->GetTooltipRegularText());
	SetupTooltip(ComboBoxOption_TargetDistributionPolicy->GetTooltipImage(), ComboBoxOption_TargetDistributionPolicy->GetTooltipRegularText());
	SetupTooltip(SliderTextBoxOption_HorizontalSpread->GetTooltipImage(), SliderTextBoxOption_HorizontalSpread->GetTooltipRegularText());
	SetupTooltip(SliderTextBoxOption_VerticalSpread->GetTooltipImage(), SliderTextBoxOption_VerticalSpread->GetTooltipRegularText());
	SetupTooltip(SliderTextBoxOption_FloorDistance->GetTooltipImage(), SliderTextBoxOption_FloorDistance->GetTooltipRegularText());
	SetupTooltip(SliderTextBoxOption_MinDistanceBetweenTargets->GetTooltipImage(), SliderTextBoxOption_MinDistanceBetweenTargets->GetTooltipRegularText());

	SliderTextBoxOption_NumHorizontalGridTargets->SetValues(MinValue_NumHorizontalGridTargets, MaxValue_NumHorizontalGridTargets, SnapSize_NumHorizontalGridTargets);
	SliderTextBoxOption_NumVerticalGridTargets->SetValues(MinValue_NumVerticalGridTargets, MaxValue_NumVerticalGridTargets, SnapSize_NumVerticalGridTargets);
	SliderTextBoxOption_HorizontalSpacing->SetValues(MinValue_HorizontalGridSpacing, MaxValue_HorizontalGridSpacing, SnapSize_HorizontalGridSpacing);
	SliderTextBoxOption_VerticalSpacing->SetValues(MinValue_VerticalGridSpacing, MaxValue_VerticalGridSpacing, SnapSize_VerticalGridSpacing);
	SliderTextBoxOption_HorizontalSpread->SetValues(MinValue_HorizontalSpread, MaxValue_HorizontalSpread, SnapSize_HorizontalSpread);
	SliderTextBoxOption_VerticalSpread->SetValues(MinValue_VerticalSpread, MaxValue_VerticalSpread, SnapSize_VerticalSpread);
	SliderTextBoxOption_FloorDistance->SetValues(MinValue_FloorDistance, MaxValue_FloorDistance, SnapSize_FloorDistance);
	SliderTextBoxOption_MinDistanceBetweenTargets->SetValues(MinValue_MinTargetDistance, MaxValue_MinTargetDistance, SnapSize_MinTargetDistance);

	SliderTextBoxOption_NumHorizontalGridTargets->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_NumVerticalGridTargets->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_HorizontalSpacing->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_VerticalSpacing->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_HorizontalSpread->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_VerticalSpread->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_FloorDistance->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_MinDistanceBetweenTargets->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);

	ComboBoxOption_BoundsScalingPolicy->ComboBox->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnSelectionChanged_BoundsScalingPolicy);
	ComboBoxOption_TargetDistributionPolicy->ComboBox->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnSelectionChanged_TargetDistributionPolicy);

	ComboBoxOption_BoundsScalingPolicy->GetComboBoxEntryTooltipStringTableKey.BindUObject(this, &ThisClass::GetComboBoxEntryTooltipStringTableKey_BoundsScalingPolicy);
	ComboBoxOption_TargetDistributionPolicy->GetComboBoxEntryTooltipStringTableKey.BindUObject(this, &ThisClass::GetComboBoxEntryTooltipStringTableKey_TargetDistributionPolicy);

	ComboBoxOption_BoundsScalingPolicy->ComboBox->ClearOptions();
	ComboBoxOption_TargetDistributionPolicy->ComboBox->ClearOptions();

	for (const EBoundsScalingPolicy& Method : TEnumRange<EBoundsScalingPolicy>())
	{
		ComboBoxOption_BoundsScalingPolicy->ComboBox->AddOption(UEnum::GetDisplayValueAsText(Method).ToString());
	}
	for (const ETargetDistributionPolicy& Method : TEnumRange<ETargetDistributionPolicy>())
	{
		ComboBoxOption_TargetDistributionPolicy->ComboBox->AddOption(UEnum::GetDisplayValueAsText(Method).ToString());
	}

	SliderTextBoxOption_NumHorizontalGridTargets->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_NumVerticalGridTargets->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_HorizontalSpacing->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_VerticalSpacing->SetVisibility(ESlateVisibility::Collapsed);
	UpdateBrushColors();
}

bool UCustomGameModesWidget_SpawnArea::UpdateAllOptionsValid()
{
	if (ComboBoxOption_BoundsScalingPolicy->ComboBox->GetSelectedOptionCount() != 1)
	{
		return false;
	}
	if (ComboBoxOption_TargetDistributionPolicy->ComboBox->GetSelectedOptionCount() != 1)
	{
		return false;
	}
	return true;
}

void UCustomGameModesWidget_SpawnArea::UpdateOptionsFromConfig()
{
	ComboBoxOption_BoundsScalingPolicy->ComboBox->SetSelectedOption(UEnum::GetDisplayValueAsText(BSConfig->TargetConfig.BoundsScalingPolicy).ToString());
	ComboBoxOption_TargetDistributionPolicy->ComboBox->SetSelectedOption(UEnum::GetDisplayValueAsText(BSConfig->TargetConfig.TargetDistributionPolicy).ToString());
	
	SliderTextBoxOption_NumHorizontalGridTargets->SetValue(BSConfig->GridConfig.NumHorizontalGridTargets);
	SliderTextBoxOption_NumVerticalGridTargets->SetValue(BSConfig->GridConfig.NumVerticalGridTargets);
	SliderTextBoxOption_HorizontalSpacing->SetValue(BSConfig->GridConfig.GridSpacing.X);
	SliderTextBoxOption_VerticalSpacing->SetValue(BSConfig->GridConfig.GridSpacing.Y);
	
	SliderTextBoxOption_HorizontalSpread->SetValue(BSConfig->TargetConfig.BoxBounds.Y);
	SliderTextBoxOption_VerticalSpread->SetValue(BSConfig->TargetConfig.BoxBounds.Z);
	SliderTextBoxOption_FloorDistance->SetValue(BSConfig->TargetConfig.FloorDistance);
	SliderTextBoxOption_MinDistanceBetweenTargets->SetValue(BSConfig->TargetConfig.MinDistanceBetweenTargets);

	SetAllOptionsValid(UpdateAllOptionsValid());
	UpdateBrushColors();
}

void UCustomGameModesWidget_SpawnArea::OnSliderTextBoxValueChanged(USliderTextBoxWidget* Widget, const float Value)
{
	if (Widget == SliderTextBoxOption_NumHorizontalGridTargets)
	{
		BSConfig->GridConfig.NumHorizontalGridTargets = Value;
	}
	else if (Widget == SliderTextBoxOption_NumVerticalGridTargets)
	{
		BSConfig->GridConfig.NumVerticalGridTargets = Value;
	}
	else if (Widget == SliderTextBoxOption_HorizontalSpacing)
	{
		BSConfig->GridConfig.GridSpacing.X = Value;
	}
	else if (Widget == SliderTextBoxOption_VerticalSpacing)
	{
		BSConfig->GridConfig.GridSpacing.Y = Value;
	}
	else if (Widget == SliderTextBoxOption_HorizontalSpread)
	{
		BSConfig->TargetConfig.BoxBounds.Y = Value;
	}
	else if (Widget == SliderTextBoxOption_VerticalSpread)
	{
		BSConfig->TargetConfig.BoxBounds.Z = Value;
	}
	else if (Widget == SliderTextBoxOption_FloorDistance)
	{
		BSConfig->TargetConfig.FloorDistance = Value;
	}
	else if (Widget == SliderTextBoxOption_MinDistanceBetweenTargets)
	{
		BSConfig->TargetConfig.MinDistanceBetweenTargets = Value;
	}
	SetAllOptionsValid(UpdateAllOptionsValid());
}

void UCustomGameModesWidget_SpawnArea::OnSelectionChanged_BoundsScalingPolicy(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Type::Direct)
	{
		return;
	}
	
	if (Selected.Num() != 1)
	{
		SetAllOptionsValid(UpdateAllOptionsValid());
		return;
	}
	
	BSConfig->TargetConfig.BoundsScalingPolicy = GetEnumFromString<EBoundsScalingPolicy>(Selected[0], EBoundsScalingPolicy::None);
	SetAllOptionsValid(UpdateAllOptionsValid());
}

void UCustomGameModesWidget_SpawnArea::OnSelectionChanged_TargetDistributionPolicy(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Type::Direct)
	{
		return;
	}
	
	if (Selected.Num() != 1)
	{
		SetAllOptionsValid(UpdateAllOptionsValid());
		return;
	}
	
	const ETargetDistributionPolicy TargetDistributionPolicy = GetEnumFromString<ETargetDistributionPolicy>(Selected[0], ETargetDistributionPolicy::None);

	if (TargetDistributionPolicy == ETargetDistributionPolicy::Grid)
	{
		SliderTextBoxOption_NumHorizontalGridTargets->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SliderTextBoxOption_NumVerticalGridTargets->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SliderTextBoxOption_HorizontalSpacing->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SliderTextBoxOption_VerticalSpacing->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		SliderTextBoxOption_NumHorizontalGridTargets->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_NumVerticalGridTargets->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_HorizontalSpacing->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_VerticalSpacing->SetVisibility(ESlateVisibility::Collapsed);
	}
	
	BSConfig->TargetConfig.TargetDistributionPolicy = TargetDistributionPolicy;
	UpdateBrushColors();
	SetAllOptionsValid(UpdateAllOptionsValid());
}

FString UCustomGameModesWidget_SpawnArea::GetComboBoxEntryTooltipStringTableKey_BoundsScalingPolicy(const FString& EnumString)
{
	const EBoundsScalingPolicy EnumValue = GetEnumFromString<EBoundsScalingPolicy>(EnumString, EBoundsScalingPolicy::None);
	return GetStringTableKeyNameFromEnum(EnumValue);
}

FString UCustomGameModesWidget_SpawnArea::GetComboBoxEntryTooltipStringTableKey_TargetDistributionPolicy(const FString& EnumString)
{
	const ETargetDistributionPolicy EnumValue = GetEnumFromString<ETargetDistributionPolicy>(EnumString, ETargetDistributionPolicy::None);
	return GetStringTableKeyNameFromEnum(EnumValue);
}