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

	SliderTextBoxOption_NumHorizontalGridTargets->SetValues(MinValue_NumHorizontalGridTargets, MaxValue_NumHorizontalGridTargets, SnapSize_NumHorizontalGridTargets);
	SliderTextBoxOption_NumVerticalGridTargets->SetValues(MinValue_NumVerticalGridTargets, MaxValue_NumVerticalGridTargets, SnapSize_NumVerticalGridTargets);
	SliderTextBoxOption_HorizontalSpacing->SetValues(MinValue_HorizontalGridSpacing, MaxValue_HorizontalGridSpacing, SnapSize_HorizontalGridSpacing);
	SliderTextBoxOption_VerticalSpacing->SetValues(MinValue_VerticalGridSpacing, MaxValue_VerticalGridSpacing, SnapSize_VerticalGridSpacing);
	SliderTextBoxOption_HorizontalSpread->SetValues(MinValue_HorizontalSpread, MaxValue_HorizontalSpread, SnapSize_HorizontalSpread);
	SliderTextBoxOption_VerticalSpread->SetValues(MinValue_VerticalSpread, MaxValue_VerticalSpread, SnapSize_VerticalSpread);
	SliderTextBoxOption_ForwardSpread->SetValues(MinValue_ForwardSpread, MaxValue_ForwardSpread, SnapSize_ForwardSpread);
	SliderTextBoxOption_FloorDistance->SetValues(MinValue_FloorDistance, MaxValue_FloorDistance, SnapSize_FloorDistance);
	SliderTextBoxOption_MinDistanceBetweenTargets->SetValues(MinValue_MinTargetDistance, MaxValue_MinTargetDistance, SnapSize_MinTargetDistance);
	SliderTextBoxOption_MinHorizontalSpread->SetValues(MinValue_DynamicSpread.Y, MaxValue_DynamicSpread.Y, SnapSize_DynamicSpread.Y);
	SliderTextBoxOption_MinVerticalSpread->SetValues(MinValue_DynamicSpread.Z, MaxValue_DynamicSpread.Z, SnapSize_DynamicSpread.Z);
	SliderTextBoxOption_MinForwardSpread->SetValues(MinValue_DynamicSpread.X, MaxValue_DynamicSpread.X, SnapSize_DynamicSpread.X);
	
	SliderTextBoxOption_StartThreshold->SetValues(MinValue_DynamicStartThreshold, MaxValue_DynamicStartThreshold, SnapSize_DynamicStartThreshold);
	SliderTextBoxOption_EndThreshold->SetValues(MinValue_DynamicEndThreshold, MaxValue_DynamicEndThreshold, SnapSize_DynamicEndThreshold);
	SliderTextBoxOption_DecrementAmount->SetValues(MinValue_DynamicDecrementAmount, MaxValue_DynamicDecrementAmount, SnapSize_DynamicDecrementAmount);

	SliderTextBoxOption_NumHorizontalGridTargets->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_NumVerticalGridTargets->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_HorizontalSpacing->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_VerticalSpacing->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_HorizontalSpread->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_VerticalSpread->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_ForwardSpread->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_FloorDistance->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_MinDistanceBetweenTargets->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_MinHorizontalSpread->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_MinVerticalSpread->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_MinForwardSpread->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_StartThreshold->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_EndThreshold->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_DecrementAmount->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);

	ComboBoxOption_BoundsScalingPolicy->ComboBox->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnSelectionChanged_BoundsScalingPolicy);
	ComboBoxOption_TargetDistributionPolicy->ComboBox->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnSelectionChanged_TargetDistributionPolicy);
	ComboBoxOption_DynamicBoundsScalingPolicy->ComboBox->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnSelectionChanged_TargetDistributionPolicy);

	ComboBoxOption_BoundsScalingPolicy->GetComboBoxEntryTooltipStringTableKey.BindUObject(this, &ThisClass::GetComboBoxEntryTooltipStringTableKey_BoundsScalingPolicy);
	ComboBoxOption_TargetDistributionPolicy->GetComboBoxEntryTooltipStringTableKey.BindUObject(this, &ThisClass::GetComboBoxEntryTooltipStringTableKey_TargetDistributionPolicy);
	ComboBoxOption_DynamicBoundsScalingPolicy->GetComboBoxEntryTooltipStringTableKey.BindUObject(this, &ThisClass::GetComboBoxEntryTooltipStringTableKey_DynamicBoundsScalingPolicy);

	ComboBoxOption_BoundsScalingPolicy->ComboBox->ClearOptions();
	ComboBoxOption_TargetDistributionPolicy->ComboBox->ClearOptions();
	ComboBoxOption_DynamicBoundsScalingPolicy->ComboBox->ClearOptions();

	TArray<FString> Options;

	for (const EBoundsScalingPolicy& Method : TEnumRange<EBoundsScalingPolicy>())
	{
		Options.Add(GetStringFromEnum(Method));
	}
	ComboBoxOption_BoundsScalingPolicy->SortAddOptionsAndSetEnumType<EBoundsScalingPolicy>(Options);
	Options.Empty();

	for (const ETargetDistributionPolicy& Method : TEnumRange<ETargetDistributionPolicy>())
	{
		Options.Add(GetStringFromEnum(Method));
	}
	ComboBoxOption_TargetDistributionPolicy->SortAddOptionsAndSetEnumType<ETargetDistributionPolicy>(Options);
	Options.Empty();

	for (const EDynamicBoundsScalingPolicy& Method : TEnumRange<EDynamicBoundsScalingPolicy>())
	{
		Options.Add(GetStringFromEnum(Method));
	}
	ComboBoxOption_DynamicBoundsScalingPolicy->SortAddOptionsAndSetEnumType<EDynamicBoundsScalingPolicy>(Options);
	Options.Empty();

	SliderTextBoxOption_NumHorizontalGridTargets->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_NumVerticalGridTargets->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_HorizontalSpacing->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_VerticalSpacing->SetVisibility(ESlateVisibility::Collapsed);
	
	SliderTextBoxOption_MinHorizontalSpread->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_MinVerticalSpread->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_MinForwardSpread->SetVisibility(ESlateVisibility::Collapsed);
	
	SliderTextBoxOption_StartThreshold->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_EndThreshold->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_DecrementAmount->SetVisibility(ESlateVisibility::Collapsed);
	
	ComboBoxOption_DynamicBoundsScalingPolicy->SetVisibility(ESlateVisibility::Collapsed);
	
	SetupWarningTooltipCallbacks();
	UpdateBrushColors();
}

void UCustomGameModesWidget_SpawnArea::UpdateAllOptionsValid()
{
	Super::UpdateAllOptionsValid();
}

void UCustomGameModesWidget_SpawnArea::UpdateOptionsFromConfig()
{
	UpdateValueIfDifferent(ComboBoxOption_BoundsScalingPolicy, GetStringFromEnum(BSConfig->TargetConfig.BoundsScalingPolicy));
	UpdateValueIfDifferent(ComboBoxOption_TargetDistributionPolicy, GetStringFromEnum(BSConfig->TargetConfig.TargetDistributionPolicy));

	UpdateValueIfDifferent(SliderTextBoxOption_NumHorizontalGridTargets, BSConfig->GridConfig.NumHorizontalGridTargets);
	UpdateValueIfDifferent(SliderTextBoxOption_NumVerticalGridTargets, BSConfig->GridConfig.NumVerticalGridTargets);
	UpdateValueIfDifferent(SliderTextBoxOption_HorizontalSpacing, BSConfig->GridConfig.GridSpacing.X);
	UpdateValueIfDifferent(SliderTextBoxOption_VerticalSpacing, BSConfig->GridConfig.GridSpacing.Y);

	UpdateValueIfDifferent(SliderTextBoxOption_ForwardSpread, BSConfig->TargetConfig.BoxBounds.X);
	UpdateValueIfDifferent(SliderTextBoxOption_HorizontalSpread, BSConfig->TargetConfig.BoxBounds.Y);
	UpdateValueIfDifferent(SliderTextBoxOption_VerticalSpread, BSConfig->TargetConfig.BoxBounds.Z);
	UpdateValueIfDifferent(SliderTextBoxOption_FloorDistance, BSConfig->TargetConfig.FloorDistance);
	UpdateValueIfDifferent(SliderTextBoxOption_MinDistanceBetweenTargets, BSConfig->TargetConfig.MinDistanceBetweenTargets);

	UpdateValueIfDifferent(SliderTextBoxOption_MinHorizontalSpread, BSConfig->DynamicSpawnAreaScaling.MinSize.Y);
	UpdateValueIfDifferent(SliderTextBoxOption_MinVerticalSpread, BSConfig->DynamicSpawnAreaScaling.MinSize.Z);
	UpdateValueIfDifferent(SliderTextBoxOption_MinForwardSpread, BSConfig->DynamicSpawnAreaScaling.MinSize.X);

	UpdateValueIfDifferent(SliderTextBoxOption_StartThreshold, BSConfig->DynamicSpawnAreaScaling.StartThreshold);
	UpdateValueIfDifferent(SliderTextBoxOption_EndThreshold, BSConfig->DynamicSpawnAreaScaling.EndThreshold);
	UpdateValueIfDifferent(SliderTextBoxOption_DecrementAmount, BSConfig->DynamicSpawnAreaScaling.DecrementAmount);

	UpdateValueIfDifferent(ComboBoxOption_DynamicBoundsScalingPolicy, GetStringArrayFromEnumArray(BSConfig->DynamicSpawnAreaScaling.DynamicBoundsScalingPolicy));
	
	UpdateDependentOptions_TargetDistributionPolicy(BSConfig->TargetConfig.TargetDistributionPolicy);

	UpdateBrushColors();
}

void UCustomGameModesWidget_SpawnArea::SetupWarningTooltipCallbacks()
{
		SliderTextBoxOption_NumHorizontalGridTargets->AddWarningTooltipData(FTooltipData("Invalid_Grid_AI_NumTargets", ETooltipImageType::Warning)).BindLambda([this]()
	{
		return (BSConfig->TargetConfig.TargetDistributionPolicy == ETargetDistributionPolicy::Grid && BSConfig->AIConfig.bEnableReinforcementLearning && BSConfig->GridConfig.NumHorizontalGridTargets %
			5 != 0);
	});
	SliderTextBoxOption_NumHorizontalGridTargets->AddDynamicWarningTooltipData(FTooltipData("Invalid_Grid_NumHorizontalTargets", ETooltipImageType::Warning),
		"Invalid_Grid_NumHorizontalTargets_Fallback", MinValue_NumHorizontalGridTargets).BindLambda([this]()
	{
		return FDynamicTooltipState(BSConfig->GridConfig.NumHorizontalGridTargets, GetMaxAllowedNumHorizontalTargets());
	});

	SliderTextBoxOption_NumVerticalGridTargets->AddWarningTooltipData(FTooltipData("Invalid_Grid_AI_NumTargets", ETooltipImageType::Warning)).BindLambda([this]()
	{
		return (BSConfig->TargetConfig.TargetDistributionPolicy == ETargetDistributionPolicy::Grid && BSConfig->AIConfig.bEnableReinforcementLearning && BSConfig->GridConfig.NumVerticalGridTargets % 5
			!= 0);
	});
	SliderTextBoxOption_NumVerticalGridTargets->AddDynamicWarningTooltipData(FTooltipData("Invalid_Grid_NumVerticalTargets", ETooltipImageType::Warning),
		"Invalid_Grid_NumVerticalTargets_Fallback", MinValue_NumVerticalGridTargets).BindLambda([this]()
	{
		return FDynamicTooltipState(BSConfig->GridConfig.NumVerticalGridTargets, GetMaxAllowedNumVerticalTargets());
	});

	SliderTextBoxOption_HorizontalSpacing->AddDynamicWarningTooltipData(FTooltipData("Invalid_Grid_HorizontalSpacing", ETooltipImageType::Warning),
		"Invalid_Grid_HorizontalSpacing_Fallback", MinValue_HorizontalGridSpacing).BindLambda([this]()
	{
		return FDynamicTooltipState(BSConfig->GridConfig.GridSpacing.X, GetMaxAllowedHorizontalSpacing());
	});

	SliderTextBoxOption_VerticalSpacing->AddDynamicWarningTooltipData(FTooltipData("Invalid_Grid_VerticalSpacing", ETooltipImageType::Warning),
		"Invalid_Grid_VerticalSpacing_Fallback", MinValue_VerticalGridSpacing).BindLambda([this]()
	{
		return FDynamicTooltipState(BSConfig->GridConfig.GridSpacing.Y, GetMaxAllowedVerticalSpacing());
	});

	ComboBoxOption_TargetDistributionPolicy->AddWarningTooltipData(FTooltipData("Invalid_HeadshotHeightOnly_AI", ETooltipImageType::Warning)).BindLambda([this]()
	{
		return BSConfig->TargetConfig.TargetDistributionPolicy == ETargetDistributionPolicy::HeadshotHeightOnly && BSConfig->AIConfig.bEnableReinforcementLearning;
	});

	SliderTextBoxOption_ForwardSpread->AddWarningTooltipData(FTooltipData("Caution_ZeroForwardDistance_MTDM_ForwardOnly_2", ETooltipImageType::Caution)).BindLambda([this]()
	{
		return BSConfig->TargetConfig.BoxBounds.X <= 0.f && BSConfig->TargetConfig.MovingTargetDirectionMode == EMovingTargetDirectionMode::ForwardOnly;
	});
}

void UCustomGameModesWidget_SpawnArea::UpdateDependentOptions_TargetDistributionPolicy(const ETargetDistributionPolicy& InTargetDistributionPolicy)
{
	if (InTargetDistributionPolicy == ETargetDistributionPolicy::Grid)
	{
		SliderTextBoxOption_MinDistanceBetweenTargets->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_NumHorizontalGridTargets->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SliderTextBoxOption_NumVerticalGridTargets->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SliderTextBoxOption_HorizontalSpacing->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SliderTextBoxOption_VerticalSpacing->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

		BSConfig->TargetConfig.BoxBounds.Y = MaxValue_HorizontalSpread;
		BSConfig->TargetConfig.BoxBounds.Z = MaxValue_VerticalSpread;
		BSConfig->TargetConfig.BoundsScalingPolicy = EBoundsScalingPolicy::Static;

		UpdateValueIfDifferent(SliderTextBoxOption_HorizontalSpread, BSConfig->TargetConfig.BoxBounds.Y);
		UpdateValueIfDifferent(SliderTextBoxOption_VerticalSpread, BSConfig->TargetConfig.BoxBounds.Z);
		UpdateValueIfDifferent(SliderTextBoxOption_FloorDistance, BSConfig->TargetConfig.FloorDistance);
		UpdateValueIfDifferent(ComboBoxOption_BoundsScalingPolicy, GetStringFromEnum(BSConfig->TargetConfig.BoundsScalingPolicy));

		SliderTextBoxOption_HorizontalSpread->SetSliderAndTextBoxEnabledStates(false);
		SliderTextBoxOption_VerticalSpread->SetSliderAndTextBoxEnabledStates(false);
		ComboBoxOption_BoundsScalingPolicy->ComboBox->SetIsEnabled(false);
	}
	else
	{
		SliderTextBoxOption_MinDistanceBetweenTargets->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SliderTextBoxOption_NumHorizontalGridTargets->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_NumVerticalGridTargets->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_HorizontalSpacing->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_VerticalSpacing->SetVisibility(ESlateVisibility::Collapsed);

		SliderTextBoxOption_HorizontalSpread->SetSliderAndTextBoxEnabledStates(true);
		SliderTextBoxOption_VerticalSpread->SetSliderAndTextBoxEnabledStates(true);
		ComboBoxOption_BoundsScalingPolicy->ComboBox->SetIsEnabled(true);
	}
	UpdateDependentOptions_BoundsScalingPolicy(BSConfig->TargetConfig.BoundsScalingPolicy);
}

void UCustomGameModesWidget_SpawnArea::UpdateDependentOptions_BoundsScalingPolicy(const EBoundsScalingPolicy& InBoundsScalingPolicy)
{
	if (InBoundsScalingPolicy == EBoundsScalingPolicy::Dynamic)
	{
		SliderTextBoxOption_MinHorizontalSpread->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SliderTextBoxOption_MinVerticalSpread->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SliderTextBoxOption_MinForwardSpread->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SliderTextBoxOption_StartThreshold->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SliderTextBoxOption_EndThreshold->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SliderTextBoxOption_DecrementAmount->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		ComboBoxOption_DynamicBoundsScalingPolicy->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		SliderTextBoxOption_MinHorizontalSpread->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_MinVerticalSpread->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_MinForwardSpread->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_StartThreshold->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_EndThreshold->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_DecrementAmount->SetVisibility(ESlateVisibility::Collapsed);
		ComboBoxOption_DynamicBoundsScalingPolicy->SetVisibility(ESlateVisibility::Collapsed);
	}
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
	else if (Widget == SliderTextBoxOption_ForwardSpread)
	{
		BSConfig->TargetConfig.BoxBounds.X = Value;
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
		RequestGameModePreviewUpdate.Broadcast();
	}
	else if (Widget == SliderTextBoxOption_MinDistanceBetweenTargets)
	{
		BSConfig->TargetConfig.MinDistanceBetweenTargets = Value;
	}
	else if (Widget == SliderTextBoxOption_MinHorizontalSpread)
	{
		BSConfig->DynamicSpawnAreaScaling.MinSize.Y = Value;
	}
	else if (Widget == SliderTextBoxOption_MinVerticalSpread)
	{
		BSConfig->DynamicSpawnAreaScaling.MinSize.Z = Value;
	}
	else if (Widget == SliderTextBoxOption_MinForwardSpread)
	{
		BSConfig->DynamicSpawnAreaScaling.MinSize.X = Value;
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

void UCustomGameModesWidget_SpawnArea::OnSelectionChanged_BoundsScalingPolicy(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Type::Direct || Selected.Num() != 1)
	{
		return;
	}

	BSConfig->TargetConfig.BoundsScalingPolicy = GetEnumFromString<EBoundsScalingPolicy>(Selected[0]);
	UpdateDependentOptions_BoundsScalingPolicy(BSConfig->TargetConfig.BoundsScalingPolicy);
	UpdateBrushColors();
	UpdateAllOptionsValid();
}

void UCustomGameModesWidget_SpawnArea::OnSelectionChanged_TargetDistributionPolicy(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Type::Direct || Selected.Num() != 1)
	{
		return;
	}

	BSConfig->TargetConfig.TargetDistributionPolicy = GetEnumFromString<ETargetDistributionPolicy>(Selected[0]);
	UpdateDependentOptions_TargetDistributionPolicy(BSConfig->TargetConfig.TargetDistributionPolicy);
	UpdateBrushColors();
	UpdateAllOptionsValid();
}

void UCustomGameModesWidget_SpawnArea::OnSelectionChanged_DynamicBoundsScalingPolicy(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Type::Direct || Selected.Num() < 1)
	{
		return;
	}

	BSConfig->DynamicSpawnAreaScaling.DynamicBoundsScalingPolicy = GetEnumArrayFromStringArray<EDynamicBoundsScalingPolicy>(Selected);
	UpdateAllOptionsValid();
}

FString UCustomGameModesWidget_SpawnArea::GetComboBoxEntryTooltipStringTableKey_BoundsScalingPolicy(const FString& EnumString)
{
	const EBoundsScalingPolicy EnumValue = GetEnumFromString<EBoundsScalingPolicy>(EnumString);
	return GetStringTableKeyNameFromEnum(EnumValue);
}

FString UCustomGameModesWidget_SpawnArea::GetComboBoxEntryTooltipStringTableKey_TargetDistributionPolicy(const FString& EnumString)
{
	const ETargetDistributionPolicy EnumValue = GetEnumFromString<ETargetDistributionPolicy>(EnumString);
	return GetStringTableKeyNameFromEnum(EnumValue);
}

FString UCustomGameModesWidget_SpawnArea::GetComboBoxEntryTooltipStringTableKey_DynamicBoundsScalingPolicy(const FString& EnumString)
{
	const EDynamicBoundsScalingPolicy EnumValue = GetEnumFromString<EDynamicBoundsScalingPolicy>(EnumString);
	return GetStringTableKeyNameFromEnum(EnumValue);
}