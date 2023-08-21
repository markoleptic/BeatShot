// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SubMenuWidgets/CustomGameModesWidget/Components/CustomGameModesWidget_Deactivation.h"
#include "WidgetComponents/BSComboBoxString.h"
#include "WidgetComponents/MenuOptionWidgets/ComboBoxOptionWidget.h"
#include "WidgetComponents/MenuOptionWidgets/SliderTextBoxWidget.h"

void UCustomGameModesWidget_Deactivation::InitComponent(FBSConfig* InConfigPtr,
                                               TObjectPtr<UCustomGameModesWidgetComponent> InNext)
{
	Super::InitComponent(InConfigPtr, InNext);
}

void UCustomGameModesWidget_Deactivation::NativeConstruct()
{
	Super::NativeConstruct();


	SetupTooltip(ComboBoxOption_TargetDeactivationConditions->GetTooltipImage(), ComboBoxOption_TargetDeactivationConditions->GetTooltipRegularText());
	SetupTooltip(ComboBoxOption_TargetDeactivationResponses->GetTooltipImage(), ComboBoxOption_TargetDeactivationResponses->GetTooltipRegularText());
	SetupTooltip(SliderTextBoxOption_DeactivatedTargetScaleMultiplier->GetTooltipImage(), SliderTextBoxOption_DeactivatedTargetScaleMultiplier->GetTooltipRegularText());
	
	SliderTextBoxOption_DeactivatedTargetScaleMultiplier->SetValues(MinValue_ConsecutiveChargeScaleMultiplier, MaxValue_ConsecutiveChargeScaleMultiplier, SnapSize_ConsecutiveChargeScaleMultiplier);
	SliderTextBoxOption_DeactivatedTargetScaleMultiplier->OnSliderTextBoxValueChanged.AddUObject(this, &ThisClass::OnSliderTextBoxValueChanged);
	
	ComboBoxOption_TargetDeactivationConditions->ComboBox->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnSelectionChanged_TargetDeactivationConditions);
	ComboBoxOption_TargetDeactivationResponses->ComboBox->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnSelectionChanged_TargetDeactivationResponses);

	ComboBoxOption_TargetDeactivationConditions->GetComboBoxEntryTooltipStringTableKey.BindUObject(this, &ThisClass::GetComboBoxEntryTooltipStringTableKey_TargetDeactivationConditions);
	ComboBoxOption_TargetDeactivationResponses->GetComboBoxEntryTooltipStringTableKey.BindUObject(this, &ThisClass::GetComboBoxEntryTooltipStringTableKey_TargetDeactivationResponses);
	
	ComboBoxOption_TargetDeactivationConditions->ComboBox->ClearOptions();
	ComboBoxOption_TargetDeactivationResponses->ComboBox->ClearOptions();
	
	for (const ETargetDeactivationCondition& Method : TEnumRange<ETargetDeactivationCondition>())
	{
		ComboBoxOption_TargetDeactivationConditions->ComboBox->AddOption(UEnum::GetDisplayValueAsText(Method).ToString());
	}
	for (const ETargetDeactivationResponse& Method : TEnumRange<ETargetDeactivationResponse>())
	{
		ComboBoxOption_TargetDeactivationResponses->ComboBox->AddOption(UEnum::GetDisplayValueAsText(Method).ToString());
	}

	SliderTextBoxOption_DeactivatedTargetScaleMultiplier->SetVisibility(ESlateVisibility::Collapsed);
	UpdateBrushColors();
}

bool UCustomGameModesWidget_Deactivation::UpdateAllOptionsValid()
{
	if (ComboBoxOption_TargetDeactivationConditions->ComboBox->GetSelectedOptionCount() < 1)
	{
		return false;
	}
	if (ComboBoxOption_TargetDeactivationResponses->ComboBox->GetSelectedOptionCount() < 1)
	{
		return false;
	}
	return true;
}

void UCustomGameModesWidget_Deactivation::UpdateOptions()
{
	ComboBoxOption_TargetDeactivationConditions->ComboBox->SetSelectedOptions(GetStringArrayFromEnumArray(ConfigPtr->TargetConfig.TargetDeactivationConditions));
	ComboBoxOption_TargetDeactivationResponses->ComboBox->SetSelectedOptions(GetStringArrayFromEnumArray(ConfigPtr->TargetConfig.TargetDeactivationResponses));
	SliderTextBoxOption_DeactivatedTargetScaleMultiplier->SetValue(ConfigPtr->TargetConfig.ConsecutiveChargeScaleMultiplier);

	if (ComboBoxOption_TargetDeactivationResponses->ComboBox->GetSelectedOptions().Contains(UEnum::GetDisplayValueAsText(ETargetDeactivationResponse::ChangeScale).ToString()))
	{
		SliderTextBoxOption_DeactivatedTargetScaleMultiplier->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		SliderTextBoxOption_DeactivatedTargetScaleMultiplier->SetVisibility(ESlateVisibility::Collapsed);
	}

	SetAllOptionsValid(UpdateAllOptionsValid());
	UpdateBrushColors();
}

void UCustomGameModesWidget_Deactivation::OnSelectionChanged_TargetDeactivationConditions(
	const TArray<FString>& Selected, const ESelectInfo::Type SelectionType)
{
	SetAllOptionsValid(UpdateAllOptionsValid());
}

void UCustomGameModesWidget_Deactivation::OnSelectionChanged_TargetDeactivationResponses(
	const TArray<FString>& Selected, const ESelectInfo::Type SelectionType)
{
	SetAllOptionsValid(UpdateAllOptionsValid());
}

FString UCustomGameModesWidget_Deactivation::GetComboBoxEntryTooltipStringTableKey_TargetDeactivationConditions(
	const FString& EnumString)
{
	const ETargetDeactivationCondition EnumValue = GetEnumFromString<ETargetDeactivationCondition>(EnumString, ETargetDeactivationCondition::None);
	return GetStringTableKeyNameFromEnum(EnumValue);
}

FString UCustomGameModesWidget_Deactivation::GetComboBoxEntryTooltipStringTableKey_TargetDeactivationResponses(
	const FString& EnumString)
{
	const ETargetDeactivationResponse EnumValue = GetEnumFromString<ETargetDeactivationResponse>(EnumString, ETargetDeactivationResponse::None);
	return GetStringTableKeyNameFromEnum(EnumValue);
}

void UCustomGameModesWidget_Deactivation::OnSliderTextBoxValueChanged(USliderTextBoxWidget* Widget, const float Value)
{
	if (Widget == SliderTextBoxOption_DeactivatedTargetScaleMultiplier)
	{
		ConfigPtr->TargetConfig.ConsecutiveChargeScaleMultiplier = Value;
	}
}
