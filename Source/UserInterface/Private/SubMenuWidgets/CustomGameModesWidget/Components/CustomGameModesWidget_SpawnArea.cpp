// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


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

bool UCustomGameModesWidget_SpawnArea::UpdateCanTransitionForward()
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

void UCustomGameModesWidget_SpawnArea::UpdateOptions()
{
	ComboBoxOption_BoundsScalingPolicy->ComboBox->SetSelectedOption(UEnum::GetDisplayValueAsText(ConfigPtr->TargetConfig.BoundsScalingPolicy).ToString());
	SliderTextBoxOption_NumHorizontalGridTargets->SetValue(ConfigPtr->GridConfig.NumHorizontalGridTargets);
	SliderTextBoxOption_NumVerticalGridTargets->SetValue(ConfigPtr->GridConfig.NumVerticalGridTargets);
	SliderTextBoxOption_HorizontalSpacing->SetValue(ConfigPtr->GridConfig.GridSpacing.X);
	SliderTextBoxOption_VerticalSpacing->SetValue(ConfigPtr->GridConfig.GridSpacing.Y);
	ComboBoxOption_TargetDistributionPolicy->ComboBox->SetSelectedOption(UEnum::GetDisplayValueAsText(ConfigPtr->TargetConfig.TargetDistributionPolicy).ToString());
	SliderTextBoxOption_HorizontalSpread->SetValue(ConfigPtr->TargetConfig.BoxBounds.Y);
	SliderTextBoxOption_VerticalSpread->SetValue(ConfigPtr->TargetConfig.BoxBounds.Z);
	SliderTextBoxOption_FloorDistance->SetValue(ConfigPtr->TargetConfig.FloorDistance);
	SliderTextBoxOption_MinDistanceBetweenTargets->SetValue(ConfigPtr->TargetConfig.MinDistanceBetweenTargets);

	SetCanTransitionForward(UpdateCanTransitionForward());
	UpdateBrushColors();
}

void UCustomGameModesWidget_SpawnArea::OnSelectionChanged_BoundsScalingPolicy(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType)
{
	if (Selected.Num() != 1)
	{
		SetCanTransitionForward(UpdateCanTransitionForward());
		return;
	}
	
	ConfigPtr->TargetConfig.BoundsScalingPolicy = GetEnumFromString<EBoundsScalingPolicy>(Selected[0], EBoundsScalingPolicy::None);
	SetCanTransitionForward(UpdateCanTransitionForward());
}

void UCustomGameModesWidget_SpawnArea::OnSelectionChanged_TargetDistributionPolicy(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType)
{
	if (Selected.Num() != 1)
	{
		SetCanTransitionForward(UpdateCanTransitionForward());
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
	
	ConfigPtr->TargetConfig.TargetDistributionPolicy = TargetDistributionPolicy;
	UpdateBrushColors();
	SetCanTransitionForward(UpdateCanTransitionForward());
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

void UCustomGameModesWidget_SpawnArea::OnSliderTextBoxValueChanged(USliderTextBoxWidget* Widget, const float Value)
{
	if (Widget == SliderTextBoxOption_NumHorizontalGridTargets)
	{
		ConfigPtr->GridConfig.NumHorizontalGridTargets = Value;
	}
	else if (Widget == SliderTextBoxOption_NumVerticalGridTargets)
	{
		ConfigPtr->GridConfig.NumVerticalGridTargets = Value;
	}
	else if (Widget == SliderTextBoxOption_HorizontalSpacing)
	{
		ConfigPtr->GridConfig.GridSpacing.X = Value;
	}
	else if (Widget == SliderTextBoxOption_VerticalSpacing)
	{
		ConfigPtr->GridConfig.GridSpacing.Y = Value;
	}
	else if (Widget == SliderTextBoxOption_HorizontalSpread)
	{
		ConfigPtr->TargetConfig.BoxBounds.Y = Value;
	}
	else if (Widget == SliderTextBoxOption_VerticalSpread)
	{
		ConfigPtr->TargetConfig.BoxBounds.Z = Value;
	}
	else if (Widget == SliderTextBoxOption_FloorDistance)
	{
		ConfigPtr->TargetConfig.FloorDistance = Value;
	}
	else if (Widget == SliderTextBoxOption_MinDistanceBetweenTargets)
	{
		ConfigPtr->TargetConfig.MinDistanceBetweenTargets = Value;
	}
}
