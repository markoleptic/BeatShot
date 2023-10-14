// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SubMenuWidgets/GameModesWidgets/Components/CustomGameModesWidget_Deactivation.h"
#include "WidgetComponents/Boxes/BSComboBoxString.h"
#include "WidgetComponents/MenuOptionWidgets/CheckBoxOptionWidget.h"
#include "WidgetComponents/MenuOptionWidgets/ComboBoxOptionWidget.h"
#include "WidgetComponents/MenuOptionWidgets/SliderTextBoxOptionWidget.h"


void UCustomGameModesWidget_Deactivation::NativeConstruct()
{
	Super::NativeConstruct();
	
	SliderTextBoxOption_DeactivationHealthLostThreshold->SetValues(MinValue_SpecificHealthLost, MaxValue_SpecificHealthLost,
		SnapSize_SpecificHealthLost);
	
	SliderTextBoxOption_DeactivationHealthLostThreshold->OnSliderTextBoxValueChanged.AddUObject(this,
		&ThisClass::OnSliderTextBoxValueChanged);

	ComboBoxOption_TargetDeactivationConditions->ComboBox->OnSelectionChanged.AddUniqueDynamic(this,
		&ThisClass::OnSelectionChanged_TargetDeactivationConditions);
	ComboBoxOption_TargetDeactivationResponses->ComboBox->OnSelectionChanged.AddUniqueDynamic(this,
		&ThisClass::OnSelectionChanged_TargetDeactivationResponses);
	ComboBoxOption_TargetDestructionConditions->ComboBox->OnSelectionChanged.AddUniqueDynamic(this,
		&ThisClass::OnSelectionChanged_TargetDestructionConditions);

	ComboBoxOption_TargetDeactivationConditions->GetComboBoxEntryTooltipStringTableKey.BindUObject(this,
		&ThisClass::GetComboBoxEntryTooltipStringTableKey_TargetDeactivationConditions);
	ComboBoxOption_TargetDeactivationResponses->GetComboBoxEntryTooltipStringTableKey.BindUObject(this,
		&ThisClass::GetComboBoxEntryTooltipStringTableKey_TargetDeactivationResponses);
	ComboBoxOption_TargetDestructionConditions->GetComboBoxEntryTooltipStringTableKey.BindUObject(this,
		&ThisClass::GetComboBoxEntryTooltipStringTableKey_TargetDestructionConditions);
	
	ComboBoxOption_TargetDeactivationConditions->ComboBox->ClearOptions();
	ComboBoxOption_TargetDeactivationResponses->ComboBox->ClearOptions();
	ComboBoxOption_TargetDestructionConditions->ComboBox->ClearOptions();

	TArray<FString> Options;
	for (const ETargetDeactivationCondition& Method : TEnumRange<ETargetDeactivationCondition>())
	{
		Options.Add(GetStringFromEnum_FromTagMap(Method));
	}
	ComboBoxOption_TargetDeactivationConditions->SortAddOptionsAndSetEnumType<ETargetDeactivationCondition>(Options);
	Options.Empty();

	for (const ETargetDeactivationResponse& Method : TEnumRange<ETargetDeactivationResponse>())
	{
		Options.Add(GetStringFromEnum_FromTagMap(Method));
	}
	ComboBoxOption_TargetDeactivationResponses->SortAddOptionsAndSetEnumType<ETargetDeactivationResponse>(Options);
	Options.Empty();

	for (const ETargetDestructionCondition& Method : TEnumRange<ETargetDestructionCondition>())
	{
		Options.Add(GetStringFromEnum_FromTagMap(Method));
	}
	ComboBoxOption_TargetDestructionConditions->SortAddOptionsAndSetEnumType<ETargetDestructionCondition>(Options);
	Options.Empty();
	
	SliderTextBoxOption_DeactivationHealthLostThreshold->SetVisibility(ESlateVisibility::Collapsed);

	SetupWarningTooltipCallbacks();
	UpdateBrushColors();
}

void UCustomGameModesWidget_Deactivation::UpdateAllOptionsValid()
{
	Super::UpdateAllOptionsValid();
}

void UCustomGameModesWidget_Deactivation::UpdateOptionsFromConfig()
{
	UpdateValueIfDifferent(ComboBoxOption_TargetDeactivationConditions,
		GetStringArrayFromEnumArray_FromTagMap(BSConfig->TargetConfig.TargetDeactivationConditions));
	UpdateValueIfDifferent(ComboBoxOption_TargetDeactivationResponses,
		GetStringArrayFromEnumArray_FromTagMap(BSConfig->TargetConfig.TargetDeactivationResponses));
	UpdateValueIfDifferent(ComboBoxOption_TargetDestructionConditions,
		GetStringArrayFromEnumArray_FromTagMap(BSConfig->TargetConfig.TargetDestructionConditions));
	UpdateValueIfDifferent(SliderTextBoxOption_DeactivationHealthLostThreshold,
		BSConfig->TargetConfig.DeactivationHealthLostThreshold);

	UpdateDependentOptions_TargetDeactivationConditions(BSConfig->TargetConfig.TargetDeactivationConditions,
		BSConfig->TargetConfig.TargetDeactivationResponses);

	UpdateBrushColors();
}

void UCustomGameModesWidget_Deactivation::SetupWarningTooltipCallbacks()
{
	ComboBoxOption_TargetDeactivationResponses->AddWarningTooltipData(FTooltipData("Invalid_Velocity_MTDM_None",
		ETooltipImageType::Caution)).BindLambda([this]()
	{
		return BSConfig->TargetConfig.MovingTargetDirectionMode == EMovingTargetDirectionMode::None && BSConfig->
			TargetConfig.TargetDeactivationResponses.Contains(ETargetDeactivationResponse::ChangeVelocity);
	});
	ComboBoxOption_TargetDeactivationResponses->AddWarningTooltipData(FTooltipData("Invalid_Direction_MTDM_None",
		ETooltipImageType::Caution)).BindLambda([this]()
	{
		return BSConfig->TargetConfig.MovingTargetDirectionMode == EMovingTargetDirectionMode::None && BSConfig->
			TargetConfig.TargetDeactivationResponses.Contains(ETargetDeactivationResponse::ChangeDirection);
	});
}

void UCustomGameModesWidget_Deactivation::UpdateDependentOptions_TargetDeactivationConditions(
	const TArray<ETargetDeactivationCondition>& Conditions, const TArray<ETargetDeactivationResponse>& Responses)
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

	if (Conditions.Contains(ETargetDeactivationCondition::OnSpecificHealthLost))
	{
		SliderTextBoxOption_DeactivationHealthLostThreshold->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		SliderTextBoxOption_DeactivationHealthLostThreshold->SetVisibility(ESlateVisibility::Collapsed);
	}
	UpdateDependentOptions_TargetDeactivationResponses(Conditions, Responses);
}

void UCustomGameModesWidget_Deactivation::UpdateDependentOptions_TargetDeactivationResponses(
	const TArray<ETargetDeactivationCondition>& Conditions, const TArray<ETargetDeactivationResponse>& Responses)
{
}

void UCustomGameModesWidget_Deactivation::OnSliderTextBoxValueChanged(USliderTextBoxOptionWidget* Widget,
	const float Value)
{
	if (Widget == SliderTextBoxOption_DeactivationHealthLostThreshold)
	{
		BSConfig->TargetConfig.DeactivationHealthLostThreshold = Value;
	}
	UpdateAllOptionsValid();
}

void UCustomGameModesWidget_Deactivation::OnSelectionChanged_TargetDeactivationConditions(
	const TArray<FString>& Selected, const ESelectInfo::Type SelectionType)
{
	if (Selected.Num() < 1 || SelectionType == ESelectInfo::Type::Direct)
	{
		return;
	}
	BSConfig->TargetConfig.TargetDeactivationConditions = GetEnumArrayFromStringArray<
		ETargetDeactivationCondition>(Selected);
	UpdateDependentOptions_TargetDeactivationConditions(BSConfig->TargetConfig.TargetDeactivationConditions,
		BSConfig->TargetConfig.TargetDeactivationResponses);
	UpdateAllOptionsValid();
}

void UCustomGameModesWidget_Deactivation::OnSelectionChanged_TargetDeactivationResponses(
	const TArray<FString>& Selected, const ESelectInfo::Type SelectionType)
{
	if (Selected.Num() < 1 || SelectionType == ESelectInfo::Type::Direct)
	{
		return;
	}

	BSConfig->TargetConfig.TargetDeactivationResponses = GetEnumArrayFromStringArray<
		ETargetDeactivationResponse>(Selected);
	UpdateDependentOptions_TargetDeactivationResponses(BSConfig->TargetConfig.TargetDeactivationConditions,
		BSConfig->TargetConfig.TargetDeactivationResponses);
	UpdateBrushColors();
	UpdateAllOptionsValid();
}

void UCustomGameModesWidget_Deactivation::OnSelectionChanged_TargetDestructionConditions(
	const TArray<FString>& Selected, const ESelectInfo::Type SelectionType)
{
	if (Selected.Num() < 1 || SelectionType == ESelectInfo::Type::Direct)
	{
		return;
	}

	BSConfig->TargetConfig.TargetDestructionConditions = GetEnumArrayFromStringArray<
		ETargetDestructionCondition>(Selected);
	UpdateAllOptionsValid();
}

FString UCustomGameModesWidget_Deactivation::GetComboBoxEntryTooltipStringTableKey_TargetDeactivationConditions(
	const FString& EnumString)
{
	return GetStringTableKeyNameFromEnum(GetEnumFromString<ETargetDeactivationCondition>(EnumString));
}

FString UCustomGameModesWidget_Deactivation::GetComboBoxEntryTooltipStringTableKey_TargetDeactivationResponses(
	const FString& EnumString)
{
	return GetStringTableKeyNameFromEnum(GetEnumFromString<ETargetDeactivationResponse>(EnumString));
}

FString UCustomGameModesWidget_Deactivation::GetComboBoxEntryTooltipStringTableKey_TargetDestructionConditions(
	const FString& EnumString)
{
	return GetStringTableKeyNameFromEnum(GetEnumFromString<ETargetDestructionCondition>(EnumString));
}
