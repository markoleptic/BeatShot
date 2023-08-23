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

void UCustomGameModesWidget_Deactivation::UpdateOptionsFromConfig()
{
	UpdateValueIfDifferent(ComboBoxOption_TargetDeactivationConditions, GetStringArrayFromEnumArray(BSConfig->TargetConfig.TargetDeactivationConditions));
	UpdateValueIfDifferent(ComboBoxOption_TargetDeactivationResponses, GetStringArrayFromEnumArray(BSConfig->TargetConfig.TargetDeactivationResponses));
	UpdateValueIfDifferent(SliderTextBoxOption_DeactivatedTargetScaleMultiplier, BSConfig->TargetConfig.ConsecutiveChargeScaleMultiplier);

	ESlateVisibility OldVis = SliderTextBoxOption_DeactivatedTargetScaleMultiplier->GetVisibility();
	ESlateVisibility NewVis = ComboBoxOption_TargetDeactivationResponses->ComboBox->GetSelectedOptions().Contains(GetStringFromEnum(ETargetDeactivationResponse::ChangeScale))
		                          ? ESlateVisibility::SelfHitTestInvisible
		                          : ESlateVisibility::Collapsed;
	if (OldVis != NewVis)
	{
		SliderTextBoxOption_DeactivatedTargetScaleMultiplier->SetVisibility(NewVis);
		UpdateBrushColors();
	}

	SetAllOptionsValid(UpdateAllOptionsValid());
}

void UCustomGameModesWidget_Deactivation::OnSliderTextBoxValueChanged(USliderTextBoxWidget* Widget, const float Value)
{
	if (Widget == SliderTextBoxOption_DeactivatedTargetScaleMultiplier)
	{
		BSConfig->TargetConfig.ConsecutiveChargeScaleMultiplier = Value;
	}
	SetAllOptionsValid(UpdateAllOptionsValid());
}

void UCustomGameModesWidget_Deactivation::OnSelectionChanged_TargetDeactivationConditions(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Type::Direct)
	{
		return;
	}
	if (Selected.Num() < 1)
	{
		SetAllOptionsValid(UpdateAllOptionsValid());
		return;
	}
	SetAllOptionsValid(UpdateAllOptionsValid());
}

void UCustomGameModesWidget_Deactivation::OnSelectionChanged_TargetDeactivationResponses(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Type::Direct)
	{
		return;
	}
	if (Selected.Num() < 1)
	{
		SetAllOptionsValid(UpdateAllOptionsValid());
		return;
	}
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