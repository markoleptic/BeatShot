// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SubmenuWidgets/GameModesWidgets/Components/CGMWC_TargetScaling.h"

#include "WidgetComponents/Boxes/BSComboBoxString.h"
#include "WidgetComponents/MenuOptionWidgets/SliderTextBoxOptionWidget.h"

void UCGMWC_TargetScaling::NativeConstruct()
{
	Super::NativeConstruct();
	
	SliderTextBoxOption_DeactivatedTargetScaleMultiplier->SetValues(MinValue_ConsecutiveChargeScaleMultiplier,
		MaxValue_ConsecutiveChargeScaleMultiplier, SnapSize_ConsecutiveChargeScaleMultiplier);
	SliderTextBoxOption_TargetScale->SetValues(MinValue_TargetScale, MaxValue_TargetScale, SnapSize_TargetScale);
	SliderTextBoxOption_MinTargetScale->SetValues(MinValue_TargetScale, MaxValue_TargetScale, SnapSize_TargetScale);
	SliderTextBoxOption_MaxTargetScale->SetValues(MinValue_TargetScale, MaxValue_TargetScale, SnapSize_TargetScale);
	SliderTextBoxOption_StartThreshold->SetValues(MinValue_DynamicStartThreshold, MaxValue_DynamicStartThreshold,
	SnapSize_DynamicStartThreshold);
	SliderTextBoxOption_EndThreshold->SetValues(MinValue_DynamicEndThreshold, MaxValue_DynamicEndThreshold,
		SnapSize_DynamicEndThreshold);
	SliderTextBoxOption_DecrementAmount->SetValues(MinValue_DynamicDecrementAmount, MaxValue_DynamicDecrementAmount,
		SnapSize_DynamicDecrementAmount);
	SliderTextBoxOption_LifetimeTargetScaleMultiplier->SetValues(MinValue_TargetScale, MaxValue_TargetScale,
		SnapSize_TargetScale);
	
	SliderTextBoxOption_DeactivatedTargetScaleMultiplier->OnSliderTextBoxValueChanged.AddUObject(this,
		&ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_TargetScale->OnSliderTextBoxValueChanged.AddUObject(this,
		&ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_MinTargetScale->OnSliderTextBoxValueChanged.AddUObject(this,
		&ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_MaxTargetScale->OnSliderTextBoxValueChanged.AddUObject(this,
		&ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_StartThreshold->OnSliderTextBoxValueChanged.AddUObject(this,
		&ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_EndThreshold->OnSliderTextBoxValueChanged.AddUObject(this,
		&ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_DecrementAmount->OnSliderTextBoxValueChanged.AddUObject(this,
		&ThisClass::OnSliderTextBoxValueChanged);
	SliderTextBoxOption_LifetimeTargetScaleMultiplier->OnSliderTextBoxValueChanged.AddUObject(this,
		&ThisClass::OnSliderTextBoxValueChanged);

	ComboBoxOption_ConsecutiveTargetScalePolicy->ComboBox->OnSelectionChanged.AddUniqueDynamic(this,
		&ThisClass::OnSelectionChanged_ConsecutiveTargetScalePolicy);

	ComboBoxOption_ConsecutiveTargetScalePolicy->GetComboBoxEntryTooltipStringTableKey.BindUObject(this,
		&ThisClass::GetComboBoxEntryTooltipStringTableKey_ConsecutiveTargetScalePolicy);

	ComboBoxOption_ConsecutiveTargetScalePolicy->ComboBox->ClearOptions();

	TArray<FString> Options;
	for (const EConsecutiveTargetScalePolicy& Method : TEnumRange<EConsecutiveTargetScalePolicy>())
	{
		Options.Add(GetStringFromEnum_FromTagMap(Method));
	}
	ComboBoxOption_ConsecutiveTargetScalePolicy->SortAddOptionsAndSetEnumType<EConsecutiveTargetScalePolicy>(Options);
	Options.Empty();

	SliderTextBoxOption_TargetScale->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_MinTargetScale->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_MaxTargetScale->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_StartThreshold->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_EndThreshold->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_DecrementAmount->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_DeactivatedTargetScaleMultiplier->SetVisibility(ESlateVisibility::Collapsed);
	SliderTextBoxOption_LifetimeTargetScaleMultiplier->SetVisibility(ESlateVisibility::Collapsed);

	SetupWarningTooltipCallbacks();
	UpdateBrushColors();
}

void UCGMWC_TargetScaling::UpdateAllOptionsValid()
{
	Super::UpdateAllOptionsValid();
}

void UCGMWC_TargetScaling::UpdateOptionsFromConfig()
{
	UpdateValueIfDifferent(SliderTextBoxOption_DeactivatedTargetScaleMultiplier,
		BSConfig->TargetConfig.ConsecutiveChargeScaleMultiplier);
	UpdateValueIfDifferent(SliderTextBoxOption_TargetScale, BSConfig->TargetConfig.MinSpawnedTargetScale);
	UpdateValueIfDifferent(SliderTextBoxOption_MinTargetScale, BSConfig->TargetConfig.MinSpawnedTargetScale);
	UpdateValueIfDifferent(SliderTextBoxOption_MaxTargetScale, BSConfig->TargetConfig.MaxSpawnedTargetScale);
	UpdateValueIfDifferent(SliderTextBoxOption_StartThreshold, BSConfig->DynamicTargetScaling.StartThreshold);
	UpdateValueIfDifferent(SliderTextBoxOption_EndThreshold, BSConfig->DynamicTargetScaling.EndThreshold);
	UpdateValueIfDifferent(SliderTextBoxOption_DecrementAmount, BSConfig->DynamicTargetScaling.DecrementAmount);
	UpdateValueIfDifferent(SliderTextBoxOption_LifetimeTargetScaleMultiplier,
		BSConfig->TargetConfig.LifetimeTargetScaleMultiplier);

	UpdateValueIfDifferent(ComboBoxOption_ConsecutiveTargetScalePolicy,
		GetStringFromEnum_FromTagMap(BSConfig->TargetConfig.ConsecutiveTargetScalePolicy));

	UpdateDependentOptions_TargetActivationResponses(BSConfig->TargetConfig.TargetActivationResponses);
	UpdateDependentOptions_TargetDeactivationResponses(BSConfig->TargetConfig.TargetDeactivationConditions,
		BSConfig->TargetConfig.TargetDeactivationResponses);
	UpdateDependentOptions_ConsecutiveTargetScalePolicy(BSConfig->TargetConfig.ConsecutiveTargetScalePolicy);

	UpdateBrushColors();
}

void UCGMWC_TargetScaling::SetupWarningTooltipCallbacks()
{
	SliderTextBoxOption_TargetScale->AddDynamicWarningTooltipData(
	FTooltipData("Invalid_Grid_MaxSpawnedTargetScale", ETooltipImageType::Warning),
	"Invalid_Grid_MaxSpawnedTargetScale_Fallback", MinValue_TargetScale, 2).BindLambda([this]()
	{
		const float Max = FMath::Max(BSConfig->TargetConfig.MaxSpawnedTargetScale,
			BSConfig->TargetConfig.MinSpawnedTargetScale);
		return FDynamicTooltipState(Max, GetMaxAllowedTargetScale());
	});
	SliderTextBoxOption_MinTargetScale->AddDynamicWarningTooltipData(
		FTooltipData("Invalid_Grid_MaxSpawnedTargetScale", ETooltipImageType::Warning),
		"Invalid_Grid_MaxSpawnedTargetScale_Fallback", MinValue_TargetScale, 2).BindLambda([this]()
	{
		return FDynamicTooltipState(BSConfig->TargetConfig.MinSpawnedTargetScale, GetMaxAllowedTargetScale());
	});
	SliderTextBoxOption_MaxTargetScale->AddDynamicWarningTooltipData(
		FTooltipData("Invalid_Grid_MaxSpawnedTargetScale", ETooltipImageType::Warning),
		"Invalid_Grid_MaxSpawnedTargetScale_Fallback", MinValue_TargetScale, 2).BindLambda([this]()
	{
		return FDynamicTooltipState(BSConfig->TargetConfig.MaxSpawnedTargetScale, GetMaxAllowedTargetScale());
	});
}

void UCGMWC_TargetScaling::UpdateDependentOptions_TargetActivationResponses(
	const TArray<ETargetActivationResponse>& InResponses)
{
	if (InResponses.Contains(ETargetActivationResponse::ApplyLifetimeTargetScaling))
	{
		SliderTextBoxOption_LifetimeTargetScaleMultiplier->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		SliderTextBoxOption_LifetimeTargetScaleMultiplier->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UCGMWC_TargetScaling::UpdateDependentOptions_TargetDeactivationResponses(
	const TArray<ETargetDeactivationCondition>& Conditions, const TArray<ETargetDeactivationResponse>& Responses)
{
	// Collapse any Responses dependent upon Conditions
	if (Conditions.Contains(ETargetDeactivationCondition::Persistant))
	{
		SliderTextBoxOption_DeactivatedTargetScaleMultiplier->SetVisibility(ESlateVisibility::Collapsed);
	}

	// Show any Responses dependent upon Conditions
	if (!Conditions.Contains(ETargetDeactivationCondition::Persistant))
	{
		if (Responses.Contains(ETargetDeactivationResponse::ApplyDeactivatedTargetScaleMultiplier))
		{
			SliderTextBoxOption_DeactivatedTargetScaleMultiplier->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		else
		{
			SliderTextBoxOption_DeactivatedTargetScaleMultiplier->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UCGMWC_TargetScaling::UpdateDependentOptions_ConsecutiveTargetScalePolicy(
	const EConsecutiveTargetScalePolicy InConsecutiveTargetScalePolicy)
{
	if (InConsecutiveTargetScalePolicy == EConsecutiveTargetScalePolicy::Static)
	{
		SliderTextBoxOption_TargetScale->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SliderTextBoxOption_MinTargetScale->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_MaxTargetScale->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		SliderTextBoxOption_TargetScale->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_MinTargetScale->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SliderTextBoxOption_MaxTargetScale->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}

	if (InConsecutiveTargetScalePolicy == EConsecutiveTargetScalePolicy::SkillBased)
	{
		SliderTextBoxOption_StartThreshold->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SliderTextBoxOption_EndThreshold->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SliderTextBoxOption_DecrementAmount->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		SliderTextBoxOption_StartThreshold->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_EndThreshold->SetVisibility(ESlateVisibility::Collapsed);
		SliderTextBoxOption_DecrementAmount->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UCGMWC_TargetScaling::OnSliderTextBoxValueChanged(USliderTextBoxOptionWidget* Widget, const float Value)
{
	if (Widget == SliderTextBoxOption_DeactivatedTargetScaleMultiplier)
	{
		BSConfig->TargetConfig.ConsecutiveChargeScaleMultiplier = Value;
	}
	else if (Widget == SliderTextBoxOption_TargetScale)
	{
		BSConfig->TargetConfig.MinSpawnedTargetScale = Value;
		BSConfig->TargetConfig.MaxSpawnedTargetScale = Value;
	}
	else if (Widget == SliderTextBoxOption_MinTargetScale)
	{
		BSConfig->TargetConfig.MinSpawnedTargetScale = Value;
	}
	else if (Widget == SliderTextBoxOption_MaxTargetScale)
	{
		BSConfig->TargetConfig.MaxSpawnedTargetScale = Value;
	}
	else if (Widget == SliderTextBoxOption_StartThreshold)
	{
		BSConfig->DynamicTargetScaling.StartThreshold = Value;
	}
	else if (Widget == SliderTextBoxOption_EndThreshold)
	{
		BSConfig->DynamicTargetScaling.EndThreshold = Value;
	}
	else if (Widget == SliderTextBoxOption_DecrementAmount)
	{
		BSConfig->DynamicTargetScaling.DecrementAmount = Value;
	}
	else if (Widget == SliderTextBoxOption_LifetimeTargetScaleMultiplier)
	{
		BSConfig->TargetConfig.LifetimeTargetScaleMultiplier = Value;
	}
	UpdateAllOptionsValid();
}

void UCGMWC_TargetScaling::OnSelectionChanged_ConsecutiveTargetScalePolicy(const TArray<FString>& Selected,
	const ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Type::Direct || Selected.Num() != 1)
	{
		return;
	}

	BSConfig->TargetConfig.ConsecutiveTargetScalePolicy = GetEnumFromString<EConsecutiveTargetScalePolicy>(Selected[0]);
	UpdateDependentOptions_ConsecutiveTargetScalePolicy(BSConfig->TargetConfig.ConsecutiveTargetScalePolicy);

	if (BSConfig->TargetConfig.ConsecutiveTargetScalePolicy == EConsecutiveTargetScalePolicy::Static)
	{
		BSConfig->TargetConfig.MinSpawnedTargetScale = SliderTextBoxOption_TargetScale->GetSliderValue();
		BSConfig->TargetConfig.MaxSpawnedTargetScale = SliderTextBoxOption_TargetScale->GetSliderValue();
	}
	else
	{
		BSConfig->TargetConfig.MinSpawnedTargetScale = SliderTextBoxOption_MinTargetScale->GetSliderValue();
		BSConfig->TargetConfig.MaxSpawnedTargetScale = SliderTextBoxOption_MaxTargetScale->GetSliderValue();
	}
	UpdateBrushColors();
	UpdateAllOptionsValid();
}

FString UCGMWC_TargetScaling::GetComboBoxEntryTooltipStringTableKey_ConsecutiveTargetScalePolicy(
	const FString& EnumString)
{
	const EConsecutiveTargetScalePolicy EnumValue = GetEnumFromString<EConsecutiveTargetScalePolicy>(EnumString);
	return GetStringTableKeyNameFromEnum(EnumValue);
}
