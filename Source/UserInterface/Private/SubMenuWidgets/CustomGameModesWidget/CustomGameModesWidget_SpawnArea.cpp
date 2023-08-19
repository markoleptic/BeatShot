// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SubMenuWidgets/CustomGameModesWidget/CustomGameModesWidget_SpawnArea.h"
#include "WidgetComponents/BSComboBoxString.h"
#include "WidgetComponents/MenuOptionWidgets/ComboBoxOptionWidget.h"
#include "WidgetComponents/MenuOptionWidgets/SliderTextBoxWidget.h"

using namespace Constants;

void UCustomGameModesWidget_SpawnArea::Init(FBSConfig* InConfigPtr, TObjectPtr<UCustomGameModesWidgetComponent> InNext)
{
	Super::Init(InConfigPtr, InNext);
	UpdateOptions();
}

void UCustomGameModesWidget_SpawnArea::NativeConstruct()
{
	Super::NativeConstruct();
	
	ComboBoxOption_BoundsScalingPolicy->SetIndentLevel(0);
	ComboBoxOption_BoundsScalingPolicy->SetShowTooltipImage(true);
	SetupTooltip(ComboBoxOption_BoundsScalingPolicy->GetTooltipImage(), ComboBoxOption_BoundsScalingPolicy->GetTooltipRegularText());
	
	SliderTextBoxOption_NumHorizontalGridTargets->SetIndentLevel(1);
	SliderTextBoxOption_NumHorizontalGridTargets->SetShowTooltipImage(true);
	SliderTextBoxOption_NumHorizontalGridTargets->SetValues(MinValue_NumHorizontalGridTargets, MaxValue_NumHorizontalGridTargets, SnapSize_NumHorizontalGridTargets);
	SetupTooltip(SliderTextBoxOption_NumHorizontalGridTargets->GetTooltipImage(), SliderTextBoxOption_NumHorizontalGridTargets->GetTooltipRegularText());
	
	SliderTextBoxOption_NumVerticalGridTargets->SetIndentLevel(1);
	SliderTextBoxOption_NumVerticalGridTargets->SetShowTooltipImage(true);
	SliderTextBoxOption_NumVerticalGridTargets->SetValues(MinValue_NumVerticalGridTargets, MaxValue_NumVerticalGridTargets, SnapSize_NumVerticalGridTargets);
	SetupTooltip(SliderTextBoxOption_NumVerticalGridTargets->GetTooltipImage(), SliderTextBoxOption_NumVerticalGridTargets->GetTooltipRegularText());

	SliderTextBoxOption_HorizontalSpacing->SetIndentLevel(1);
	SliderTextBoxOption_HorizontalSpacing->SetShowTooltipImage(true);
	SliderTextBoxOption_HorizontalSpacing->SetValues(MinValue_HorizontalGridSpacing, MaxValue_HorizontalGridSpacing, SnapSize_HorizontalGridSpacing);
	SetupTooltip(SliderTextBoxOption_HorizontalSpacing->GetTooltipImage(), SliderTextBoxOption_HorizontalSpacing->GetTooltipRegularText());

	SliderTextBoxOption_VerticalSpacing->SetIndentLevel(1);
	SliderTextBoxOption_VerticalSpacing->SetShowTooltipImage(true);
	SliderTextBoxOption_VerticalSpacing->SetValues(MinValue_VerticalGridSpacing, MaxValue_VerticalGridSpacing, SnapSize_VerticalGridSpacing);
	SetupTooltip(SliderTextBoxOption_VerticalSpacing->GetTooltipImage(), SliderTextBoxOption_VerticalSpacing->GetTooltipRegularText());

	ComboBoxOption_TargetDistributionPolicy->SetIndentLevel(0);
	ComboBoxOption_TargetDistributionPolicy->SetShowTooltipImage(true);
	SetupTooltip(ComboBoxOption_TargetDistributionPolicy->GetTooltipImage(), ComboBoxOption_TargetDistributionPolicy->GetTooltipRegularText());

	SliderTextBoxOption_HorizontalSpread->SetIndentLevel(0);
	SliderTextBoxOption_HorizontalSpread->SetShowTooltipImage(true);
	SliderTextBoxOption_HorizontalSpread->SetValues(MinValue_HorizontalSpread, MaxValue_HorizontalSpread, SnapSize_HorizontalSpread);
	SetupTooltip(SliderTextBoxOption_HorizontalSpread->GetTooltipImage(), SliderTextBoxOption_HorizontalSpread->GetTooltipRegularText());

	SliderTextBoxOption_VerticalSpread->SetIndentLevel(0);
	SliderTextBoxOption_VerticalSpread->SetShowTooltipImage(true);
	SliderTextBoxOption_VerticalSpread->SetValues(MinValue_VerticalSpread, MaxValue_VerticalSpread, SnapSize_VerticalSpread);
	SetupTooltip(SliderTextBoxOption_VerticalSpread->GetTooltipImage(), SliderTextBoxOption_VerticalSpread->GetTooltipRegularText());

	SliderTextBoxOption_FloorDistance->SetIndentLevel(0);
	SliderTextBoxOption_FloorDistance->SetShowTooltipImage(true);
	SliderTextBoxOption_FloorDistance->SetValues(MinValue_FloorDistance, MaxValue_FloorDistance, SnapSize_FloorDistance);
	SetupTooltip(SliderTextBoxOption_FloorDistance->GetTooltipImage(), SliderTextBoxOption_FloorDistance->GetTooltipRegularText());

	SliderTextBoxOption_MinDistanceBetweenTargets->SetIndentLevel(0);
	SliderTextBoxOption_MinDistanceBetweenTargets->SetShowTooltipImage(true);
	SliderTextBoxOption_MinDistanceBetweenTargets->SetValues(MinValue_MinTargetDistance, MaxValue_MinTargetDistance, SnapSize_MinTargetDistance);
	SetupTooltip(SliderTextBoxOption_MinDistanceBetweenTargets->GetTooltipImage(), SliderTextBoxOption_MinDistanceBetweenTargets->GetTooltipRegularText());

	SliderTextBoxOption_NumHorizontalGridTargets->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_NumVerticalGridTargets->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_HorizontalSpacing->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_VerticalSpacing->SetVisibility(ESlateVisibility::Collapsed);

	ComboBoxOption_BoundsScalingPolicy->ComboBox->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnSelectionChanged_BoundsScalingPolicy);
	ComboBoxOption_TargetDistributionPolicy->ComboBox->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnSelectionChanged_TargetDistributionPolicy);

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
	SliderTextBoxOption_HorizontalSpread->SetValue(ConfigPtr->TargetConfig.BoxBounds.X);
	SliderTextBoxOption_VerticalSpread->SetValue(ConfigPtr->TargetConfig.BoxBounds.Y);
	SliderTextBoxOption_FloorDistance->SetValue(ConfigPtr->TargetConfig.FloorDistance);
	SliderTextBoxOption_MinDistanceBetweenTargets->SetValue(ConfigPtr->TargetConfig.MinDistanceBetweenTargets);
	
	SetCanTransitionForward(UpdateCanTransitionForward());
	UpdateBrushColors();
}

void UCustomGameModesWidget_SpawnArea::OnSelectionChanged_BoundsScalingPolicy(const TArray<FString>& Selected,
                                                                              const ESelectInfo::Type SelectionType)
{
}

void UCustomGameModesWidget_SpawnArea::OnSelectionChanged_TargetDistributionPolicy(const TArray<FString>& Selected,
	const ESelectInfo::Type SelectionType)
{
}
