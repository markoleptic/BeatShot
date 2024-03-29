﻿// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SubMenuWidgets/GameModesWidgets/Components/CGMWC_SpawnArea.h"

#include "Components/CheckBox.h"
#include "WidgetComponents/Boxes/BSComboBoxString.h"
#include "WidgetComponents/MenuOptionWidgets/ComboBoxOptionWidget.h"
#include "WidgetComponents/MenuOptionWidgets/ConstantMinMaxMenuOptionWidget.h"
#include "WidgetComponents/MenuOptionWidgets/SliderTextBoxOptionWidget.h"

using namespace Constants;


void UCGMWC_SpawnArea::NativeConstruct()
{
	Super::NativeConstruct();

	SliderTextBoxOption_NumHorizontalGridTargets->SetValues(MinValue_NumHorizontalGridTargets,
		MaxValue_NumHorizontalGridTargets, SnapSize_NumHorizontalGridTargets);
	SliderTextBoxOption_NumVerticalGridTargets->SetValues(MinValue_NumVerticalGridTargets,
		MaxValue_NumVerticalGridTargets, SnapSize_NumVerticalGridTargets);
	SliderTextBoxOption_HorizontalSpacing->SetValues(MinValue_HorizontalGridSpacing, MaxValue_HorizontalGridSpacing,
		SnapSize_HorizontalGridSpacing);
	SliderTextBoxOption_VerticalSpacing->SetValues(MinValue_VerticalGridSpacing, MaxValue_VerticalGridSpacing,
		SnapSize_VerticalGridSpacing);
	MenuOption_HorizontalSpread->SetValues(MinValue_HorizontalSpread, MaxValue_HorizontalSpread,
		SnapSize_HorizontalSpread);
	MenuOption_VerticalSpread->SetValues(MinValue_VerticalSpread, MaxValue_VerticalSpread, SnapSize_VerticalSpread);
	MenuOption_ForwardSpread->SetValues(MinValue_ForwardSpread, MaxValue_ForwardSpread, SnapSize_ForwardSpread);
	SliderTextBoxOption_FloorDistance->
		SetValues(MinValue_FloorDistance, MaxValue_FloorDistance, SnapSize_FloorDistance);

	SliderTextBoxOption_StartThreshold->SetValues(MinValue_DynamicStartThreshold, MaxValue_DynamicStartThreshold,
		SnapSize_DynamicStartThreshold);
	SliderTextBoxOption_EndThreshold->SetValues(MinValue_DynamicEndThreshold, MaxValue_DynamicEndThreshold,
		SnapSize_DynamicEndThreshold);
	SliderTextBoxOption_DecrementAmount->SetValues(MinValue_DynamicDecrementAmount, MaxValue_DynamicDecrementAmount,
		SnapSize_DynamicDecrementAmount);

	SliderTextBoxOption_NumHorizontalGridTargets->OnSliderTextBoxValueChanged.AddUObject(this,
		&ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_NumVerticalGridTargets->OnSliderTextBoxValueChanged.AddUObject(this,
		&ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_HorizontalSpacing->OnSliderTextBoxValueChanged.AddUObject(this,
		&ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_VerticalSpacing->OnSliderTextBoxValueChanged.AddUObject(this,
		&ThisClass::OnSliderTextBoxValueChanged);

	MenuOption_HorizontalSpread->OnMinMaxMenuOptionChanged.AddUObject(this, &ThisClass::OnMinMaxValueChanged);
	MenuOption_VerticalSpread->OnMinMaxMenuOptionChanged.AddUObject(this, &ThisClass::OnMinMaxValueChanged);
	MenuOption_ForwardSpread->OnMinMaxMenuOptionChanged.AddUObject(this, &ThisClass::OnMinMaxValueChanged);

	MenuOption_HorizontalSpread->SetUseMinAsConstant(false);
	MenuOption_VerticalSpread->SetUseMinAsConstant(false);
	MenuOption_ForwardSpread->SetUseMinAsConstant(false);

	SliderTextBoxOption_FloorDistance->OnSliderTextBoxValueChanged.AddUObject(this,
		&ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_StartThreshold->OnSliderTextBoxValueChanged.AddUObject(this,
		&ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_EndThreshold->OnSliderTextBoxValueChanged.AddUObject(this,
		&ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_DecrementAmount->OnSliderTextBoxValueChanged.AddUObject(this,
		&ThisClass::OnSliderTextBoxValueChanged);

	ComboBoxOption_TargetDistributionPolicy->ComboBox->OnSelectionChanged.AddUniqueDynamic(this,
		&ThisClass::OnSelectionChanged_TargetDistributionPolicy);
	ComboBoxOption_TargetDistributionPolicy->GetComboBoxEntryTooltipStringTableKey.BindUObject(this,
		&ThisClass::GetComboBoxEntryTooltipStringTableKey_TargetDistributionPolicy);

	ComboBoxOption_TargetDistributionPolicy->ComboBox->ClearOptions();

	TArray<FString> Options;
	for (const ETargetDistributionPolicy& Method : TEnumRange<ETargetDistributionPolicy>())
	{
		Options.Add(GetStringFromEnum_FromTagMap(Method));
	}
	ComboBoxOption_TargetDistributionPolicy->SortAddOptionsAndSetEnumType<ETargetDistributionPolicy>(Options);
	Options.Empty();

	SliderTextBoxOption_NumHorizontalGridTargets->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_NumVerticalGridTargets->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_HorizontalSpacing->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_VerticalSpacing->SetVisibility(ESlateVisibility::Collapsed);

	SliderTextBoxOption_StartThreshold->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_EndThreshold->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_DecrementAmount->SetVisibility(ESlateVisibility::Collapsed);

	SetupWarningTooltipCallbacks();
	UpdateBrushColors();
}

void UCGMWC_SpawnArea::UpdateAllOptionsValid()
{
	Super::UpdateAllOptionsValid();
}

void UCGMWC_SpawnArea::UpdateOptionsFromConfig()
{
	UpdateValueIfDifferent(ComboBoxOption_TargetDistributionPolicy,
		GetStringFromEnum_FromTagMap(BSConfig->TargetConfig.TargetDistributionPolicy));

	UpdateValueIfDifferent(SliderTextBoxOption_NumHorizontalGridTargets, BSConfig->GridConfig.NumHorizontalGridTargets);
	UpdateValueIfDifferent(SliderTextBoxOption_NumVerticalGridTargets, BSConfig->GridConfig.NumVerticalGridTargets);
	UpdateValueIfDifferent(SliderTextBoxOption_HorizontalSpacing, BSConfig->GridConfig.GridSpacing.X);
	UpdateValueIfDifferent(SliderTextBoxOption_VerticalSpacing, BSConfig->GridConfig.GridSpacing.Y);
	
	UpdateValueIfDifferent(SliderTextBoxOption_FloorDistance, BSConfig->TargetConfig.FloorDistance);
	UpdateValueIfDifferent(SliderTextBoxOption_StartThreshold, BSConfig->DynamicSpawnAreaScaling.StartThreshold);
	UpdateValueIfDifferent(SliderTextBoxOption_EndThreshold, BSConfig->DynamicSpawnAreaScaling.EndThreshold);
	UpdateValueIfDifferent(SliderTextBoxOption_DecrementAmount, BSConfig->DynamicSpawnAreaScaling.DecrementAmount);
	
	UpdateSpread();
	UpdateDependentOptions_TargetDistributionPolicy();
	UpdateDependentOption_BoundsScalingPolicy();

	UpdateBrushColors();
}

void UCGMWC_SpawnArea::SetupWarningTooltipCallbacks()
{
	SliderTextBoxOption_NumHorizontalGridTargets->AddDynamicWarningTooltipData(
		FTooltipData("Invalid_Grid_NumHorizontalTargets", ETooltipImageType::Warning),
		"Invalid_Grid_NumHorizontalTargets_Fallback", MinValue_NumHorizontalGridTargets).BindLambda([this]()
	{
		return FDynamicTooltipState(BSConfig->GridConfig.NumHorizontalGridTargets, GetMaxAllowedNumHorizontalTargets(),
			!SliderTextBoxOption_NumHorizontalGridTargets->IsVisible());
	});
	SliderTextBoxOption_NumVerticalGridTargets->AddDynamicWarningTooltipData(
		FTooltipData("Invalid_Grid_NumVerticalTargets", ETooltipImageType::Warning),
		"Invalid_Grid_NumVerticalTargets_Fallback", MinValue_NumVerticalGridTargets).BindLambda([this]()
	{
		return FDynamicTooltipState(BSConfig->GridConfig.NumVerticalGridTargets, GetMaxAllowedNumVerticalTargets(),
			!SliderTextBoxOption_NumVerticalGridTargets->IsVisible());
	});

	SliderTextBoxOption_HorizontalSpacing->AddDynamicWarningTooltipData(
		FTooltipData("Invalid_Grid_HorizontalSpacing", ETooltipImageType::Warning),
		"Invalid_Grid_HorizontalSpacing_Fallback", MinValue_HorizontalGridSpacing).BindLambda([this]()
	{
		return FDynamicTooltipState(BSConfig->GridConfig.GridSpacing.X, GetMaxAllowedHorizontalSpacing(),
			!SliderTextBoxOption_HorizontalSpacing->IsVisible());
	});

	SliderTextBoxOption_VerticalSpacing->AddDynamicWarningTooltipData(
		FTooltipData("Invalid_Grid_VerticalSpacing", ETooltipImageType::Warning),
		"Invalid_Grid_VerticalSpacing_Fallback", MinValue_VerticalGridSpacing).BindLambda([this]()
	{
		return FDynamicTooltipState(BSConfig->GridConfig.GridSpacing.Y, GetMaxAllowedVerticalSpacing(),
			!SliderTextBoxOption_VerticalSpacing->IsVisible());
	});

	ComboBoxOption_TargetDistributionPolicy->AddWarningTooltipData(FTooltipData("Invalid_HeadshotHeightOnly_AI",
		ETooltipImageType::Warning)).BindLambda([this]()
	{
		return BSConfig->TargetConfig.TargetDistributionPolicy == ETargetDistributionPolicy::HeadshotHeightOnly &&
			BSConfig->AIConfig.bEnableReinforcementLearning;
	});

	MenuOption_ForwardSpread->AddWarningTooltipData(FTooltipData("Caution_ZeroForwardDistance_MTDM_ForwardOnly_2",
		ETooltipImageType::Caution)).BindLambda([this]()
	{
		return BSConfig->TargetConfig.BoxBounds.X <= 0.f && BSConfig->TargetConfig.MovingTargetDirectionMode ==
			EMovingTargetDirectionMode::ForwardOnly;
	});
}

void UCGMWC_SpawnArea::UpdateDependentOptions_TargetDistributionPolicy()
{
	if (BSConfig->TargetConfig.TargetDistributionPolicy == ETargetDistributionPolicy::Grid)
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
}

void UCGMWC_SpawnArea::UpdateSpread()
{
	const bool bTracking = BSConfig->TargetConfig.TargetDamageType == ETargetDamageType::Tracking;
	const bool bHeadShotHeightOnly = BSConfig->TargetConfig.TargetDistributionPolicy ==
		ETargetDistributionPolicy::HeadshotHeightOnly;
	const bool bGrid = BSConfig->TargetConfig.TargetDistributionPolicy == ETargetDistributionPolicy::Grid;

	UpdateSpreadWidgetState(MenuOption_ForwardSpread, bTracking, bHeadShotHeightOnly, bGrid);
	UpdateSpreadWidgetState(MenuOption_HorizontalSpread, bTracking, bHeadShotHeightOnly, bGrid);
	UpdateSpreadWidgetState(MenuOption_VerticalSpread, bTracking, bHeadShotHeightOnly, bGrid);

	const bool bForwardSame = BSConfig->DynamicSpawnAreaScaling.StartBounds.X == BSConfig->TargetConfig.BoxBounds.X;
	const bool bHorizontalSame = BSConfig->DynamicSpawnAreaScaling.StartBounds.Y == BSConfig->TargetConfig.BoxBounds.Y;
	const bool bVerticalSame = BSConfig->DynamicSpawnAreaScaling.StartBounds.Z == BSConfig->TargetConfig.BoxBounds.Z;

	UpdateValuesIfDifferent(MenuOption_ForwardSpread, bTracking || bForwardSame,
		BSConfig->DynamicSpawnAreaScaling.StartBounds.X, BSConfig->TargetConfig.BoxBounds.X);
	UpdateValuesIfDifferent(MenuOption_HorizontalSpread, bTracking || bGrid || bHorizontalSame,
		BSConfig->DynamicSpawnAreaScaling.StartBounds.Y, BSConfig->TargetConfig.BoxBounds.Y);
	UpdateValuesIfDifferent(MenuOption_VerticalSpread, bTracking || bGrid || bHeadShotHeightOnly || bVerticalSame,
		BSConfig->DynamicSpawnAreaScaling.StartBounds.Z, BSConfig->TargetConfig.BoxBounds.Z);
}

void UCGMWC_SpawnArea::UpdateDependentOption_BoundsScalingPolicy()
{
	const bool bAnyNonConstantSpread = !MenuOption_ForwardSpread->IsInConstantMode()
		|| !MenuOption_HorizontalSpread->IsInConstantMode()
		|| !MenuOption_VerticalSpread->IsInConstantMode();
	const bool bGrid = BSConfig->TargetConfig.TargetDistributionPolicy == ETargetDistributionPolicy::Grid;
	const bool bTracking = BSConfig->TargetConfig.TargetDamageType == ETargetDamageType::Tracking;
	
	if (bAnyNonConstantSpread && !bGrid && !bTracking)
	{
		SliderTextBoxOption_StartThreshold->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SliderTextBoxOption_EndThreshold->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SliderTextBoxOption_DecrementAmount->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		BSConfig->TargetConfig.BoundsScalingPolicy = EBoundsScalingPolicy::Dynamic;
	}
	else
	{
		SliderTextBoxOption_StartThreshold->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_EndThreshold->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_DecrementAmount->SetVisibility(ESlateVisibility::Collapsed);
		BSConfig->TargetConfig.BoundsScalingPolicy = EBoundsScalingPolicy::Static;
	}
}

void UCGMWC_SpawnArea::OnSliderTextBoxValueChanged(USliderTextBoxOptionWidget* Widget, const float Value)
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
	else if (Widget == SliderTextBoxOption_FloorDistance)
	{
		BSConfig->TargetConfig.FloorDistance = Value;
		RequestGameModePreviewUpdate.Broadcast();
	}
	else if (Widget == SliderTextBoxOption_StartThreshold)
	{
		BSConfig->DynamicSpawnAreaScaling.StartThreshold = Value;
	}
	else if (Widget == SliderTextBoxOption_EndThreshold)
	{
		BSConfig->DynamicSpawnAreaScaling.EndThreshold = Value;
	}
	else if (Widget == SliderTextBoxOption_DecrementAmount)
	{
		BSConfig->DynamicSpawnAreaScaling.DecrementAmount = Value;
	}

	UpdateAllOptionsValid();
}

void UCGMWC_SpawnArea::OnMinMaxValueChanged(UConstantMinMaxMenuOptionWidget* Widget, const bool bChecked,
	const float Min, const float Max)
{
	if (Widget == MenuOption_ForwardSpread)
	{
		BSConfig->DynamicSpawnAreaScaling.StartBounds.X = Min;
		BSConfig->TargetConfig.BoxBounds.X = Max;
	}
	else if (Widget == MenuOption_HorizontalSpread)
	{
		BSConfig->DynamicSpawnAreaScaling.StartBounds.Y = Min;
		BSConfig->TargetConfig.BoxBounds.Y = Max;
	}
	else if (Widget == MenuOption_VerticalSpread)
	{
		BSConfig->DynamicSpawnAreaScaling.StartBounds.Z = Min;
		BSConfig->TargetConfig.BoxBounds.Z = Max;
	}

	UpdateDependentOption_BoundsScalingPolicy();
	UpdateAllOptionsValid();
}

void UCGMWC_SpawnArea::UpdateSpreadWidgetState(UConstantMinMaxMenuOptionWidget* Widget, const bool bTracking,
	const bool bHeadShotHeightOnly, const bool bGrid)
{
	FSpreadWidgetState State(EMenuOptionEnabledState::Enabled);
	
	if (Widget == MenuOption_ForwardSpread)
	{
		if (bTracking)
		{
			State.SubWidgetEnabledState = EMenuOptionEnabledState::DependentMissing;
			State.SubWidgetTooltipKey = "DM_ForwardSpread_Tracking";
		}
	}
	else if (Widget == MenuOption_VerticalSpread)
	{
		if (bGrid)
		{
			if (FMath::IsNearlyEqual(MenuOption_VerticalSpread->GetSnapSize(), HeadshotHeight_VerticalSpread))
			{
				MenuOption_VerticalSpread->SetValues(MinValue_VerticalSpread, MaxValue_VerticalSpread,
					SnapSize_VerticalSpread);
			}
			BSConfig->TargetConfig.BoxBounds.Z = MaxValue_VerticalSpread;
			BSConfig->DynamicSpawnAreaScaling.StartBounds.Z = MaxValue_VerticalSpread;
			
			State.MenuOptionEnabledState = EMenuOptionEnabledState::DependentMissing;
			State.MenuOptionTooltipKey = "DM_VerticalSpread_Grid";
		}
		else if (bHeadShotHeightOnly)
		{
			if (!FMath::IsNearlyEqual(MenuOption_VerticalSpread->GetSnapSize(), HeadshotHeight_VerticalSpread))
			{
				MenuOption_VerticalSpread->SetValues(HeadshotHeight_VerticalSpread, HeadshotHeight_VerticalSpread,
					HeadshotHeight_VerticalSpread);
			}

			BSConfig->TargetConfig.BoxBounds.Z = HeadshotHeight_VerticalSpread;
			BSConfig->DynamicSpawnAreaScaling.StartBounds.Z = HeadshotHeight_VerticalSpread;
			
			State.MenuOptionEnabledState = EMenuOptionEnabledState::DependentMissing;
			State.MenuOptionTooltipKey = "DM_VerticalSpread_HeadshotHeightOnly";
		}
		else
		{
			if (FMath::IsNearlyEqual(MenuOption_VerticalSpread->GetSnapSize(), HeadshotHeight_VerticalSpread))
			{
				MenuOption_VerticalSpread->SetValues(MinValue_VerticalSpread, MaxValue_VerticalSpread,
					SnapSize_VerticalSpread);
				BSConfig->TargetConfig.BoxBounds.Z =
					FMath::Max(BSConfig->TargetConfig.BoxBounds.Z, MinValue_VerticalSpread);
				BSConfig->DynamicSpawnAreaScaling.StartBounds.Z = FMath::Max(
					BSConfig->DynamicSpawnAreaScaling.StartBounds.Z, MinValue_VerticalSpread);
			}
		}
		
		if (bTracking)
		{
			State.SubWidgetEnabledState = EMenuOptionEnabledState::DependentMissing;
			State.SubWidgetTooltipKey = "DM_VerticalSpread_Tracking";
		}
	}
	else if (Widget == MenuOption_HorizontalSpread)
	{
		if (bGrid)
		{
			BSConfig->TargetConfig.BoxBounds.Y = MaxValue_HorizontalSpread;
			BSConfig->DynamicSpawnAreaScaling.StartBounds.Y = MaxValue_HorizontalSpread;
			State.MenuOptionEnabledState = EMenuOptionEnabledState::DependentMissing;
			State.MenuOptionTooltipKey = "DM_HorizontalSpread_Grid";
		}

		if (bTracking)
		{
			State.SubWidgetEnabledState = EMenuOptionEnabledState::DependentMissing;
			State.SubWidgetTooltipKey = "DM_HorizontalSpread_Tracking";
		}
	}
	SetMenuOptionEnabledStateAndAddTooltip(Widget, State.MenuOptionEnabledState, State.MenuOptionTooltipKey);
	SetSubMenuOptionEnabledStateAndAddTooltip(Widget, UCheckBox::StaticClass(), State.SubWidgetEnabledState,
		State.SubWidgetTooltipKey);
}

void UCGMWC_SpawnArea::OnSelectionChanged_TargetDistributionPolicy(const TArray<FString>& Selected,
	const ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Type::Direct || Selected.Num() != 1) return;

	BSConfig->TargetConfig.TargetDistributionPolicy = GetEnumFromString_FromTagMap<
		ETargetDistributionPolicy>(Selected[0]);
	UpdateSpread();
	UpdateDependentOptions_TargetDistributionPolicy();
	UpdateDependentOption_BoundsScalingPolicy();
	UpdateBrushColors();
	UpdateAllOptionsValid();
	RequestGameModePreviewUpdate.Broadcast();
}

FString UCGMWC_SpawnArea::GetComboBoxEntryTooltipStringTableKey_TargetDistributionPolicy(const FString& EnumString)
{
	const ETargetDistributionPolicy EnumValue = GetEnumFromString_FromTagMap<ETargetDistributionPolicy>(EnumString);
	return GetStringTableKeyNameFromEnum(EnumValue);
}
