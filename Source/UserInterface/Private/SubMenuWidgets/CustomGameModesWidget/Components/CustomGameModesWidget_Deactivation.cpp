// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SubMenuWidgets/CustomGameModesWidget/Components/CustomGameModesWidget_Deactivation.h"
#include "WidgetComponents/BSComboBoxString.h"
#include "WidgetComponents/MenuOptionWidgets/ComboBoxOptionWidget.h"
#include "WidgetComponents/MenuOptionWidgets/SliderTextBoxWidget.h"

void UCustomGameModesWidget_Deactivation::InitComponent(FBSConfig* InConfigPtr, TObjectPtr<UCustomGameModesWidgetComponent> InNext)
{
	Super::InitComponent(InConfigPtr, InNext);
}

void UCustomGameModesWidget_Deactivation::NativeConstruct()
{
	Super::NativeConstruct();
	
	SetupTooltip(ComboBoxOption_TargetDeactivationConditions->GetTooltipImage(), ComboBoxOption_TargetDeactivationConditions->GetTooltipImageText());
	SetupTooltip(ComboBoxOption_TargetDeactivationResponses->GetTooltipImage(), ComboBoxOption_TargetDeactivationResponses->GetTooltipImageText());
	SetupTooltip(SliderTextBoxOption_DeactivatedTargetScaleMultiplier->GetTooltipImage(), SliderTextBoxOption_DeactivatedTargetScaleMultiplier->GetTooltipImageText());
	SetupTooltip(ComboBoxOption_TargetDestructionConditions->GetTooltipImage(), ComboBoxOption_TargetDestructionConditions->GetTooltipImageText());
	
	SliderTextBoxOption_DeactivatedTargetScaleMultiplier->SetValues(MinValue_ConsecutiveChargeScaleMultiplier, MaxValue_ConsecutiveChargeScaleMultiplier, SnapSize_ConsecutiveChargeScaleMultiplier);
	SliderTextBoxOption_DeactivatedTargetScaleMultiplier->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	
	ComboBoxOption_TargetDeactivationConditions->ComboBox->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnSelectionChanged_TargetDeactivationConditions);
	ComboBoxOption_TargetDeactivationResponses->ComboBox->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnSelectionChanged_TargetDeactivationResponses);
	ComboBoxOption_TargetDestructionConditions->ComboBox->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnSelectionChanged_TargetDestructionConditions);
	
	ComboBoxOption_TargetDeactivationConditions->GetComboBoxEntryTooltipStringTableKey.BindUObject(this, &ThisClass::GetComboBoxEntryTooltipStringTableKey_TargetDeactivationConditions);
	ComboBoxOption_TargetDeactivationResponses->GetComboBoxEntryTooltipStringTableKey.BindUObject(this, &ThisClass::GetComboBoxEntryTooltipStringTableKey_TargetDeactivationResponses);
	ComboBoxOption_TargetDestructionConditions->GetComboBoxEntryTooltipStringTableKey.BindUObject(this, &ThisClass::GetComboBoxEntryTooltipStringTableKey_TargetDestructionConditions);

	ComboBoxOption_TargetDeactivationConditions->ComboBox->ClearOptions();
	ComboBoxOption_TargetDeactivationResponses->ComboBox->ClearOptions();
	ComboBoxOption_TargetDestructionConditions->ComboBox->ClearOptions();

	for (const ETargetDeactivationCondition& Method : TEnumRange<ETargetDeactivationCondition>())
	{
		ComboBoxOption_TargetDeactivationConditions->ComboBox->AddOption(GetStringFromEnum(Method));
	}
	for (const ETargetDeactivationResponse& Method : TEnumRange<ETargetDeactivationResponse>())
	{
		ComboBoxOption_TargetDeactivationResponses->ComboBox->AddOption(GetStringFromEnum(Method));
	}
	for (const ETargetDestructionCondition& Method : TEnumRange<ETargetDestructionCondition>())
	{
		ComboBoxOption_TargetDestructionConditions->ComboBox->AddOption(GetStringFromEnum(Method));
	}

	SliderTextBoxOption_DeactivatedTargetScaleMultiplier->SetVisibility(ESlateVisibility::Collapsed);
	UpdateBrushColors();
}

bool UCustomGameModesWidget_Deactivation::UpdateAllOptionsValid()
{
	return true;
}

void UCustomGameModesWidget_Deactivation::UpdateOptionsFromConfig()
{
	UpdateValueIfDifferent(ComboBoxOption_TargetDeactivationConditions, GetStringArrayFromEnumArray(BSConfig->TargetConfig.TargetDeactivationConditions));
	UpdateValueIfDifferent(ComboBoxOption_TargetDeactivationResponses, GetStringArrayFromEnumArray(BSConfig->TargetConfig.TargetDeactivationResponses));
	UpdateValueIfDifferent(SliderTextBoxOption_DeactivatedTargetScaleMultiplier, BSConfig->TargetConfig.ConsecutiveChargeScaleMultiplier);
	UpdateValueIfDifferent(ComboBoxOption_TargetDestructionConditions, GetStringArrayFromEnumArray(BSConfig->TargetConfig.TargetDestructionConditions));

	UpdateDependentOptions_TargetDeactivationConditions(BSConfig->TargetConfig.TargetDeactivationConditions, BSConfig->TargetConfig.TargetDeactivationResponses);
	
	UpdateBrushColors();
	SetAllOptionsValid(UpdateAllOptionsValid());
}

void UCustomGameModesWidget_Deactivation::UpdateDependentOptions_TargetDeactivationConditions(const TArray<ETargetDeactivationCondition>& Conditions, const TArray<ETargetDeactivationResponse>& Responses)
{
	// Persistant Deactivation Condition
	if (Conditions.Contains(ETargetDeactivationCondition::Persistant))
	{
		if (!BSConfig->TargetConfig.TargetDeactivationResponses.IsEmpty())
		{
			BSConfig->TargetConfig.TargetDeactivationResponses.Empty();
		}
		UpdateValueIfDifferent(ComboBoxOption_TargetDeactivationResponses, TArray<FString>());
		ComboBoxOption_TargetDeactivationResponses->ComboBox->SetIsEnabled(false);
	}
	// No Persistant Deactivation Condition
	else
	{
		ComboBoxOption_TargetDeactivationResponses->ComboBox->SetIsEnabled(true);
	}
	UpdateDependentOptions_TargetDeactivationResponses(Conditions, Responses);
}

void UCustomGameModesWidget_Deactivation::UpdateDependentOptions_TargetDeactivationResponses(const TArray<ETargetDeactivationCondition>& Conditions, const TArray<ETargetDeactivationResponse>& Responses)
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

void UCustomGameModesWidget_Deactivation::OnSliderTextBoxValueChanged(USliderTextBoxWidget* Widget, const float Value)
{
	if (Widget == SliderTextBoxOption_DeactivatedTargetScaleMultiplier)
	{
		BSConfig->TargetConfig.ConsecutiveChargeScaleMultiplier = Value;
		SetAllOptionsValid(UpdateAllOptionsValid());
	}
}

void UCustomGameModesWidget_Deactivation::OnSelectionChanged_TargetDeactivationConditions(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType)
{
	if (Selected.Num() < 1 || SelectionType == ESelectInfo::Type::Direct)
	{
		SetAllOptionsValid(UpdateAllOptionsValid());
		return;
	}
	BSConfig->TargetConfig.TargetDeactivationConditions = GetEnumArrayFromStringArray<ETargetDeactivationCondition>(Selected);
	UpdateDependentOptions_TargetDeactivationConditions(BSConfig->TargetConfig.TargetDeactivationConditions, BSConfig->TargetConfig.TargetDeactivationResponses);
	SetAllOptionsValid(UpdateAllOptionsValid());
}

void UCustomGameModesWidget_Deactivation::OnSelectionChanged_TargetDeactivationResponses(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType)
{
	if (Selected.Num() < 1 || SelectionType == ESelectInfo::Type::Direct)
	{
		SetAllOptionsValid(UpdateAllOptionsValid());
		return;
	}

	BSConfig->TargetConfig.TargetDeactivationResponses = GetEnumArrayFromStringArray<ETargetDeactivationResponse>(Selected);
	UpdateDependentOptions_TargetDeactivationResponses(BSConfig->TargetConfig.TargetDeactivationConditions, BSConfig->TargetConfig.TargetDeactivationResponses);
	SetAllOptionsValid(UpdateAllOptionsValid());
}

void UCustomGameModesWidget_Deactivation::OnSelectionChanged_TargetDestructionConditions(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType)
{
	if (Selected.Num() < 1 || SelectionType == ESelectInfo::Type::Direct)
	{
		SetAllOptionsValid(UpdateAllOptionsValid());
		return;
	}

	BSConfig->TargetConfig.TargetDestructionConditions = GetEnumArrayFromStringArray<ETargetDestructionCondition>(Selected);
	SetAllOptionsValid(UpdateAllOptionsValid());
}

FString UCustomGameModesWidget_Deactivation::GetComboBoxEntryTooltipStringTableKey_TargetDeactivationConditions(const FString& EnumString)
{
	return GetStringTableKeyNameFromEnum(GetEnumFromString<ETargetDeactivationCondition>(EnumString));
}

FString UCustomGameModesWidget_Deactivation::GetComboBoxEntryTooltipStringTableKey_TargetDeactivationResponses(const FString& EnumString)
{
	return GetStringTableKeyNameFromEnum(GetEnumFromString<ETargetDeactivationResponse>(EnumString));
}

FString UCustomGameModesWidget_Deactivation::GetComboBoxEntryTooltipStringTableKey_TargetDestructionConditions(const FString& EnumString)
{
	return GetStringTableKeyNameFromEnum(GetEnumFromString<ETargetDestructionCondition>(EnumString));
}