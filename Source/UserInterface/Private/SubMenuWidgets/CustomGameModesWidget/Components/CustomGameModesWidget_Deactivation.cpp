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
	//if (ComboBoxOption_TargetDeactivationConditions->ComboBox->GetSelectedOptionCount() < 1)
	//{
	//	return false;
	//}
	return true;
}

void UCustomGameModesWidget_Deactivation::UpdateOptionsFromConfig()
{
	UpdateValueIfDifferent(ComboBoxOption_TargetDeactivationConditions, GetStringArrayFromEnumArray(BSConfig->TargetConfig.TargetDeactivationConditions));
	UpdateValueIfDifferent(ComboBoxOption_TargetDeactivationResponses, GetStringArrayFromEnumArray(BSConfig->TargetConfig.TargetDeactivationResponses));
	UpdateValueIfDifferent(SliderTextBoxOption_DeactivatedTargetScaleMultiplier, BSConfig->TargetConfig.ConsecutiveChargeScaleMultiplier);
	UpdateValueIfDifferent(ComboBoxOption_TargetDestructionConditions, GetStringArrayFromEnumArray(BSConfig->TargetConfig.TargetDestructionConditions));

	UpdateDependentOptions_TargetDeactivationConditions();
	
	UpdateBrushColors();
	SetAllOptionsValid(UpdateAllOptionsValid());
}

void UCustomGameModesWidget_Deactivation::UpdateDependentOptions_TargetDeactivationConditions()
{
	// Persistant Deactivation Condition
	if (BSConfig->TargetConfig.TargetDeactivationConditions.Contains(ETargetDeactivationCondition::Persistant))
	{
		if (!BSConfig->TargetConfig.TargetDeactivationResponses.IsEmpty())
		{
			BSConfig->TargetConfig.TargetDeactivationResponses.Empty();
		}
		// Empty Target Deactivation Responses Combo Box
		UpdateValueIfDifferent(ComboBoxOption_TargetDeactivationResponses, TArray<FString>());
		// Disable Target Deactivation Responses Combo Box
		ComboBoxOption_TargetDeactivationResponses->ComboBox->SetIsEnabled(false);
		// Collapse any dependent options
		SliderTextBoxOption_DeactivatedTargetScaleMultiplier->SetVisibility(ESlateVisibility::Collapsed);
	}
	// No Persistant Deactivation Condition
	else
	{
		// Enable Target Deactivation Responses Combo Box
		ComboBoxOption_TargetDeactivationResponses->ComboBox->SetIsEnabled(true);
		// Show any Target dependent options if present in Target Deactivation Responses
		if (BSConfig->TargetConfig.TargetDeactivationResponses.Contains(ETargetDeactivationResponse::ApplyDeactivatedTargetScaleMultiplier))
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
	UpdateDependentOptions_TargetDeactivationConditions();
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