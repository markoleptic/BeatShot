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

	SetupTooltip(ComboBoxOption_BoundsScalingPolicy->GetTooltipImage(), ComboBoxOption_BoundsScalingPolicy->GetTooltipImageText());
	SetupTooltip(SliderTextBoxOption_NumHorizontalGridTargets->GetTooltipImage(), SliderTextBoxOption_NumHorizontalGridTargets->GetTooltipImageText());
	SetupTooltip(SliderTextBoxOption_NumVerticalGridTargets->GetTooltipImage(), SliderTextBoxOption_NumVerticalGridTargets->GetTooltipImageText());
	SetupTooltip(SliderTextBoxOption_HorizontalSpacing->GetTooltipImage(), SliderTextBoxOption_HorizontalSpacing->GetTooltipImageText());
	SetupTooltip(SliderTextBoxOption_VerticalSpacing->GetTooltipImage(), SliderTextBoxOption_VerticalSpacing->GetTooltipImageText());
	SetupTooltip(ComboBoxOption_TargetDistributionPolicy->GetTooltipImage(), ComboBoxOption_TargetDistributionPolicy->GetTooltipImageText());
	SetupTooltip(SliderTextBoxOption_HorizontalSpread->GetTooltipImage(), SliderTextBoxOption_HorizontalSpread->GetTooltipImageText());
	SetupTooltip(SliderTextBoxOption_VerticalSpread->GetTooltipImage(), SliderTextBoxOption_VerticalSpread->GetTooltipImageText());
	SetupTooltip(SliderTextBoxOption_ForwardSpread->GetTooltipImage(), SliderTextBoxOption_ForwardSpread->GetTooltipImageText());
	SetupTooltip(SliderTextBoxOption_FloorDistance->GetTooltipImage(), SliderTextBoxOption_FloorDistance->GetTooltipImageText());
	SetupTooltip(SliderTextBoxOption_MinDistanceBetweenTargets->GetTooltipImage(), SliderTextBoxOption_MinDistanceBetweenTargets->GetTooltipImageText());

	SliderTextBoxOption_NumHorizontalGridTargets->SetValues(MinValue_NumHorizontalGridTargets, MaxValue_NumHorizontalGridTargets, SnapSize_NumHorizontalGridTargets);
	SliderTextBoxOption_NumVerticalGridTargets->SetValues(MinValue_NumVerticalGridTargets, MaxValue_NumVerticalGridTargets, SnapSize_NumVerticalGridTargets);
	SliderTextBoxOption_HorizontalSpacing->SetValues(MinValue_HorizontalGridSpacing, MaxValue_HorizontalGridSpacing, SnapSize_HorizontalGridSpacing);
	SliderTextBoxOption_VerticalSpacing->SetValues(MinValue_VerticalGridSpacing, MaxValue_VerticalGridSpacing, SnapSize_VerticalGridSpacing);
	SliderTextBoxOption_HorizontalSpread->SetValues(MinValue_HorizontalSpread, MaxValue_HorizontalSpread, SnapSize_HorizontalSpread);
	SliderTextBoxOption_VerticalSpread->SetValues(MinValue_VerticalSpread, MaxValue_VerticalSpread, SnapSize_VerticalSpread);
	SliderTextBoxOption_ForwardSpread->SetValues(MinValue_ForwardSpread, MaxValue_ForwardSpread, SnapSize_ForwardSpread);
	SliderTextBoxOption_FloorDistance->SetValues(MinValue_FloorDistance, MaxValue_FloorDistance, SnapSize_FloorDistance);
	SliderTextBoxOption_MinDistanceBetweenTargets->SetValues(MinValue_MinTargetDistance, MaxValue_MinTargetDistance, SnapSize_MinTargetDistance);

	SliderTextBoxOption_NumHorizontalGridTargets->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_NumVerticalGridTargets->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_HorizontalSpacing->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_VerticalSpacing->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_HorizontalSpread->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_VerticalSpread->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_ForwardSpread->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_FloorDistance->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_MinDistanceBetweenTargets->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);

	ComboBoxOption_BoundsScalingPolicy->ComboBox->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnSelectionChanged_BoundsScalingPolicy);
	ComboBoxOption_TargetDistributionPolicy->ComboBox->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnSelectionChanged_TargetDistributionPolicy);

	ComboBoxOption_BoundsScalingPolicy->GetComboBoxEntryTooltipStringTableKey.BindUObject(this, &ThisClass::GetComboBoxEntryTooltipStringTableKey_BoundsScalingPolicy);
	ComboBoxOption_TargetDistributionPolicy->GetComboBoxEntryTooltipStringTableKey.BindUObject(this, &ThisClass::GetComboBoxEntryTooltipStringTableKey_TargetDistributionPolicy);

	ComboBoxOption_BoundsScalingPolicy->ComboBox->ClearOptions();
	ComboBoxOption_TargetDistributionPolicy->ComboBox->ClearOptions();

	TArray<FString> Options;
	
	for (const EBoundsScalingPolicy& Method : TEnumRange<EBoundsScalingPolicy>())
	{
		Options.Add(GetStringFromEnum(Method));
	}
	ComboBoxOption_BoundsScalingPolicy->SortAndAddOptions(Options);
	Options.Empty();
	
	for (const ETargetDistributionPolicy& Method : TEnumRange<ETargetDistributionPolicy>())
	{
		Options.Add(GetStringFromEnum(Method));
	}
	ComboBoxOption_TargetDistributionPolicy->SortAndAddOptions(Options);
	Options.Empty();
	
	SliderTextBoxOption_NumHorizontalGridTargets->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_NumVerticalGridTargets->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_HorizontalSpacing->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_VerticalSpacing->SetVisibility(ESlateVisibility::Collapsed);

	OptionWarning_NumHorizontalTargets.MinAllowed = MinValue_NumHorizontalGridTargets;
	OptionWarning_NumHorizontalTargets.TooltipTextKey = "Invalid_Grid_NumHorizontalTargets";
	OptionWarning_NumHorizontalTargets.FallbackText = GetTooltipTextFromKey("Invalid_Grid_NumHorizontalTargets_Fallback");

	OptionWarning_NumVerticalTargets.MinAllowed = MinValue_NumVerticalGridTargets;
	OptionWarning_NumVerticalTargets.TooltipTextKey = "Invalid_Grid_NumVerticalTargets";
	OptionWarning_NumVerticalTargets.FallbackText = GetTooltipTextFromKey("Invalid_Grid_NumVerticalTargets_Fallback");

	OptionWarning_HorizontalSpacing.MinAllowed = MinValue_HorizontalGridSpacing;
	OptionWarning_HorizontalSpacing.TooltipTextKey = "Invalid_Grid_HorizontalSpacing";
	OptionWarning_HorizontalSpacing.FallbackText = GetTooltipTextFromKey("Invalid_Grid_HorizontalSpacing_Fallback");

	OptionWarning_VerticalSpacing.MinAllowed = MinValue_VerticalGridSpacing;
	OptionWarning_VerticalSpacing.TooltipTextKey = "Invalid_Grid_VerticalSpacing";
	OptionWarning_VerticalSpacing.FallbackText = GetTooltipTextFromKey("Invalid_Grid_VerticalSpacing_Fallback");
	
	UpdateBrushColors();
}

bool UCustomGameModesWidget_SpawnArea::UpdateAllOptionsValid()
{
	TArray<FTooltipWarningValue> UpdateArray;
	bool bRequestComponentUpdate = false;
	
	// SliderTextBoxOption_NumHorizontalGridTargets
	if (BSConfig->TargetConfig.TargetDistributionPolicy == ETargetDistributionPolicy::Grid &&
		BSConfig->AIConfig.bEnableReinforcementLearning &&
		BSConfig->GridConfig.NumHorizontalGridTargets % 5 != 0)
	{
		UpdateArray.Emplace("Invalid_Grid_AI_NumTargets");
	}
	OptionWarning_NumHorizontalTargets.UpdateArray(UpdateArray, BSConfig->GridConfig.NumHorizontalGridTargets, GetMaxAllowedNumHorizontalTargets());
	if (UpdateTooltipWarningImages(SliderTextBoxOption_NumHorizontalGridTargets, UpdateArray))
	{
		bRequestComponentUpdate = true;
	}
	UpdateArray.Empty();
	
	// SliderTextBoxOption_NumVerticalGridTargets
	if (BSConfig->TargetConfig.TargetDistributionPolicy == ETargetDistributionPolicy::Grid &&
		BSConfig->AIConfig.bEnableReinforcementLearning &&
		BSConfig->GridConfig.NumVerticalGridTargets % 5 != 0)
	{
		UpdateArray.Emplace("Invalid_Grid_AI_NumTargets");
	}
	OptionWarning_NumVerticalTargets.UpdateArray(UpdateArray, BSConfig->GridConfig.NumVerticalGridTargets, GetMaxAllowedNumVerticalTargets());
	if (UpdateTooltipWarningImages(SliderTextBoxOption_NumVerticalGridTargets, UpdateArray))
	{
		bRequestComponentUpdate = true;
	}
	UpdateArray.Empty();

	// SliderTextBoxOption_HorizontalSpacing
	OptionWarning_HorizontalSpacing.UpdateArray(UpdateArray, BSConfig->GridConfig.GridSpacing.X, GetMaxAllowedHorizontalSpacing());
	if (UpdateTooltipWarningImages(SliderTextBoxOption_HorizontalSpacing, UpdateArray))
	{
		bRequestComponentUpdate = true;
	}
	UpdateArray.Empty();

	// SliderTextBoxOption_VerticalSpacing
	OptionWarning_VerticalSpacing.UpdateArray(UpdateArray, BSConfig->GridConfig.GridSpacing.Y, GetMaxAllowedVerticalSpacing());
	if (UpdateTooltipWarningImages(SliderTextBoxOption_VerticalSpacing, UpdateArray))
	{
		bRequestComponentUpdate = true;
	}
	UpdateArray.Empty();

	// ComboBoxOption_TargetDistributionPolicy
	if (BSConfig->TargetConfig.TargetDistributionPolicy == ETargetDistributionPolicy::HeadshotHeightOnly &&
		BSConfig->AIConfig.bEnableReinforcementLearning)
	{
		UpdateArray.Emplace("Invalid_HeadshotHeightOnly_AI");
	}
	if (UpdateTooltipWarningImages(ComboBoxOption_TargetDistributionPolicy, UpdateArray))
	{
		bRequestComponentUpdate = true;
	}
	UpdateArray.Empty();
	
	if (bRequestComponentUpdate)
	{
		RequestComponentUpdate.Broadcast();
		return false;
	}

	if (!SliderTextBoxOption_NumHorizontalGridTargets->GetTooltipWarningImageKeys().IsEmpty() ||
		!SliderTextBoxOption_NumVerticalGridTargets->GetTooltipWarningImageKeys().IsEmpty() ||
		!SliderTextBoxOption_HorizontalSpacing->GetTooltipWarningImageKeys().IsEmpty() ||
		!SliderTextBoxOption_VerticalSpacing->GetTooltipWarningImageKeys().IsEmpty() ||
		!ComboBoxOption_TargetDistributionPolicy->GetTooltipWarningImageKeys().IsEmpty())
	{
		return false;
	}
	
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

	UpdateDependentOptions_TargetDistributionPolicy(BSConfig->TargetConfig.TargetDistributionPolicy);
	
	UpdateBrushColors();
	SetAllOptionsValid(UpdateAllOptionsValid());
}

void UCustomGameModesWidget_SpawnArea::UpdateDependentOptions_TargetDistributionPolicy(const ETargetDistributionPolicy& InTargetDistributionPolicy)
{
	if (InTargetDistributionPolicy == ETargetDistributionPolicy::Grid)
	{
		SliderTextBoxOption_NumHorizontalGridTargets->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SliderTextBoxOption_NumVerticalGridTargets->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SliderTextBoxOption_HorizontalSpacing->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SliderTextBoxOption_VerticalSpacing->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SliderTextBoxOption_HorizontalSpread->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_VerticalSpread->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_FloorDistance->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_MinDistanceBetweenTargets->SetVisibility(ESlateVisibility::Collapsed);
		BSConfig->TargetConfig.BoxBounds.Y = MaxValue_HorizontalSpread;
		BSConfig->TargetConfig.BoxBounds.Z = MaxValue_VerticalSpread;
		BSConfig->TargetConfig.FloorDistance = MinValue_FloorDistance;
		UpdateValueIfDifferent(SliderTextBoxOption_HorizontalSpread, BSConfig->TargetConfig.BoxBounds.Y);
		UpdateValueIfDifferent(SliderTextBoxOption_VerticalSpread, BSConfig->TargetConfig.BoxBounds.Z);
		UpdateValueIfDifferent(SliderTextBoxOption_FloorDistance, BSConfig->TargetConfig.FloorDistance);
	}
	else
	{
		SliderTextBoxOption_NumHorizontalGridTargets->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_NumVerticalGridTargets->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_HorizontalSpacing->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_VerticalSpacing->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_HorizontalSpread->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SliderTextBoxOption_VerticalSpread->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SliderTextBoxOption_FloorDistance->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SliderTextBoxOption_MinDistanceBetweenTargets->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
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
	else if (Widget == SliderTextBoxOption_ForwardSpread)
	{
		BSConfig->TargetConfig.BoxBounds.X = Value;
	}
	SetAllOptionsValid(UpdateAllOptionsValid());
}

void UCustomGameModesWidget_SpawnArea::OnSelectionChanged_BoundsScalingPolicy(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Type::Direct || Selected.Num() != 1)
	{
		SetAllOptionsValid(UpdateAllOptionsValid());
		return;
	}
	
	BSConfig->TargetConfig.BoundsScalingPolicy = GetEnumFromString<EBoundsScalingPolicy>(Selected[0]);
	SetAllOptionsValid(UpdateAllOptionsValid());
}

void UCustomGameModesWidget_SpawnArea::OnSelectionChanged_TargetDistributionPolicy(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Type::Direct || Selected.Num() != 1)
	{
		SetAllOptionsValid(UpdateAllOptionsValid());
		return;
	}

	BSConfig->TargetConfig.TargetDistributionPolicy = GetEnumFromString<ETargetDistributionPolicy>(Selected[0]);
	UpdateDependentOptions_TargetDistributionPolicy(BSConfig->TargetConfig.TargetDistributionPolicy);
	
	UpdateBrushColors();
	SetAllOptionsValid(UpdateAllOptionsValid());
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

int32 UCustomGameModesWidget_SpawnArea::GetMaxAllowedNumHorizontalTargets() const
{
	// HorizontalSpread = MaxTargetSize * NumHorizontalTargets + HorizontalSpacing * (NumHorizontalTargets - 1)
	return (GetHorizontalSpread() + BSConfig->GridConfig.GridSpacing.X) / (GetMaxTargetDiameter() + BSConfig->GridConfig.GridSpacing.X);
}

int32 UCustomGameModesWidget_SpawnArea::GetMaxAllowedNumVerticalTargets() const
{
	return (GetVerticalSpread() + BSConfig->GridConfig.GridSpacing.Y) / (GetMaxTargetDiameter() + BSConfig->GridConfig.GridSpacing.Y);
}

float UCustomGameModesWidget_SpawnArea::GetMaxAllowedTargetScale() const
{
	const float MaxAllowedHorizontal = (GetHorizontalSpread() - BSConfig->GridConfig.GridSpacing.X * BSConfig->GridConfig.NumHorizontalGridTargets + BSConfig->GridConfig.GridSpacing.X) / (SphereTargetDiameter * BSConfig->GridConfig.NumHorizontalGridTargets);
	const float MaxAllowedVertical = (GetVerticalSpread() - BSConfig->GridConfig.GridSpacing.Y * BSConfig->GridConfig.NumVerticalGridTargets + BSConfig->GridConfig.GridSpacing.Y) / (SphereTargetDiameter * BSConfig->GridConfig.NumVerticalGridTargets);
	if (MaxAllowedVertical < MaxAllowedHorizontal)
	{
		return MaxAllowedVertical;
	}
	return MaxAllowedHorizontal;
}

float UCustomGameModesWidget_SpawnArea::GetMaxAllowedHorizontalSpacing() const
{
	const float TotalTargetWidth = GetMaxTargetDiameter() * BSConfig->GridConfig.NumHorizontalGridTargets;
	return (GetHorizontalSpread() - TotalTargetWidth) / (BSConfig->GridConfig.NumHorizontalGridTargets - 1);
}

float UCustomGameModesWidget_SpawnArea::GetMaxAllowedVerticalSpacing() const
{
	const float TotalTargetHeight = GetMaxTargetDiameter() * BSConfig->GridConfig.NumVerticalGridTargets;
	return (GetVerticalSpread() - TotalTargetHeight) / (BSConfig->GridConfig.NumVerticalGridTargets - 1);
}

float UCustomGameModesWidget_SpawnArea::GetMinRequiredHorizontalSpread() const
{
	const float TotalTargetWidth = GetMaxTargetDiameter() * BSConfig->GridConfig.NumHorizontalGridTargets;
	return BSConfig->GridConfig.GridSpacing.X * (BSConfig->GridConfig.NumHorizontalGridTargets - 1) + TotalTargetWidth;
}

float UCustomGameModesWidget_SpawnArea::GetMinRequiredVerticalSpread() const
{
	const float TotalTargetHeight = GetMaxTargetDiameter() * BSConfig->GridConfig.NumVerticalGridTargets;
	return BSConfig->GridConfig.GridSpacing.Y * (BSConfig->GridConfig.NumVerticalGridTargets - 1) + TotalTargetHeight;
}

float UCustomGameModesWidget_SpawnArea::GetHorizontalSpread() const
{
	return MaxValue_HorizontalSpread + BSConfig->TargetConfig.MaxSpawnedTargetScale * SphereTargetDiameter;
}

float UCustomGameModesWidget_SpawnArea::GetVerticalSpread() const
{
	return MaxValue_VerticalSpread + BSConfig->TargetConfig.MaxSpawnedTargetScale * SphereTargetDiameter;
}

float UCustomGameModesWidget_SpawnArea::GetMaxTargetDiameter() const
{
	return BSConfig->TargetConfig.MaxSpawnedTargetScale * SphereTargetDiameter;
}
